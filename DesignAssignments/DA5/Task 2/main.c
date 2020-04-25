/*
 * DA5_T2.c
 *
 * Created: 4/21/2020 3:36:49 PM
 * Author : John Paulo Lumbres
 */ 

/*LM35 is broken, tried applying direct heat, yet value still doesn't change*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define UBRR_9600 103
#define LATCH 2 //PD4
#define CLK 5 //PD7
#define DATA 3 //PB0
#define SHIFT_REGISTER DDRB
#define SHIFT_PORT PORTB 

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

void spi_init(){
	//SPI enabled, master mode 
	SPCR0 |= (1<<SPE)|(1<<MSTR);
	//open latch
	SHIFT_PORT &= ~(1<<LATCH);
}

void store(){
	//opens and close the latch to store 
	SHIFT_PORT |= (1<<LATCH);
	SHIFT_PORT &= ~(1<<LATCH);
}

void display(uint8_t data){
	//store data to SPDR0 register 
	SPDR0 = data;
	//send the data to slave 
	while(!(SPSR0 &(1<<SPIF)));
}

int main(void)
{
	DDRC &= ~(1<<PC4);	//PC4 as input 
	PORTC |=(1<<PC4);	//enable pc4 
	//have data, latch and clk outputs on portb
	SHIFT_REGISTER |= (1<<DATA)|(1<<LATCH)|(1<<CLK);	
	//initialize portb to 0; 
	SHIFT_PORT &= ~((1<<DATA)|(1<<LATCH)|(1<<CLK));
	adc_init();
	spi_init();
	
	int val1, val2, val3;
    while (1) 
    {
		read_adc();
		//mod adc val to get one's value 
		val1 = (adc_val)%10;
		//divide and mod adc val to get ten's place value 
		val2 = (adc_val/10)%10;
		//divide and mode val to get the hundred's place value 
		val3 = (adc_val/100)%10;
		for (int i=0; i<35000; i++) { //repeat to see display 
			display(segment_map[val1]);	//send one's value 
			display(segment_select[3]);	//send which led to turn on 
			store();	
			display(segment_map[val2]);	//send ten's value
			display(segment_select[2]);	//send which led to turn on 
			store();
			display(segment_map[val3]);	//send hundered's value
			display(segment_select[1]);	//send which led to turn on 
			store();
		}
    }
}