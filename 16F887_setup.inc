;PIC16F887 initialization code
;(sets up all ports as digital I/O, with 8MHz internal oscillator)
;
;Copyleft 2008, M. Eric Carr / Paleotechnologist.Net
;(You are free to use this under the Creative Commons license,
; for any purposes including commercial use, as long as this header
; is left intact or attribution is otherwise given.)
;----------------------------------------------------------------

Set16F887:

	banksel OPTION_REG
	bcf	OPTION_REG,5	;Timer0 transition on Fosc/4, not 
				;T0CKI pin
	;INTCON is OK as-is

	;PIE1 is OK as-is

	;PIE2 is OK as-is

	;PIR1 and PIR2 provide status info and are OK as-is

	;PCON is OK as-is (control BOREN through config word)

	banksel	ANSEL
	clrf	ANSEL		;All pins set to digital I/O

	banksel ANSELH
	clrf	ANSELH		;These pins as digital I/O also

	banksel	OSCCON
	movlw	0x71		;Internal oscillator / 8MHz
	movwf	OSCCON

	;T1CON is OK as-is

	;T2CON is OK as-is

	;CM1CON0 should be OK as-is (possible issue with bits 0-2?)

	;CM2CON0 should be OK as-is (possible issue with bits 0-2?)

	;CM2CON1 is OK as-is

	;SRCON should be OK as-is

	;VRCON is OK as-is

	;ADCON0 is OK as-is

	;ADCON1 is OK as-is
	
	;CCP1CON should be OK as-is

	;CCP2CON should be OK as-is

	banksel 0x00

	return

