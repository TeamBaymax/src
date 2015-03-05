/*
 * File:   main.c
 * Author: Emily Lazalde
 * Description: Controlling stepper motors. Straight line and 90 deg turn
 *
 * Created on March 2, 2015, 10:06 AM
 */

//#include <stdio.h>
//#include <stdlib.h>
#include<p24F16KA301.h>

_FOSCSEL(FNOSC_FRC & SOSCSRC_DIG);    // 8 MHz Oscillator, Clear SOSCSRC to use pins 9,10 as digital i/o

/*
 * Timer3 interrupt service routine
 * This function executes every time the micro receives an interrupt
 * originating from Timer3. The micro knows the interrupt is from Timer3
 * when the Timer3 interrupt flag (T3IF) is set.
 */
void _ISR _T3Interrupt(void)
{
    // Remember to clear the Timer1 interrupt flag when this ISR is entered.
    _T3IF = 0;

    // Place in this ISR whatever code should be executed when the timer
    // reaches the period (PR3) that you specify
       if(_RB12 == _RA4){
            //Change the period & directions - do a 90 deg turn
//            if( PR3 == 625 ){
                PR3 = 200;
                _RB12 = 1;
                _RA4 = 0;
        }
        else{
            _RA4 = 0;
            _RB12 = 0;
            OC3R = 0;
            PR3 = 625;
        }
}

int main(int argc, char** argv) {

   /* Input pin 3 controls output pin 14, Input pin 9 controls output pin 4*/

   // Configure the digital I/O ports
    TRISA = 0;          //Set A ports to output
    TRISB = 0;          //Set B ports to output

    ANSA = 0;          //disables analog input
    ANSB = 0x000;      //disable analog input

 // Configure Timer1 using T1CON register
    _TON = 1;           //enable Timer1
    _TCS = 0;           //Set source to internal clock
    _TCKPS = 0b01;      //Select prescale value - 1/8
    PR1 = 5000;         //Set timer period so freq is 100 Hz
    TMR1 = 0;
 // Timer 2
    T2CONbits.TON = 1;           //enable Timer2
    T2CONbits.TCS = 0;           //Set source to internal clock
    T2CONbits.TCKPS = 0b01;      //Select prescale value - 1/8
    PR2 = 5000;                  //Set timer period so freq is 100 Hz
    TMR2 = 0;
 // Timer 3
    T3CONbits.TON = 1;           //enable Timer3
    T3CONbits.TCS = 0;           //Set source to internal clock
    T3CONbits.TCKPS = 0b10;      //Select prescale value - 1/64
    PR3 = 625;                  //Set timer period so freq is 100 Hz
    TMR3 = 0;

 // Configure Output Compare
    OC1CON1bits.OCTSEL = 0b100;     //Select Timer1 to be timer source
    OC1CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC1CON2bits.SYNCSEL = 0b01011;  //Select Timer1 as synchronization source
 // Output 2
    OC2CON1bits.OCTSEL = 0b000;     //Select Timer2 to be timer source
    OC2CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC2CON2bits.SYNCSEL = 0b01100;  //Select Timer2 as synchronization source
 // Output 3 - Stepper Motors
    OC3CON1bits.OCTSEL = 0b001;     //Select Timer3 to be timer source
    OC3CON1bits.OCM = 0b110;        //Select Edge-Aligned PWM mode
    OC3CON2bits.SYNCSEL = 0b01101;  //Select Timer3 as synchronization source

 // Configure Timer3 interrupt
    // Remember to enable the Timer1 interrupt source (IEC0 register) and
    // set the Timer1 interrupt priority to 4 (IPC0) register
    IPC2bits.T3IP = 4;      //Set Priority
    IEC0bits.T3IE = 1;      //Enable Interrupt
    _T3IF = 0;              //Clear Interrupt flag (IFS0bits.T3IF)


 // Configure inputs
//    config_ad();

 // <editor-fold defaultstate="collapsed" desc="while(1)">

    while(1){

        _RB1 = 0;   //Stepper
        _RA4 = 0;
        OC3R = 0.5*PR3;

    }
        // </editor-fold>
    return (0);
}


 /*
  * Data Sheet: http://ww1.microchip.com/downloads/en/DeviceDoc/39995d.pdf
  *
  * Family Reference Manual: http://www.microchip.com/TechDoc.aspx?type=ReferenceManuals
  * 
  * Tips and Tricks: https://docs.google.com/document/d/1YN8Au7-waxfx1F3gALHjsmbpb9-qOa-AEzvvl6XvJO0/edit
  *
  *
  */