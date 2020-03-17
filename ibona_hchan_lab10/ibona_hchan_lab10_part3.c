/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 10  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */ 
#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "io.c"
#include "keypad.h"

enum KeypadStates { KP_START, KP_INIT, KP_WAIT };          // Keypad start state vars
enum CursorStates { CUR_START, CUR_INIT, CUR_WAIT };   // Cursor state vars
unsigned char curr_key;                                 // last key pressed

// Keypad SM
int TickFct_KP(int state) {

    // Transitions
    switch(state) {
        case KP_START:
        state = KP_INIT;           // advance to INIT state
        break;
        case KP_INIT:
        state = KP_WAIT;           // advance to WAIT state
        break;
        case KP_WAIT:
        break;
        default:
        state = KP_START; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case KP_WAIT:
        // update current key being pressed
        curr_key = GetKeypadKey();
        break;
        default: break;
    }

    return state;
}

// Cursor position and char draw SM
int TickFct_Cursor(int state) {
    // Transitions
    switch(state) {
        case CUR_START:
        state = CUR_INIT;           // advance to INIT state
        break;
        case KP_INIT:
        state = CUR_WAIT;           // advance to WAIT state
        break;
        case CUR_WAIT:
        break;
        default:
        state = CUR_START; break;   // something went wrong, go back to START
    }

    // Actions
    switch(state) {
        case CUR_WAIT:
        // make sure a button is pressed
        if(curr_key != '\0') {
            LCD_Cursor(1);              // move lcd cursor
            LCD_WriteData(curr_key);    // update lcd display
        }
        break;
        default: break;
    }

    return state;
}

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00;  // LCD control lines
    DDRB = 0xFF; PORTB = 0x00;  // set to outputs and initialize
    DDRC = 0xF0; PORTC = 0x0F;  // set to in/outputs and initialize
    DDRD = 0xFF; PORTD = 0x00;  // LCD data lines

    // Period for tasks
    unsigned long int KP_Task_calc = 10;
    unsigned long int Cursor_Task_calc = 100;

    // Calculate GCD
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(KP_Task_calc, Cursor_Task_calc);

    // Gcd for all tasks or smallest time unit for tasks
    unsigned long int GCD = tmpGCD;

    // Recalc GCD periods for scheduler
    unsigned long int KP_period = KP_Task_calc/GCD;
    unsigned long int Cursor_period = Cursor_Task_calc/GCD;

    static task KP_Task, Cursor_Task;
    task *tasks[] = { &KP_Task, &Cursor_Task };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    KP_Task.state = KP_START;     // initialize task state
    KP_Task.period = KP_period;   // set period
    KP_Task.elapsedTime = KP_period; // initialize elapsed time
    KP_Task.TickFct = &TickFct_KP;   // set task tick function

    Cursor_Task.state = CUR_START;     // initialize task state
    Cursor_Task.period = Cursor_period;   // set period
    Cursor_Task.elapsedTime = Cursor_period; // initialize elapsed time
    Cursor_Task.TickFct = &TickFct_Cursor;   // set task tick function

    LCD_init();                 // initialize LCD screen
    LCD_ClearScreen();          // clear LCD screen

    TimerSet(GCD);              // set timer period
    TimerOn();                  // enable timer

    unsigned short i;           // scheduler loop iterator

    while(1) {
        // scheduler loop
        for(i = 0; i < numTasks; i++) {
            // task is ready to tick
            if(tasks[i]->elapsedTime == tasks[i]->period) {
                // setting next state for task
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                // reset the elapsed time for the next tick
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 1;
        }
        while(!TimerFlag);      // wait for a period
        TimerFlag = 0;          // reset TimerFlag
    }
}
