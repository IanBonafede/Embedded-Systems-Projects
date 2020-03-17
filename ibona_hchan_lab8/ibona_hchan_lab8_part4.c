/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 8  Exercise 4
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

#define MAX 0x70

enum States { Start, Init, Wait } state;      // state vars
unsigned char tmpB;                           // temp var for output PORT manip
unsigned short analog_in;

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
        case Init: tmpB = 0; break;    // initialize temp var
        case Wait:
            analog_in = ADC;           // get analog input
            tmpB = (char)analog_in;    // store lower 8 bits
            if (tmpB <= 15) { PORTB = 0x00; }
            else if (tmpB > 15 && tmpB <= 29) { PORTB = 0x01; }
            else if (tmpB > 29 && tmpB <= 43) { PORTB = 0x03; }
            else if (tmpB > 43 && tmpB <= 57) { PORTB = 0x07; }
            else if (tmpB > 57 && tmpB <= 71) { PORTB = 0x0F; }
            else if (tmpB > 71 && tmpB <= 85) { PORTB = 0x1F; }
            else if (tmpB > 85 && tmpB <= 99) { PORTB = 0x3F; }
            else if (tmpB > 99 && tmpB <= 113) { PORTB = 0x7F; }
            else { PORTB = 0xFF; }
            break;
        default: break;
    }
}

int main(void)
{
    DDRB = 0xFF; PORTB = 0x00;

    state = Start;
    tmpB = 0;

    ADC_init();

    while(1){ Tick(); }
}