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
    state = searchgarage;
    period = locating;

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
            // flag = 255 if error
            // flag = 0 if no beacon
            // flag = 1 if one camera sees beacon
            // flag = 2 if two cameras see beacon

//          debug_2_ints(x1,y1);
//          debug_2_ints(x2,y2);

//          debug_float(r);
//          debug_float(theta);
        }
        // <editor-fold defaultstate="collapsed" desc="State Machine">

        char status;
        switch(period){
            case locating: // if we are in the first 10 seconds of the game
                switch(state){

                    case search:
                        status = circleSearch(LEFT, flag);
                        if(status ==1) // found beacon
                            state = aligntheta;
                        break;

                    case aligntheta:
                        status = alignTheta(flag);
                        if (status == 255) // lost beacon
                            state = search;
                        else if(status == 1) // aligned
                            state = aligndist;
                        break;

                    case aligndist:
                        status = alignDist(33.5, flag);
                        if(status == 254) // traveled out of window
                            state = aligntheta;
                        else if(status == 255) // error state
                            state = search;
                        else if(status = 1) // reached desired distance
                            state = wait;
                        break;

                    case wait:
                        // look at game timer
                        status = openloopTurn(90.0, RIGHT, flag);
                        state = searchgarage;
                        break;

                    case searchgarage:
                        if(flag){ // we can see the beacon right away
                            state = aligncollect;
                            period = scoring;
                        }else{ // we cannot see the beacon
                            openloopDist(5.0,REVERSE,flag);
                            openloopTurn(5,RIGHT,flag);
                        }
                         break;
                }
                break;
            
            case loading: // loading new balls
                switch(state){
                    case search: // search for the garage
                        status = circleSearch(LEFT,flag);
                        if(status ==1);
                            state = aligntheta;
                        break;

                    case aligntheta: // align to garage
                        status = alignTheta(flag);
                        if(status==1)
                            state = aligndist;
                        break;

                    case aligndist: // go get balls
                        status = alignDist(33.5, flag);
                        if(status == 254) // traveled out of window
                            state = aligntheta;
                        else if(status == 255)
                            state = search;
                        else if(status = 1) // reached desired distance
                            state = collect;
                        break;

                    case collect:
                        openloopDist(5, FORWARD, flag);
                        loadBalls(6);
                        openloopDist(33.5, REVERSE,flag);
                        period = scoring;
                        state = search;
                        break;
                     break;

                    default:
                        state = search;
                        break;
                }
                break;
            case scoring: // finding goals and shooting
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
                        if(status == 254) // traveled out of window
                            state = aligntheta;
                        else if(status == 255)
                            state = search;
                        else if(status = 1) // reached desired distance
                            state = collect;
                        break;

                    case shoot:
                        shootBalls(6);
                        openloopTurn(90,LEFT,flag);
                        period = loading;
                        state = search;

                    default:
                        state = search;
                        break;
                }
                break;

          default: // default error state
              period = loading;
              state = search;
              break;
            } // period case structure
        // </editor-fold>
        
        } // while(1) loop
     
} // int main()









