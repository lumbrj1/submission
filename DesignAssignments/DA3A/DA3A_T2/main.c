/*
 * DA3A_T2.c
 *
 * Created: 3/10/2020 6:39:23 PM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#define BAUDRATE 9600
#define BAUD_PRESCALLER (((F_CPU / (BAUDRATE * 16UL))) -1)

void USART_init(void) {
	UBRR0H = (uint8_t)(BAUD_PRESCALLER >> 8);
	UBRR0L = (uint8_t)(BAUD_PRESCALLER);
	UCSR0B = (1 << TXEN0)|(0<<RXEN0); // Enable receiver, transmitter & RX interrupt//asynchronous 8 N 1  // mega328
	UCSR0C = (0 << UMSEL01) |
	(0 << UMSEL00) | // 00 async operation, 01 synch operation
	(0 << UPM01) | // Parity -0 Disabled, 0 Reserved, 1 Enabled Even, 1 Enabled Odd
	(0 << UPM00) | // Parity -0 Disabled, 1 Reserved, 0 Enabled Even, 1 Enabled Odd
	(0 << USBS0) |// stop Bits -0 = 1bit 1 = 2bit
	(1 << UCSZ01) |// 8 Data bits
	(1 << UCSZ00) |//
	(0 << UCPOL0);// for Synch Mode only -clock polarity
}

void USART_tx_string( char *data ) {
	while ((*data != '\0')) {	
		while (!(UCSR0A & (1 <<UDRE0)));
		UDR0 = *data;
		data++;
	}
}

volatile unsigned integer=1;	//creating an unsigned variable to display 
volatile float float_val=1.1;	//creating a float variable to display 
char outs[20];	//part of the lecture 

int main(void) {
	USART_init();	//calls unto the usart_init function 
	
	TIMSK1 =(1<<TOIE1); //enables overflow interrupt 
	TCCR1B = 5;	//prescaler 1024
	TCNT1 = 49911;	//65535-(16e6/1024)-1 = 49911, 1 sec delay 
	sei();	//enable global interrupt
	
	while (1)
	{
	}
}

ISR (TIMER1_OVF_vect)
{
	USART_tx_string("\r\nHello World!!\r\n");			//output hello world 
	snprintf(outs, sizeof(outs),"%d\r\n", integer);		//output the value on the variable integer
	USART_tx_string(outs);
	snprintf(outs, sizeof(outs),"%f\r\n", float_val);	//output the value on the variable float_val
	USART_tx_string(outs);
	
	integer++;			//increase integer by 1 
	float_val+=.222;	//increase the floating value by .222
	TCNT1 = 49911;		//reset Timer1
}
