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

void _ISR _T3Interrupt(void)
{
    _T3IF = 0; // clear interrupt flag

}

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
void motorsSetup()
{
    _TRISA0 = 0;            //Set to Output
    _TRISA2 = 0;
    _TRISB1 = 0;
    T3CONbits.TON = 1;      //Enable
    T3CONbits.TCS = 0;      //Set source to Internal Clock
    T3CONbits.TCKPS = 0b10; //Prescale: 1/64
    PR3 = 625;//5000;       //Set Period so freq = 100 Hz
    TMR3 = 0;
    _T3IE = 1;              //Enable Interrupt
    _T3IF = 0;              //Clear Interrupt flag
    _T3IP = 4;              //Priority
    OC3CON1bits.OCTSEL = 0b001;     //Select Timer3 to be timer source
    OC3CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b01101;  //Select Timer3 as synchronization source
    
    OC3R = PR3/2.0;         //Duty Cycle
}

void startDrive(unsigned int direction){

        if(direction == FORWARD){         //drive forward
             _RA2 = 0;
             _RA1 = 0;

        }
        else{                       //drive reverse
            _RA2 = 1;
            _RA1 = 1;
        }
       if( T3CONbits.TON == 0) T3CONbits.TON = 1;           //enable Timer3
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

       if( T3CONbits.TON == 0) T3CONbits.TON = 1;           //enable Timer3
}

void stop(){
    T3CONbits.TON = 0;           //disable Timer3
    _RA1 = 0;
    _RA2 = 0;
}

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
#endif	/* MOTORS_H */

