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
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#define PI 3.14159265359
#define X1_OFFSET 572 // These are the values when the robot is looking directly at
#define X2_OFFSET 467 // a light in the corner from the center of the ring
#define Y1_OFFSET 514
#define Y2_OFFSET 521

#define PHI1 0.0794
#define PHI2 0.0794
#define D 5

#define DEBUG 1


char beacon; // indicator of whether or not we see the beacon
volatile int x1,x2,y1,y2; //coordinates of image in camrea view
volatile float theta, r; // angle and distance from IR beacon
volatile float r_window = 2.0; // error allowed when aligning theta
volatile float theta_window = 3.0*PI/180.0; // error allowed when aligning theta



/**
 * vision_setup a function that initializes the i2c communication with the ir
 * cameras and sends the appropriate configuration to them.  This must be run
 * before see_beaon() will work
 */
void vision_setup()
{
    i2c1_init(100);
    i2c2_init(100);

    ir1_init();
    ir2_init();

    beacon = 0;
}

/**
 * see_beacon a function that uses the IR cameras to give a distance and bearing
 * measurement to the beacon from the robot  Requires that vision_setup be run before
 * @see vision_setup()
 * @param theta a pointer to the returned bearing measurement (radians).
 * @param r a pointer to the returned distance measurement (inches)
 * @return 0 if unable to see.  1 if using only one camera, 2 if using both, 255 if in error
 */
char see_beacon(float* thetaptr, float* rptr)
{
    x1 = y1 = x2 = y2 = 0;
    ir1_request(&x1, &y1);
    ir2_request(&x2, &y2);
    
    char flag;
    
    if(x1 == 0 || x2 == 0)
    {
        // in error
        flag = 255;
    }
    else if(x1 < 1023 && x2 < 1023) // seeing with both eyes
    {
        stereo_vision(x1, x2, thetaptr, rptr);
        flag = 2;
    }
    else if(x1 < 1023 || x2 < 1023) // seeing with one eye
    {
        mono_vision(x1,y1,x2,y2,thetaptr,rptr);
        flag = 1;
    }
    else
    {
        // not seeing
        flag = 0;
    }
    return flag;

}

void stereo_vision(int x1, int x2, float* theta, float* r)
{
      // linearly interpolate gamma1 and gamma2
      float gamma1, gamma2;
      gamma1 = (x1-X1_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
      gamma2 = (x2-X2_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;

      // calculate distance from camera to features r1 and r2
      float r1 = fabs(sin(PI/2.0-PHI2-gamma2)/(sin(PHI1+gamma1+PHI2+gamma2)/D));
      float r2 = fabs(sin(PI/2.0-(PHI1+gamma1))/(sin(PHI1+gamma1+PHI2+gamma2)/D));

      // find r and theta
      *r = pow(pow(r1,2.0)+pow(D/2.0,2.0)-2.0*r1*(D/2.0)*cos(PI/2.0-PHI1-gamma1),1/2.0);
      float rad = *r;
      *theta = asin(r1*(sin(PI/2.0-PHI1-gamma1)/rad)) - PI/2.0; //stuck always negative, due to range of asin()
      float th = *theta;
      if(r1>r2)
      {
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

