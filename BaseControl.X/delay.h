/* 
 * File:   delay.h
 * Author: ecestudent
 *
 * Created on March 14, 2015, 11:17 AM
 */

#ifndef DELAY_H
#define	DELAY_H

/* this function delays roughly the number of microseconds that it receives in
 as input.  It assumes use of the 8 MHz RC occilator*/
void DelayuSec(unsigned int s)
{
    int k = 0;
    while(k < s)
        k++;
}

#endif	/* DELAY_H */

