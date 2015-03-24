/* 
 * File:   i2c_helper.h
 * Author: ecestudent
 *
 * Created on March 14, 2015, 10:43 AM
 */

#ifndef I2C1_HELPER_H
#define	I2C1_HELPER_H
#include "p24F16KA301.h"
#include "delay.h"

//function initiates I2C1 module to baud rate BRG
void i2c1_init(int BRG)
{
   int temp;

   // I2CBRG = 194 for 10Mhz OSCI with PPL with 100kHz I2C clock
   I2C1BRG = BRG;
   I2C1CONbits.I2CEN = 0;	// Disable I2C Mode
   I2C1CONbits.DISSLW = 1;	// Disable slew rate control
   IFS1bits.MI2C1IF = 0;	 // Clear Interrupt
   I2C1CONbits.I2CEN = 1;	// Enable I2C Mode
   temp = I2C1RCV;	 // read buffer to clear buffer full
   reset_i2c1_bus();	 // set bus to idle
}

//function iniates a start condition on bus
void i2c1_start(void)
{
   int x = 0;
   I2C1CONbits.ACKDT = 0;	//Reset any previous Ack
   DelayuSec(10);
   I2C1CONbits.SEN = 1;	//Initiate Start condition
   Nop();

   //the hardware will automatically clear Start Bit
   //wait for automatic clear before proceding
   while (I2C1CONbits.SEN)
   {
      DelayuSec(1);
      x++;
      if (x > 20)
      break;
   }
   DelayuSec(2);
}

void i2c1_restart(void)
{
   int x = 0;

   I2C1CONbits.RSEN = 1;	//Initiate restart condition
   Nop();

   //the hardware will automatically clear restart bit
   //wait for automatic clear before proceding
   while (I2C1CONbits.RSEN)
   {
      DelayuSec(1);
      x++;
      if (x > 20)	break;
   }

   DelayuSec(2);
}

//Resets the I2C bus to Idle
void reset_i2c1_bus(void)
{
   int x = 0;

   //initiate stop bit
   I2C1CONbits.PEN = 1;

   //wait for hardware clear of stop bit
   while (I2C1CONbits.PEN)
   {
      DelayuSec(1);
      x ++;
      if (x > 20) break;
   }
   I2C1CONbits.RCEN = 0;
   IFS1bits.MI2C1IF = 0; // Clear Interrupt
   I2C1STATbits.IWCOL = 0;
   I2C1STATbits.BCL = 0;
   DelayuSec(10);
}

//basic I2C byte send
char send_i2c1_byte(int data)
{
   int i;

   while (I2C1STATbits.TBF) { }
   IFS1bits.MI2C1IF = 0; // Clear Interrupt
   I2C1TRN = data; // load the outgoing data byte

   // wait for transmission
   for (i=0; i<500; i++)
   {
      if (!I2C1STATbits.TRSTAT) break;
      DelayuSec(1);

      }
      if (i == 500) {
      return(1);
   }

   // Check for NO_ACK from slave, abort if not found
   if (I2C1STATbits.ACKSTAT == 1)
   {
      reset_i2c1_bus();
      return(1);
   }

   DelayuSec(1);
   return(0);
}

//function reads data, returns the read data, no ack
char i2c1_read(void)
{
   int i = 0;
   char data = 0;

   //set I2C module to receive
   I2C1CONbits.RCEN = 1;

   //if no response, break
   while (!I2C1STATbits.RBF)
   {
      i++;
      if (i > 2000) break;
   }

   //get data from I2CRCV register
   data = I2C1RCV;

   //return data
   return data;
}

//function reads data, returns the read data, with ack
char i2c1_read_ack(void)	//does not reset bus!!!
{
   int i = 0;
   char data = 0;

   //set I2C module to receive
   I2C1CONbits.RCEN = 1;

   //if no response, break
   while (!I2C1STATbits.RBF)
   {
      i++;
      if (i > 2000) break;
   }

   //get data from I2CRCV register
   data = I2C1RCV;

   //set ACK to high
   I2C1CONbits.ACKEN = 1;

   //wait before exiting
   DelayuSec(1);

   //return data
   return data;
}

void I2C1write(char addr, char value)
{
   i2c1_start();
   send_i2c1_byte(addr);
   send_i2c1_byte(value);
   reset_i2c1_bus();
}

void I2C1write2bytes(char addr, char val1, char val2)
{
    i2c1_start();
   send_i2c1_byte(addr);
   send_i2c1_byte(val1);
   send_i2c1_byte(val2);
   reset_i2c1_bus();
}

void I2C1write4bytes(char addr, char val1, char val2, char val3, char val4)
{
    i2c1_start();
   send_i2c1_byte(addr);
   send_i2c1_byte(val1);
   send_i2c1_byte(val2);
   send_i2c1_byte(val3);
   send_i2c1_byte(val4);
   reset_i2c1_bus();
}

void I2C1requestFrom(char addr, char val, char* buffer)
{
//   i2c_start();
//   send_i2c_byte(addr|0);
//   //send_i2c_byte(subaddr);
//   DelayuSec(20);

   i2c1_start();
   send_i2c1_byte(addr | 0x01);
   short i=0;
   for(i=0;i<val-1;i++)
   {
      buffer[i] = i2c1_read_ack();
      DelayuSec(200);
   }
   buffer[val-1] = i2c1_read();

   reset_i2c1_bus();
}

#endif
