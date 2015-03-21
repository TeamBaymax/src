/* 
 * File:   infrared.h
 * Author: ecestudent
 *
 * Created on March 20, 2015, 6:13 PM
 */

#ifndef INFRARED_H
#define	INFRARED_H

#include "i2c1_helper.h"
#include "i2c2_helper.h"

char IR1sensorAddress = 0xB0;
char IR2sensorAddress = 0xB0;

void ir1_init()
{
    I2C1write2bytes(IR1sensorAddress, 0x30, 0x01); DelayuSec(10000);
    I2C1write2bytes(IR1sensorAddress, 0x30, 0x08); DelayuSec(10000);
    I2C1write2bytes(IR1sensorAddress, 0x06, 0x90); DelayuSec(10000);
    I2C1write2bytes(IR1sensorAddress, 0x08, 0xC0); DelayuSec(10000);
    I2C1write2bytes(IR1sensorAddress, 0x1A, 0x40); DelayuSec(10000);
    I2C1write2bytes(IR1sensorAddress, 0x33, 0x33); DelayuSec(10000);
}

void ir1_request(char* xptr, char* yptr)
{
    I2C1write(IR1sensorAddress, 0x36);
    char data[4] = {0, 0, 0, 0};
    I2C1requestFrom(IR1sensorAddress, 4, &data);

    xptr[1] = data[1];
    yptr[1] = data[2];
    char s = data[3];
    xptr[0] = (s & 0x30) >> 4;
    yptr[0] = (s & 0xC0) >> 6;

    //I2Cwrite(arduinoAddress, (s & 0x30));
}

void ir2_init()
{
    I2C2write2bytes(IR2sensorAddress, 0x30, 0x01); DelayuSec(10000);
    I2C2write2bytes(IR2sensorAddress, 0x30, 0x08); DelayuSec(10000);
    I2C2write2bytes(IR2sensorAddress, 0x06, 0x90); DelayuSec(10000);
    I2C2write2bytes(IR2sensorAddress, 0x08, 0xC0); DelayuSec(10000);
    I2C2write2bytes(IR2sensorAddress, 0x1A, 0x40); DelayuSec(10000);
    I2C2write2bytes(IR2sensorAddress, 0x33, 0x33); DelayuSec(10000);
}

void ir2_request(char* xptr, char* yptr)
{
    I2C2write(IR1sensorAddress, 0x36);
    char data[4] = {0, 0, 0, 0};
    I2C2requestFrom(IR2sensorAddress, 4, &data);

    xptr[1] = data[1];
    yptr[1] = data[2];
    char s = data[3];
    xptr[0] = (s & 0x30) >> 4;
    yptr[0] = (s & 0xC0) >> 6;

    //I2Cwrite(arduinoAddress, (s & 0x30));
}

#endif	/* INFRARED_H */

