;=============================================================
;                                                            ;
; Delay_8.inc                                                  ;
;                                                            ;
; Library of delay routines for 8-bit PIC microcontrollers,  ;
; calibrated to 8MHz clock speed.                            ;
; (accurate to the exact number of clock cycles,             ;
; including the call/return overhead.)                       ;
;                                                            ;
; Copyleft 2008: M. Eric Carr / Intellectualism.Org          ;
;                                                            ;
; You are free to re-use this code in any open-source        ;
; application, or to use it for private use, as long         ;
; as this header remains unmodified.                         ;
;                                                            ;
; If you find this code useful, I would appreciate an        ;
; email at eric@intellectualism.org . Thanks!                ;
;                                                            ;
;=============================================================

; Usage: 
; ------
; Call these routines to implement a delay of
; the specified amount of time. For instance, to
; implement a 20-microsecond delay, use the following:
;
;      call	  Delay_20us
;
; (Combine these times to produce any desired delay.)
;
;
; Optimizations: 
; --------------
; These routines were written with the following priorities:
; (1) Each delay routine, including call overhead, shall
;     be accurate to the exact clock cycle (assuming a
;     perfectly accurate system clock.)
; (2) Subroutines shall use no more than one call stack level.
; (3) Variable usage should be minimized
; (4) Code size should be minimized while maintaining readability
; (5) Subroutines are standalone and shall have no dependencies,
;     other than the delay variables.
;
;
; Delay times available at 8MHz*
; ------------------------------
; 2us / 5us
; 10us / 20us / 50us
; 100us / 200us / 500us
; 1ms / 2ms / 5ms
; 10ms / 20ms / 50ms
; 100ms / 200ms / 500ms
; 1s / 2s / 5s
; 10s / 20s
;
;
; Quirks:
; -------
; These routines don't know about (and don't care about)
; interrupts. If they're interrupted, YMMV.
;
; * (1us timing -- two instructions -- is less than the call/
;    return overhead. This should be handled with inline code.)
; * 20s is the largest delay implemented, since it is near
;   the limit of three nested loops. Larger delays are of course
;   possible, but I had to stop somewhere.

;=======================================================



;Local variables, for delay subroutine library use
;(Not true protected local/private variables, since
; this doesn't exist in PIC16 assembly, but hopefully
; these memory locations are not used elsewhere...)



;Delay_1us  **IMPOSSIBLE @ 2MIPS; USE GOTO $+1 or NOP/NOP INSTEAD**
;			**A macro is NOT provided since this is dangerous
;			  if CALLed carelessly. Better to let it fail at
;			  assembly time. **


Delay_2us:				;2usec delay @ 8MHz (4 cycles)
		return				

Delay_5us:				;5usec delay @ 8MHz (10 cycles)
		goto $+1
		goto $+1
		goto $+1
		return		

Delay_10us:				;10usec delay @ 8MHz (20 cycles)
		movlw	0x05
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		return


Delay_20us:				;20usec delay @ 8MHz (40 cycles)
		movlw	0x0B
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		goto	$+1
		return



Delay_50us:				;50usec delay @ 8MHz (100 cycles)
		movlw	0x1F
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		goto	$+1
		return


Delay_100us:			;100usec delay @ 8MHz (200 cycles)
		movlw	0x41
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		return


Delay_200us:			;200usec delay @ 8MHz (400 cycles)
		movlw	0x83
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		goto	$+1
		return


Delay_500us:			;500usec delay @ 8MHz (1000 cycles)
		movlw	0x05
		movwf	Delay2
		movlw	0x41
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		return


Delay_1ms:			;1msec delay @ 8MHz (2000 cycles)
		movlw	0x03
		movwf	Delay2
		movlw	0xDC
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		goto	$+1
		nop
		return



Delay_2ms:			;2msec delay @ 8MHz (4000 cycles)
		movlw	0x2F
		movwf	Delay2
		movlw	0x1B
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		return



Delay_5ms:			;5msec delay @ 8MHz (10k cycles)
		movlw	0x13		;0x95
		movwf	Delay2
		movlw	0xAE		;0x15
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		nop
		return



Delay_10ms:			;10msec delay @ 8MHz (20k cycles) --OK
		movlw	0x1A
		movwf	Delay2
		movlw	0xFF
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		nop
		return


Delay_20ms:			;20msec delay @ 8MHz (40k cycles)
		movlw	0x5f
		movwf	Delay2
		movlw	0x8b
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		return



Delay_50ms:			;50msec delay @ 8MHz (100k cycles)
		movlw	0xc6
		movwf	Delay2
		movlw	0xa7
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		goto	$+1
		goto	$+1
		nop
		return





Delay_100ms:			;100msec delay @ 8MHz (200k cycles)
		movlw	0xC7
		movwf	Delay3
		movlw	0x8F
		movwf	Delay2
		movlw	0x01
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		return


Delay_200ms:			;200msec delay @ 8MHz (400k cycles)
		movlw	0x0E
		movwf	Delay3
		movlw	0x31
		movwf	Delay2
		movlw	0xc1
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		nop
		return



Delay_500ms:			;500msec delay @ 8MHz (1M cycles)
		movlw	0x2E
		movwf	Delay3
		movlw	0xBD
		movwf	Delay2
		movlw	0x25
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		nop
		return


Delay_1s:			;1sec delay @ 8MHz (2M cycles)
		movlw	0xA7
		movwf	Delay3
		movlw	0xA4
		movwf	Delay2
		movlw	0x17
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		goto	$+1
		nop
		return

Delay_2s:			;2sec delay @ 8MHz (4M cycles)
		movlw	0xFE
		movwf	Delay3
		movlw	0xF6
		movwf	Delay2
		movlw	0x14
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		goto	$+1
		nop
		return


Delay_5s:			;5sec delay @ 8MHz (10M cycles)
		movlw	0xB9
		movwf	Delay3
		movlw	0xE6
		movwf	Delay2
		movlw	0x4D
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		goto	$+1
		goto	$+1
		nop
		return

Delay_10s:			;10sec delay @ 8MHz (20M cycles)
		movlw	0xAB
		movwf	Delay3
		movlw	0xCF
		movwf	Delay2
		movlw	0xBB
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9
		goto	$+1
		goto	$+1
		goto	$+1
		return

Delay_20s:			;20sec delay @ 8MHz (40M cycles)
		movlw	0xF1
		movwf	Delay3
		movlw	0xDD
		movwf	Delay2
		movlw	0xF9
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1
		decfsz	Delay2,f
		goto	$-5
		decfsz	Delay3,f
		goto	$-9

		movlw	0x06
		movwf	Delay1
		decfsz	Delay1,f
		goto	$-1

		nop

		return
