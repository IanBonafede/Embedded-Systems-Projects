/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 7  Exercise 1 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */



#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

// state enum
enum SM1States {SM1Init, first, second, third} SM1state;
enum SM2States {SM2Init, off, on} SM2state;
enum SM3States {SM3Init, quiet, loud} SM3state;
enum SM4States {SM4Init, output} SM4state;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned char threeBits = 0x00;
unsigned char oneBit = 0x00;
unsigned char soundBit = 0x00;
unsigned char tmpB = 0x00;
unsigned char tempA = 0x00;


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


void SM1tick() {
	switch(SM1state) { // transitions
		case SM1Init: SM1state = first; break;
		case first: SM1state = second; break;
		case second: SM1state = third; break;
		case third: SM1state = first; break;
		default : SM1state = SM1Init; break;
	}
	switch(SM1state) { // actions
		case SM1Init: break;
		case first: threeBits = 0x01; break;
		case second: threeBits = 0x02; break;
		case third: threeBits = 0x04; break;
		default : threeBits = 0x00; break;
	}
}

void SM2tick() {
	switch(SM2state) { // transitions
		case SM2Init: SM2state = off; break;
		case off: SM2state = on; break;
		case on: SM2state = off; break;
		default : SM2state = SM2Init; break;
	}
	switch(SM2state) { // actions
		case SM2Init: break;
		case off: soundBit = 0x00; break;
		case on: soundBit = 0x08; break;
		default : soundBit = 0x00; break;
	}
}

void SM3tick() {
	tempA = ~PINA & 0x04;
	switch(SM3state) { // transitions
		case SM3Init: SM3state = quiet; break;
		case quiet:
			if(tempA) {
				SM3state = loud; break;
			}
			else {
				SM3state = quiet; break;
			}
		case loud: SM3state = quiet; break;
		default : SM3state = SM3Init; break;
	}
	switch(SM3state) { // actions
		case SM3Init: break;
		case quiet: oneBit = 0x00; break;
		case loud: oneBit = 0x10; break;
		default : oneBit = 0x00; break;
	}
}

void SM4tick() {
	switch(SM4state) { // transitions
		case SM4Init: SM4state = output; break;
		case output: SM4state = output; break;
	}
	switch(SM4state) { // actions
		case SM4Init: break;
		case output: tmpB = threeBits + oneBit + soundBit; break;
	}
	PORTB = tmpB;
}

int main(void)
{
	unsigned long timer3 = 2;
	unsigned long timer2 = 1000;
	unsigned long timer1 = 300;
	const unsigned long timerPeriod = 2;
	// inputs
	DDRA = 0x00; PORTA =0xFF;
	//initialize outputs
	DDRB = 0xFF; PORTB = 0x00;
	tmpB = 0x00;
	
	TimerSet(timerPeriod);
    TimerOn();

	// initial state
	SM1state = SM1Init;
	SM2state = SM2Init;
	SM3state = SM3Init;
	SM4state = SM4Init;
	//main loop
	while(1){
		if(timer1 >= 300) {
			SM1tick();
			timer1 =0;
		}
		if(timer2 >= 1000) {
			SM2tick();
			timer2 = 0;
		}
		if(timer3 >= 2){
			SM3tick();
			timer3 = 0;
		}
		
		SM4tick();
		
		while(!TimerFlag);
		TimerFlag = 0;
		
		timer1 += timerPeriod;
		timer2 += timerPeriod;
		timer3 += timerPeriod;
		}
	
	
}

