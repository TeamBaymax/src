/*
 * File:   delay.h
 * Author: ecestudent
 *
 * Created on March 14, 2015, 11:17 AM
 */

#ifndef DELAY_H
#define	DELAY_H

/* this function delays s number of clock cycles that it receives in
 as input.  It assumes use of the 8 MHz RC occilator*/
void DelayuSec(unsigned int s)
{
    int k = 0;
    while(k < s)
        k++;
}

/* This function delays s number of milliseconds.  It assumes the use of the 8MHz RC
 * Oscillator
 */
void Delay(int s)
{
    int k = 0;
    while(k < s)
    {
        DelayuSec(667);
        k++;  // 667 is equivalent to 0.001 seconds
    }
}

#endif	/* DELAY_H */

