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


float game_timer; // global variable that is the game time in seconds

void _ISR _T1Interrupt(void)
{
    _T1IF = 0; // clear interrupt flag
    timeToReadI2C = 1;
}

void _ISR _T2Interrupt(void)
{
    _T2IF = 0; // clear interrupt flag
    game_timer = game_timer + 0.1;
}

float initial_r;
char flag;
int main(void)
{
    state = search;

    vision_setup();
    motorsSetup();

    _TRISA1 = 0; // I know this is redundant, but it doesn't work otherwise
    _TRISA2 = 0;
    _TRISB1 = 0;
    
    theta_window = 3.0*PI/180.0;
    timeToReadI2C = 1;

    // enable Timer 1 and setup Interrupt for I2C throttling
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b10;
    PR1 = 10000; // period to read I2C
    TMR1 = 0;
    _T1IE = 1;
    _T1IF = 0;

    // enable Timer 2 as game timer
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b11; // post scale by 256 (15625 cycles per second)
    PR2 = 1563; // results in 10 Hz
    TMR2 = 0;
    _T2IE = 1;
    _T2IF = 0;
    game_timer = 0.0;

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
        switch(period){
            case locating:
            {
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
                        waitUntil(5.0);
                        break;

                    default:
                        state = search;
                        break;
                }
        // </editor-fold>

    }
    							//end of program
}







