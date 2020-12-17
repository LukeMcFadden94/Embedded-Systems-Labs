/*	Author: Luke McFadden
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab 13  Exercise 3
 *	Exercise Description:
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 * Demo Link: https://youtu.be/KPbsIPnI77g
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#include "timer.h"
#include "scheduler.h"
#endif

void A2D_init()     
    {
      ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
    // ADEN: Enables analog-to-digital conversion
    // ADSC: Starts analog-to-digital conversion
    // ADATE: Enables auto-triggering, allowing for constant
    //      analog to digital conversions.
    }

unsigned short rate = 100;
unsigned char pattern = 0x80;   // LED pattern - 0: LED off; 1: LED on
unsigned char row = 0xFE;       // Row(s) displaying pattern. 
                                // 0: display pattern on row
                                // 1: do NOT display pattern on row

// Middle value of joystick is 512.
// However at rest, the LED will continue to move, 
// suggesting that something is making it so that the ADC
// value isn't always perfectly at 512 when the joystick is at rest.
// I have found that when joystick is at rest, the ADC varies between
// 540 and 560 - attempting to make the range any tigher allows for the LED
// to move unexpectedly, even when joystick is at rest.

enum Speed {stop, rateLeft, rateRight};
int SpeedSM(int state)
{
    unsigned short input = ADC;

    switch (state)  // Transitions begin
    {
        // Left is 0 through <540
        // Right is 560 - 1024
        case stop:                         
            if ((input >= 540) && (input < 560))        // 540 - <560
                state = stop;
            else if (input < 540)                       // 0 - <540
                state = rateLeft;
            else if (input >= 560)                      // 560 - 1024
                 state = rateRight;
            break;

        case rateLeft:
            if (input < 540)                            // 0 - <540
                    state = rateLeft;
            else if ((input >= 540) && (input < 560))   // 540 - <560
                state = stop;
            else if (input >= 560)                      // 560 - 1024
                 state = rateRight;
            break;

        case rateRight:
            if (input >= 560)                           // 560 - 1024
                 state = rateRight;
            else if (input < 540)                       // 0 - <540
                state = rateLeft;
            else if ((input >= 540) && (input < 560))   // 540 - <560
                state = stop;
            break;

        default:
            state = stop;
            break;
    }               // Transitions end

    switch (state)  // State actions begin
    {
        case stop: 
            TimerSet(100);
            break;

        case rateLeft:                                          // 0 - <540   
            // 0 - <90 == 100
            // 90 - <260 == 250
            // 260 - <410 == 500
            // 410 - <540 == 1000
            if (input < 90)
                TimerSet(100);
            else if ((input >= 90) && (input < 260))
                TimerSet(250);
            else if ((input >= 260) && (input < 410))
                TimerSet(500);
            else if ((input >= 410) && (input < 540))
                TimerSet(1000);
            break;

        case rateRight:                                          // 560 - 1024  
            // 560 - <672 == 1000
            // 672 - <800 == 500
            // 800 - <946 == 250
            // 946 - 1024 == 100
            if ((input >= 560) && (input < 672))
                TimerSet(1000);
            else if ((input >= 672) && (input < 800))
                TimerSet(500);
            else if ((input >= 800) && (input < 946))
                TimerSet(250);
            else if (input >= 946)
                TimerSet(100);
            break;
    }               // State actions end

    return state;
}

enum Joystick {noMove, left, right};
int JoystickSM(int state)
{
    unsigned short input = ADC;

    switch (state)  // Transitions begin
    {
        case noMove:
            if ((input >= 540) && (input <= 560))
                state = noMove;
            else if (input < 540)
                state = left;
            else if (input > 560)
                state = right;
            break;

        case left:
            if (input < 540)
                state = left;
            else if ((input >= 540) && (input <= 560))
                state = noMove;
            else if (input > 560)
                state = right;
            break;

        case right:
            if (input > 560)
                state = right;
            else if ((input >= 540) && (input <= 560))
                state = noMove;
            else if (input < 540)
                state = left;
            break;

        default:
            state = noMove;
            break;
    }               // Transitions end

    switch (state)  // State actions begin
    {
        case noMove:               
            break;

        case left:
            if (pattern == 0x80)           
                pattern = 0x01;               
            else if (pattern < 0x80)            
                pattern = (pattern << 1);   

            //row = (0xFF - pattern); 
            break;

        case right:
            if (pattern == 0x01)
                pattern = 0x80;
            else if (pattern > 0x01)
                pattern = (pattern >> 1);

            //row = (0xFF - pattern); 
            break;
    }               // State actions end

    return state;
}

enum LEDMatrix {display};
int LEDMatrixSM(int state)
{
    switch (state)  // Transitions begin
    {
        case display:
            state = display;
            break;

        default:
            state = display;
            break;
    }               // Transitions end

    switch (state)  // State actions begin
    {
        case display:    
            PORTC = pattern;
            PORTD = row;
            break;
    }               // State actions end

    return state;
}

int main(void) 
{
    DDRA = 0x00; PORTA = 0xFF;  // Joystick input
    DDRC = 0xFF; PORTC = 0x00;  // LED Matrix pattern
    DDRD = 0xFF; PORTD = 0x00;  // LED Matrix rows

    static task task1, task2, task3;
    task *tasks[] = {&task1, &task2, &task3};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    task1.state = stop;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &SpeedSM;

    task2.state = noMove;
    task2.period = 50;
    task2.elapsedTime = task2.period;
    task2.TickFct = &JoystickSM;

    task3.state = display;
    task3.period = 50;
    task3.elapsedTime = task3.period;
    task3.TickFct = &LEDMatrixSM;

    unsigned long GCD = tasks[0]->period;
    for (unsigned short k = 1; k < numTasks; k++)
    {
        GCD = findGCD(GCD, tasks[k]->period);
    }

    TimerSet(GCD);
    TimerOn();
    A2D_init();

    while (1) 
    {
        for (int i = 0; i < numTasks; i++ ) {
            if ( tasks[i]->elapsedTime == tasks[i]->period ) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += GCD;
        }

        while (!TimerFlag);
        TimerFlag = 0;
    }

    return 1;
}
