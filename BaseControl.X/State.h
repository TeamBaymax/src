/* 
 * File:   State.h
 * Author: Emily
 *
 * Created on March 19, 2015, 11:54 AM
 */

#ifndef STATE_H
#define	STATE_H

typedef enum{
    start, align0, forward0, reverse0,  //begin
    at_center,                          //oriented
    //add collector states here         //collecting
    scan, align2, shoot,                //scoring
} State;

#endif	/* STATE_H */

