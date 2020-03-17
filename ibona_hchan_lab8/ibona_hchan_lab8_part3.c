/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 8  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

enum States { Start, Init, Wait } state;      // state vars
unsigned char tmpB, tmpD;                     // temp var for output PORT manip
unsigned short analog_in;

#define MAX 0x70

void ADC_init() {
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE) ;
    // ADEN: setting this bit enables analog-to-digital coversion.
    // ADSC: setting this bit starts the first conversion
    // ADATE: setting this bit enables auto-triggering. Since we are
    //      in Free Running Mode, a new conversion will trigger whenever
    //      the previous conversion completes.
}

void Tick() {
    // State Transitions
    switch (state) {
        case Start: state = Init; break;
        case Init: state = Wait; break;
        case Wait: break;
        default: state = Start; break;
    }

    // State Actions
    switch (state) {
        case Init:
            tmpB = tmpD = 0; break;    // initialize temp var
        case Wait:
            analog_in = ADC;           // get analog input
            tmpB = (char)analog_in;    // store lower 8 bits
            if (tmpB >= MAX/2) {
                PORTB = 0xFF;
                PORTD = 0xFF;
            }                
            else {
                 PORTB = 0x00;
                 PORTD = 0x00;
            }                 
            break;
        default: break;
    }
}

int main(void)
{
    DDRB = 0xFF; PORTB = 0x00;	// set PORTB to outputs and initialize
    DDRD = 0xFF; PORTD = 0x00;	// set PORTD to outputs and initialize

    state = Start;              // initialize state
    tmpB = 0;
    tmpD = 0;

    ADC_init();

    while(1) { Tick(); }
}