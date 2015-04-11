/*
 * File:   servo.h
 * Author: gary
 *
 * Created on March 27, 2015, 3:10 PM
 */

#ifndef SERVO_H
#define	SERVO_H



#include <p24F16KA301.h>
#include "game_timer.h"

volatile int balls;

/**
 * the stepper utilizes timer 2, which is the timer used for the game timer.
 * In order to move the servo the
 */
void servoSetup()
{

    // Output 2
    OC2CON1bits.OCTSEL = 0b000;     //Select Timer2 to be timer source
    OC2CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC2CON2bits.SYNCSEL = 0b01100;  //Select Timer2 as synchronization source
    OC2R = 16;
    OC2R = 0;
}

void moveServo()
{
    OC2R = 37;                      //Set Dutycycle 0.05*PR2
    float time = game_timer;
    while(game_timer < time + 0.5)
        ;
    OC2R = 16;                      //Set Duty
    while(game_timer < time + 1.0)
        ;
    OC2R = 0;                       //off
}





#endif	/* SERVO_H */

