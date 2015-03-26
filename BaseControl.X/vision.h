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
#define X1_OFFSET 567 // These are the values when the robot is looking directly at
#define X2_OFFSET 514 // a light in the corner from the center of the ring
#define Y1_OFFSET 458
#define Y2_OFFSET 514

#define PHI1 4.55207
#define PHI2 4.55207
#define D 5

#define DEBUG 1


int x1, x2, y1, y2;

/**
 * vision_setup a function that initializes the i2c communication with the ir
 * cameras and sends the appropriate configuration to them.  This must be run
 * before see_beaon() will work
 */
void vision_setup()
{

    // cycle SDA and SCL Lines to avoid them getting stuck
    int i =0;
    for(i = 0; i<10; i++)
    {
        LATBbits.LATB2 = 1;
        LATBbits.LATB8 = 1;
        LATBbits.LATB9 = 1;
        LATBbits.LATB15 = 1;
        LATBbits.LATB2 = 0;
        LATBbits.LATB8 = 0;
        LATBbits.LATB9 = 0;
        LATBbits.LATB15 = 0;
    }

    i2c1_init(100);
    i2c2_init(100);

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
    x1 = y1 = x2 = y2 = 0;
    ir1_request(&x1, &y1);
    ir2_request(&x2, &y2);

    if(x1 == 0 || x2 == 0) // i2c froze
    {
        vision_setup();
        *theta = PI/2.0;
        *r = 30;
        return 0b0;
    }

    char flag;

    if(x1 == 0 && x2 ==0) // error
        flag = 3;
    if(x1 < 1023 || x2 < 1023) // saw somehtihng
    {
        if(x1 >= 1023 || x2>=1023) // only able to see with one eye
        {
          mono_vision(x1,y1, x2, y2, &theta, &r);
          flag = 1;
        }
        else
        {
          stereo_vision(x1, x2, &theta, &r);
          flag = 2;
        }
    }
    else // didn't see anything
    {
         flag = 0;
    }
    return flag;
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

