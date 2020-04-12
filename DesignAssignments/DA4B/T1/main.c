/*
 * DA4B.c
 *
 * Created: 4/8/2020 5:18:19 PM
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
volatile unsigned int enter=0;
volatile unsigned int counter=0;
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
	TCCR1B = (1<<WGM12)|(1<<CS11);	//CTC mode, prescaler 256
	OCR1A = 65535;
	sei();	//enable global interrupt 
	TCNT1 = 0;	
}

int main(void)
{
	int spd_control; 
	DDRC &= ~(1<<PC0);				//make PC0 input 
	DDRD = 0xF0;					// Make PORTD lower pins as output 
	adc_init();			
	USART_init(UBRR_9600);
	timer_init();

	//period = 5000;					// Set period in between two steps of Stepper Motor 
	//min = 5ms, max = 3ms;

	while (1)
	{
		read_adc();
		spd_control = adc_val/10;	//divide adc_val to be easier to manipulate
		if (spd_control>=313){	
			OCR1A = 4999;	//min => 5ms 
		} else if (spd_control<=194) {
			OCR1A = 2999;	//max => 3ms 
		} else {
			OCR1A = spd_control*15;	//varying speed 
		}
		USART_tx_string("\r\nspeed value (OCR1A) : ");	//display OCR1A value to ensure it doesn't exceed DC 95%
		snprintf(outs,sizeof(outs),"%3d\r\n",OCR1A);
		USART_tx_string(outs);
		enter=0;	//initialize values 
		TCNT1=0;
		counter=0;
		TIMSK1 |= (1 << OCIE1A);	//enable COMPA interrupt 
		while(counter!=500){};		//sequence loader
		TIMSK1 &= ~(1 << OCIE1A);	//disable COMPA interrupt					
		_delay_ms(1000);
	}
}
ISR(TIMER1_COMPA_vect){
	/*plays the sequence in order to make the motor run */
	if (enter==0){	
		PORTD = 0x90;	
		enter+=1;
	}
	else if (enter==1){
		PORTD = 0x80;
		enter+=1;
	}
	else if (enter==2){
		PORTD = 0xC0;
		enter+=1;
	}
	else if (enter==3){
		PORTD = 0x40;
		enter+=1;
	}
	else if (enter==4){
		PORTD = 0x60;
		enter+=1;
	}
	else if (enter==5){
		PORTD = 0x20;
		enter+=1;
	}
	else if (enter==6){
		PORTD = 0x30;
		enter+=1;
	}
	else if (enter==7){
		PORTD = 0x10;
		enter=0;
		counter+=1;
	}
}