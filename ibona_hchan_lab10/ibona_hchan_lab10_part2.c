/*	Partner(s) Name & E-mail:
 *  Ian Bonafede ibona001@ucr.edu
 *  Ho Yee Chan hchan069@ucr.edu
 *	Lab Section: 026
 *	Assignment: Lab 10  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */ 

#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "io.c"

enum Scr_States { SCR_Start, SCR_INIT, SCR_WAIT };      // Scroll start state vars
enum Cursor_States { CUR_START, CUR_INIT, CUR_WAIT };   // Cursor state vars
const unsigned char MESSAGE[] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
    ' ', ' ', ' ', ' ', ' ', 'C', 'S', '1', '2', '0',
    'B', ' ', 'i', 's', ' ', 'L', 'e', 'g', 'e', 'n',
    'd', '.', '.', '.', ' ', 'w', 'a', 'i', 't', ' ',
    'f', 'o', 'r', ' ', 'i', 't', ' ', 'D', 'A', 'R',
'Y', '!'};

const unsigned short MSG_LENGTH = sizeof(MESSAGE) / sizeof(char);     // message length
unsigned char st_pos;       // shared start position var, Scr updates, Cursor reads

// Scroll start position SM
int TickFct_Scr(int state) {
    // Transitions
    switch(state) {
        case SCR_Start: state = SCR_INIT;  break;
        case SCR_INIT:
            st_pos = 0;
            state = SCR_WAIT;
            break;
        case SCR_WAIT: break;
        default: state = SCR_Start; break;
    }

    switch(state) {     // Actions
        case SCR_Start: break;
        case SCR_INIT: break;
        case SCR_WAIT:
            if (st_pos >= MSG_LENGTH) 
                st_pos = 0;
            else 
                st_pos++;
            break;
        default: break;
    }

    return state;
}

// Cursor position and char draw SM
int TickFct_Cursor(int state) {
    static unsigned char i, j;

    // Transitions
    switch(state) {
        case CUR_START: state = CUR_INIT;  break;
        case CUR_INIT:
            i = 1;                      // start at first marker
            j = st_pos;                 // init start position
            LCD_Cursor(1);              // reset cursor position
            state = CUR_WAIT;           // advance to WAIT state
            break;
        case CUR_WAIT: break;
        default: state = CUR_START; break;   // something went wrong, go back to START
    }

    switch(state) { // Actions
        case CUR_WAIT:
            // write blanks if out of we're going to be out of range
            if(j >= MSG_LENGTH) 
                LCD_WriteData(' ');
            // if not out of range, then write current character
            else 
                LCD_WriteData(MESSAGE[j]);
            // check if we've reached the end of the display
            if (i >= 16) {
                state = CUR_INIT;
            } else {
                i++; 
                j++;
            } break;
        default: break;
    }

    return state;
}

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00;  // LCD control lines
    DDRD = 0xFF; PORTD = 0x00;  // LCD data lines

    // Period for tasks
    unsigned long int Scroll_Task_calc = 300;
    unsigned long int Cursor_Task_calc = 5;

    // Calculate GCD
    unsigned long int tmpGCD = 1;
    tmpGCD = findGCD(Scroll_Task_calc, Cursor_Task_calc);

    // Gcd for all tasks or smallest time unit for tasks
    unsigned long int GCD = tmpGCD;

    // Recalc GCD periods for scheduler
    unsigned long int Scroll_period = Scroll_Task_calc/GCD;
    unsigned long int Cursor_period = Cursor_Task_calc/GCD;

    static task Scroll_Task, Cursor_Task;
    task *tasks[] = { &Scroll_Task, &Cursor_Task };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    Scroll_Task.state = SCR_Start;     // initialize task state
    Scroll_Task.period = Scroll_period;   // set period
    Scroll_Task.elapsedTime = Scroll_period; // initialize elapsed time
    Scroll_Task.TickFct = &TickFct_Scr;   // set task tick function

    Cursor_Task.state = CUR_START;     // initialize task state
    Cursor_Task.period = Cursor_period;   // set period
    Cursor_Task.elapsedTime = Cursor_period; // initialize elapsed time
    Cursor_Task.TickFct = &TickFct_Cursor;   // set task tick function

    LCD_init();                 // initialize LCD screen
    LCD_ClearScreen();          // clear LCD screen

    TimerSet(GCD);              // set timer period
    TimerOn();                  // enable timer

    unsigned short i;           // scheduler loop iterator

    while(1)
    {
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