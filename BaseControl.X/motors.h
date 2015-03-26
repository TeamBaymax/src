/* 
 * File:   motors.h
 * Author: gary
 *
 * Created on March 24, 2015, 2:18 PM
 */

#ifndef MOTORS_H
#define	MOTORS_H

#include <p24F16KA301.h>

#define RIGHT 0
#define LEFT 1
#define FORWARD 0
#define REVERSE 1


/*void initialize(){
//    current_state = start;
//    r = 0;
//    theta = 0;
//
//    step_counter = 0;
//    step_max = 100000;

    _RA2= 0;   //Stepper1
    _RA1 = 0;   //Stepper2
    OC3R = 0.5*PR3; //Stepper duty cycle
    T3CONbits.TON = 0;           //disable Timer3
}*/
<<<<<<< HEAD
void motorsSetup()
{
    _TRISA0 = 0;
    _TRISA2 = 0;
    _TRISB1 = 0;
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T2CONbits.TCKPS = 0b01;
    TMR2 = 0;
    _T2IE = 1;
    _T2IF = 0;
    _T2IP = 4;
    OC3CON1bits.OCTSEL = 0b000;
    OC3CON1bits.OCM = 0b110;
    OC3CON2bits.SYNCSEL = 0b01100;
    PR2 = 5000;
    OC3R = PR2/2;
}

void startDrive(unsigned int direction){
    if(direction == FORWARD){         //drive forward
=======

void startDrive(unsigned int direction){
    if(direction == 0){         //drive forward
>>>>>>> master
         _RA2 = 0;
         _RA1 = 0;

    }
    else{                       //drive reverse
        _RA2 = 1;
        _RA1 = 1;
    }

    T3CONbits.TON = 1;           //enable Timer3
}

void startTurn(unsigned int direction){
    if(direction == 0){         //turn right
         _RA2 = 0;
         _RA1 = 1;

    }
    else{                       //turn left
        _RA2 = 1;
        _RA1 = 0;
    }

    T3CONbits.TON = 1;           //enable Timer3
}

void stop(){
    T3CONbits.TON = 0;           //disable Timer3
    _RA1 = 0;
    _RA2 = 0;
}

<<<<<<< HEAD
// Straight line - distance [inches], direction forward = 0, reverse = 1
void straight(float distance, unsigned int direction){  //inches
    if(direction == 0){         //drive forward
         _RA2 = 0;
         _RA1 = 0;

    }
    else{                       //drive reverse
        _RA2 = 1;
        _RA1 = 1;
    }
    //step_counter = 0;

    //200 steps = WHEEL_DIA*pi inches;
    //step_max = 200.0/(3.14159265*WHEEL_DIA) * distance;
    T3CONbits.TON = 1;           //enable Timer3
}

// Turn, angle [degrees], direction right = 0, left = 1;
void turn(float angle, unsigned int direction){

    if(direction == 0){         //turn right
         _RA2 = 0;
         _RA1 = 1;

    }
    else{                       //turn left
        _RA2 = 1;
        _RA1 = 0;
    }
    //step_counter = 0;

    //steps = 200/(pi*WHEEL_DIA) * 7.85/2 * angle * pi/180; minus 5 fudgefactor
    //step_max = 152.0/90.0 * angle;
    T3CONbits.TON = 1;           //enable Timer3
}

=======
>>>>>>> master
#endif	/* MOTORS_H */

