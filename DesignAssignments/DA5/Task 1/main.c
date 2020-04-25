/*
 * DA5_T1.c
 *
 * Created: 4/21/2020 10:09:41 AM
 * Author : John Paulo Lumbres
 */ 

/*LM35 is broken, tried applying direct heat, yet value still doesn't change*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#define UBRR_9600 103
#define LATCH_DIO 4 //PD4
#define CLK_DIO 7 //PD7
#define DATA_DIO 0 //PB0

//declaring variable for controlling and storing values
volatile unsigned int adc_val;
//number 0 to 9 
const uint8_t segment_map[] ={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0X80, 0X90};

//digit select
const uint8_t segment_select[] = {0xF1,0xF2,0xF4,0xF8};

void adc_init(void)
{
	//AVcc reference, left adjust, ADC4 = PC4 = LM35,ADC enable
	ADMUX =(1<<REFS0)|(1<<ADLAR)|(1<<MUX2);	
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1);	//prescaler 64
}

void read_adc(void)
{
	unsigned char i=4;
	adc_val = 0;	//initial value
	while(i--){	//get 4 values of ADC
		ADCSRA |= (1<<ADSC);
		while((ADCSRA & (1<<ADIF))==0);
		ADCSRA |= (1<<ADIF);
		adc_val+=ADCH;
		_delay_ms(50);
	}
	adc_val = adc_val/4;	//have the average
}

void display(uint8_t sel, uint8_t light){
	for(uint8_t i=0; i<8; i++){	//writing data of segment map
		if (0 == (light & _BV(7 - i))) {
			PORTB &= (0<<DATA_DIO);
		} else {
			PORTB |= (1<<DATA_DIO);
		}
		//send pulse to clk dio 
		PORTD |=(1<<CLK_DIO);
		PORTD &=(0<<CLK_DIO);
	}
	for (uint8_t i=0; i<8; i++){	//writing data of segment select
		if (0 == (sel & _BV(7 - i))) {
			PORTB &= (0<<DATA_DIO);
		} else {
			PORTB |= (1<<DATA_DIO);
		}
		//send pulse to clk dio
		PORTD |=(1<<CLK_DIO);
		PORTD &=(0<<CLK_DIO);
	}
}

int main(void)
{
	DDRC &= ~(1<<PC4);	//pc4 as input 
	PORTC |=(1<<PC4);	//pc4 enabled 
	DDRB |= (1<<DATA_DIO);	//data_dio is output for portb
	DDRD |= (1<<CLK_DIO)|(1<<LATCH_DIO);
	//clk_dio and latch_dio are output for portd
	adc_init();		//initialize adc 
	int val1, val2, val3;	//declare int variables
    while (1) 
    {
		read_adc();	//read adc val from sensor 
		//mod adc val to get one's value 
		val1 = (adc_val)%10;	
		//divide and mod adc val to get ten's place value 
		val2 = (adc_val/10)%10;	
		//divide and mode val to get the hundred's place value 
		val3 = (adc_val/100)%10;
		
		for(int i=0; i<101;i++){	//repeat 100 times 
			PORTD &= ~(1<<LATCH_DIO);	//latch to get values
			//display one's value on the last 7 segment display  
			display(segment_select[3],segment_map[val1]);	
			PORTD |= (1<<LATCH_DIO);	//close latch 
			_delay_ms(5);	
			PORTD &= ~(1<<LATCH_DIO);
			//display ten's value on the 2nd to the last 7 segment display 
			display(segment_select[2],segment_map[val2]);	
			PORTD |= (1<<LATCH_DIO);
			_delay_ms(5);
			PORTD &= ~(1<<LATCH_DIO);
			//display hundred's value on the first 7 segment display 
			display(segment_select[1],segment_map[val3]);	
			PORTD |= (1<<LATCH_DIO);
			_delay_ms(5);
		}
    }
}

