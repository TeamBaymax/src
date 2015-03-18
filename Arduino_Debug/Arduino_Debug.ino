#include <Wire.h> 
#include <math.h>
byte r1Value;
byte r2Value;
int camera_number;
int x1, x2, y1, y2;

void setup() 
{ 
  r1Value = 0;
  r2Value = 0;
 Wire.begin(0x04);                // join i2c bus with address #2 
 camera_number = 1;
 Wire.onRequest(requestEvent); // register event 
 Wire.onReceive(receiveEvent);
 Serial.begin(115200);           // start serial for output 
} 
volatile char c[2] = {0, 0};
void loop() 
{ 
  float r = 0;
  float theta = 0;
  float phi1 = 0*PI/180.0;
  float phi2 = 0*PI/180.0;
  float d = 3;
  stero_vision(d, phi1, x1, phi2, x2, &theta, &r);
  
  Serial.print("  x1 = ");
  Serial.print(x1,DEC);
  Serial.print("  y1 = ");
  Serial.print(y1,DEC);
  Serial.print("  x2 = ");
  Serial.print(x2,DEC);
  Serial.print("  y2 = ");
  Serial.print(y2,DEC);
  Serial.print("  R =  ");
  Serial.print(r,DEC);
  Serial.print("  theta = ");
  Serial.println(theta*180.0/PI,DEC);
} 


void receiveEvent(int n)
{
  switch(camera_number) {
    case 1:
      x1 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      y1 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      camera_number = 2;
      break;
    case 2:
      x2 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      y2 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      camera_number =1;
      break;
  } 
}
// function that executes whenever data is received from master 
// this function is registered as an event, see setup() 
void requestEvent() 
{
 Wire.write(0x03); 
}

void stero_vision(float d, float phi1, float x1, float phi2, float x2, float* theta, float* r)
{
  // linearly interpolate gamma1 and gamma2
  float gamma1, gamma2;
  gamma1 = (x1-512.0)/1024.0 * 33.0/180.0*PI;
  gamma2 = (x2-512.0)/1024.0 * 33.0/180.0*PI;
  
  // calculate distance from camera to features r1 and r2
  float r1 = sin(PI/2.0-(phi2-gamma2))/(sin(PI-(PI/2.0-(phi1+gamma1) + (PI/2.0-(phi2-gamma2))))/d);
  float r2 = sin(PI/2.0-(phi1+gamma1))/(sin(PI-(PI/2.0-(phi1+gamma1) + (PI/2.0-(phi2-gamma2))))/d);
  
  // find r and theta
  *r = pow(pow(r1,2.0) + pow((d/2.0),2.0) - 2.0*r1*d/2.0*cos(PI/2.0-(phi1+gamma1)),0.5);
  float rad = *r;
  *theta = acos(pow(r1,2.0)-pow(rad,2.0)-pow((d/2.0),2.0)/(-2.0*rad*d/2.0)) - PI;
  
}
