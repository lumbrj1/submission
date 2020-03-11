/*
 * DA3A_T1.c
 *
 * Created: 3/10/2020 5:44:52 PM
 * Author : John Paulo Lumbres
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
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

volatile unsigned integer=1;	//declare unsigned variable 
volatile float float_val=1.1;	//declare float variable 
char outs[20];	//part of instructions

int main(void) {
	USART_init();	//call usart_init function 
	
	while (1)
	{
		USART_tx_string("\r\nHello World!!\r\n");	//display string 
		snprintf(outs, sizeof(outs),"%d\r\n", integer);	//display integer
		USART_tx_string(outs);
		snprintf(outs, sizeof(outs),"%f\r\n", float_val);	//display floating value 
		USART_tx_string(outs);
		_delay_ms(1000);	//delay for 1 sec
		
		integer++;	//increase value of integer 
		float_val+=.2;	//increase value of floating value 
	}
}
