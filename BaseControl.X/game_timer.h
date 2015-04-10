/*
 * File:   gameTimer.h
 * Author: James
 *
 * Created on March 27, 2015, 3:33 PM
 */

#ifndef GAME_TIMER_H
#define	GAME_TIMER_H

#include <p24F16KA301.h>

float game_timer; // global variable that is the game time in seconds
char round; // the current round of the game 0 is the first 15 seconds, 1-3 are the scoring rounds

void _ISR _T2Interrupt(void){
    _T2IF = 0; // clear interrupt flag
    game_timer = game_timer + 0.02;

    if(game_timer >= 10.0 && game_timer <= 40.0)
        round = 1;
    if(game_timer >= 40.0 && game_timer <= 70.0)
        round = 2;
    if(game_timer >= 70.0 && game_timer <= 100.0)
        round = 3;
}

void game_time_setup()
{
    // enable Timer 2 as game timer
    T2CONbits.TON = 1;
    T2CONbits.TCS = 0;
    T2CONbits.TCKPS = 0b11; // post scale by 256 (15625 cycles per second)
    PR2 = 312; // results in 50 Hz
    TMR2 = 0;
    _T2IE = 1;
    _T2IF = 0;

    game_timer = 0.0;
    round = 0;
}



#endif	/* GAMETIMER_H */