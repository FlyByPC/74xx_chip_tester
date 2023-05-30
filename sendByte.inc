SendByte:	
    movlw	0x08
    movwf	bitsToWrite
	
ClockLowWaitWrite:

    ;Clock low received. Expecting to clock data out
    ;(one bit per low-to-high transition of clock)
    ;Clock idles at low state.
    
    ;(Don't wait for the low clock on the first cycle)

    ;Put PORTE.1 into output mode
    call	OutputMode
	    
ClockWaitIDWrite:

    call	WaitForHighClock

    ;Clock has gone high. Send the next bit

    bcf	    PORTE, 1
    btfsc   tempByte,7
    bsf	    PORTE, 1
    rlf	    tempByte, f

    call	WaitForLowClock
    
    decfsz	bitsToWrite, f
    goto	ClockLowWaitWrite

    ;Put PORTE.1 back into input mode
    ;bcf	    PORTE, 1	;TEST -- maybe helps turn off pull-ups??
    call    InputMode


    ;Done sending. Return
    return

    goto	$   ;GUARD

