/*
 * Midterm.c
 *
 * Created: 3/19/2020 6:21:35 PM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#define UBRR_9600 103 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>

volatile long total, loop;
volatile unsigned int adc_temp;	//global variables
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
		ADCSRA |=(1<<ADSC);	//keep reading 
		while(ADCSRA & (1<<ADSC));	//reading in signals
		adc_temp+= ADC;	//storing signals
		_delay_ms(50);
	}
	adc_temp = adc_temp/4;	//average a few samples 
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

void USART_flush(void) {
	unsigned char dummy;
	while (UCSR0A & (1<<RXC0))
	dummy = UDR0;
}

void timer_init(){	//initialize timer 
	TCCR1B = 5;	//prescaler 1024
	TIMSK1 = 1;	//enable overflow interrupt
	TCNT1 = 0;	//set TCNT1 to 0 
	sei();	//enable global interrupt
}
uint8_t uart_string(char *string){ //receives input characters and stores to the parameter pointer char 
	unsigned char x;	//declaring variables 
	uint8_t i=0;
	while(1){
		x = uart_receive();	//takes in the character from an input and store it on x 
		if ((x=='\r') || (x=='\n')){	//if the character receives is not null or next line
			string[i]=0; break;		//store the character 0 and break 
		} else {	
			string[i]=x;	//store the x to the string array 
			i++;	//increase the value of i 
		}
	}
	return i;	//return i 
}

int main(void){
	DDRB = 0b00100100;	//set PB5 and PB2 as output 
	int first_time = 0;	//declare variables 
	long delay, blink;
	int ftemp, temp;
	char message[50]; 	//declaring a char array with size of 50 
	timer_init();	//initialize timer 
	adc_init();		//initialize adc  
	USART_init(UBRR_9600);	//initialize usart
	PORTB |= (1<<PB5)|(1<<PB2);	//turn off all LEDs
	while(1)
	{
		read_adc();	//read value of sensor 
		ftemp = (((adc_temp-32)*5)/9);	//turn value from sensor to fahrenheit
		if (((message[0]=='h')& (message[1]=='\0'))|(first_time==0)){	//if reboot or 'h' is input 
			first_time=1;	//for reboot purpose 
			USART_tx_string("\r\nHELP SCREEN\r\n");	//print all these messages 
			USART_tx_string("\r\nh - help screen");
			USART_tx_string("\r\nt - temperature in C");
			USART_tx_string("\r\nT - temperature in F");
			USART_tx_string("\r\no - turn on LED");
			USART_tx_string("\r\nO - turn off LED");
			USART_tx_string("\r\ns - send string to terminal");
			USART_tx_string("\r\ni - send integer to terminal, blink LED\r\n\n");
		}
		if ((message[0]=='t')& (message[1]=='\0')){	//if input is 't'
			USART_tx_string("\r\nCentigrade: ");	//displays centigrade value
			snprintf(outs,sizeof(outs),"%3d\r\n",ftemp);	
			USART_tx_string(outs);
		}
		if ((message[0]=='T')& (message[1]=='\0')){	//if input is 'T'
			USART_tx_string("\r\nFahrenheit: ");	//displays fahrenheit value
			snprintf(outs,sizeof(outs),"%3d\r\n",adc_temp);
			USART_tx_string(outs);
		}
		if ((message[0]=='o')& (message[1]=='\0')){	//if input is 'o'
			PORTB &= ~(1<<PB5);	//turn on LED on PB5
			USART_tx_string("\r\nPB5 LED is on\r\n");	//display message to indicate the task has been done 
		}
		if ((message[0]=='O')& (message[1]=='\0')){	//if input is 'O'
			PORTB |= (1<<PB5);	//turn off LED on PB5
			USART_tx_string("\r\nPB5 LED is off\r\n");	//display message to indicate the task has been done 
		}
		if ((message[0]=='s')& (message[1]=='\0')){	//if input is 's'
			USART_tx_string("\r\ninsert string\nPress Y to stop sending strings");	//display to give user instructions 
			while(message[0]!='Y'){
				temp = uart_string(message);	//calls function to receive string
				if(message[0]!='Y'){		//if 'Y' is not pressed, continue displaying the received string 
					USART_tx_string(message);
				}
				USART_tx_string("\r\n");	//skips line 
			}	 
			USART_tx_string("\r\ndone sending strings\r\n");
		}
		if ((message[0]=='i')& (message[1]=='\0')){	//if input is 'i'
			USART_tx_string("\r\ninsert integer, integer will be converted to seconds of delays\r\n"); //display instructions 
			for(int i =0;i<50;i++){
				message[i]='\0';	//clears the message array 
			}
			while(message[0]=='\0'){	//while there's no input
				temp = uart_string(message);	//continue receiving input 
			}
			delay = atoi(message);	//changes string to integer
			delay = (15625*delay)-1;	//determines TCNT1
			blink = (int)(delay/65535);	//takes how many times overflow will occur 
			total=delay;	//have total store delay 
			loop=blink;		//have loop store blink
			if (delay>0){	//if delay is valid 
				TCNT1=0;	//reset TCNT1 timer
				PORTB &= ~(1<<PB2);	//turn on PB2 LED
				while(loop!=0){};	//loop til all overflow has been dealt with 
				TCNT1=0;	//reset timer 
				while(loop==0){	//while loop to continue doing the delay 
					if (TCNT1>total){	//if TCNT1 is greater than total 
						PORTB |= (1<<PB2);	//turn off LED
						loop=1;	//have loop=1 to exit while loop 
					}
				}
				USART_tx_string("\rdone blinking\r\n");	//display message 
			}
		}
		temp=uart_string(message);	//takes in input to start tasks
	}
}

ISR(TIMER1_OVF_vect){
	if (loop!=0){	//if loop equals 0 , then skip the following codes 
		loop-=1;	//decrease loop by 1 
		total-=65535;	//decrease total by the max of Timer 1 
	}
	TCNT1=0;	//reset timer 
}