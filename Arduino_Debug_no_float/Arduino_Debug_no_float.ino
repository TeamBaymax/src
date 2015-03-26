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
float r_rec, theta_rec;
float r1, r2;
byte flag;

void setup() 
{ 
  flag = 1;
  r1Value = 0;
  r2Value = 0;
  r_rec = 5.5;
  theta_rec = 0;
 Wire.begin(0x04);                // join i2c bus with address #2 
 camera_number = 1;
 Wire.onRequest(requestEvent); // register event 
 Wire.onReceive(receivefloatEvent);
 Serial.begin(115200);           // start serial for output 
} 
volatile char c[2] = {0, 0};
void loop() 
{ 
  float r = 0;
  float theta = 0;
  float phi1 = 0.0794;
  float phi2 = 0.0794;
  float d = 5;
  if(flag)
  {
<<<<<<< HEAD:Arduino_Debug_no_float/Arduino_Debug_no_float.ino
//    if(x1 >= 1023 && x2>=1023) // unable to see anything
//    {
//      Serial.println("unable to see");
//    }
//    else
//    {
      if(x1 >= 1023 || x2>=1023) // only able to see with one eye
      {
        mono_vision(d, phi1, x1, phi2, x2, &theta, &r);
      }
      else
      {
        stereo_vision(d, phi1, x1, phi2, x2, &theta, &r);
      }
      
      
      Serial.print("x1 = ");
      Serial.print(x1,DEC);
      Serial.print("\t  y1 = ");
      Serial.print(y1,DEC);
      Serial.print("\t  x2 = ");
      Serial.print(x2,DEC);
      Serial.print("\t  y2 = ");
      Serial.print(y2,DEC);
      Serial.print("\t  r_rec =  ");
      Serial.print(r_rec,DEC);
      Serial.print("\t  theta_rec = ");
      Serial.println(theta_rec*180/PI,DEC);
//      Serial.print("\t  R_calc =  ");
//      Serial.print(r,DEC);
//      Serial.print("\t  theta_calc = ");
//      Serial.println(theta*180.0/PI,DEC);
      flag = 0;
  
    //}
  } 
=======
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
>>>>>>> master:Arduino_Debug/Arduino_Debug.ino
}

void receive2intsEvent(int n)
{
      int int1, int2;
      int1 = Wire.read();
      int1 = (int1 | Wire.read() << 8) & 0b1111111111;
      int2 = Wire.read();
      int2 = (y1 | Wire.read() << 8) & 0b1111111111;
      
    Serial.print("int1 = ");
    Serial.print(int1);
    Serial.print("\t int1 = ");
    Serial.println(int1);
}

void receivefloatEvent(int n)
{
<<<<<<< HEAD:Arduino_Debug_no_float/Arduino_Debug_no_float.ino
  flag = 1;
  char* floatptr1 = (char*)&r_rec;
  char* floatptr2 = (char*)&theta_rec;
  switch(camera_number) {
    
    case 1:
      x1 = (Wire.read()<<8 | Wire.read()) & 0b1111111111;
      y1 = (Wire.read()<<8 | Wire.read()) & 0b1111111111;
      camera_number = 2;
      break;
      
    case 2:
      x2 = (Wire.read()<<8 | Wire.read()) & 0b1111111111;
      y2 = (Wire.read()<<8 | Wire.read()) & 0b1111111111;
      camera_number =3;
      break;
      
    case 3: 
      floatptr1[3] = Wire.read();
      floatptr1[2] = Wire.read();
      floatptr1[1] = Wire.read();
      floatptr1[0] = Wire.read();
      camera_number = 4;
      break;
      
    case 4: 
      floatptr2[3] = Wire.read();
      floatptr2[2] = Wire.read();
      floatptr2[1] = Wire.read();
      floatptr2[0] = Wire.read();
      camera_number = 1;
      break;
      
  } 
=======
      float float1 = 0;
      char* floatptr = (char*)&float1;
      floatptr[3] = Wire.read();
      floatptr[2] = Wire.read();
      floatptr[1] |= Wire.read();
      floatptr[0] |= Wire.read();
      
    Serial.print("float1 = ");
    Serial.println(float1);
>>>>>>> master:Arduino_Debug/Arduino_Debug.ino
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
