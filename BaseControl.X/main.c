/*
 * File:   main.c
 * Author: James
 *
 * Created on March 7, 2015, 2:18 PM
 */

#include <p24F16KA301.h>
#include "i2c1_helper.h"
#include "i2c2_helper.h"
//#include "vision.h"
//#include "motors.h"
#include "i2c_debug.h"
#include "State.h"
//#include "servo.h"
//#include "game_timer.h"


_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided




float initial_r;
int main(void)
{

    char current_active_goal = 0b000;
    char used_goals = 0b000;
    Period period = locating;
    State state = search;
    VisionFlag vision_flag;

    vision_setup();
    motorsSetup();
    game_time_setup();
    servoSetup();
    float course_angle;

    _TRISA1 = 0; // I know this is redundant, but it doesn't work otherwise
    _TRISA2 = 0;
    _TRISB1 = 0;
    _TRISB0 = 0;
    _TRISB12 = 0;
    _TRISA6 = 0;

    theta_window = 3.0*PI/180.0;
    char current_quadrant = 0b000;
    while(1)
    {
        course_angle = getAngle();
        if(timeToReadVision) // Refresh Vision Data
        {
            vision_flag = see_beacon(&theta, &r);
         // I2C Debug
//          debug_2_ints(x1,y1);
//          debug_2_ints(x2,y2);
//          debug_float(r);
//          debug_float(course_angle);
        }
        // <editor-fold defaultstate="expanded" desc="State Machine">

        char status;

        if(game_timer > 100.0){ // game is over
            period = finishing;
            state = search;
            //stopShooter();
        }

        switch(period){

            // <editor-fold defaultstate="expanded" desc="Locating">
            case locating: // if we are in the first 10 seconds of the game
                switch(state){

                    case search:
                        status = circleSearch(LEFT, vision_flag);
                        if(vision_flag) // found beacon
                            //if(game_timer > 5.0) // we have missed the first light
                                //period = loading;
                                //this kills the process
                            state = aligntheta;
                        break;

                    case aligntheta:
                        status = alignTheta(vision_flag);
                        if (status == LOSTBEACON) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(33.5, vision_flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON){ // error state
                            openloopTurn(90.0, LEFT, vision_flag);
                            state = searchgarage; // this is a total hack
                        }else if(status == 1){ // reached desired distance
                            openloopTurn(90.0, LEFT, vision_flag);
                            state = wait;
                        }
                        break;

                    case wait:
                        if(game_timer > 5.0){
                            state = searchgarage;
                        }
                        break;

                    case searchgarage:
                        if(vision_flag){ // we can see the beacon right away
                            resetAngle();
                            state = aligntheta;
                            period = loading;
                        }else{ // we cannot see the beacon
                            openloopDist(3.0,REVERSE,vision_flag);
                            openloopTurn(10.0,LEFT,vision_flag);
                        }
                         break;

                    case halt:
                        stop();
                        break;

                    default:
                        state = search;
                }
                break;
                //</editor-fold>

            // <editor-fold defaultstate="expanded" desc="Loading">
            case loading: // loading new balls
                switch(state){
                    case search:
                        if(course_angle > 180.0)
                            status = searchGarage(LEFT, vision_flag, course_angle);
                        else
                            status = searchGarage(RIGHT, vision_flag, course_angle);
                        if(status == 1){
                                state = aligntheta;
                        }
                        break;

                    case aligntheta:
                        status = alignTheta(vision_flag);
                        if (status == LOSTBEACON) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            resetAngle(); // reset garage angle memory
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(16.0, vision_flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON) // error state
                            state = search;
                        else if(status == 1) // reached desired distance
                            state = collect;
                        break;

                    case collect:
                        status = alignDist(16.0, vision_flag);
                        if(status == 1) // redundant check
                        {
                            openloopDist(5, FORWARD, vision_flag);
                            retractServo();
                            if(game_timer > 10.0)
                            {
                                loadBalls(6);
                                openloopDist(11.0, REVERSE,vision_flag);
                                status = alignDist(16.0, vision_flag);
                                //openloopTurn(90.0,LEFT,vision_flag);
                                current_quadrant = getcurrentQuadrant(course_angle);
                                period = scoring;
                                state = search;
                            }
                        }
                        break;
                     break;

                    default:
                        state = search;
                        break;
                }
                break;
                //</editor-fold>

            // <editor-fold defaultstate="expanded" desc="Scoring">
            case scoring: // finding goals and shooting
                switch(state){
                    case search:
                        if(last_round != round)
                        {
                            used_goals = current_active_goal | used_goals;
                            last_round = round;
                        }
                        if(current_quadrant & 0b001) // looking at goal three
                            status = searchGoal(RIGHT, vision_flag, course_angle);
                        else if(current_quadrant & 0b100) // looking at goal one
                            status = searchGoal(LEFT, vision_flag, course_angle);
                        else if(current_quadrant & 0b010){ // looking at goal two
                            if(used_goals & 0b001) // if goal three has been used
                                status = searchGoal(RIGHT, vision_flag, course_angle);
                            else
                                status = searchGoal(LEFT, vision_flag, course_angle);
                        }else
                        {
                            if(used_goals & 0b100) // right goal has been used
                                status = searchGoal(RIGHT, vision_flag, course_angle);
                            else
                                status = searchGoal(LEFT, vision_flag, course_angle);
                        }
                        spinShooter();
                        if(status == 0)
                        {
                            period = scoring;
                            state = search;
                        }
                        if(status == 1) // if we see a goal
                        {
                            state = aligntheta;
                        }
                        break;

                    case aligntheta:
                        status = alignTheta(vision_flag);
                        if (status == LOSTBEACON) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(33.5, vision_flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON) // error state
                            state = search;
                        else if(status == 1) // reached desired distance
                            state = aimturret;
                        break;

                    case aimturret:
                        status = aim(0.5*PI/180.0,vision_flag);
                        if(status==LOSTBEACON)
                            state = search;
                        else if(status ==1)
                        {
                            current_active_goal = getcurrentQuadrant(course_angle);
                            state = shoot;
                        }
                        break;

                    case shoot:
                        status = shootBalls(vision_flag);
                        if(status == LOSTBEACON)
                        {
                            current_quadrant = getcurrentQuadrant(course_angle);
                            state = search; // look for another goal
                        }
                        else if(status == 1) // finished shooting
                        {
                            //openloopTurn(90.0,LEFT,vision_flag);
                            stopShooter();
                            period = loading;
                            state = search;
                        }                        
                        break;

                    case halt:
                        stop();
                        break;

                    default:
                        state = search;
                        break;
                }
                break;
                //</editor-fold>

            // <editor-fold defaultstate="expanded" desc="Finishing">
            case finishing:
                switch(state){
                    case search:
                        status = circleSearch(LEFT, vision_flag); //modify
                        if(status == 1)
                            state = aligntheta;
                        break;

                    case aligntheta: // align to goal
                        status = alignTheta(vision_flag);
                        if(status==1)
                            state = aligndist;
                        break;

                    case aligndist: // go get the right distance away
                        status = alignDist(33.5, vision_flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON)
                            state = search;
                        else if(status = 1) // reached desired distance
                            state = end;
                        break;
                    case end:
                        startTurn(RIGHT);
                        balls = 15;
                        advanceBall();
                        break;
                }
                break;
                //</editor-fold>

          default: // default error state
              period = loading;
              state = search;
              break;
            } // period case structure
        // </editor-fold>

    }

} // int main()









