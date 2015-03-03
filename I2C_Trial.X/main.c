/* 
 * File:   main.c
 * Author: James Jackson
 *
 * Created on March 2, 2015, 10:12 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <i2c.h>
#include <p24F16KA301.h>

#define USE_AND_OR	// To enable AND_OR mask setting for I2C.
#include <i2c.h>


_FOSCSEL(FNOSC_FRC); // 8 MHz oscillator

// calculate baud rate of I2C
#define Fosc	(8000000) 	// crystal
#define Fcy		(Fosc*4/2)	// w.PLL (Instruction Per Second)
#define Fsck	400000		// 400kHz I2C
#define I2C_BRG	((Fcy/2/Fsck)-1)

/*
 * 
 */
int main(int argc, char** argv) {

    unsigned char SlaveAddress;
    unsigned char *pWrite, *pRead;
    unsigned int i;

    // The data to be written should be end with '\0' if using
    // char MasterputsI2C1(unsigned char *) library function.
    unsigned char tx_data[] = {'P', 'I', 'C', '2', '4', 'F', '\0'};

    unsigned char rx_data[9];	// "MCHP I2C"
    char status;

    // Disable Watch Dog Timer
    RCONbits.SWDTEN = 0;
    // for LED
    ODCAbits.ODA6 = 0;
    TRISAbits.TRISA6 = 0;

    pWrite = tx_data;
    pRead = rx_data;

    //Enable channel
    OpenI2C1( I2C_ON, I2C_BRG );

    SlaveAddress = 0x61;	/* 0b1100001 Slave MCU address (customized)
                                 * 4 msb is 1100, customized as slave MCU
                                 * 3 lsb is 001, customized as the first slave MCU
                                 * We need to change this to the IR sensor
                                 */

    while (1) {
            while (1) {
            /*
             *	Write one byte to slave MCU. [1]
             */
                    StartI2C1();	//Send the Start Bit
                    IdleI2C1();		//Wait to complete

                    MasterWriteI2C1((SlaveAddress<<1)|0); //transmit write command
                    IdleI2C1();		//Wait to complete
                    //ACKSTAT is 0 when slave acknowledge,
                    //if 1 then slave has not acknowledge the data.
                    if( I2C1STATbits.ACKSTAT ) {
                            StopI2C1();	//Send the Stop condition
                            IdleI2C1();	//Wait to complete
                    } else {
                            break;
                    }
            }
            // write some bytes continuously
            // The data to be written should be end with '\0' if using
            // char MasterputsI2C1(unsigned char *) library function.
            status = MasterputsI2C1(pWrite);

            if (status == -3)
                    while (1); // not sure what this does... it seems like to

            StopI2C1();	//Send the Stop condition
            IdleI2C1();	//Wait to complete
            /*
             *	End writing. [1]
             */

            /*
             * Now Readback several data from slave MCU. [2]
             */

            StartI2C1();	//Send the Start Bit
            IdleI2C1();		//Wait to complete

            MasterWriteI2C1((SlaveAddress<<1)|1); //transmit read command
            IdleI2C1();		//Wait to complete

            // read some bytes back
            status = MastergetsI2C1(9, pRead, 20);		// "MCHP I2C"

            if (status!=0)
                    while(1);

            StopI2C1();	//Send the Stop condition
            IdleI2C1();	//Wait to complete
            /*
             *	End reading several bytes. [3]
             */

            __builtin_btg((unsigned int *)&LATA, 6);

            for (i=0; i<50000; i++);
    }

    return 0;
}

