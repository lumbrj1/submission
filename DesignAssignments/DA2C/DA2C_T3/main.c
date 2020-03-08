/*
 * DA2C_T3_C.c
 *
 * Created: 3/5/2020 5:43:41 PM
 * Author : John Paulo Lumbres 
 */ 
#define F_CPU 16000000UL
#include <avr/interrupt.h>
#include <avr/io.h>

volatile int timerOverFlow1=0;	//global variables declaration
volatile int TimerOverFlow2=0;

int main(void)
{
    DDRB = 0x0c;	//make PB2 and PB3 output 
	DDRC &= (0<<PC3);	//make PC3 input 
	PORTB |= (1<<PB2);	//turn off PB2 LED
	
	TCCR0A |= (1<<COM0A0)|(1<<WGM01);	//CTC mode and COMPA0 on 
	TCCR0B = 4;	//set prescaler to 256
	TIMSK0 = 2;	//enable output compare match A interrupt 
	TCNT0 = 0;	//set TCNT0 to 0
	
	sei();	//enable global interrupt
	OCR0A =255;	//set new top as 255
	
    while (1) 
    {
		if (PINC & (1<<PC3)) {}	//checks if PC3 is pressed
		else {
			TimerOverFlow2=0;	//reset timerOverFlow2
			PORTB &=~(1<<PB2);	//turn on LED PB2
		}
		if (timerOverFlow1<101) //if timerOverFlow1 is less than 101
		{
			PORTB |= (1<<PB3);	//turn on LED on PB3 
		}
		if (timerOverFlow1>=101) //if timerOverFlow1 is greater than or equal to 101
		{
			PORTB &= ~(1<<PB3);	//turn off LED on PB3 
		}
		if (timerOverFlow1>182) //if timerOverFlow1 is greater than 182
		{
			TCNT0 =0;	//reset TCNT0 
			while (TCNT0<30){};	//delay 
			timerOverFlow1=0;	//reset timerOverFlow1
		}
		if (TimerOverFlow2>=490) //if timerOverFlow2 is greater than or equal to 490
		{
			PORTB |= (1<<PB2);	//turn on LED on PB2 
			TimerOverFlow2=0;	//reset timerOverFlow2
		}
    }
}

ISR (TIMER0_COMPA_vect)
{
	timerOverFlow1++;	//increase the counters by 1 
	TimerOverFlow2++;	
}
