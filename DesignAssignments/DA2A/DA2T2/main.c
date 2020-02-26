/*
 * DA2A_T2.c
 *
 * Created: 2/24/2020 12:04:15 AM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void)
{

	DDRB |= (1<<PB3);	//set PB3 as output 
	DDRB |= (1<<PB2);	//set PB2 as output 
	DDRC &= (0<<PC3);	//set PC3 as input 
	PORTC |=(1<<PC3);	//set PORTC.3 as 1 
	while(1)
		{
			if (PINC & (1<<PC3))	//if button is not pressed 
			{
				PORTB |= (1<<PB2);	//PB2 LED is off 
			}		
			else 
			{
				PORTB &=~(1<<PB2);	//PB2 LED is on
				_delay_ms(2000);	//delay for 2 sec 
				PORTB |= (1<<PB2);	//PB2 LED is off 
			}
			PORTB |= (1<<PB3);	//turn on LED 
			_delay_ms(412.5);	//delay for .4125 sec
			PORTB &=~(1<<PB3);	//turn off LED
			_delay_ms(337.5);	//delay for .3375 sec 
		}
	return 0; 
}

