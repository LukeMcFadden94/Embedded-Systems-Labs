/*	Author: Luke McFadden
 *	Lab Section: 023
 *	Assignment: Lab 2  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif	

int main(void) {
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRC = 0xFF; PORTC = 0x00; // Configure port C's 8 pins as outputs, initialize to 0s
	//unsigned char tempB = 0x00; // Temporary variable to hold the value of B
	unsigned char tempA = 0x00; // Temporary variable to hold the value of A
	unsigned char cntavail = 0x00;
	
	
	while(1) 
	{
		// Get how many cars are present
		tempA = PINA & 0x0F;	

		// Get values from each parking spot, add to total
		cntavail = ((tempA & 0x08) >> 3) + ((tempA & 0x04) >> 2) + ((tempA & 0x02) >> 1) + (tempA & 0x01); 

		// Assign PORTC equal to max 
		PORTC = 4 - cntavail;

		// If lot is full, set PC7 to 1
		if (cntavail == 4)
		{
			PORTC = PORTC | 0x80;
		}
	}
	
	return 0;
}


