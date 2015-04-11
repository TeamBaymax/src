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
char flag;
int main(void)
{
    
    period = locating;
    state = search;

    vision_setup();
    motorsSetup();
    game_time_setup();
    servoSetup();

    _TRISA1 = 0; // I know this is redundant, but it doesn't work otherwise
    _TRISA2 = 0;
    _TRISB1 = 0;
    _TRISB0 = 0;
    _TRISB12 = 0;
    _TRISA6 = 0;
    
    theta_window = 3.0*PI/180.0;

    while(1)
    {
        if(timeToReadVision) // Refresh Vision Data
        {
            flag = see_beacon(&theta, &r);
         
         // I2C Debug
//          debug_2_ints(x1,y1);
//          debug_2_ints(x2,y2);
//          debug_float(r);
//          debug_float(getAngle());
      
        }
        // <editor-fold defaultstate="expanded" desc="State Machine">

        char status;

        if(game_timer > 105.0){ // game is over
            period = finishing;
            state = search;
            stopShooter();
        }

        switch(period){

            // <editor-fold defaultstate="expanded" desc="Locating">
            case locating: // if we are in the first 10 seconds of the game
                switch(state){

                    case search:
                        status = circleSearch(LEFT, flag);
                        if(flag) // found beacon
                            //if(game_timer > 5.0) // we have missed the first light
                                //period = loading;
                                //this kills the process
                            state = aligntheta;
                        break;

                    case aligntheta:
                        status = alignTheta(flag);
                        if (status == LOSTBEACON) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(33.5, flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON){ // error state
                            openloopTurn(90.0, LEFT, flag);
                            state = searchgarage; // this is a total hack
                        }else if(status == 1){ // reached desired distance
                            openloopTurn(90.0, LEFT, flag);
                            state = wait;
                        }
                        break;

                    case wait:
                        if(game_timer > 5.0){
                            state = searchgarage;
                        }
                        break;

                    case searchgarage:
                        if(flag){ // we can see the beacon right away
                            state = aligntheta;
                            period = loading;
                        }else{ // we cannot see the beacon
                            openloopDist(3.0,REVERSE,flag);
                            openloopTurn(10.0,LEFT,flag);
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
                        //status = searchGarage(LEFT, flag);
                        status = circleSearch(LEFT,flag);
                        if(status == 1) // found beacon
                            state = aligntheta;
                        break;

                    case aligntheta:
                        status = alignTheta(flag);
                        if (status == LOSTBEACON) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            resetAngle(); // reset garage angle memory
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(15.0, flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON) // error state
                            state = search;
                        else if(status == 1) // reached desired distance
                            state = collect;
                        break;

                    case collect:
                        openloopDist(5, FORWARD, flag);
                        loadBalls(6);
                        openloopDist(20.0, REVERSE,flag);
                        openloopTurn(90.0,LEFT,flag);
                        period = scoring;
                        state = search;
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
                        //status = searchGoal(LEFT, flag);
                        status = circleSearch(LEFT,flag);
                        spinShooter();
                        if(status == 1) // found beacon
                            state = aligntheta;
                        break;

                    case aligntheta:
                        status = alignTheta(flag);
                        if (status == LOSTBEACON) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(33.5, flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON) // error state
                            state = search;
                        else if(status == 1) // reached desired distance
                            state = aimturret;
                        break;

                    case aimturret:
                        status = aim(0.5*PI/180.0,flag);
                        if(status==LOSTBEACON)
                            state = search;
                        else if(status ==1)
                            state = shoot;
                        break;

                    case shoot:
                        status = shootBalls(flag);
                        if(status == LOSTBEACON)
                        {
                            state = search; // look for another goal
                        }
                        else if(status == 1) // finished shooting
                        {
                            openloopTurn(90.0,LEFT,flag);
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
                        status = circleSearch(LEFT, flag); //modify
                        if(status == 1)
                            state = aligntheta;
                        break;

                    case aligntheta: // align to goal
                        status = alignTheta(flag);
                        if(status==1)
                            state = aligndist;
                        break;

                    case aligndist: // go get the right distance away
                        status = alignDist(33.5, flag);
                        if(status == OUTOFWINDOW) // traveled out of window
                            state = aligntheta;
                        else if(status == LOSTBEACON)
                            state = search;
                        else if(status = 1) // reached desired distance
                            state = end;
                        break;
                    case end:
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









