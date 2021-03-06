/*	Author: Luke McFadden
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab 7  Exercise 2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://youtu.be/SlwREkzOmGg
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void ADC_init()
{
    ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
}

int main(void) 
{
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    ADC_init();

    while (1) 
    {
        unsigned short x = ADC;
        unsigned char tempB = 0x00;
        unsigned char tempD = 0x00;

        tempB = (char)x;
        tempD = (char)(x >> 8);

        PORTB = tempB;
        PORTD = tempD;
    }

    return 1;
}
