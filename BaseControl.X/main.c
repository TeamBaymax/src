#include "p24F16KA301.h"
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#include "vision.h"
#include "i2c_debug.h"


_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided

void DelayuSec(unsigned int s);
char timeToReadI2C;
float theta_window;

void _ISR _T1Interrupt(void)
{
    _T1IF = 0; // clear interrupt flag
    timeToReadI2C = 1;
}

int main(void)
{
    theta_window = 0.5*PI/180.0;
    timeToReadI2C = 1;
    _TRISA0 = 0;
    _TRISA2 = 0;
    _TRISB1 = 0;

    int x1,x2;
    int y1,y2;
    float r =0;
    float theta=0;

    T1CONbits.TON = 1;
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 0b10;
    PR1 = 10000; // period to read I2C
    TMR1 = 0;
    _T1IE = 1;
    _T1IF = 0;

    i2c1_init(100);
    i2c2_init(100);

    ir1_init();
    ir2_init();

    while(1)
    {
        if(timeToReadI2C)
        {
            x1 = y1 = x2 = y2 = 0;
            ir1_request(&x1, &y1);
            ir2_request(&x2, &y2);
            timeToReadI2C = 0;

            //debug_2_ints(x1,y1);
            //debug_2_ints(1004,988);
            float test = 10.5;

            //int thint = (int)test;
            //int rint = (int)r;
            //char* thptr = &thint;
            //char* rptr = &rint;
            //I2C2write4bytes(arduinoAddress, xptr1[0], xptr1[1], yptr1[0], yptr1[1]);
            //I2C2write4bytes(arduinoAddress, xptr2[1], xptr2[0], yptr2[1], yptr2[0]);
            //I2C2write4bytes(arduinoAddress, thptr[1], thptr[0], rptr[1], rptr[0]);
            stereo_vision(x1,x2, &theta,&r);
            //debug_2_ints(x1,y1);
            //debug_2_ints(x2,y2);
            debug_float(r);
            debug_float(theta);
        }
        if(theta < -1.0*theta_window)
        {
            LATAbits.LATA0 = 1;
            LATAbits.LATA2 = 0;
            LATBbits.LATB1 = 1; // to be replaced by PWM on OCR3
        }
        else if(theta > theta_window)
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
    							//end of program
}







