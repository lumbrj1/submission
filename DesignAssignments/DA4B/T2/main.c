/*
 * DA4b_T2.c
 *
 * Created: 4/10/2020 10:57:42 PM
 * Author : John Paulo Lumbres
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#define F_CPU 8000000UL
#define UBRR_9600 103

volatile unsigned int adc_val;
char outs[20];

void adc_init(void)
{
	ADMUX = (0<<REFS1)|
	(1<<REFS0)|	//AVcc reference
	(1<<ADLAR)|	//left adjust
	(0<<MUX2)|
	(0<<MUX1)|
	(0<<MUX0);	//ADC0 = PC0 = potentiometer
	ADCSRA = (1<<ADEN)|	//ADC enable
	(0<<ADSC)|
	(0<<ADATE)|
	(0<<ADIF)|
	(0<<ADIE)|
	(1<<ADPS2)|
	(1<<ADPS1)|
	(1<<ADPS0);	//prescaler 128
}

void read_adc(void)
{
	unsigned char i=4;
	adc_val = 0;	//initial value
	while(i--){	//get 4 values of ADC
		ADCSRA |= (1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		adc_val+=ADC;
		_delay_ms(50);
	}
	adc_val = adc_val/4;	//have the average
}

void USART_init(unsigned int ubrr) {	//initializes USART
	UBRR0H = (unsigned char)(ubrr>>8);	//seting UBBR0
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);	//enable receiver, transmitter & RX interrupt
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);	//asynchronous 8 N 1
}

void USART_tx_string(char *data) {	//void to display on terminal
	while((*data !='\0')){
		while(!(UCSR0A &(1<<UDRE0)));	//while not done reading
		UDR0 = *data;
		data++;	//keep moving to next bit
	}
}

unsigned char uart_receive(void){
	while(!(UCSR0A &(1<<7)));	//while rxc0 is not set, keep reading bits
	return UDR0;
}

void timer_init(){
	TCNT1 = 0;		/* Set timer1 count zero */
	ICR1 = 4999;		/* Set TOP count for timer1 in ICR1 register */

	/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
	TCCR1A = (1<<WGM11)|(1<<COM1A1);
	TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
}

int main(void)
{
	DDRB |= (1<<PB1);	// Make OC1A pin as output 
	DDRC &= ~(1<<PC0);	// PCO input, potentiometer
	adc_init();
	timer_init();
	USART_init(UBRR_9600);
	int	position;
	while(1)
	{
		read_adc();	//read value of potentiometer
		position = 100 + (adc_val/10);	//configure the value of potentiometer for better fit the range
		USART_tx_string("\r\nposition : ");	//display position value
		snprintf(outs,sizeof(outs),"%3d\r\n",position);
		USART_tx_string(outs);
		if (position<=200){	
			OCR1A = 200;	//set to 0 degree
		} else if (position>=550) {
			OCR1A = 550;	//set to 180 degree
		} else {
			OCR1A = position;	//varying degree based on pot value
		}
	}
}
