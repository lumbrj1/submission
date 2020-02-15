;
; AssemblerApplication1.asm
;
; Created: 2/7/2020 6:50:17 PM
; Author : John Paulo Lumbres
;

.include  <m328pbdef.inc>
.def    mc32u0  = r16   ; multiplicand byte 0
.def    mc32u1  = r17   ; multiplicand byte 1
.def	mc32u2	= r18	; multiplicand byte 2 
.def	mc32u3	= r19	; multiplicand byte 3
.def    mp32u0  = r20   ; multiplier byte 0
.def    mp32u1  = r21   ; multiplier byte 1
.def	mp32u2	= r22	; multiplier byte 2 
.def	mp32u3	= r23	; multiplier byte 3
.def    m64u0   = r24   ; result byte 0 (LSB)
.def    m64u1   = r25   ; result byte 1
.def    m64u2   = r26   ; result byte 2
.def    m64u3   = r27   ; result byte 3 
.def	m64u4	= r28	; result byte 4
.def	m64u5	= r29	; result byte 5
.def	m64u6	= r30	; result byte 6 
.def	m64u7	= r31	; result byte 7 (MSB)

.org 0 
	clr r24	;clear register r24
	clr r25	;clear register r25
	clr r26	;clear register r26
	clr r27	;clear register r27
	clr r28	;clear register r28
	clr r29	;clear register r29
	clr r30	;clear register r30
	clr r31	;clear register r31
	ldi mp32u0, 0xee	;load values for byte 0 of multiplier
	ldi mp32u1, 0xee	;load values for byte 1 of multiplier
	ldi mp32u2, 0xee	;load values for byte 2 of multiplier
	ldi mp32u3, 0xee	;load values for byte 3 of multiplier

	ldi mc32u0, 0xee	;load values for byte 0 of multiplicand 
	ldi mc32u1, 0xee	;load values for byte 1 of multiplicand
	ldi mc32u2, 0xee	;load values for byte 2 of multiplicand 
	ldi mc32u3, 0xee	;load values for byte 3 of multiplicand 

	mov m64u0, mp32u0	;copy values of byte 0 of multiplier to byte 0 of result 
	mov m64u1, mp32u1	;copy values of byte 1 of multiplier to byte 1 of result 
	mov m64u2, mp32u2	;copy values of byte 2 of multiplier to byte 2 of result 
	mov m64u3, mp32u3	;copy values of byte 3 of multiplier to byte 3 of result 

	push mp32u3	;store value of mp32u3 to stack
	push mp32u2	;store value of mp32u2 to stack 
	push mp32u1	;store value of mp32u1 to stack

mpy32u:
	ldi mp32u3, 128	;reuse reg 23 to load in value 128 
	ldi mp32u2, 0	;reuse reg 22 to load in value 1
	ldi mp32u1, 32	;reuse reg 21 to load in value 32

	sbrc m64u0, 0	;skip next instruction if 1st bit of m64u0 is 0 
	call initial	;call to subordinate initial

	jmp mul32	;jump to mul32
initial:
	add m64u4, mc32u0	;add values of byte 0 multiplicand to byte 4 of result
	add m64u5, mc32u1	;add values of byte 1 multiplicand to byte 5 of result
	add m64u6, mc32u2	;add values of byte 2 multiplicand to byte 6 of result
	add m64u7, mc32u3	;add values of byte 3 multiplicand to byte 7 of result
	ret	;return to master call

mul32:
	lsr m64u0	;logical shift right result's byte 0

	lsr m64u1	;logical shift right result's byte 1 
	brcc w1		;branch to w1 if carry is 0 
	add m64u0, mp32u3	;add 128 to byte 0 of result 

w1:
	lsr m64u2	;logical shift right result's byte 2
	brcc w2		;branch to w2 if carry is 0 
	add m64u1, mp32u3	;add 128 to byte 1 of result 
w2:
	lsr m64u3	;logical shift right result's byte 3
	brcc w3		;branch to w3 if carry is 0 
	add m64u2, mp32u3	;add 128 to byte 2 of result 
w3:
	lsr m64u4	;logical shift right result's byte 4
	brcc w4		;branch to w4 if carry is 0 
	add m64u3, mp32u3	;add 128 to byte 3 of result 
w4:
	lsr m64u5	;logical shift right result's byte 5
	brcc w5		;branch to w5 if carry is 0 
	add m64u4, mp32u3	;add 128 to byte 4 of result 
w5:
	lsr m64u6	;logical shift right result's byte 6
	brcc w6		;branch to w6 if carry is 0 
	add m64u5, mp32u3	;add 128 to byte 5 of result 
w6:
	lsr m64u7	;logical shift right result's byte 7 
	brcc w7	;branch to cont if carry is 0 
	add m64u6, mp32u3	;add 128 to byte 6 of result 
w7:	
	cpi mp32u2, 0	;compare reg 22 to 0
	breq cont	;branch to cont if they're mp32u2 is equal 0 
	add m64u7, mp32u3	;add 128 to byte 7 of result 
	clr mp32u2	;clear reg 22

cont:
	sbrc m64u0, 0	;skip next instruction if bit 0 of result's byte 0 is 0
	jmp addContent	;jump to addContent 
loop:
	dec mp32u1	;decrease reg 21 by 1 
	brne mul32	;branch to mul32 if reg 21 is not 0

end:
	pop mp32u1	;restore previous value of multiplier byte 1
	pop mp32u2	;restore previous value of multiplier byte 2
	pop mp32u3	;restore previous value of multiplier byte 3
end2:
	jmp end2	;repeated loop 
addContent:
	add m64u4, mc32u0	;add value of byte 0 of multiplicand to byte 4 of result
	adc m64u5, mc32u1	;add value of byte 1 of multiplicand to byte 5 of result with carry
	adc m64u6, mc32u2	;add value of byte 2 of multiplicand to byte 6 of result with carry
	adc m64u7, mc32u3	;add value of byte 3 of multiplicand to byte 7 of result with carry
	brcc loop	;branch to loop if carry is 0 
	ldi mp32u2, 1	;change value of reg 22 to 1 
	jmp loop	;jump to loop 

