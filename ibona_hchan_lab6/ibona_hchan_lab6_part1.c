/* Ho Yee Chan hchan069@ucr.edu
* Ian Bonafede ibona001@ucr.edu
* Lab Section: 026
* Assignment: Lab 6 Exercise 1
* Exercise Description: [optional - include for your own benefit]
*
* I acknowledge all content contained herein, excluding template or example
* code, is my own original work.
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

enum STATE {Wait, Increment, IncrementWait, Decrement, DecrementWait, Reset, ResetWait} currentState, nextState;
    
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned char tmpIn = 0x00;
unsigned char count = 0x00;

void TimerOn() {
    // AVR timer/counter controller register TCCR1
    TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
    // bit2bit1bit0=011: pre-scaler /64
    // 00001011: 0x0B
    // SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
    // Thus, TCNT1 register will count at 125,000 ticks/s

    // AVR output compare register OCR1A.
    OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
    // We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
    // So when TCNT1 register equals 125,
    // 1 ms has passed. Thus, we compare to 125.
    // AVR timer interrupt mask register
    TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

    //Initialize avr counter
    TCNT1=0;

    _avr_timer_cntcurr = _avr_timer_M;
    // TimerISR will be called every _avr_timer_cntcurr milliseconds

    //Enable global interrupts
    SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
    TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
    TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
    // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
    _avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
    if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
        TimerISR(); // Call the ISR that the user uses
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

void tick() {
    // 1) Read Inputs and assign to variables
    currentState = nextState;
    tmpIn = ~PINA & 0x03;
    
    // 2) Perform Computation
    switch(currentState) { // transitions
        case Wait:
        if(tmpIn == 0x00) {
            nextState = Wait;
        }
        if(tmpIn == 0x01) {
            nextState = Increment;
        }
        if(tmpIn == 0x02) {
            nextState = Decrement;
        }
        if(tmpIn == 0x03) {
            nextState = Reset;
        }
        break;
        
        case IncrementWait:
        if(tmpIn == 0x01) {
            nextState = Increment;
        }
        else {
            nextState = Wait;
        }
        break;
        
        case DecrementWait:
        if(tmpIn == 0x02) {
            nextState = Decrement;
        }
        else {
            nextState = Wait;
        }
        break;
        
        case ResetWait:
        if(tmpIn == 0x03) {
            nextState = ResetWait;
        }
        else {
            nextState = Wait;
        }
        break;
        
        case Increment:
        nextState = IncrementWait;
        break;
        
        case Decrement:
        nextState = DecrementWait;
        break;
        
        case Reset:
        nextState = ResetWait;
        break;
    }
    switch(currentState) { // actions
        case Wait: break;
        case Increment:
            if(count < 0x09) {
                count++;
            }
            break;
        case IncrementWait: break;
        case Decrement:
            if(count > 0x00) {
               count--;
            }
            break;
        case DecrementWait: break;
        case Reset:
            count = 0x00;
            break;
        case ResetWait: break;
    }
    
    LCD_Cursor(1);
    LCD_WriteData(count + '0');
}

int main ( void )
{
    DDRA = 0x00; PORTA = 0xFF; // input
    DDRC = 0xFF; PORTC = 0x00; // LCD data lines
    DDRD = 0xFF; PORTD = 0x00; // LCD control lines
    
    TimerSet(500);
    TimerOn();

    // Initializes the LCD display
    LCD_init ();
        
    currentState = Wait;
	nextState = Wait;
	
    while(1)
	{
        tick();
        
        while (!TimerFlag);
        TimerFlag = 0;
	}
}