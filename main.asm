#include <PIC16F887.INC>
#include "config.inc"
#include <xc.inc>

#define MY_PIC_ID 0x01    ;Defines the chip ID to listen for.

    ;TODO: Implement power pins. These should be on PORTA.0 and PORTB.7,
    ;      unless we want to use another setup. (Use the direct ports.)

    
    ;Chip Tester worker PIC
    ;----------------------
    ; * Waits for Reset line to go high
    ; * Waits for Reset line to go low
    ; * Calls ReadByte to read in PIC_ID (ESP32 drives clock)
    ;   - (If PIC_ID does not match its own, goes back to wait for Reset)
    ; * Calls ReadByte to read in IOMASK_H
    ; * Calls ReadByte to read in IOMASK_L
    ; * Calls ReadByte to read in HLMASK_H
    ; * Calls ReadByte to read in HLMASK_L
    ; * Sets TRISC/TRISD according to IOMASK_H and IOMASK_L
    ; * Sets PORTC/PORTD according to HLMASK_H and HLMASK_L
    ; * Reads PORTA and PORTB
    ; * Clocks out PORTA contents
    ; * Clocks out PORTB contents
    ; * Goes back to wait for another Reset
    
    
    ;PORTA (read-only) and PORTC (read/write): bottom row of pins L to R
    ;PORTB (read-only) and PORTD (read/write): top row of pins L to R
    
    
    
;Allocate memory for the variables we will use.
;Yes, manually, because it works this way and that's winning, for MPLabX.
Delay1		equ	0x20
Delay2		equ	0x21
Delay3		equ	0x22
temp		equ	0x23
bitsToRead	equ	0x24
tempByte	equ	0x25
picID		equ	0x26
IOMaskH		equ	0x27
IOMaskL		equ	0x28
HLMaskH		equ	0x29
HLMaskL		equ	0x2A
aTemp		equ	0x2B
bTemp		equ	0x2C
bitsToWrite	equ	0x2D
    
	
;Magic voodoo that makes the assembler happy. (I miss MPLab Classic.)
PSECT MainCode,delta=2,class=CODE

;Start of main code
    org 0x00
main:
	nop
	nop
	nop
	nop
Start:	
	;Housekeeping routine to set the '887 to all digital I/O
	call	Set16F887
	
	;Make PORTA all inputs (direct-read port)
	banksel TRISA
	movlw	0xFF
	movwf	TRISA
	
	;Make PORTB all inputs (direct-read port)
	banksel TRISB
	movlw	0xFF
	movwf	TRISB
	
	;Make PORTC all inputs (for now) (bidirectional, via resistors)
	banksel TRISC
	movlw	0xFF
	movwf	TRISC

	;Make PORTD all inputs (for now) (bidirectional, via resistors)
	banksel TRISD
	movlw	0xFF
	movwf	TRISD
	
	;Make PORTE all inputs (for now; E.1 is bidirectional)
	banksel TRISE
	movlw	0xFF ;11111111
	movwf	TRISE
	
	;Back to back 00 for main program section
	banksel 0x00
	
loop:	
	;Wait for command from ESP32
	;(Wait for RESET to go high and then low, to begin)
	
	call	WaitForHighReset
	call	WaitForLowReset

	;Got the Reset signal. Wait for a byte and store it as picID
	call	ReadByte    ;Puts result in tempByte
	movf	tempByte, w ;Copy to W
	movwf	picID	    ;Store it in picID
	
	;If this is not our ID, go back and wait for another Reset
	movf	picID, w
	xorlw	MY_PIC_ID   ;Compare with defined constant
	xorlw	0x01
	btfsc	STATUS, 2
	goto	loop		;Not my ID. Go back and wait for Reset.

	;Okay, this is us. Continue.
	
	;Read in the next byte (high byte of I/O mask)
	call	ReadByte
	movf	tempByte, w
	movwf	IOMaskH

	;Read in the next byte (low byte of I/O mask)
	call	ReadByte
	movf	tempByte, w
	movwf	IOMaskL

	;Read in the next byte (high byte of H/L mask)
	call	ReadByte
	movf	tempByte, w
	movwf	HLMaskH

	;Read in the next byte (low byte of H/L mask)
	call	ReadByte
	movf	tempByte, w
	movwf	HLMaskL

	;Got what we need.
	;Implement the IO mask on PORTC and PORTD.
	;(PORTA and PORTB remain read-only)
	movf	IOMaskL, w
	BANKSEL	TRISC
	movwf	TRISC
	BANKSEL	0x00
	movf	IOMaskH, w
	BANKSEL	TRISD
	movwf	TRISD
	BANKSEL	0x00
	
	;Implement the H/L mask on PORTC and PORTD
	movf	HLMaskL, w
	movwf	PORTC
	movf	HLMaskH, w
	movwf	PORTD

	;call	Delay_20us   ;Give the chip some time to respond 
	nop
	
	;Read back the results on PORTA and PORTB
	movf	PORTA, w
	movwf	aTemp
	movf	PORTB, w
	movwf	bTemp
	
	;Wait for the clock from the ESP32 to clock the results back in
	movf	aTemp, w
	movwf	tempByte
	call	SendByte
	
	movf	bTemp, w
	movwf	tempByte
	call	SendByte
	
	goto	loop

#include "16F887_setup.inc"	
#include "delay_8.inc"
#include "readByte.inc"
#include "sendByte.inc"

WaitForLowClock:
    btfsc PORTE, 2
    goto    WaitForLowClock
    return

WaitForHighClock:
    btfss PORTE, 2
    goto    WaitForHighClock
    return

WaitForHighReset:
	btfss	PORTE, 0 ;Wait for Reset to go high
	goto	WaitForHighReset
	return
    
WaitForLowReset:
	btfsc	PORTE, 0 ;Wait for Reset to go high
	goto	WaitForLowReset
	return
    
InputMode:
    banksel TRISE
    bsf	TRISE,1
    banksel 0x00
    return
	
OutputMode:
    banksel TRISE
    bcf	TRISE,1
    banksel 0x00
    return

    end