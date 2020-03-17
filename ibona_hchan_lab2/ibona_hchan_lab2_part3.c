/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 2  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

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
	DDRC = 0xFF; PORTC = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpC; // intermediate variable used for port updates
	unsigned char tmpIn;
	unsigned char tmpBelt;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		tmpC = 0x00;
		tmpIn = PINA & 0x0F; // the first 4 (3-0) for gas level stuff
		tmpBelt = PINA>>4;   // the last  3 (6-4) for seatbelt light
		// 2) Perform Computation
		
		if(tmpIn == 0) {
			tmpC =  SetBit(tmpC, 6, 1);
		}
		if(tmpIn > 0) {
			tmpC = tmpC | SetBit(tmpC, 5, 1) | SetBit(tmpC, 6, 1);
		}
		if(tmpIn > 2) {
			tmpC = tmpC | SetBit(tmpC, 4, 1) | SetBit(tmpC, 6, 1);
		}
		if(tmpIn > 4) {
			tmpC = tmpC | SetBit(tmpC, 3, 1);
			tmpC = tmpC & SetBit(tmpC, 6, 0);
		}
		if(tmpIn > 6) {
			tmpC = tmpC | SetBit(tmpC, 2, 1);
			tmpC = tmpC & SetBit(tmpC, 6, 0);
		}
		if(tmpIn > 9) {
			tmpC = tmpC | SetBit(tmpC, 1, 1);
			tmpC = tmpC & SetBit(tmpC, 6, 0);
		}
		if(tmpIn > 12) {
			tmpC = tmpC | SetBit(tmpC, 0, 1);
			tmpC = tmpC & SetBit(tmpC, 6, 0);
		}
		if(tmpBelt == 3) {
			tmpC = tmpC | SetBit(tmpC, 7, 1);
		}
		
		PORTC = tmpC;
	}
}


