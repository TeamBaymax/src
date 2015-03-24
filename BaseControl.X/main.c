#include "p24F16KA301.h"
#include "i2c1_helper.h"
#include "i2c2_helper.h"
#include "vision.h"
#include "infrared.h"
#include "State.h"

#define CENTER 33.5
#define WHEEL_DIA 2.5

_FOSCSEL(FNOSC_FRC); //8 MHz
//_FOSCSEL(FNOSC_LPRC); //31 kHz
//_FOSCSEL(FNOSC_LPFRC); //500 kHz subdivided
//_FOSCSEL(FNOSC_FRC & SOSCSRC_DIG);    // 8 MHz Oscillator, Clear SOSCSRC to use pins 9,10 as digital i/o

//char arduinoAddress = 0x04<<1;

// <editor-fold defaultstate="collapsed" desc="Parameters">
const float r_window = 0.5*PI/180.;     //radians
const float theta_window = 4.0;         //inches

State current_state;
volatile int step_counter, step_max; //step max == 152 for 90 deg turn
volatile int angle_count;
float theta, r;
// </editor-fold>
// <editor-fold defaultstate="collapsed" desc="Function Declarations">
//Configuration
void config();
void initialize();

int main(void)
{
    config();
    initialize();
    //vision_setup();
     T3CONbits.TON = 1;           //enable Timer3
    while(1)
    {
      
        //see_beacon(&theta, &r);
    }
    							//end of program
}

void config(){

   // Configure the digital I/O ports
    _TRISA1 = 0;
    _TRISA2 = 0;
    _TRISB1 = 0;

    ANSA = 0x000;          //disables analog input
    ANSB = 0x000;      //disable analog input

    _TRISB13 = 1;       //digital input bumper 1 LEFT
    _TRISB14 = 1;       //digital input bumper 2 RIGHT

// // Configure Timer1 using T1CON register
//    _TON = 1;           //enable Timer1
//    _TCS = 0;           //Set source to internal clock
//    _TCKPS = 0b01;      //Select prescale value - 1/8
//    PR1 = 5000;         //Set timer period so freq is 100 Hz
//    TMR1 = 0;
// // Timer 2
//    T2CONbits.TON = 1;           //enable Timer2
//    T2CONbits.TCS = 0;           //Set source to internal clock
//    T2CONbits.TCKPS = 0b01;      //Select prescale value - 1/8
//    PR2 = 5000;                  //Set timer period so freq is 100 Hz
//    TMR2 = 0;
// Timer 3
    T3CONbits.TON = 1;           //enable Timer3
    T3CONbits.TCS = 0;           //Set source to internal clock
    T3CONbits.TCKPS = 0b10;      //Select prescale value - 1/64
    PR3 = 625;                  //Set timer period so freq is 100 Hz
    TMR3 = 0;

// // Configure Output Compare
//    OC1CON1bits.OCTSEL = 0b100;     //Select Timer1 to be timer source
//    OC1CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
//    OC1CON2bits.SYNCSEL = 0b01011;  //Select Timer1 as synchronization source
// // Output 2
//    OC2CON1bits.OCTSEL = 0b000;     //Select Timer2 to be timer source
//    OC2CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
//    OC2CON2bits.SYNCSEL = 0b01100;  //Select Timer2 as synchronization source
 // Output 3 - Stepper Motors
    OC3CON1bits.OCTSEL = 0b001;     //Select Timer3 to be timer source
    OC3CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b01101;  //Select Timer3 as synchronization source
    OC3CON2bits.OCTRIG = 0b0;       //Trigger

 // Configure Timer3 interrupt
    IPC2bits.T3IP = 4;      //Set Priority
    IEC0bits.T3IE = 1;      //Enable Interrupt
    _T3IF = 0;              //Clear Interrupt flag (IFS0bits.T3IF)
}

void initialize(){
    current_state = start;
    r = 0;
    theta = 0;

    step_counter = 0;
    step_max = 100000;

    _RA2= 0;   //Stepper1
    _RA1 = 0;   //Stepper2
    OC3R = 0.5*PR3; //Stepper duty cycle
    T3CONbits.TON = 0;           //disable Timer3
}

void startDrive(unsigned int direction){
    if(direction == 0){         //drive forward
         _RA2 = 0;
         _RA1 = 0;

    }
    else{                       //drive reverse
        _RA2 = 1;
        _RA1 = 1;
    }

    T3CONbits.TON = 1;           //enable Timer3
}

void startTurn(unsigned int direction){
    if(direction == 0){         //turn right
         _RA2 = 0;
         _RA1 = 1;

    }
    else{                       //turn left
        _RA2 = 1;
        _RA1 = 0;
    }

    T3CONbits.TON = 1;           //enable Timer3
}

void stop(){
    T3CONbits.TON = 0;           //disable Timer3
    _RA1 = 0;
    _RA2 = 0;
}