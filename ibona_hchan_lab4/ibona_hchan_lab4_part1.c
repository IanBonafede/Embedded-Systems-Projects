/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 4  Exercise 1
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
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char tmpB = 0x00; // intermediate variable used for port updates
	unsigned char tmpIn = 0x00;
	while(1)
	{
		// 1) Read Inputs and assign to variables
		tmpB = 0x00;
		tmpIn = ~PINA & 0x0F;
		// 2) Perform Computation
		
		if(tmpIn == 0) {
			tmpB =  SetBit(tmpB, 6, 1);
		}
		if(tmpIn > 0) {
			tmpB = tmpB | SetBit(tmpB, 5, 1) | SetBit(tmpB, 6, 1);
		}
		if(tmpIn > 2) {
			tmpB = tmpB | SetBit(tmpB, 4, 1) | SetBit(tmpB, 6, 1);
		}
		if(tmpIn > 4) {
			tmpB = tmpB | SetBit(tmpB, 3, 1);
			tmpB = tmpB & SetBit(tmpB, 6, 0);
		}
		if(tmpIn > 6) {
			tmpB = tmpB | SetBit(tmpB, 2, 1);
			tmpB = tmpB & SetBit(tmpB, 6, 0);
		}
		if(tmpIn > 9) {
			tmpB = tmpB | SetBit(tmpB, 1, 1);
			tmpB = tmpB & SetBit(tmpB, 6, 0);
		}
		if(tmpIn > 12) {
			tmpB = tmpB | SetBit(tmpB, 0, 1);
			tmpB = tmpB & SetBit(tmpB, 6, 0);
		}
		
		PORTB = tmpB;
	}
}


