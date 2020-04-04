/*
 * DA4A.c
 *
 * Created: 4/1/2020 3:02:33 PM
 * Author : John Paulo Lumbres
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#define SPEED 6
#define MTR_1 5
#define MTR_2 4 
#define SW (PINC&(1<<1))
#define F_CPU 16000000UL
#define UBRR_9600 103

volatile unsigned int adc_val;
volatile unsigned int enter=0;
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
	TCCR0A = (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);	// fast PWM
	TCCR0B = 0x05;	//prescaler 1024
	OCR0A = 0;	//initialize OCR0A to 0 i.e. min speed 
}

void interrupt_init(){
	PCICR = 2;	//enable PCMSK1 scan 
	PCMSK1=2;	//set PCINT1 to trigger an interrupt on state change
	sei();		//global interrupt enable 
}

int main(void)
{
	DDRC &= ~((1<<PC1)|(1<<PC0));	//PC1 and PC0 are inputs 
	int spd_control;	//declare a variable 
	adc_init();	//initialize adc 
	USART_init(UBRR_9600);	//initialize usart 
	timer_init();	//initialize timer0
	interrupt_init();	//initialize interrupt 
	PORTC |= (1<<PC1);	//pull-up enable
	DDRD = 0x70;	//make PD4,5,6 output 
	while(1) 
	{
		PORTD |= (1<<SPEED);	
		read_adc();	//get ADC value from ADC0
		spd_control = adc_val/10;	//divide ADC value by 10 so speed of motor is easier to control 
		if (spd_control>=242){	//adc value is bigger than max Timer0
			OCR0A = 242;	//95% DC 
		} else {
			OCR0A = spd_control;	//get speed of motor 
		}
		USART_tx_string("\r\nspeed value (OCR0A) : ");	//display OCR0A value to ensure it doesn't exceed DC 95% 
		snprintf(outs,sizeof(outs),"%3d\r\n",OCR0A);
		USART_tx_string(outs);
		_delay_ms(10000); //delay 
	}
}

ISR(PCINT1_vect){
	_delay_ms(1000);	
	if (enter==0){	//if first time on pressing the button, 
		PORTD &= ~(1<<MTR_1);	//turn on motor 
		PORTD |= (1<<MTR_2);
		enter++;
		USART_tx_string("\r\nturn on\r\n");	//display message 
	}
	else if (enter==1){	//necessary for when button goes up 
		enter++;
	}
	else if (enter==2){	//second time user pushed the button 
		PORTD &= ~(1<<MTR_1);	//turn off motor 
		PORTD &= ~(1<<MTR_2);
		enter++;
		USART_tx_string("\r\nturn off\r\n");
	}
	else if (enter==3){
		enter=0;	//reset counter 
	}
}


