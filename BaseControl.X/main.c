#include "p24F16KA301.h"
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#include "vision.h"
#include "motors.h"
#include "i2c_debug.h"


_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided

void DelayuSec(unsigned int s);
char timeToReadI2C;

void setup()
{
    vision_setup();
}

void _ISR _T1Interrupt(void)
{
    _T1IF = 0; // clear interrupt flag
    timeToReadI2C = 1;
}

int main(void)
{

    setup();
    _TRISA0 = 0;
    _TRISA2 = 0;
    _TRISB1 = 0;
    
    theta_window = 0.5*PI/180.0;
    timeToReadI2C = 1;

    float r =0;
    float theta=0;

    // enable Timer 1 and setup Interrupt
    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b10;
    PR1 = 10000; // period to read I2C
    TMR1 = 0;
    _T1IE = 1;
    _T1IF = 0;

    char flag;
    while(1)
    {
        if(timeToReadI2C)
        {
            timeToReadI2C = 0;
            
            flag = see_beacon(&theta, &r)

            debug_2_ints(x1,y1);
            debug_2_ints(x2,y2);

            debug_float(r);
            debug_float(theta);
        }
        if(!flag)
        {
            LATAbits.LATA0 = 1;
            LATAbits.LATA2 = 0;
            LATBbits.LATB1 = 1; // to be replaced by PWM on OCR3
        }
        else
        {
            if(theta > 1.0*theta_window)
            {
                LATAbits.LATA0 = 1;
                LATAbits.LATA2 = 0;
                LATBbits.LATB1 = 1; // to be replaced by PWM on OCR3
            }
            else if(theta < -1.0*theta_window)
            {
                LATAbits.LATA0 = 0;
                LATAbits.LATA2 = 1;
                LATBbits.LATB1 = 1; // to be replaced by PWM on OCR3
            }
            else
            {
                LATBbits.LATB1 = 0;
                LATAbits.LATA0 = 0;
                LATAbits.LATA2 = 0;
            }
        }
    }
    							//end of program
}







