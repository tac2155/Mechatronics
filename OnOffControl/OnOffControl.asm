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

		call		initAD 				; call to initialize A/D conversion
		call		SetupDelay			; delay for A/D setup

		bsf			STATUS,RP0			; select register bank 1
		movlw		B'11111100'			; move 0xFC into W register
		movwf		TRISD				; Port D pins 0 and 1 output
		movlw		H'ff'				; move 0xFF itno W register
		movwf		TRISC				; configure port C as all inputs
		clrf		TRISB				; configure port B as all outputs
		movlw		B'00000111'			; move 0x07 to W register
		movwf		TRISE				; set pins 0-2 as input on port E
		
		bcf			STATUS,RP0			; select register bank 0
		clrf		Count 				; clear counter register
		clrf		State 				; clear the state register

;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

waitGreenPress							; initial wait for green button

		btfsc 		PORTC,0 			; see if green button pressed
		goto		GreenPressInitial	; green button is pressed
		goto 		waitGreenPress 		; keep checking

waitPress   							; wait press when in a mode

		btfsc 		PORTC,0 			; see if green button pressed
		goto		GreenPress			; green button is pressed
		btfsc		PORTC,1 			; see if red button is pressed
		goto		RedPress
		goto 		waitPress 			; keep checking		

GreenPressInitial

		btfss		PORTC,0 			; check if green button still pressed
		goto 		waitGreenPress		; noise - keep checking
		goto		GreenRelease

GreenPress

		btfss		PORTC,0 			; check if red button still pressed
		goto		waitPress			; noise - keep checking

GreenRelease

		btfsc		PORTC,0 			; check if green button released
		goto		GreenRelease		; no - wait

		call 		SwitchDelay 		; let the switch debounce

		bcf 		PORTD,0 			; turn off main transistor
		bcf 		PORTD,1 			; turn off second transistor
		clrf 		Count 				; clear count		

		comf		PORTE,W 			; complement of octal switch bits
		andlw		B'00000111'			; and with 0x07 for mode
		movwf		State 				; store switch in state register
		goto 		ModeSelect 			; goto ModeSelect

RedPress
	
		btfss		PORTC,1 			; check if red button still pressed
		goto		waitPress			; noise - keep checking

RedRelease
	
		btfsc		PORTC,1 			; check if red button released
		goto 		RedRelease 	 		; no - keep checking for release
		call        SwitchDelay         ; let switch debounce
		return


ModeSelect 								; determines what mode octal switch is in
		
		btfsc		State,2 			; if bit 2 is one
		goto		ModeError 			; error - not 1, 2, or 3
		btfsc		State,1 			; if bit 1 is one
		goto		ModeSelectA 		; goto ModeSelectA
		btfsc		State,0 			; if bit 0 is one
		goto		Mode1               ; switch is on one, goto Mode1
		goto		ModeError 			; otherwise, switch is 0, error

ModeSelectA 							; determines if switch is 2 or 3
		
		btfss		State,0   			; if bit 0 is 0
		goto		Mode2				; switch is set to 2, goto Mode 2
		movlw		B'00000100'			; otherwise, store 0x04 in B register
		movwf		State 				; store in state register
		goto		Mode3 				; goto Mode 3

;
;	Mode 1 - turn engage and disengage solenoid with red button

Mode1

		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs
		call 		waitPress	
		bsf			PORTD,0  			; turn on transistor
		call		waitPress
		bcf 		PORTD,1 			; turn off transistor
		incf		Count,F 			; increment count
		goto		Mode1 				; return to start of mode 1		

;
; Mode 2 - engages solenoid for time based on potentiometer
Mode2

		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs
		call 		waitPress
		bsf			ADCON0,GO 			; start A/D conversion
		call 		waitLoop     		; wait for A/D to finish
		movf 		ADRES,W 			; get A/D value
		movwf 		Count 				; store in count
		call		PotenCheck
		bsf			PORTD,0 			; turn on transistor
		call 		SolednoidRetract	; wait for solenoid to retract 
		goto		SolenoidTime 		; time for solenoid to be on
		
Mode3

		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs
	
		goto		Mode3

waitLoop

		btfsc 		ADCON0				; check if A/D finished
		goto		waitLoop			; A/D not finished, wait 
		return							; return to mode

PotenCheck								; checks for error - set to 0
		incf		Count 				; increase count
		decfz 		Count 				; decrease count
		return
		goto 		ModeError			; if count 0, error

SolenoidTime

		call 		timeLoop 			; count one second
		btfsc 		PORTC,1 			; check if red button pressed
		goto		TimeReset			; reset solednoid time
		decfz 		Count  				; decrease count
		goto 		SolenoidTime
		bcf 		PORTD,0 			; turn of transistor

TimeReset
		call		RedPress
		movf 		ADRES,W 			; get A/D value
		movwf 		Count 				; store in count
		goto		SolenoidTime		; restart solenoid time

SolednoidRetract
		btfss		PORTD,2 			; check if sensor on
		goto		SolednoidRetract	; no - wait
		return

SolenoidExtend
		btfsc		PORTD,2
		goto		SolenoidExtend
		return
;

;
;	Delay for switch to debounce

SwitchDelay

		movlw		D'20'				; Load with decimal 20
		movwf		Temp

delay
		decfsz		Temp,F				; 60usec delay
		goto 		delay
		return

;

ModeError
		movf 		State,W 			; move state to W register
		iorlw 		B'00001000'			; and with 08h
		movwf 		PORTB 				; turn on error LEDs
		call		timeLoop			; time one second
		clrf		PORTB				; clear LEDs
		call		timeLoop 			; time one second
		goto 		ModeError 			; infinite loop

;
;	1 second time loop

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
;

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;	InitializeAD - initializes and sets up A/D hardware
;	Select AN0 to AN3 as analog inputs, proper clock period, and read AN0

initAD 

		bsf			STATUS,RP0				; select register bank 1
		movlw		B'00000100'				; RA0, RA1, RA3 analog inputs, all other digital
		movwf		ADCON1					; move to special function A/D register
		bcf			STATUS,RP0				; select register bank 0
		movlw		B'01000001'				; select 8 * oscillator, analog input 0, turn on
		movwf		ADCON0					; move to spectial function A/D register
		return

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;
;	This routing is a software delay of 10uS required for A/D setup.
;	At a 4Mhz clock, the loop takes 3Us, so initialize the register Temp with
;	a value of 3 to give 9us, plus the move etc should result in total time
; 	of > 10uS

SetupDelay

		movlw		03h						; load Temp with hex 3
		movwf		Temp

setDelay
		
		decfsz		Temp,F 					; Delay loop
		goto		setDelay
		return

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;

isrService

		goto		isrService			; error - - stay here

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		END