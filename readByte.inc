ReadByte:	
    call	InputMode   ;Makes data pin an input
    clrf	tempByte     ;Start out with all bits low.
			    ;(tempByte is the interface to this routine)

    call	WaitForLowClock			    
    
    ;Low clock signal. Expecting data to be clocked in
    ;(one bit per low-to-high transition of clock)
    ;Clock idles at low state.

    movlw   0x08
    movwf   bitsToRead
	
ClockWaitID:
    
    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,7
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,6
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,5
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,4
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,3
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,2
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,1
    call    WaitForLowClock

    call    WaitForHighClock
    btfsc   PORTE, 1
    bsf	    tempByte,0
    call    WaitForLowClock

    
    ;Clock has gone high. Read in the next bit.
    ;rlf	    tempByte, f ;Rotate first. When last bit is set, we're done
    ;bcf	    tempByte, 0
    ;btfsc   PORTE, 1    
    ;bsf	    tempByte, 0

    ;Wait for the clock to go low again
    ;call    WaitForLowClock

    ;Decrement bitsToRead and loop back if not yet zero
    ;decfsz	bitsToRead, f
    ;goto	ClockWaitID

    return ;If we're done, return to the calling process
	   ;(with results in tempByte)
	   
    goto	$   ;GUARD

