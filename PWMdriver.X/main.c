/*
 * File:   main.c
 * Author: Emily
 *
 * Created on March 23, 2015, 9:34 PM
 */

#include <p24F16KA301.h>


_FOSCSEL(FNOSC_FRC & SOSCSRC_DIG);    // 8 MHz Oscillator, Clear SOSCSRC to use pins 9,10 as digital i/o

void config();
void initialize();

volatile int Aprev;

void __attribute__ ((__interrupt__)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0; // Clear CN interrupt

   if(_RA1 == 0){      //if A transitioned from high to low
     T3CONbits.TON = 0;
   }
   else if(_RA1 == 1){
     T3CONbits.TON = 1;
   }

    Aprev = _RA1;
}

int main(int argc, char** argv) {
    config();
    initialize();
    while(1){}
    return 0;
}

void config(){

   // Configure the digital I/O ports
    TRISA = 0;          //Set A ports to output
    TRISB = 0;          //Set B ports to output

    ANSA = 0;          //disables analog input
    ANSB = 0x000;      //disable analog input

    _TRISA1 = 1;          // Pin 3 to be input (CN23)
    CNEN1bits.CN3IE = 1;  // Enable CN3 pin 3 for interrupt detection
    IEC1bits.CNIE = 1;    // Enable CN interrupts
    IFS1bits.CNIF = 0;    // Reset CN interrupt

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

 // Configure Timer3 interrupt
//    IPC2bits.T3IP = 4;      //Set Priority
//    IEC0bits.T3IE = 1;      //Enable Interrupt
//    _T3IF = 0;              //Clear Interrupt flag (IFS0bits.T3IF)
}
void initialize(){
    OC3R = 0.5*PR3; //Stepper duty cycle
    T3CONbits.TON = 0;           //disable Timer3
}
