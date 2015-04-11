/* 
 * File:   State.h
 * Author: Emily
 *
 * Created on March 19, 2015, 11:54 AM
 */

#ifndef STATE_H
#define	STATE_H

#include <p24F16KA301.h>
#include "vision.h"
#include "motors.h"
#include "game_timer.h"
#include "servo.h"

#define RIGHT 0
#define LEFT 1
#define FORWARD 0
#define REVERSE 1

#define LOSTBEACON 3
#define OUTOFWINDOW 4

#define CENTER 33.5

typedef enum{
    search, aligntheta, aligndist, //aligned, //begin
    //at_center,
    wait,                          //oriented
    searchgarage,
    collect,//aligncollect, gocollect, collect,          //collecting
    //searchgoal, aligngoal, distgoal,
    shoot,                //scoring
    aimturret,
    end,                         //end states - end: gets out of dispensing zone
    halt
} State;

typedef enum{
    locating, // beginning of game, locating garage and orienting self
    loading,  // getting new balls
    scoring,  // finding goals and shooting
    finishing,//lsat 5 seconds of game - no goals active, get away from walls and garage
} Period;

char circleSearch(char dir, VisionFlag flag){
    if(flag)
    {
        PR3 = 500;
        stop();
        return 1;
    }
    else
    {
        PR3 = 500;
        startTurn(dir);
        return 0;
    }

}

char alignTheta(VisionFlag flag){
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
        return LOSTBEACON;
    }
    return 0;

}

char alignDist(float r_set, VisionFlag flag){
    if(flag)
    {
        if(theta > 2.0*theta_window || theta < -2.0*theta_window)
        {
            // we have traveled out of the window
            stop();
            return OUTOFWINDOW;
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
               return LOSTBEACON;
               // should never enter this state
           }
        }
    }
    else
    {
        // error state
        stop();
        return LOSTBEACON;
    }
}

<<<<<<< HEAD
char openloopDist(float r_set, char direction, VisionFlag flag){
=======
char aim(float window, char flag)
{
    char status;
    if(flag)
    {
        theta_window = window; // reduce the windo
        PR3 = 1200; // slow down motors
        status = alignTheta(flag);
        if(status == 1) // aligned
        {
            theta_window = 3.0*PI/180.0; // open up window
            PR3 = 500; // speed up motors again
        }
        return status;
    }
    else
    {
        theta_window = 3.0*PI/180.0; // open up window
        PR3 = 500; // speed up motors again
        return LOSTBEACON;
    }
}

char openloopDist(float r_set, char direction, char flag){
>>>>>>> master
    straight(r_set, direction);
    return 1;
}

char openloopTurn(float theta_set, char direction, VisionFlag flag){
    turn(theta_set, direction);
    return 1;
}

loadBalls(int n){
    int i=0;
    while(i<n){
        moveServo();
        balls++;
        i++;
    }
}

char shootBalls(char flag){
    //spinShooter();
    if(balls > 0) // we still have balls in the hopper
    {
        if(flag){
            // we see the beacon
            advanceBall(); // shoot a ball
            balls--;
            return 0; // not finished
        }
        else{
            return LOSTBEACON;
        }
    }
    else
    {
        return 1; // finished
    }
    //stopShooter();

}

char searchGarage(char direction, char flag)
{

    if(flag) // found beacon
    {
        if(getAngle() > 330.0 || getAngle() < 30.0){ // in range of the garage
            PR3 = 500;
            stop();
            return 1; // return found garage
        }else return 0;
    }else{
    PR3 = 500;
    startTurn(direction);
    return 0;
    }
}

char searchGoal(char direction, char flag)
{

    if(flag) // found beacon
    {
        if(getAngle() < 330.0 && getAngle() > 30.0){ // in range of the goals
            stop();
            return 1; // return found garage
        }
        else
        {
            return 0; //keep going
        }
    }else{
    startTurn(direction);
    return 0;
    }
}

#endif	/* STATE_H */

