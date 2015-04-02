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

typedef enum{
    search, aligntheta, aligndist, //aligned, //begin
    at_center, wait,                          //oriented
    searchgarage, aligncollect, gocollect, collect,          //collecting
    searchgoal, aligngoal, distgoal, shoot,                //scoring
    end, finish                         //end states - end: gets out of dispensing zone, finish: stop
} State;

typedef enum{
    locating, // beginning of game, locating garage and orienting self
    loading,  // getting new balls
    scoring,  // finding goals and shooting
    finishing,//lsat 5 seconds of game - no goals active, get away from walls and garage
} Period;

State state;
Period period;

char circleSearch(char dir, char flag){
    if(flag)
    {
        stop();
        return 1;
    }
    else
    {
        startTurn(dir);
        return 0;
    }

}

char alignTheta(char flag){
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
            return 1;
        }
    }
    else
    {
        stop();
        return 255;
    }
    return 0;

}

char alignDist(float r_set, char flag){
    if(flag)
    {
        if(theta > 2.0*theta_window || theta < -2.0*theta_window)
        {
            // we have traveled out of the window
            stop();
            return 254;
            if(state == distgoal) state = aligngoal;
        }
        else
        {

           if(r > (r_set-r_window) && r < (r_set+r_window))
           {
               stop();
               return 1;
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
               return 255;
               // should never enter this state
           }
        }
    }
    else
    {
        // error state
        stop();
        return 255;
    }
}

char openloopDist(float r_set, char direction, char flag){
    straight(r_set, direction);
    return 1;
}

char openloopTurn(float theta_set, char direction, char flag){
    turn(theta_set, direction);
    return 1;
}

char waitUntil(float time){
    if(game_timer > time)
    {
        return 1;
    }
    else return 0;
}

loadBalls(int n){
    int i=0;
    while(i<n){
        moveServo();
        balls++; balls++;
        i++;
    }
}

shootBalls(int n){
    spinShooter();
    int i = 0;
    for(i=0; i<n; i++)
    {
        advanceBall();
    }
    stopShooter();

}

#endif	/* STATE_H */

