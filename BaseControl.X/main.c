#include "p24F16KA301.h"
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#include "vision.h"
#include "infrared.h"
#include "State.h"
#include "motors.h"
#include <stdlib.h>

#define CENTER 33.5
#define WHEEL_DIA 2.5

_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided
//_FOSCSEL(FNOSC_FRC & SOSCSRC_DIG);    // 8 MHz Oscillator, Clear SOSCSRC to use pins 9,10 as digital i/o

//char arduinoAddress = 0x04<<1;

// <editor-fold defaultstate="collapsed" desc="Parameters">
const float r_window = 5;                    //inches
const float theta_window = 0.5*PI/180;       //radians

State current_state;
volatile int step_counter, step_max; //step max == 152 for 90 deg turn
volatile int angle_count;
float theta, r;
// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Function Declarations">
//Configuration
void initialize();
void updateBeacon()
    {char temp = see_beacon(&theta, &r);}
void Align();
float getError(float set_point, float measured_value);
char getDirection(float set_point, float measured_value);

int main(void)
{
    initialize();
    vision_setup();

    while(1)
    {
    //    startTurn(1);
        Align();
    //    toCenter();
    }							//end of program
    return 0;
}


void initialize(){
    current_state = start;
    r = 0;
    theta = 0;

    _TRISA2 = 0; // Stepper Dir L
    _TRISA1 = 0; // Stepper Dir R
    _TRISB1 = 0; // Stepper On or Off

    _RA2= 0;   //Stepper1
    _RA1 = 0;  //Stepper2
    _RB1 = 0;  // Don't move
}


void Start(){
//    PR3 = 625;                  //Set timer period so freq is 500 Hz (fast)
    startTurn(0);
//    while(1);
    while(see_beacon(&theta, &r) == '0'){}    //Doesn't see any light
    stop();
//    PR3 = 625;                  //Set timer period so freq is 100 Hz (slow)

    current_state = align0;
}
void Align(){
    updateBeacon();
    //theta = PI/2.0;
    while(1)
    {
        char* xptr1 = &x1;
        char* yptr1 = &y1;

        char* xptr2 = &x2;
        char* yptr2 = &y2;

        I2C2write4bytes(arduinoAddress, xptr1[1], xptr1[0], yptr1[1], yptr1[0]);
        I2C2write4bytes(arduinoAddress, xptr2[1], xptr2[0], yptr2[1], yptr2[0]);

        char status = see_beacon(&theta, &r);

        //if(status==2)
        //{
            if(x1 > X1_OFFSET + 20 )
            {
                if(x2>X2_OFFSET + 20)
                {
                    startTurn(RIGHT); // Turn Right
                }
                else
                {
                    startDrive(0); // backup
                }
            }
            else if (x1 < X1_OFFSET - 20)
            {
                if(x2 < X2_OFFSET -20)
                {
                startTurn(LEFT); // turn left
                }
                else
                {
                    startDrive(1); // Drive Forward
                }
            }
            else
            {
                if(x2>X2_OFFSET +20)
                {
                    startDrive(1); // Drive Forward
                }
                else if (x2 < X2_OFFSET-20)
                {
                    startDrive(0); // Drive Backwards
                }
                else
                {
                    //we made it!!

                }
                
            }
            
        //}
        //else
        //{
        //}// returned did not see light
            //startTurn(0); // turn right
    }
    stop();

    if(current_state == align0) current_state = forward0;
    if(current_state == align1) current_state = forward1;
    if(current_state == align2) current_state = shoot;
}
void toCenter(){
    startDrive(1);
    while(see_beacon(&theta, &r) == '0'){}
    startDrive(getDirection(CENTER, r));
    while(abs(getError(CENTER, r)) > r_window) {
        if(see_beacon(&theta, &r) == 'm'){
            //stop();
            Align();
            startDrive(getDirection(CENTER, r));
        }
    }
    stop();

    current_state = at_center;
}

float getError(float set_point, float measured_value){
    return set_point - measured_value;
}

char getDirection(float set_point, float measured_value){
    if(getError(set_point, measured_value) > 0)
        return 0;   //forward or right
    else return 1;  //backward or left
}