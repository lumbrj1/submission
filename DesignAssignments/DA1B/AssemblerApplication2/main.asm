;
; designAsssignment1b.asm
;
; Created: 2/15/2020 9:41:13 AM
; Author : John Paulo Lumbres
;

.include <m328pbdef.inc>

;results of sum of 0x0300 => R3:R2:R1
;results of sum of 0x0600 => R6:R5:R4
;results of sum of 0x0500 => R9:R8:R7
;results of sum of 0x0700 => R12:R11:R10
;results of sum of 0x0800 => R15:R14:R13

;r25:r24 = 0x0800
;r23:r22 = 0x0700
;x = 0x0300
;y = 0x0500
;z = 0x0600

;r16 = value
;r17 = div7
;r18 = div3
;r19 = div7bool
;r20 = div3bool 
;r21 = quotient 

;labeling 
.def value = r16	;set value to r16
.def div7 = r17		;set div7 to r17
.def div3 = r18		;set div3 to r18
.def div7bool = r19	;set div7bool to r19
.def div3bool = r20	;set div3bool to r20
.def savedVal = r21	;set savedVal to r21 

.org 0
.cseg

;initializing 
	ldi r25, 0x08	;load 0x08 to r25
	ldi r23, 0x07	;load 0x07 to r23
	ldi YH, 0x05	;load 0x05 to YH
	ldi XH, 0x03	;load 0x03 to XH
	ldi ZH, 0x06	;load 0x06 to ZH
	ldi value, 25	;load 25 to value 
	ldi div7, 7		;load 7 to div7 
	ldi div3, 3		;load 3 to div3
	jmp loop		;jump to loop 

end:	
	jmp end	;continuous looping 

add300:
	clr div7bool	;clear div7bool 
	clr div3bool	;clear div3bool 
	mov div3bool, r3	;copy value of r3 to div3bool 
	mov div7bool, r2	;copy value of r2 to div7bool 
	mov savedVal, r1	;copy value of r1 to savedVal 
	add savedVal, value	;add savedVal and value 
	adc div7bool, r0	;add carry bit and div7bool 
	adc div3bool, r0	;add carry bit and div3bool 
	mov r3, div3bool	;copy value of div3bool to r3
	mov r2, div7bool	;copy value of div7bool to r2
	mov r1, savedVal	;copy value of savedVal to r1

loop:
	inc value	;increase value by 1 
	cpi value, 226	;compare value with 226 
	breq end	; branch to 3nd if equal 
	st X+, value	;store value to address X and X increaases by 1 

	clr div7bool	;clear div7bool 
	clr div3bool	;clear div3bool 
	mov savedVal, value ;copy value to savedVal
;divide by 7 
l1:	
	sub value, div7 ;subtract value by 7 
	brcc l1	;branch if carry is 0 
	
	add value, div7 ;add 7 to value
	tst value		;check if value is 0 
	brne divide3	;branch to divide3 if value is not 0 
	ldi div7bool, 1	;load 1 to div7bool 

divide3:	
	mov value, savedVal	;copy number of savedVal to value 

l2:
	sub value, div3	;subtract value by 3 
	brcc l2	;branch to l2 if carry is 0 

	add value, div3	;add 3 to value 
	tst value	;check if value is 0 
	brne addmem	;branch to addmem if value is not 0 
	ldi div3bool, 1 ;load 1 to div3bool 

addmem:
	mov value, savedVal		;copy savedVal to valu 
	tst div7bool			;check if div7bool is 0 
	breq check3				;branch to check3 if div7bool is 0 
	cp div7bool, div3bool	;compare div7bool and div3bool 
	breq bothdivisible		;branch to bothdivisible if they're the same 
	st Y+, value			;store value on address Y and increase Y by 1 
	clr div7bool			;clear div7bool 
	clr div3bool			;clear div3bool 
	mov div3bool, r9		;copy r9 to div3bool 
	mov div7bool, r8		;copy r8 to div7bool 
	mov savedVal, r7		;copy r7 to savedVal
	add savedVal, value		;add value and savedVal 
	adc div7bool, r0		;add div7bool and carry 
	adc div3bool, r0		;add div3bool and carry 
	mov r9, div3bool		;copy div3bool to r9
	mov r8, div7bool		;copy div7bool to r8
	mov r7, savedVal		;copy savedVal to r7
	jmp add300				;jump to add300
check3:
	tst div3bool			;check if div3bool is 0 
	breq nodivisible		;if zero, then branch to nodivisible 
	st Z+, value			;store value on address Z and increase Z by 1 
	clr div7bool			;clear div7bool 
	clr div3bool			;clear div3bool 
	mov div3bool, r6		;copy r6 to div3bool 
	mov div7bool, r5		;copy r5 to div7bool 
	mov savedVal, r4		;copy r4 to savedVal 
	add savedVal, value		;add savedVal and value		
	adc div7bool, r0		;add carry and div7bool 
	adc div3bool, r0		;add carry and div3bool 
	mov r6, div3bool		;copy div3bool to r6
	mov r5, div7bool		;copy div7bool to r5
	mov r4, savedVal		;copy savedVal to r4
	jmp add300				;jump to add300 
bothdivisible:				
	push YH					;push value of YH
	push YL					;push value of YL
	mov YL, r22				;copy r22 to YL
	mov YH, r23				;copy r23 to YH
	st Y+, value			;store value to Y and add 1 to Y 
	mov r22, YL				;copy YL to r22
	mov r23, YH				;copy YH to r23
	pop YL					;pop value to YL 
	pop YH					;pop value to YH
	clr div7bool			;clear div7bool 
	clr div3bool			;clear div3bool 
	mov div3bool, r12		;copy r12 to div3bool 
	mov div7bool, r11		;copy r11 to div7bool 
	mov savedVal, r10		;copy r10 to savedVal
	add savedVal, value		;add savedVal and value
	adc div7bool, r0		;add carry and div7bool 
	adc div3bool, r0		;add carry and div3bool 
	mov r12, div3bool		;copy div3bool to r12
	mov r11, div7bool		;copy div7bool to r11
	mov r10, savedVal		;copy savedVal to r10 
	jmp add300				;jump to add300 
nodivisible:
	push YH					;push value of YL
	push YL					;push value of YH 
	mov YL, r24				;copy r24 to YL
	mov YH, r25				;copy r25 to YH
	st Y+, value			;store value on Y and increase Y by 1 
	mov r24, YL				;copy YL to r24
	mov r25, YH				;copy YH to r25
	pop YL					;pop value to YL
	pop YH					;pop value to YH
	clr div7bool			;clear div7bool 	
	clr div3bool			;clear div3bool 
	mov div3bool, r15		;copy r15 to div3bool 
	mov div7bool, r14		;copy r14 to div7bool 
	mov savedVal, r13		;copy r13 to savedVal 
	add savedVal, value		;add savedVal and value 
	adc div7bool, r0		;add carry and div7bool 
	adc div3bool, r0		;add carry and div3bool 
	mov r15, div3bool		;copy div3bool to r15
	mov r14, div7bool		;copy div7bool to r14
	mov r13, savedVal		;copy savedVal to r13
	jmp add300				;jump to add300	