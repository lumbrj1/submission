/*
 * DA2a2.c
 *
 * Created: 2/22/2020 11:00:08 AM
 * Author : John Paulo Lumbres 
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

int main(void) 
{
	//duty cycle: 55%
	//period: .75 sec
	//LED should be on for .4125 sec = 6600000 cycles
	//LED should be off for .3375 sec = 5400000 cycles
	DDRB=0x08;	//set PORTB 3 to output 
	
	while(1) 
	{
		PORTB |= (1<<PB3);	//turn on LED 
		_delay_ms(412.5);	//delay for .4125 sec 
		PORTB &=~(1<<PB3);	//turn off LED 
		_delay_ms(337.5);	//delay for .3375 sec 
	}	
	return 0;
}