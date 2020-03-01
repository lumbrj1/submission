/*
 * DA2b_C.c
 *
 * Created: 2/28/2020 6:48:17 PM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main(void)
{
	DDRB = 1<<2;		//set PB2 as output
	DDRC = 0b11110111; 	//set PC3 as input
	PORTD = 1<<2;		//set INT0 to 1 
	EICRA = 0x2;		//Falling edge of INT0 generates interrupt  
	EIMSK = (1<<INT0);	//external interrupt enable 
	sei();				//set global interrupt to 1; 
	
	PORTB |=(1<<2);		//set LED to low
	while(1)
	{
		if (PINC & (1<<PC3))	//if switch is not pressed 
		{
			PORTD |= (1<<2);	//set INT0 to 1 
		} else {				//else if switch is pressed 
			PORTD &=~(1<<2);	//set INT0 to 0 to generate interrupt 
		}
	}
}
ISR (INT0_vect)	
{
	PORTB = 0;			//turn on LED 
	_delay_ms(2000);	//delay for 2 sec 
	PORTB = 4;			//turn off LED 
}
