		LIST P=16F74
		title "On-Off Control"
		__CONFIG B'11111110110010'

;************************************************************
;
; Insert Description of program
;
;************************************************************

	#include <P16F74.INC>				;include file for device

;	Variable declarations

Timer2	EQU		20h
Timer1	EQU		21h
Timer0	EQU		22h
Temp	EQU		23h						; temporary storage variable
Count	EQU		24h						; count storage variable
State	EQU		25h						; cprogam state register


		ORG		00h						; reset vector

		goto 	initPort				; goto start of routine

		ORG 	04h						; interrupt vector
		goto	isrService				; jump to interrupt routine


		ORG		10h

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;	Port Initialization

initPort

		clrf		PORTB				; clear Port B
		clrf		PORTD				; clear Port D
		clrf		PORTC				; clear port C
		clrf		PORTE				; clear Port E
		bsf			STATUS,RP0			; select register bank 1
		movlw		H'02'				; move 0x02 into W register
		movwf		ADCON1				; set port E as digital I/O
		movlw		B'00000111'			; move 0x07 to W register
		movwf		TRISE				; set pins 0-2 as input on port E
		movlw		B'11111100'			; move 0xFC into W register
		movwf		TRISD				; Port D pins 0 and 1 output
		movlw		H'ff'				; move 0xFF itno W register
		movwf		TRISC				; configure port C as all inputs
		clrf		TRISB				; configure port B as all outputs
		
		bcf			STATUS,RP0			; select register bank 0
		clrf		Count 				; clear counter register
		clrf		State 				; clear the state register

;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

waitGreenPress

		btfsc 		PORTC,0 			; see if green button pressed
		goto		GreenPress			; green button is pressed
		goto 		waitGreenPress 		; keep checking

waitRedPress
	
		btfsc		PORTC,1
		goto		RedPress
		goto		waitRedPress

waitPressMode   						; wait press when in a mode

		btfsc 		PORTC,0 			; see if green button pressed
		goto		GreenPress			; green button is pressed
		btfsc		PORTC,1 			; see if red button is pressed
		goto		RedPress
		goto 		waitPressMode 		; keep checking		

GreenPress

		btfss		PORTC,0 			; check if green button still pressed
		goto 		waitPressMode		; noise - keep checking

GreenRelease

		btfsc		PORTC,0 			; check if green button released
		goto		GreenRelease		; no - wait

		call 		SwitchDelay 		; let the switch debounce

		bcf 		PORTD,0 			; turn off transistors
		bcf 		PORTD,1 			;

		comf		PORTE,W 			; complement of octal switch bits
		andlw		B'00000111'			; and with 0x07 for mode
		movwf		State 				; store switch in state register
		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs

		goto 		ModeSelect 			; goto ModeSelect

RedPress
	
		btfss		PORTC,1 			; check if red button still pressed
		goto		waitPressMode		; noise - keep checking

RedRelease
	
		btfsc		PORTC,1 			; check if red button released
		goto 		RedRelease 	 		; no - keep checking for release
		call        SwitchDelay         ; let switch debounces

		return


ModeSelect 								; determines what mode octal switch is in
		
		clrf 		Count 				; clear count
		btfsc		State,2 			; if bit 2 is one
		goto		modeError 			; error - not 1, 2, or 3
		btfsc		State,1 			; if bit 1 is one
		goto		ModeSelectA 		; goto ModeSelectA
		btfsc		State,0 			; if bit 0 is one
		goto		Mode1               ; swith is in one, goto Mode1
		goto		modeError 			; otherwise, switch is 0, error

ModeSelectA 							; determines if switch is 2 or 3
		
		btfsc		State,0   			; if bit 0 is 1
		goto		Mode3 				; swith is set to 3, goto Mode 3
		goto		Mode2 				; otherwise, goto Mode2


Mode1		
		call 		waitPressMode
		
		bsf			PORTD,0  			; turn on transistor
		goto		Mode1Extend        	; go to solenoid extend

Mode1Extend
		
		call		waitPressMode
		bcf 		PORTD,0 			; turn off transistor
		incf		Count,F 			; increment count
		goto		Mode1 				; return to start of mode 1

Mode2
		
		
		goto 		Mode2
		
Mode3
		
		goto		Mode3

SwitchDelay

		movlw		D'20'				; Load with decimal 20
		movwf		Temp

delay
		decfsz		Temp,F				; 60usec delay
		goto 		delay
		return

modeError
		movf 		State,W 			; move state to W register
		iorlw 		B'00001000'			; and with 08h
		movwf 		PORTB 				; turn on error LEDs
		call		timeLoop			; time one second
		clrf		PORTB				; clear LEDs
		call		timeLoop 			; time one second
		goto 		modeError 			; infinite loop

timeLoop
		
		movlw		06h					; get most significant hex value + 1
		movwf		Timer2				; store it in count register
		movlw 		16h					; get next most significant hex value
		movwf		Timer1				; store in count register
		movlw 		15h					; get least significant hex value
		movwf		Timer0				; store in count register

timeDelay

		decfsz		Timer0,F 			; Delay loop;
		goto 		timeDelay
		decfsz		Timer1,F 			; delay loop
		goto		timeDelay
		decfsz		Timer2,F 			; delay loop
		goto		timeDelay

return	

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;

isrService

		goto		isrService			; error - - stay here

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		END