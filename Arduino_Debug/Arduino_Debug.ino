#include <Wire.h> 
#include <math.h>
#define X1_OFFSET 567 // These are the values when the robot is looking directly at 
#define X2_OFFSET 514 // a light in the corner from the center of the ring
#define Y1_OFFSET 458
#define Y2_OFFSET 514


byte r1Value;
byte r2Value;
int camera_number;
int x1, x2, y1, y2;
float r1, r2;

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
  float phi1 = (PI/2.0-acos(2.5/31.5));
  float phi2 = (PI/2.0-acos(2.5/31.5));
  float d = 5;
  if(x1 >= 1023 && x2>=1023) // unable to see anything
  {
    Serial.println("unable to see");
  }
  else
  {
    if(x1 >= 1023 || x2>=1023) // only able to see with one eye
    {
      mono_vision(d, phi1, x1, phi2, x2, &theta, &r);
    }
    else
    {
      stereo_vision(d, phi1, x1, phi2, x2, &theta, &r);
    }
    
    
    Serial.print("\t  x1 = ");
    Serial.print(x1,DEC);
    Serial.print("\t  y1 = ");
    Serial.print(y1,DEC);
    Serial.print("\t  x2 = ");
    Serial.print(x2,DEC);
    Serial.print("\t  y2 = ");
    Serial.print(y2,DEC);
    Serial.print("\t  R =  ");
    Serial.print(r,DEC);
    Serial.print("\t  theta = ");
    Serial.print(theta*180.0/PI,DEC);
    Serial.print("\t  r1 =  ");
    Serial.print(r1,DEC);
    Serial.print("\t  r2 = ");
    Serial.println(r2,DEC);
  }
} 


void receiveEvent(int n)
{
  //switch(camera_number) {
    //case 1:
      x1 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      y1 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      //camera_number = 2;
      //break;
    //case 2:
      //x2 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      //y2 = (Wire.read() | Wire.read() << 8) & 0b1111111111;
      //camera_number =1;
      //break;
  } 
}
// function that executes whenever data is received from master 
// this function is registered as an event, see setup() 
void requestEvent() 
{
 Wire.write(0x03); 
}

void mono_vision(float d, float phi1, float x1, float phi2, float x2, float* theta, float* r)
{
   float beta, x, y;
   if(x1 >= 1023) // can't see with left eye
   {
     x = x2;
     y = y2;
     beta = (y2-Y2_OFFSET);
     *r = 2.4*tan(beta);
     *theta = (x2-X2_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
     r1 = 0;
     r2 = 0;  
     
   }
   else if(x2 >= 1023){ // can't see with right eye
     x = x1;
     y = y1;
     beta = (y1-Y1_OFFSET);
     *r = 2.4*tan(beta);
     *theta = (x1-X1_OFFSET)/1023.0 * 33.0/360.0*2.0*PI; 
     r1 = 0;
     r2 = 0;    
   }
   else
   {
     return; // don't do anything, you can see with both eyes
   }
   
}

void stereo_vision(float d, float phi1, float x1, float phi2, float x2, float* theta, float* r)
{
  // linearly interpolate gamma1 and gamma2
  float gamma1, gamma2;
  gamma1 = (x1-X1_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
  gamma2 = (x2-X2_OFFSET)/1023.0 * 33.0/360.0*2.0*PI;
//  Serial.print("gamma1 = ");
//  Serial.print(gamma1,DEC);
//  Serial.print("\t gamma2 = ");
//  Serial.println(gamma2,DEC); 
  // calculate distance from camera to features r1 and r2
  r1 = sin(PI/2.0-phi2-gamma2)/(sin(phi1+gamma1+phi2+gamma2)/d);
  r2 = sin(PI/2.0-(phi1+gamma1))/(sin(phi1+gamma1+phi2+gamma2)/d);
  
  // find r and theta
  *r = pow(pow(r1,2.0)+pow(d/2.0,2.0)-2.0*r1*(d/2.0)*cos(PI/2.0-phi1-gamma1),1/2.0);
  float rad = *r;
  *theta = asin(r1*(sin(PI/2.0-phi1-gamma1)/rad)) - PI/2.0; //stuck always positive, due to range of asin()
  if(r1>r2)
  {
    float th = *theta;
    *theta = -th;
  }
  
}
