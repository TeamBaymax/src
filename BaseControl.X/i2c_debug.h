/* 
 * File:   i2c_debug.h
 * Author: gary
 *
 * Created on March 24, 2015, 5:09 PM
 */

#ifndef I2C_DEBUG_H
#define	I2C_DEBUG_H

#include "i2c2_helper.h"

char arduinoAddress = 0x04<<1;

void debug_2_ints(int x1, int y1)
{

    int val1 = x1;
    int val2 = y1;
    char* ptr1 = &val1;
    char* ptr2 = &val2;

     I2C2write4bytes(arduinoAddress, ptr1[1], ptr1[0], ptr2[1], ptr2[0]);
}

void debug_float(float val)
{
    char* ptr1 = &val;
    I2C2write4bytes(arduinoAddress, ptr1[3], ptr1[2], ptr1[1], ptr1[0]);
}

#endif	/* I2C_DEBUG_H */

