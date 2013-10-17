		LIST P=16F74
		title "Counting Program"
		__CONFIG B'11111110110010'
		
		#include <P16F74.INC>
		
; Variable Declarations

Count	equ		20h				; counter
Temp	equ		21h				; temporary register
State	equ		22h				; program state register
		
		
		org 00h					; Assembler directive - reset vector
		
		goto 	initPort
		
		org 	04h				; interrupt vector
		goto 	isrService		; jump to interrupt routine (dummy)
		
		org 10h					; beginning of program storage
		
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

; Port Initialization

initPort

		clrf	PORTD			; Clear Port D output latches
		clrf	PORTC			; Clear Port C output latches
		bsf		STATUS,RP0		; Set bit in STATUS register for bank 1
		movlw	B'11111111'		; move hex value FF into W register
		movwf	TRISC			; Configure port C as all inputs
		clrf	TRISD			; configure port D as all outputs
		bcf		STATUS,RP0		; clear bit in STATUS register for bank 0
		clrf	Count			; zero the counter
		
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

waitPress	
		btfsc	PORTC,0			; see if green button pressed
		goto	GreenPress		; green button is pressed
		btfsc	PORTC,1			; see if red button pressed
		goto	RedPress		; red button is pressed
		goto 	waitPress		; keep checking
		
GreenPress
		btfss	PORTC,0			; check if green button still pressed
		goto 	waitPress		; noise - keep checking for press
		
GreenRelease		
		btfsc	PORTC,0			; check if green button released
		goto	GreenRelease	; no - wait
		
		call 	SwitchDelay		; let switch debounce
		goto	IncCount		; increment the counter
		
RedPress
		btfss	PORTC,1			; check if red button still pressed
		goto 	waitPress		; noise - keep checking for press	
		
RedRelease
		btfsc	PORTC,1			; check if red button released
		goto	RedRelease		; no - keep checking for release
		
		call	SwitchDelay		; let the switch debounce
		decf	Count,F			; decrement the count - store in register
		goto	outCount		; output count on LEDs
		
IncCount	
		incf	Count,F			; increment the count - store in register
		
outCount
		movf	Count,W			; move the count to W register
		movwf	PORTD			; display count on Port D
		
		goto 	waitPress		; wait for next button press
		
SwitchDelay
		movlw	D'20'			; Load with decimal 20
		movwf	Temp
delay
		decfsz	Temp,F			; 60usec delay
		goto 	delay
		return
		
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

isrService
		goto isrService			; error - stay here
		
;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		END						; Assembler directive - end of program
		
		
		