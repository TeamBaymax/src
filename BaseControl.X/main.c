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
//
//           debug_2_ints(x1,y1);
//            debug_2_ints(x2,y2);

//            debug_float(r);
//            debug_float(theta);
        }

        // <editor-fold defaultstate="collapsed" desc="State Machine">
        switch(period){
            case locating:{
                switch(state){
                    case search:
                        circleSearch(LEFT, flag);
                        break;
                    case aligntheta:
                        alignTheta(flag);
                        break;
                    case aligndist:
                        alignDist(33.5, flag);
                        break;
                    case wait:
                        state = search;
                        break;

                    case searchgarage:
//                        findGarage(flag);     --not implemented yet
                        circleSearch(LEFT, flag);
                         break;
                    case aligncollect:
                        alignTheta(flag);
                        break;
                    case gocollect:
                        alignDist(12.0, flag);
                        break;
                    case load:
                        loadBalls(6);
                    case donecollect:
                        //turn 180 and drive to center
                        break;

                    case searchgoal:
                        circleSearch(LEFT, flag);
                        break;
                    case aligngoal:
                        alignTheta(flag);
                        break;
                    case distgoal:
                        alignDist(33.5,flag);
                        break;
                    case shoot:



                    default:
                        state = search;
                        break;
                }
            }
        }
        // </editor-fold>
   }
}//end of program









