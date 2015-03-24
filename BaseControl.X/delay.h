/* 
 * File:   delay.h
 * Author: ecestudent
 *
 * Created on March 14, 2015, 11:17 AM
 */

#ifndef DELAY_H
#define	DELAY_H

void DelayuSec(unsigned int s)
{
    int k = 0;
    while(k < s)
        k++;
}

#endif	/* DELAY_H */

