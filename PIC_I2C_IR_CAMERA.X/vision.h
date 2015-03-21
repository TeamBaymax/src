/* 
 * File:   vision.h
 * Author: ecestudent
 *
 * Created on March 20, 2015, 6:03 PM
 */



#ifndef VISION_H
#define	VISION_H




#include <math.h>
#include "infrared.h"
#define PI = 3.14159265359
#define X1_OFFSET 567 // These are the values when the robot is looking directly at
#define X2_OFFSET 514 // a light in the corner from the center of the ring
#define Y1_OFFSET 458
#define Y2_OFFSET 514

#define PHI1 4.55207
#define PHI2 4.55207
#define D 5



/** 
 * vision_setup a function that initializes the i2c communication with the ir
 * cameras and sends the appropriate configuration to them.  This must be run
 * before see_beaon() will work
 */
void vision_setup()
{
    // start up i2c
    i2c1_init(100);
    i2c2_init(100);

    // initialize cameras
    ir1_init();
    ir2_init();
}

/**
 * see_beacon a function that uses the IR cameras to give a distance and bearing
 * measurement to the beacon from the robot  Requires that vision_setup be run before
 * @see vision_setup()
 * @param theta a pointer to the returned bearing measurement (radians).
 * @param r a pointer to the returned distance measurement (inches)
 * @return 0 if unable to see.  'm' if using only one camera, 's' if using both
 */
char see_beacon(float* theta, float* r)
{
    // request new values from cameras
    int x1,y1,x2,y2;
    x1 = y1 = x2 = y2 = 0;
    ir1_request(&x1, &y1);
    ir2_request(&x2, &y2);
    char flag;

    // check results
    if(x1 >= 1023 && x2>=1023) // unable to see anything
    {
        flag = 0;
    }
    else
    {
        if(x1 >= 1023 || x2>=1023) // only able to see with one eye
        {
          mono_vision(x1,y1, x2, y2, &theta, &r);
          flag = 'm';
        }
        else
        {
          stereo_vision(x1, x2, &theta, &r);
          flag = 's';
        }
    }
}

void stereo_vision(float x1, float x2, float* theta, float* r)
{
      // linearly interpolate gamma1 and gamma2
      float gamma1, gamma2;
      gamma1 = (x1-X1_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
      gamma2 = (x2-X2_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;

      // calculate distance from camera to features r1 and r2
      float r1 = sin(PI/2.0-PHI2-gamma2)/(sin(PHI1+gamma1+PHI2+gamma2)/D);
      float r2 = sin(PI/2.0-(PHI1+gamma1))/(sin(PHI1+gamma1+PHI2+gamma2)/D);

      // find r and theta
      *r = pow(pow(r1,2.0)+pow(d/2.0,2.0)-2.0*r1*(d/2.0)*cos(PI/2.0-PHI1-gamma1),1/2.0);
      float rad = *r;
      *theta = asin(r1*(sin(PI/2.0-PHI1-gamma1)/rad)) - PI/2.0; //stuck always negative, due to range of asin()
      if(r1>r2)
      {
        float th = *theta;
        *theta = -th;
  }

}

void mono_vision(float x1, float y1,float x2, float y2, float* theta, float* r)
{
   float beta, x, y;
   if(x1 >= 1023) // can't see with left eye
   {
     x = x2;
     y = y2;
     beta = (y2-Y2_OFFSET);
     *r = 2.4*tan(beta);
     *theta = (x2-X2_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
   }
   else if(x2 >= 1023){ // can't see with right eye
     x = x1;
     y = y1;
     beta = (y1-Y1_OFFSET);
     *r = 2.4*tan(beta);
     *theta = (x1-X1_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
   }
   else
   {
     return; // don't do anything, you can see with both eyes
   }

}

#endif	/* VISION_H */

