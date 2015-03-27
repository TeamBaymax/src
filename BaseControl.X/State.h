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

#define RIGHT 0
#define LEFT 1
#define FORWARD 0
#define REVERSE 1

#define CENTER 33.5

//char beacon //defined in vision.h -> indicator of if a beacon is on


typedef enum{
    search, aligntheta, aligndist, aligned, forward0, reverse0,  //begin
    at_center, wait,                          //oriented
    align1, forward1, reverse1,          //collecting
    scan, align2, shoot,                //scoring
    end, finish                         //end states - end: gets out of dispensing zone, finish: stop
} State;
State state;

typedef enum{
    locating, // beginning of game, locating garage and orienting self
    scoring   // most of the game, collecting balls and shooting
} Period;

Period period;

circleSearch(char dir, char flag)
{
    if(flag)
    {
        stop();
        state = aligntheta;

    }
    else
    {
        startTurn(dir);
    }

}

alignTheta(char flag)
{
    if(flag)
    {
        if(theta > 1.0*theta_window)
        {
            startTurn(LEFT);
        }
        else if(theta < -1.0*theta_window)
        {
            startTurn(RIGHT);
        }
        else
        {
            stop();
            state = aligndist;
        }
    }
    else
    {
        stop();
        state = search;
    }

}

alignDist(float r_set, char flag)
{
    if(flag)
    {
        if(theta > 1.0*theta_window || theta < -1.0*theta_window)
        {
            // we have traveled out of the window
            stop();
            state = aligntheta;
        }
        else
        {
           if(r > (r_set-r_window) && r < (r_set+r_window))
           {
             stop();
             state = aligned;
           }
           else if (r < (r_set - r_window))
           {
               startDrive(REVERSE);
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
    }
    else
    {
        // error state
        stop();
        state = search;
    }
}

char waitUntil(float time)
{
    if(game_timer > time)
    {
        return 1;
    }
    else return 0;
}

findGarage()
{


    
}




#endif	/* STATE_H */

