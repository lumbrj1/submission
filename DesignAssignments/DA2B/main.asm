;
; DA2B.asm
;
; Created: 2/27/2020 5:15:45 PM
; Author : John Paulo Lumbres
;
.include <m328pbdef.inc>
.org 0			;reset
jmp main	
.org 0x02		;INT0 interrupt
jmp INT0_vect
main:
	ldi r20, HIGH(RAMEND)
	out SPH, r20
	ldi r20, LOW(RAMEND)
	out SPL, r20		;initialize stack 

	cbi DDRC, 3		;set PORTC 3 as input33
	sbi DDRB, 2		;set PORTB 2 as output 
	sbi PORTB, 2	;turn LED on PORTB 2 "OFF" 
	sbi PORTD, 2	;pull-up activated  
	ldi r25, 8		;store constant value 8 on reg 25
	ldi r24, 2		;store constant value 2 on reg 24
	clr r23			;clear reg 23

	ldi r21, 0x2		;make INT0 falling edge triggered 
	sts EICRA,r21
	ldi r21, 1<<INT0	;enable INT0 
	out EIMSK, r20		
	sei					;enable interrupts 

here: 
	in r22, PINC	;take the value from PINC input 
	and r22, r25	;mask r22 with r25
	cpi r22, 8		;compare r22 with 8 to check if PC3 is pressed or not
	brne b1			;branch to b1 if button is not pressed
	out PORTD, r23	;change value of INT0 to 0 to trigger interrupt
b1: out PORTD, r24	;change back INT0 to 1
	jmp here		

INT0_vect:
	cbi PORTB, 2	;turn on LED 
	call delay_2s	;call delay for 2 sec 
	sbi PORTB, 2	;turn off LED 
	reti


delay_2s:	;delay: 32061655 cycles = 2.004 sec 
	clr r17			
	ldi r18, 165
l1: 
	ldi r19, 255
l2:
	ldi r20, 253
delay3:	
	dec r20
	brne delay3
	dec r19
	brne l2
	dec r18
	brne l1
	out  PORTC, r17
	ret

