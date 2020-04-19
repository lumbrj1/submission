/*
 * midterm2.c
 *
 * Created: 4/13/2020 4:06:33 PM
 * Author : John Paulo Lumbres
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#define MTR_1 5
#define MTR_2 4 
#define SW (PINC&(1<<1))
#define UBRR_9600 103

//declaring variable for controlling and storing values
volatile unsigned int power, revCtr0=0, revCtr1=0, escape=0, valcap0=20, valcap1=20;
volatile unsigned int rise0_0, fall0_0, rise0_1, fall0_1;
volatile unsigned int rise1_0, fall1_0, rise1_1, fall1_1;
volatile unsigned int t1ovf=0, t2ovf=0;
volatile unsigned int res1=0, res2=0, res4=0;
volatile float rpm1, rpm2, rpm4;
volatile unsigned int adc_val;
char outs[20];

void adc_init(void)
{
	ADMUX =(1<<REFS0)|(1<<ADLAR);	//AVcc reference, left adjust, ADC0 = PC0 = potentiometer,ADC enable	
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

void timer0_init(){
	TCCR0A = (1<<COM0A1)|(1<<WGM01)|(1<<WGM00);	//fast PWM 
	TCCR0B = (1<<CS01);	//prescaler 8 
}

void timer1_init(){
	TCCR1A = 0;	//normal mode 
	TCCR1B = (1<<CS11);	//prescaler 8 
	TIMSK1 = 1;	//overflow interrupt enabled 
}

void timer2_init(){
	TCCR2A = 0; //normal mode
	TCCR2B = (1<<CS21);	//prescaler 8 
	TIMSK2 = 1;	//overflow interrupt enabled 
}

void pc_int(){
	PCMSK1 = (1<<PCINT9);	//INT9 (PC1) enabled
	PCICR = (1<<PCIE1);	//Pin change interrupt enabled 
}

void int_interrupt(){
	EIMSK = 0x03;	//INT0 and INT1 external interrupt
	EICRA = 0x0f;	//rising edge for both 
	
}

void calculate(){
	//factor to multiply ticks by to get rsm 
	//(1920)/16000000 = 0.00096
	//rpm = 60/rms 
	_delay_ms(100);
	//get average of the 20 values 
	res1= res1/20;
	res2= res2/20;
	res4= res4/20;
	
	//using the value and formula calculated above
	rpm1 = 60/((float)res1 * 0.00096);
	rpm2 = 60/((float)res2 * 0.00096);
	rpm4 = 60/((float)res4 * 0.00096);
}
void initial() {
	rpm1=rpm2=rpm4=0;	//initial values
	escape=1;
	valcap0=20;
	valcap1=20;
}


int main(void)
{
	int spd_control;	//declare a variable
	adc_init();	//initialize adc 
	USART_init(UBRR_9600);	//initialize usart 
	timer0_init();	//initialize timers 
	timer1_init();
	timer2_init();
	pc_int();	//initialize interrupts 
	int_interrupt();
	DDRC &= ~((1<<PC1)|(1<<PC0));	//PC1 and PC0 are inputs
	PORTC = (1<<PC1);	//pull-up enabled
	DDRD |= (1<<PD6)|(1<<PD5)|(1<<PD4);	//PD6, PD5, PD4 outputs
	DDRD &= ~((1<<PD3)|(1<<PD2));	//PD2, PD3 inputs 
	PORTD =0;
	sei();	//global interrupt enabled
	
	while(1) 
	{
		read_adc();	//get ADC value from ADC0
		spd_control=adc_val*5;	//due to potentiometer problem i had to calibrate value
		USART_tx_string("\r\nspeed value : ");	//display spd_value value to ensure it doesn't exceed DC 95% 
		snprintf(outs,sizeof(outs),"%3d\r\n",spd_control);
		USART_tx_string(outs);
		_delay_ms(1000);
		if (spd_control>=255){	//max speed is 255
			OCR0A = 255;
		} else {
			OCR0A = spd_control;
		}
		escape=0;
		while ((escape==0)&(power==1)){	//measure 1x, 2x, 4x under a set speed
			if (valcap1==0 && valcap0==0){	//if program got 20 values of capture
				calculate();	//calculate rpm for 1x, 2x, 4x 
				USART_tx_string("\r\n1X: ");	//display all values calculated 
				snprintf(outs, sizeof(outs), "%f\r\n ", (float)rpm1);
				USART_tx_string(outs);
				USART_tx_string("\r\n2X: ");
				snprintf(outs, sizeof(outs), "%f\r\n ", (float)rpm2);
				USART_tx_string(outs);
				USART_tx_string("\r\n4X: ");
				snprintf(outs, sizeof(outs), "%f\r\n ", (float)rpm4);
				USART_tx_string(outs);
				initial();	//reset control variables
			}
		}
	}
}

ISR (PCINT1_vect){
	if (PINC &(1<<PC1)) {	//sense only one edge 
		if (power==0){	//if motor is off
			_delay_ms(50);
			USART_tx_string("\r\nturn on\r\n");
			power=1;
			TCNT0=0;
			PORTD |= (1<<MTR_1);	//turn on motor CCW
			PORTD &= ~(1<<MTR_2);
			_delay_ms(500);
		} else {	//if motor is on
			_delay_ms(50);
			USART_tx_string("\r\nturn off\r\n");
			power=0;
			PORTD &= ~(1<<MTR_1);	//turn motor off 
			PORTD &= ~(1<<MTR_2);
			_delay_ms(500);
		}
	}
}

ISR (TIMER1_OVF_vect){
	if (power){	//if motor is on 
		t1ovf++;	//start counting the overflow 
	} 
}
ISR (TIMER2_OVF_vect){
	if (power){	//if motor is on 
		t2ovf++;	//start counting the overflow 
	}
}

ISR (INT0_vect){
	if ((power==1) & (valcap0>0)){ //if motor is on and there's no 20 data read yet 
		if (revCtr0==0){	//read the first rise 
			rise0_0 = ((uint32_t)(TCNT1) + (uint32_t)(t1ovf * 0x10000));	//take tick at the moment of first rise 
			EICRA &= ~(1<<ISC00);	//change to sensing falling edge 
			revCtr0++;	
		} else if (revCtr0==1){	//read the first fall 
			fall0_0 =  ((uint32_t)(TCNT1) + (uint32_t)(t1ovf * 0x10000));	//take tick at the moment of first fall 
			EICRA |= (1<<ISC00);	//change to sensing rising edge 
			revCtr0++;
		} else if (revCtr0==2){	//read the second rising edge 
			rise0_1 =  ((uint32_t)(TCNT1) + (uint32_t)(t1ovf * 0x10000));	//take tick at the moment of second rise 
			EICRA &= ~(1<<ISC00);	//change to sensing falling edge 
			revCtr0++;	
		} else if (revCtr0==3){	//read the second falling edge 
			fall0_1 = ((uint32_t)(TCNT1) + (uint32_t)(t1ovf * 0x10000));	//take tick at the moment of second fall 
			EICRA |= (1<<ISC00);	//change to sensing rising edge 
			revCtr0=0;	//restart capture 
			t1ovf=0;	//reset overflow
			res1 += (rise0_1 - rise0_0);	//keep adding the difference between first rise and second rise 
			res2 += (((rise0_1 - rise0_0) + (fall0_1 -fall0_0))/2);	//avg of difference between rising and falling 
			TCNT1=0;	//reset timer1
			valcap0--;	//decrease data counter 
		}
	}
}
ISR (INT1_vect){
	if ((power==1)  & (valcap1>0)){	//if motor is on and there's no 20 data read yet 
		if (revCtr1==0) { //read first rising edge 
			rise1_0 = ((uint32_t)(TCNT2) + (uint32_t)(t2ovf * 0x100));	//take the tick at the moment of first rise 
			EICRA &= ~(1<<ISC10);//change to sensing falling edge 
			revCtr1++;
		} else if (revCtr1==1){	//read first falling edge 
			fall1_0 = ((uint32_t)(TCNT2) + (uint32_t)(t2ovf * 0x100));	//take the tick at the moment of first fall 
			EICRA |= (1<<ISC10); //change to sensing rising edge 
			revCtr1++;
		} else if (revCtr1==2){	//read the second rising edge 
			rise1_1 = ((uint32_t)(TCNT2) + (uint32_t)(t2ovf * 0x100));	//take the tick at the momemnt of second rise 
			EICRA &= ~(1<<ISC10);	//change to sensing falling edge 
			revCtr1++;
		} else if (revCtr1==3){	//read the second falling edge 
			fall1_1 = ((uint32_t)(TCNT2) + (uint32_t)(t2ovf * 0x100));	//take the tick at the moment of second fall 
			EICRA |= (1<<ISC10);	//change to sensing rising edge 
			revCtr1=0;	//reset capture 
			t2ovf=0;	//reset overflow counter 
			TCNT2=0;	//reset timer2 
			//avg of ticks between rising edge and falling edge for both INT1 and INT0
			res4 += (((rise0_1-rise0_0) + (fall0_1-fall0_0)+ (rise1_1-rise1_0)+ (fall1_1-fall1_0))/4);
			valcap1--; // decrease data counter 
		}
	}
}	
