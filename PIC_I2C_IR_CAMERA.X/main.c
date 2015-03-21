#include "p24F16KA301.h"
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#include "vision.h"
#include "infrared.h"


_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided

//char arduinoAddress = 0x04<<1;

float r,theta;
float phi1,phi2;
float d;
int x1, x2, y1, y2;

void setup()
{
  r = 0;
  theta = 0;
  phi1 = (PI/2.0-acos(2.5/31.5));
  phi2 = (PI/2.0-acos(2.5/31.5));
  d = 5;
  x1 = x2 = y1 = y2 = 1023;
}


int main(void)
{
    //Configure pin 2 and 6 for outputs
    _TRISA0 = 0;
    _RA0 = 0;

    i2c1_init(100);
    i2c2_init(100);

    ir1_init();
    ir2_init();

    while(1)
    {
        
        
    }
    							//end of program
}