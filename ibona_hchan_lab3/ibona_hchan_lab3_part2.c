/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 3  Exercise 2 
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
	DDRC = 0xFF; PORTC = 0x07; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpC = 0x07; // intermediate variable used for port updates
	currentState = Wait;
	nextState = Wait;
	
	while(1)
	{
		// 1) Read Inputs and assign to variables
		currentState = nextState;
		
		// 2) Perform Computation
		switch(currentState) {
			case Wait: 
				if(PINA == 0x00) {
					nextState = Wait;
				}
				if(PINA == 0x01) {
					nextState = Increment;
				}
				if(PINA == 0x02) {
					nextState = Decrement;
				}
				if(PINA == 0x03) {
					nextState = Reset;
				}
			break;
			case IncrementWait:
				if(PINA == 0x01) {
					nextState = IncrementWait;
				}
				else {
					nextState = Wait;
				}
			break;
			case DecrementWait:
				if(PINA == 0x02) {
					nextState = DecrementWait;
				}
				else {
					nextState = Wait;
				}
			break;
			case ResetWait:
				if(PINA == 0x03) {
					nextState = ResetWait;
				}
				else {
					nextState = Wait;
				}
			break;
			case Increment:
				if(tmpC < 0x09)
					tmpC++;
				nextState = IncrementWait;
				break;
			case Decrement:
				tmpC--;
				nextState = DecrementWait;
				break;
			case Reset:
				tmpC == 0x00;
				nextState = ResetWait;
				break;
		}
		
		PORTC = tmpC;
	}
}

