/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 4  Exercise 2 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */



#include <avr/io.h>


enum STATE {Wait, Increment, IncrementWait, Decrement, DecrementWait, Reset, ResetWait} currentState, nextState;

// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x07; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpB = 0x07; // intermediate variable used for port updates
	unsigned char tmpIn = 0x00;
	currentState = Wait;
	nextState = Wait;
	
	while(1)
	{
		// 1) Read Inputs and assign to variables
		
		currentState = nextState;
		tmpIn = 0x00;
		tmpIn = ~PINA & 0x03;
		
		
		// 2) Perform Computation
		switch(currentState) {
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
					nextState = IncrementWait;
				}
				else {
					nextState = Wait;
				}
			break;
			case DecrementWait:
				if(tmpIn == 0x02) {
					nextState = DecrementWait;
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
				if(tmpB < 0x09)
					tmpB++;
				nextState = IncrementWait;
				break;
			case Decrement:
				if(tmpB > 0x00)
					tmpB--;
				nextState = DecrementWait;
				break;
			case Reset:
				tmpB = 0x00;
				nextState = ResetWait;
				break;
		}
		
		PORTB = tmpB;
	}
}

