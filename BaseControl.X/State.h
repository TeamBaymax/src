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
#include "game_timer.h"
#include "servo.h"

#define RIGHT 0
#define LEFT 1
#define FORWARD 0
#define REVERSE 1

#define CENTER 33.5

//char beacon //defined in vision.h -> indicator of if a beacon is on

typedef enum{
    search, aligntheta, aligndist, //aligned, //begin
    at_center, wait,                          //oriented
    searchgarage, aligncollect, gocollect, load, donecollect,          //collecting
    searchgoal, aligngoal, distgoal, shoot,                //scoring
    end, finish                         //end states - end: gets out of dispensing zone, finish: stop
} State;

typedef enum{
    locating, // beginning of game, locating garage and orienting self
    scoring,   // most of the game, collecting balls and shooting
    finishing,  //lsat 5 seconds of game - no goals active, get away from walls and garage
} Period;

State state;
Period period;

circleSearch(char dir, char flag){
    if(flag)
    {
        stop();
        if(state == search) state = aligntheta;
        else if(state == searchgarage) state = aligncollect;
        else if(state == searchgoal) state = aligngoal;
    }
    else
    {
        startTurn(dir);
    }

}

alignTheta(char flag){
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
            if(state == aligntheta) state = aligndist;
            else if(state == aligncollect) state = gocollect;
            else if(state == aligngoal) state = distgoal;
        }
    }
    else
    {
        stop();
        if(state == aligntheta) state = search;
        else if(state == aligncollect) state = searchgarage;
        else if(state == aligngoal) state = searchgoal;
    }

}

alignDist(float r_set, char flag){
    if(flag)
    {
        if(theta > 2.0*theta_window || theta < -2.0*theta_window)
        {
            // we have traveled out of the window
            stop();
            if(state == aligndist) state = aligntheta;
            else if(state == gocollect) state = aligncollect;
            else if(state == distgoal) state = aligngoal;
        }
        else
        {
           if(r > (r_set-r_window) && r < (r_set+r_window))
           {
             stop();
            if(state == aligndist) state = wait;
            else if(state == gocollect) state = load;
            else if(state == distgoal) state = shoot;
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
               //state = search;
           }
        }
    }
    else
    {
        // error state
        stop();
        if(state == aligndist) state = search;
        else if(state == gocollect) state = searchgarage;
        else if(state == distgoal) state = searchgoal;
    }
}

char waitUntil(float time){
    if(game_timer > time)
    {
        return 1;
    }
    else return 0;
}

findGarage(char flag){

    //turn 90 or something.
    
}

loadBalls(int n){
    int i=0;
    while(i<n){
        moveServo();
        balls++; balls++;
        i++; i++;   //each time this loops it collects 2 balls

    }
    state = donecollect;
}


#endif	/* STATE_H */

