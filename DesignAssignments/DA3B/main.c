/*
 * DA_3B.c
 *
 * Created: 3/14/2020 6:06:26 PM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#define UBRR_9600 103 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

volatile int t_overflow;		//declaring global variables 
volatile unsigned int adc_temp;
char outs[20];

void adc_init(void)
{			
	ADMUX = (0<<REFS1)|	//reference selection bits	
	(1<<REFS0)|			//AVcc - external cap at AREF
	(0<<ADLAR)|			//ADC Left Adjust Result 
	(1<<MUX2)|			//Analog Channel Selection bits 
	(0<<MUX1)|			//PC5 (ADC5)
	(1<<MUX0);
	ADCSRA = (1<<ADEN)|	//ADC enable 
	(0<<ADSC)|			//ADC start conversion 
	(0<<ADATE)|			//ADC auto trigger enable 
	(0<<ADIF)|			//ADC interrupt flag
	(0<<ADIE)|			//ADC interrupt enable 
	(1<<ADPS2)|			//ADC prescaler select bits
	(0<<ADPS1)|			
	(1<<ADPS0);			//select channel 
}

void read_adc(void){
	unsigned char i=4;	
	adc_temp = 0;
	while(i--){
		ADCSRA |=(1<<ADSC);
		while(ADCSRA & (1<<ADSC));
		adc_temp+= ADC;
		_delay_ms(50);
	}
	adc_temp = adc_temp/4;	//average a few samples 
}

void USART_init(unsigned int ubrr) {
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<TXEN0);	//enable receiver, transmitter & RX interrupt 
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);	//asynchronous 8 N 1 
}

void USART_tx_string(char *data) {
	while((*data !='\0')){
		while(!(UCSR0A &(1<<UDRE0)));
		UDR0 = *data;
		data++;
	}
}

void timer_init(void){
	//TCNT = ((16MHz/256)*.1)-1 =6249
	TCCR1B = (1<<CS12);		//prescaler = 256 
	TIMSK1 = (1<<TOIE1);	//enable overflow 
	TCNT1 = 59284;			//TCNT1 = 65535 - 6250 = 59284
	t_overflow=0;			//overflow counter = 0
	sei();					//enable global interrupt
}

int main(void){
	int ftemp;	
	adc_init();		//initialize adc 
	timer_init();	//initialize timer 
	USART_init(UBRR_9600);	//intialize usart
	while(1)
	{
		read_adc();	//reads from ADC5 
		ftemp = adc_temp;	
		ftemp = (((adc_temp-32)*5)/9);	//stores the Fahrenheit conversion of the adc_temp 
		USART_tx_string("Fahrenheit: ");	//displays fahrenheit value 
		snprintf(outs,sizeof(outs),"%3d\r\n",adc_temp);	
		USART_tx_string(outs);
		USART_tx_string("Centigrade: ");	//displays centigrade value 
		snprintf(outs,sizeof(outs),"%3d\r\n\n",ftemp);
		USART_tx_string(outs);
		t_overflow=0;		//reset overflow counter 
		while(t_overflow<5){};	//.5 sec delay 
	}
}

ISR(TIMER1_OVF_vect){
	t_overflow++;	//increase overflow counter 
	TCNT1=59286;	//reset TCNT1 
}
