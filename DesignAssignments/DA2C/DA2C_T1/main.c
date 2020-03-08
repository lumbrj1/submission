/*
 * DC2C_T1_C.c
 *
 * Created: 3/2/2020 10:56:06 AM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>

int main(void)
{
	DDRB = 0x0c;	//make PB2 and PB3 output 
	DDRC &= (0<<PC3);	//make PC3 input 
	int timerOverFlow1=0, timerOverFlow2=0;	//variables for timer overflow counter 
	TCCR0B = 4;	//set prescaler to 256
	TCNT0 = 0;	//set timer to 0 
	PORTB |= (1<<PB2);	//turn off LED on PB2 
	
	while (1)
	{
		while ((TIFR0 & 0x01) == 0); 
		TCNT0 = 0;	//reset timer
		TIFR0 = 1; //reset overflow flag
		timerOverFlow1++;	//increase overflow counters
		timerOverFlow2++;
		
		if (PINC & (1<<PC3)){} //checks if PC3 is pushed
		else{
			timerOverFlow2=0;	//reset overflow timerOverFlow2 to 0 
			PORTB &=~(1<<PB2);	//turn on LED on PB2 
		}		
		
		if (timerOverFlow1<102) {	//if timerOverFlow1 is less than 102
			PORTB |= (1<<PB3);	//turn on PB3 LED 
		}
		if (timerOverFlow1>=102) {	//if timerOverFlow1 is greater than or equal to 102 
			TCNT0=0;			//reset TCNT0 
			while(TCNT0<27){};	//add extra delay 
			PORTB &= ~(1<<PB3);	//turn off PB3 LED 
		}
		if (timerOverFlow1>182)	{//if timerOverFlow1 is greater than to 182
			TCNT0=0;	//reset TCNT0
			while(TCNT0<184){};	//add extra delay 
			timerOverFlow1=0;	//reset timerOverFlow1 to  0 
		}
		if (timerOverFlow2>490)	{//if counter 2 is greater than 490
			PORTB |= (1<<PB2);	//turn off LED on PB2 
			timerOverFlow2=0;	//reset timerOverFlow2 
		}
	}
}
