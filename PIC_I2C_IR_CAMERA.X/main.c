#include "p24F16KA301.h"
#include "i2c1_helper.h"
#include "i2c2_helper.h"


_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided

void DelayuSec(unsigned int s);
char IR1sensorAddress = 0xB0;
char IR2sensorAddress = 0xB0;
char arduinoAddress = 0x04<<1;

int main(void)
{
    //Configure pin 2 and 6 for outputs
    _TRISA0 = 0;
    _RA0 = 0;

    i2c1_init(100);
    i2c2_init(100);

    ir1_init();
    ir2_init();

    int x1,x2;
    int y1,y2;
    while(1)
    {
        x1 = y1 = x2 = y2 = 0;
        ir1_request(&x1, &y1);
        ir2_request(&x2, &y2);

        char* xptr1 = &x1;
        char* yptr1 = &y1;

        char* xptr2 = &x2;
        char* yptr2 = &y2;

        I2C2write4bytes(arduinoAddress, xptr1[1], xptr1[0], yptr1[1], yptr1[0]);
        I2C2write4bytes(arduinoAddress, xptr2[1], xptr2[0], yptr2[1], yptr2[0]);

        DelayuSec(100000);
    }
    							//end of program
}

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