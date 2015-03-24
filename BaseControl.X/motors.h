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


void initialize(){
    current_state = start;
    r = 0;
    theta = 0;

    step_counter = 0;
    step_max = 100000;

    _RA2= 0;   //Stepper1
    _RA1 = 0;   //Stepper2
    OC3R = 0.5*PR3; //Stepper duty cycle
    T3CONbits.TON = 0;           //disable Timer3
}

void startDrive(unsigned int direction){
    if(direction == 0){         //drive forward
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

#endif	/* MOTORS_H */

