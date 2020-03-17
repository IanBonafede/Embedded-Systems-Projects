/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 3  Exercise 3 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */



#include <avr/io.h>

// state enum
enum States {Start, Init, Locked, PressFirst, ReleaseFirst, PressSecond, ReleaseSecond, Unlocked, PressLock} state;

// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

void tick() {
	switch(state) { // transitions
		case Start:
			state = Init; break;
		case Init:
			state = Locked; break;
		case Locked:
			if(PINA == 0x04)
				state = PressFirst;
			else
				state = Locked;
			break;
		case PressFirst:
			if(PINA == 0x04)
				state = PressFirst;
			else if(PINA == 0x00)
				state = ReleaseFirst;
			else 
				state = Locked;
			break;
		case ReleaseFirst:
			if(PINA == 0x02)
				state = PressSecond;
			else if(PINA == 0x00)
				state = ReleaseFirst;
			else
				state = Locked;
			break;
		case PressSecond:
			if(PINA == 0x02)
				state = PressSecond;
			else if(PINA == 0x00)
				state = ReleaseSecond;
			else 
				state = Locked;
			break;
		case ReleaseSecond:
			if(PINA == 0x02)
				state = ReleaseSecond;
			else if(PINA == 0x00)
				state = Unlocked;
			else 
				state = Locked;
			break;
		case Unlocked:
			if(PINA == 0x80)
				state = PressLock;
			else
				state = Unlocked;
			break;
		case PressLock:
			if(PINA == 0x80)
				state = PressLock;
			else
				state = Unlocked;
			break;
	}
	switch(state) { // actions
		case Start: break;
		case Init: break;
		case Locked: 
			PORTB = 0x00; // locked
			break;
		case PressFirst: break;
		case ReleaseFirst: break;
		case PressSecond: break;
		case ReleaseSecond: break;
		case Unlocked:
			PORTB = 0x01; // unlocked
			break;
	}
}

int main(void)
{
	//initialize outputs
	DDRB = 0xFF; PORTB = 0x00; // locked
	DDRA = 0x00; PORTA = 0xFF; // A = input
	// initial state
	state = Start;
	//main loop
	while(1){tick();}
	
}

