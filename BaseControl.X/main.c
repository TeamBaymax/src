/*
 * File:   main.c
 * Author: Emily Lazalde
 * Description: Controlling stepper motors. Straight line and 90 deg turn
 *
 * Created on March 2, 2015, 10:06 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <p24F16KA301.h>
#include "State.h"
#include "../PIC_I2C_IR_CAMERA.X/vision.h"

_FOSCSEL(FNOSC_FRC & SOSCSRC_DIG);    // 8 MHz Oscillator, Clear SOSCSRC to use pins 9,10 as digital i/o

// <editor-fold defaultstate="collapsed" desc="Parameters">
const float wheel_dia = 2.5;            //inches
const float r_window = 0.5*PI/180.;     //radians 
const float theta_window = 4.0;         //inches

State current_state;
volatile int step_counter, step_max; //step max == 152 for 90 deg turn
volatile int angle_count;
float theta, r;

//Functions
void config();
void initialize();

void updateBeacon(){char temp = see_beacon(&theta, &r);}

float getError(float set_point, float measured_value);
unsigned int getDirection(float set_point, float measured_value);
void startDrive(unsigned int direction);
void startTurn(unsigned int direction);
void stop();
void straight(float distance, unsigned int direction);
void turn(float angle, unsigned int direction);

void Start();
void Align();
void cornerDrive();
void Drive(float set_point);
void Forward0();
void Reverse0();
void atCenter();
void Align1();
void Reverse1();
void Forward1();
void Scan();
void Align2();
void Shoot();
void End();
void Finish();
// </editor-fold>

int main(int argc, char** argv) {

    config();
    initialize();

    //Beginning States
    Start();
    Align();
    cornerDrive();



    // <editor-fold defaultstate="collapsed" desc="while(1)">

    while(1){

//        if(current_state == start){
//            Start();
//        }
//        else if(current_state == align0){
//            Drive();
//        }
//        else if(current_state == forward0){
//            Forward0();
//        }
//        else if(current_state == reverse0){
//            Reverse0();
//        }
//        else if(current_state == at_center){
//            atCenter();
//        }

            //add collector states here

//        else if(current_state == scan){
//            Scan();
//        }
//        else if(current_state == align2){
//            Align2();
//        }
//        else if(current_state == shoot){
//            Shoot();
//        }
//        else if(current_state == end){
//            End()
//        }
//        else if(current_state == finish){
//            Finish()
//        }




    }
        // </editor-fold>
    return (0);
}

void config(){

   // Configure the digital I/O ports
    TRISA = 0;          //Set A ports to output
    TRISB = 0;          //Set B ports to output

    ANSA = 0;          //disables analog input
    ANSB = 0x000;      //disable analog input

    _TRISB13 = 1;       //digital input bumper 1
    _TRISB14 = 1;       //digital input bumper 2

 // Configure Timer1 using T1CON register
    _TON = 1;           //enable Timer1
    _TCS = 0;           //Set source to internal clock
    _TCKPS = 0b01;      //Select prescale value - 1/8
    PR1 = 5000;         //Set timer period so freq is 100 Hz
    TMR1 = 0;
 // Timer 2
    T2CONbits.TON = 1;           //enable Timer2
    T2CONbits.TCS = 0;           //Set source to internal clock
    T2CONbits.TCKPS = 0b01;      //Select prescale value - 1/8
    PR2 = 5000;                  //Set timer period so freq is 100 Hz
    TMR2 = 0;
 // Timer 3
    T3CONbits.TON = 1;           //enable Timer3
    T3CONbits.TCS = 0;           //Set source to internal clock
    T3CONbits.TCKPS = 0b10;      //Select prescale value - 1/64
    PR3 = 625;                  //Set timer period so freq is 100 Hz
    TMR3 = 0;

 // Configure Output Compare
    OC1CON1bits.OCTSEL = 0b100;     //Select Timer1 to be timer source
    OC1CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC1CON2bits.SYNCSEL = 0b01011;  //Select Timer1 as synchronization source
 // Output 2
    OC2CON1bits.OCTSEL = 0b000;     //Select Timer2 to be timer source
    OC2CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC2CON2bits.SYNCSEL = 0b01100;  //Select Timer2 as synchronization source
 // Output 3 - Stepper Motors
    OC3CON1bits.OCTSEL = 0b001;     //Select Timer3 to be timer source
    OC3CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b01101;  //Select Timer3 as synchronization source

 // Configure Timer3 interrupt
    IPC2bits.T3IP = 4;      //Set Priority
    IEC0bits.T3IE = 1;      //Enable Interrupt
    _T3IF = 0;              //Clear Interrupt flag (IFS0bits.T3IF)
}
void initialize(){
    current_state = start;

    step_counter = 0;
    step_max = 0;

    _RB1 = 0;   //Stepper1
    _RA4 = 0;   //Stepper2
    OC3R = 0.5*PR3; //Stepper duty cycle
    T3CONbits.TON = 0;           //disable Timer3
}

// <editor-fold defaultstate="collapsed" desc="Controlling Functions">

float getError(float set_point, float measured_value){
    return set_point - measured_value;
}

unsigned int getDirection(float set_point, float measured_value){
    if(getError(set_point, measured_value) > 0)
        return 0;   //forward or right
    else return 1;  //backward or left
}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="Navigation Functions">

void startDrive(unsigned int direction){
    if(direction == 0){         //drive forward
         _RB12 = 0;
         _RA4 = 0;

    }
    else{                       //drive reverse
        _RB12 = 1;
        _RA4 = 1;
    }
    T3CONbits.TON = 1;           //enable Timer3
}

void startTurn(unsigned int direction){
    if(direction == 0){         //turn right
         _RB12 = 0;
         _RA4 = 1;

    }
    else{                       //turn left
        _RB12 = 1;
        _RA4 = 0;
    }
    T3CONbits.TON = 1;           //enable Timer3
}

void stop(){
    T3CONbits.TON = 0;           //disable Timer3
    _RA4 = 0;
    _RB12 = 0;
}

// Straight line - distance [inches], direction forward = 0, reverse = 1
void straight(float distance, unsigned int direction){  //inches
    if(direction == 0){         //drive forward
         _RB12 = 0;
         _RA4 = 0;

    }
    else{                       //drive reverse
        _RB12 = 1;
        _RA4 = 1;
    }
    step_counter = 0;

    //200 steps = wheel_dia*pi inches;
    step_max = 200.0/(3.14159265*wheel_dia) * distance;
    T3CONbits.TON = 1;           //enable Timer3
}

// Turn, angle [degrees], direction right = 0, left = 1;
void turn(float angle, unsigned int direction){

    if(direction == 0){         //turn right
         _RB12 = 0;
         _RA4 = 1;

    }
    else{                       //turn left
        _RB12 = 1;
        _RA4 = 0;
    }
    step_counter = 0;

    //steps = 200/(pi*wheel_dia) * 7.85/2 * angle * pi/180; minus 5 fudgefactor
    step_max = 152.0/90.0 * angle;
    T3CONbits.TON = 1;           //enable Timer3
}


/*
 * Timer3 interrupt service routine
 * This function executes every time the micro receives an interrupt
 * originating from Timer3. The micro knows the interrupt is from Timer3
 * when the Timer3 interrupt flag (T3IF) is set.
 */
void _ISR _T3Interrupt(void)
{
    // Remember to clear the Timer1 interrupt flag when this ISR is entered.
    _T3IF = 0;

    step_counter++;

    if(step_counter >= step_max){   //stop moving
            _RA4 = 0;
            _RB12 = 0;
            T3CONbits.TON = 0;           //disable Timer3
    }



    //Milestone 4 code
//    if(step_counter == 200){ //start turning
//                _RB12 = 1;
//                _RA4 = 0;
//    }
//    else if(step_counter >= 352){   //stop turning 152 steps = 90 deg
//            _RA4 = 0;
//            _RB12 = 0;
//            OC3R = 0;
//    }

}
// </editor-fold>

// <editor-fold defaultstate="collapsed" desc="State Functions">

void Start(){
    PR3 = 125;                  //Set timer period so freq is 500 Hz (fast)
    startTurn(0);
    while(see_beacon(&theta, &r) == '0');    //Doesn't see any light
    stop();
    PR3 = 625;                  //Set timer period so freq is 100 Hz (slow)

    current_state = align0;
}
void Align(){
    updateBeacon();
    startTurn(getDirection(0, theta));
    while(abs(getError(0, theta)) > theta_window)
        {updateBeacon();}
    stop();
    
    if(current_state == align0) current_state = forward0;
    if(current_state == align1) current_state = forward1;
    if(current_state == align2) current_state = shoot; 
}
void cornerDrive(){
    float set_point = 0;
    updateBeacon();
    startDrive(getDirection(set_point, r));
    while(_RB13 == 0 || _RB14 ==0) {
        if(see_beacon(&theta, &r) == 'm'){
            //stop();
            Align();
            startDrive(getDirection(set_point, r));
        }
        else if(_RB13 != _RB14){
            int dir = _RB14;
            startDrive(1);
            DelayuSec(1000000);
            startTurn(dir);    //then don't align
            DelayuSec(1000000);
//            Align();
            updateBeacon();
            startDrive(getDirection(set_point,r));
        }
    }
    stop();

    if(current_state == forward0) {current_state = reverse0;}    
}
void Drive(float set_point){
    updateBeacon();
    startDrive(getDirection(set_point, r));
    while(abs(getError(set_point, r)) > r_window) {
        if(see_beacon(&theta, &r) == 'm'){
            //stop();
            Align();
            startDrive(getDirection(set_point, r));
        }
        else if(see_beacon(&theta, &r) == '0'){
            //check bumpers
        }
    }
    stop();

    if(current_state == forward0) {current_state = reverse0;}
    //    if(current_state == forward1) {current_state = reverse1;}
}
//void Forward0(){
//    startDrive(getDirection());
//    while(abs(getError) > 0.1);     //do we want this to be 0 exactly?
//    stop();
//
//    if(current_state == forward0) {current_state = align0;}
//    if(current_state == forward1) {current_state = reverse1;}
//}
//void Reverse0(){
//
//}
void atCenter(){
    
}
void Align1();
void Reverse1();
void Forward1();
void Scan();
void Align2();
void Shoot();
void End();
void Finish();

// </editor-fold>

 /* References:
  *
  * Data Sheet: http://ww1.microchip.com/downloads/en/DeviceDoc/39995d.pdf
  *
  * Family Reference Manual: http://www.microchip.com/TechDoc.aspx?type=ReferenceManuals
  *
  * Tips and Tricks: https://docs.google.com/document/d/1YN8Au7-waxfx1F3gALHjsmbpb9-qOa-AEzvvl6XvJO0/edit
  *
  *
  */