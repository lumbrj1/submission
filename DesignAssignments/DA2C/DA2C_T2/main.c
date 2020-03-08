/*
 * DA2C_T2_C.c
 *
 * Created: 3/2/2020 10:27:06 PM
 * Author : John Paulo Lumbres 
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>

volatile int timerOverFlow1=0;	//global variable so counters can be access anywhere
volatile int timerOverFlow2=0;

int main(void)
{
	DDRB = 0x0c;	//make PB2 and PB3 output 
	DDRC &= (0<<PC3);	//make PC3 input 
	PORTB |= (1<<PB2);	//turn off LED on PB2
	
	TCCR0A = 0;
	TCCR0B = 4;	//set prescaler to 256 
	TCNT0 = 0;	//initial TCNT0 to 0 
	TIMSK0 = 1;	//enable overflow flag 
	
	sei();	//enable global interrupt 
    while (1) 
    {
		if (PINC & (1<<PC3)) {} //checks if button is pressed 
		else {
			timerOverFlow2=0;	//reset timerOverFlow to 0 
			PORTB &=~(1<<PB2);	//
		}
		if (timerOverFlow1<102){ //if timerOverFlow1 is less than 102 
			PORTB |= (1<<PB3);	//turn on LED on PB3 
		}
		if (timerOverFlow1>=102){	//if timerOverFlow1 is greater than or equal to 102 
			while(TCNT0<25){};	//delay
			PORTB &= ~(1<<PB3);		//turn off LED on PB3
		}
		if (timerOverFlow1>182)		//if timerOverFlow1 is greater than 182
		{	
			while(TCNT0<183){};		//delay
			timerOverFlow1=0;		//reset timerOverFlow1
		} 
		if (timerOverFlow2>490)
		{
			PORTB |= (1<<PB2);		//turn off led on PB2 
			timerOverFlow2=0;		//reset timerOverFlow2
		}
	}
}

ISR(TIMER0_OVF_vect)
{
	timerOverFlow1++;	//increase counters by 1 
	timerOverFlow2++;
}




