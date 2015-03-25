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

#define PHI1 0.0794
#define PHI2 0.0794
#define D 5

#define DEBUG 1

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
}

/**
 * see_beacon a function that uses the IR cameras to give a distance and bearing
 * measurement to the beacon from the robot  Requires that vision_setup be run before
 * @see vision_setup()
 * @param theta a pointer to the returned bearing measurement (radians).
 * @param r a pointer to the returned distance measurement (inches)
 * @return 0 if unable to see.  'm' if using only one camera, 's' if using both
 */
//char see_beacon(float* theta, float* r)
//{
//    x1 = y1 = x2 = y2 = 0;
//    ir1_request(&x1, &y1);
//    ir2_request(&x2, &y2);
//
//    int theta_int = (int)*theta;
//    int r_int = (int)*r;
//
//    char* x1ptr = &x2;
//    char* y1ptr = &y1;
//    char* x2ptr = &x2;
//    char* y2ptr = &y2;
//
//    //char* thptr = &theta_int;
//    //char* rptr = &r_int;
//
//    I2C2write4bytes(arduinoAddress, x1ptr[1], x1ptr[0], y1ptr[1], y1ptr[0]);
//    I2C2write4bytes(arduinoAddress, x2ptr[1], x2ptr[0], y2ptr[1], y2ptr[0]);
//    //I2C2write4bytes(arduinoAddress, thptr[1], thptr[0], rptr[1], rptr[0]);
//
//    char flag;
//
////    if(x1 == 0 || x2 ==0)
////    {// error
////        flag = 3;
////        return flag;
////    }
////    else if(x1 < 1023 || x2 < 1023) // saw somehtihng
////    {
////        if(x1 >= 1023 || x2>=1023) // only able to see with one eye
////        {
////          mono_vision(x1,y1, x2, y2, &theta, &r);
////          flag = 1;
////        }
////        else
////        {
////          stereo_vision(x1, x2, &theta, &r);
////          flag = 2;
////        }
////    }
////    else // didn't see anything
////    {
////         flag = 0;
////    }
//    return flag;
//}

void stereo_vision(float x1, float x2, float* theta, float* r)
{
    char arduinoAddress = 0x04<<1;
      int y1 = 0;
      int y2 = 0;
//      char* xptr1 = &x1;
//      char* yptr1 = &y1;
//      char* xptr2 = &x2;
//      char* yptr2 = &y2;
//      I2C2write4bytes(arduinoAddress, xptr1[1], xptr1[0], yptr1[1], yptr1[0]);
//      I2C2write4bytes(arduinoAddress, xptr2[1], xptr2[0], yptr2[1], yptr2[0]);

      // linearly interpolate gamma1 and gamma2
      float gamma1, gamma2;
      gamma1 = (x1-X1_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
      gamma2 = (x2-X2_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;

      //debug_float(gamma1);
      //debug_float(gamma2);

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

