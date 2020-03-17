/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 9  Exercise 2 
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */



#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

typedef struct task {
	int state;                  // Task's current state
	unsigned long period;       // Task period
	unsigned long elapsedTime;  // Time elapsed since last task tick
	int (*TickFct)(int);        // Task tick function
} task;

#define tasksNum 1
task tasks[tasksNum];

#define note_C4 261.63
#define note_D4 293.66
#define note_E4 329.63
#define note_F4 349.23
#define note_G4 392
#define note_A4 440
#define note_B4 493.88
#define note_C5 523.25

#define machinePeriod 1

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

unsigned char tempA = 0;


// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {

 static double current_frequency; // Keeps track of the currently set frequency

// Will only update the registers when the frequency changes, otherwise allows 

// music to play uninterrupted.

 if (frequency != current_frequency) {

  if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter

  else { TCCR3B |= 0x03; } // resumes/continues timer/counter   

  

  // prevents OCR3A from overflowing, using prescaler 64

  // 0.954 is smallest frequency that will not result in overflow

  if (frequency < 0.954) { OCR3A = 0xFFFF; }

 

  // prevents OCR0A from underflowing, using prescaler 64     // 31250 is largest frequency that will not result in underflow 

  else if (frequency > 31250) { OCR3A = 0x0000; }

 

  // set OCR3A based on desired frequency   

  else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }

 

  TCNT3 = 0; // resets counter

  current_frequency = frequency; // Updates the current frequency

 }

}

 

void PWM_on() {

 TCCR3A = (1 << COM3A0);

  // COM3A0: Toggle PB3 on compare match between counter and OCR0A

 TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30); 

  // WGM02: When counter (TCNT0) matches OCR0A, reset counter

  // CS01 & CS30: Set a prescaler of 64

 set_PWM(0);

}

 

void PWM_off() {

 TCCR3A = 0x00;

 TCCR3B = 0x00;

}




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
    for(int i = 0; i < tasksNum; i++)
    {
	    if(tasks[i].elapsedTime >= tasks[i].period) {
		    tasks[i].state = tasks[i].TickFct(tasks[i].state);
		    tasks[i].elapsedTime = 0;
	    }
	    tasks[i].elapsedTime += tasks[i].period;
    }
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


// Bit-access function
unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}



// state enum
#define shortNoteLength 500
#define longNoteLength 1000

enum SM1states {SM1_Init, SM1_Off, SM1_PressOn, SM1_On, SM1_Finish} SM1state;
float song[] = {note_C4, note_C4, note_G4, note_G4,
				note_A4, note_A4, note_G4, note_F4,
				note_F4, note_E4, note_E4, note_D4, 
				note_D4, note_C4};
float duration[] = {shortNoteLength, shortNoteLength, shortNoteLength, shortNoteLength,
					shortNoteLength, shortNoteLength, longNoteLength, shortNoteLength,
					shortNoteLength, shortNoteLength, shortNoteLength, shortNoteLength, 
					shortNoteLength, longNoteLength};
unsigned long i = 0;
unsigned long k = 0;
unsigned long l = 0;
unsigned char downTime = 10;

int SM1tick(int state) {
	tempA = ~PINA & 0x01;
	
	switch(state) { // transitions
		case SM1_Init: state = SM1_Off; break;
		case SM1_Off: state = (tempA) ? SM1_PressOn : SM1_Off; break;
		case SM1_PressOn: state = (tempA) ? SM1_PressOn : SM1_On; break;
		case SM1_On: 
			if(i < sizeof(song)/sizeof(song[0]) )
				state = SM1_On;
			else if (tempA)
				state = SM1_Finish;
			else
				state = SM1_Off;
			break;
		case SM1_Finish: state = (tempA) ? SM1_Finish : SM1_Off; break;
	}
	switch(state) { // actions
		case SM1_Init: break;
		case SM1_Off: 
			PWM_off(); 
			i = 0;
			k = 0;
			l = 0;
			break;
		case SM1_PressOn: PWM_on(); break;
		case SM1_On: 
			if (k < duration[i]) {
				set_PWM(song[i]);
				k++;
			}
			else if (l < downTime){
				set_PWM(0);
				l++;
			}
			else {
				k = 0;
				l = 0;
				i++;
			}
			 break;
		case SM1_Finish: break;
	}
	return state;
}

int main(void)
{	
	// set tasks
	
	unsigned char j = 0;
	tasks[j].state = SM1_Init;
	tasks[j].period = machinePeriod;
	tasks[j].elapsedTime = 0;
	tasks[j].TickFct = &SM1tick;
	
	j++;

	//initialize outputs
	DDRB = 0xFF; PORTB = 0x00;
	DDRA = 0x00; PORTA = 0xFF;	

	TimerSet(machinePeriod);
    TimerOn();

	//main loop
	while(1){}
		
	return 0;
	
}

