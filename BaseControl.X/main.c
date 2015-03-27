#include <p24F16KA301.h>
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#include "vision.h"
#include "motors.h"
#include "i2c_debug.h"
#include "State.h"



_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided

void DelayuSec(unsigned int s);
char timeToReadI2C;

void _ISR _T1Interrupt(void)
{
    _T1IF = 0; // clear interrupt flag
    timeToReadI2C = 1;
}

char flag;
int main(void)
{

    vision_setup();
    motorsSetup();

    _TRISA1 = 0; // I know this is redundant, but it doesn't work otherwise
    _TRISA2 = 0;
    _TRISB1 = 0;
    
    theta_window = 3.0*PI/180.0;
    timeToReadI2C = 1;

//    float r =0;
//    float theta=0;

    // enable Timer 1 and setup Interrupt
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b10;
    PR1 = 10000; // period to read I2C
    TMR1 = 0;
    _T1IE = 1;
    _T1IF = 0;

//    char flag;
    while(1)
    {
        if(timeToReadI2C) // Refresh Vision Data
        {
            timeToReadI2C = 0;
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

        //this part isn't really working yet. can't get alignTheta() to work
        // <editor-fold defaultstate="collapsed" desc="State Machine">
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

            case aligned:
                state = search;
                break;

            default:
                state = search;
                break;
        }
        // </editor-fold>

    }
    							//end of program
}







