;
; DA2a.asm
;
; Created: 2/20/2020 6:36:33 PM
; Author : John Paulo Lumbres 
;

.include  <m328pbdef.inc>	
	;duty cycle: 55%
	;period: .75 sec
	;LED should be on for .4125 sec	= 6600000 cycles 
	;LED should be off for .3375 sec = 5400000 cycles
DC:
	sbi DDRB, 3		;set portb 3 as output
	sbi PORTB, 3	;light up LED
	call delay_4125ms	;call delay for .4125 sec
	cbi PORTB, 3	;turn off LED 
	call delay_3375ms	;call delay for .3375 sec 
	rjmp DC			;jump back to DC 

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

delay_3375ms:	;delay: 5401873 cycle = .3376 sec 
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

	