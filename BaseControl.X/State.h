/* 
 * File:   State.h
 * Author: Emily
 *
 * Created on March 19, 2015, 11:54 AM
 */

#ifndef STATE_H
#define	STATE_H

#include "vision.h"
#include "motors.h"

#define RIGHT 1
#define LEFT 0
#define FORWARD 1
#define BACKWARD 0

#define CENTER 33.5

//char beacon //defined in vision.h -> indicator of if a beacon is on
State state;

typedef enum{
    search, aligntheta, alignR, aligned, forward0, reverse0,  //begin
    at_center,                          //oriented
    align1, forward1, reverse1,          //collecting
    scan, align2, shoot,                //scoring
    end, finish                         //end states - end: gets out of dispensing zone, finish: stop
} State;

circleSearch(char dir)
{
    if(!beacon)
    {
        startTurn(dir);
    }
    else
    {
        state = aligntheta;
    }

}

alignTheta()
{
    if(beacon)
    {
        if( theta < theta_window && theta > -1.0*theta_window)
        {
            state = alignR;
        }
        else if(theta > theta_window)
        {
            startTurn(LEFT);
        }
        else if(theta < -1.0*theta_window)
        {
            startTurn(RIGHT);
        }
        else
        {
            //should never enter this state
            state = search;
        }
    }
    else
    {
        state = search;
    }
}

alignDist(float r_set)
{
    if(beacon)
    {
       if(r > (r_set-r_window) && r < (r_set+r_window))
       {
         state = aligned;
       }
       else if (r < (r_set - r_window))
       {
           startDrive(BACKWARD);
       }
       else if (r > (r_window + r_set))
       {
           startDrive(FORWARD);
       }
       else
       {
           // should never enter this state
           state = search;
       }
    }
    else
    {
        state = search;
    }

}

findGarage()
{
    
}




#endif	/* STATE_H */

