/*
 * DA6_T2.c
 *
 * Created: 4/30/2020 11:03:05 PM
 * Author : John Paulo Lumbres
 */ 

/*
 * ATmega16 Interface with MPU-6050
 * http://www.electronicwings.com
 *
 */ 


#define F_CPU 16000000UL /* Define CPU clock Frequency e.g. here its 8MHz */
#include <avr/io.h>	/* Include AVR std. library file */
#include <util/delay.h>	/* Include delay header file */
#include <inttypes.h>/* Include integer type header file */
#include <avr/interrupt.h>
#include <stdlib.h>	/* Include standard library file */
#include <stdio.h>/* Include standard library file */
#include "MPU6050_def.h"/* Include MPU6050 register define file */
#include "i2c_master.h"/* Include I2C Master header file */
#include "uart.h"/* Include USART header file */

float Acc_x,Acc_y,Acc_z,Temperature,Gyro_x,Gyro_y,Gyro_z;

void MPU6050_Init()	/* Gyro initialization function */
{
	_delay_ms(150);/* Power up time >100ms */
	I2C_Start_Wait(0xD0);/* Start with device write address */
	I2C_Write(SMPLRT_DIV);/* Write to sample rate register */
	I2C_Write(0x07);/* 1KHz sample rate */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(PWR_MGMT_1);/* Write to power management register */
	I2C_Write(0x01);/* X axis gyroscope reference frequency */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(CONFIG);/* Write to Configuration register */
	I2C_Write(0x00);/* Fs = 8KHz */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(GYRO_CONFIG);/* Write to Gyro configuration register */
	I2C_Write(0x18);/* Full scale range +/- 2000 degree/C */
	I2C_Stop();

	I2C_Start_Wait(0xD0);
	I2C_Write(INT_ENABLE);/* Write to interrupt enable register */
	I2C_Write(0x01);
	I2C_Stop();
}

void MPU_Start_Loc()
{
	I2C_Start_Wait(0xD0);	/* I2C start with device write address */
	I2C_Write(ACCEL_XOUT_H);/* Write start location address from where to read */ 
	I2C_Repeated_Start(0xD1);	/* I2C start with device read address */
}

void Read_RawValue()
{
	MPU_Start_Loc();	/* Read Gyro values */
	Acc_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Acc_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Temperature = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_x = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_y = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Ack());
	Gyro_z = (((int)I2C_Read_Ack()<<8) | (int)I2C_Read_Nack());
	I2C_Stop();
}

/*variables, definitions for filter*/
float pitch, roll;
volatile float accData[2], gyrData[2];
volatile float pitchAcc, rollAcc;
volatile char buffer[20], float_[10];
#define ACCELEROMETER_SENSITIVITY 16384.0
#define GYROSCOPE_SENSITIVITY 16.4 
#define dt 0.01

void ComplementaryFilter()
{
	//float pitchAcc, rollAcc;
	// Integrate the gyroscope data -> int(angularSpeed) = angle
	pitch += ((float)gyrData[0] / GYROSCOPE_SENSITIVITY) * dt;
	// Angle around the X-axis
	roll -= ((float)gyrData[1] / GYROSCOPE_SENSITIVITY) * dt;
	// Angle around the Y-axis
	// Compensate for drift with accelerometer data if !bullshit
	// Sensitivity = -2 to 2 G at 16Bit -> 2G = 32768 && 0.5G = 8192
	int forceMagnitudeApprox = abs(accData[0]) + abs(accData[1]) + abs(accData[2]);
	if (forceMagnitudeApprox > 8192 && forceMagnitudeApprox < 32768)
	{
		// Turning around the X axis results in a vector on the Y-axis
		pitchAcc = atan2f((float)accData[1], (float)accData[2]) * 180 / M_PI;
		pitch = pitch * 0.98 + pitchAcc * 0.02;
		// Turning around the Y axis results in a vector on the X-axis
		rollAcc = atan2f((float)accData[0], (float)accData[2]) * 180 / M_PI;
		roll = roll * 0.98 + rollAcc * 0.02;
	}
}

void timer1_init(){
	TCCR1A |= (1<<COM1A0);	//toggle on compare
	TCCR1B |=(1<<WGM12)|(1<<CS11);	//CTC mode, prescaler 8
	TIMSK1|=(1<<TOIE1);	//enable overflow interrupt 
	OCR2A = 19999;	//10 ns 
	TCNT1=0;	
	sei();	//enable global interrupt 
}

int main()
{
	timer1_init();
	I2C_Init();		/* Initialize I2C */
	MPU6050_Init();		/* Initialize MPU6050 */
	USART_Init(9600);	/* Initialize USART with 9600 baud rate */
	
	while(1)
	{
		Read_RawValue();
		/*Take raw values and store them into array*/
		accData[0]=Acc_x;
		accData[1]=Acc_y;
		accData[2]=Acc_z;
		
		gyrData[0]=Gyro_x;
		gyrData[1]=Gyro_y;
		gyrData[2]=Gyro_z;
		
		/*display values of pitch and roll*/
		dtostrf( pitch, 3, 2, float_ );
		sprintf(buffer,"%s,\t",float_);
		USART_SendString(buffer);
			
		dtostrf( roll, 3, 2, float_ );
		sprintf(buffer,"%s,\r\n",float_);
		USART_SendString(buffer);
	}
}
ISR (TIMER1_OVF_vect) {
	/*do the filter every 10 ms*/
	TCNT1=0;
	ComplementaryFilter();
}