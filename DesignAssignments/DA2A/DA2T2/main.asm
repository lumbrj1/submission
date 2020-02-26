;
; DA2T2.asm
;
; Created: 2/23/2020 10:28:30 PM
; Author : John Paulo Lumbres
;


	cbi DDRC, 3		;set PORTC 3 as input
	sbi DDRB, 2		;set PORTB 2 as output 
	sbi PORTB, 2	;turn LED on PORTB 2 "OFF" 

DC:
	sbis PINC, 3	;skip next instruction if button is press
	jmp button		; jump to button section 
	
	sbi DDRB, 3		;set portb 3 as output 
	sbi PORTB, 3	;light up LED 
	call delay_4125ms	;call delay for .4125 sec 
	cbi PORTB, 3	;turn off LED 
	call delay_3375ms	;call delay for .3375 sec 
	rjmp DC			;jump back to DC 
		
button:
	cbi PORTB, 2	;turn on LED 
	call delay_2s	;call delay for 2 sec 
	sbi PORTB, 2	;turn off LED 
	rjmp DC			; jump back to DC 

delay_4125ms:	;delay: 6604679 cycle, .4128 sec 
	ldi r18, 43
loop:
	ldi r17, 207
wait:
	nop
	ldi r16, 246
delay:
	dec r16
	brne delay 
	dec r17  
	brne wait
	dec r18
	brne loop
	ret

delay_3375ms:	;delay: 5401865 cycle = .3376 sec 	
	ldi r18, 35
loop2:
	ldi r17, 208
wait2:
	nop
	ldi r16, 246
delay2:
	dec r16
	brne delay2 
	dec r17  
	brne wait2
	dec r18
	brne loop2

	ret

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

