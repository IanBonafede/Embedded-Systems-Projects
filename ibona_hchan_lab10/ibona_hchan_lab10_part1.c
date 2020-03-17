/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 10  Exercise 1 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */


#include <avr/io.h>
#include "keypad.h"
#include "scheduler.h"

enum States { Start, Init, Wait };      // state vars
task KeypadTask;                            // Keypad task

int TickFct_Keypad(int state) {

    unsigned char value;            // temp char to hold keypad value
    
    // Transitions
    switch(state) {
        case Start:
        state = Wait; break;    // advance to INIT state
        case Wait:
        break;
        default:
        state = Start; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case Wait:
            value = GetKeypadKey();     // get current key pressed

            // output appropriate LED output
            switch(value) {
                case '\0': PORTB = 0x1F; break;
                case '1': PORTB = 0x01; break;
                case '2': PORTB = 0x02; break;
                case '3': PORTB = 0x03; break;
                case '4': PORTB = 0x04; break;
                case '5': PORTB = 0x05; break;
                case '6': PORTB = 0x06; break;
                case '7': PORTB = 0x07; break;
                case '8': PORTB = 0x08; break;
                case '9': PORTB = 0x09; break;
                case 'A': PORTB = 0x0A; break;
                case 'B': PORTB = 0x0B; break;
                case 'C': PORTB = 0x0C; break;
                case 'D': PORTB = 0x0D; break;
                case '*': PORTB = 0x0E; break;
                case '0': PORTB = 0x00; break;
                case '#': PORTB = 0x0F; break;
                default: PORTB = 0x1B; break;
            }
            break;
        default: break;
    }

    return state;
}

int main(void)
{
    DDRB = 0xFF; PORTB = 0x00;	// set PORTB to outputs and initialize
    DDRC = 0xF0; PORTC = 0x0F;	// set PORTC to inputs/outputs and initialize

    KeypadTask.state = Start;       // initialize task state
    KeypadTask.TickFct = &TickFct_Keypad;   // set task tick function

    while(1) {
        KeypadTask.state = KeypadTask.TickFct(KeypadTask.state);    // tick task and advance states
    }
}
