		LIST P=16F74
		title "On-Off Control"
		__CONFIG B'11111110110010'

;************************************************************
;
; 	Tim Cousins tac2155
;	Jamie Collier
;	Chi Yung Fung
; 	Mechatronics, Fall 2013
;	Case Study 3 - On/Off Control
;
; 	This program gives simple control to engage and release a solenoid.
;	It has three modes. 

;	Mode 1 simply engages the solenoid when the red button is pressed on 
;	the microchip board, and released when the red button is pressed again.
;	This mode is used to adjust the solenoid, so that when the photoresistor 
; 	registers the infared LED in the solenoid test fixture, it  gives a 
;	positive output when hooked into a comparator on the protoboard.

;	Mode 2 engages the solenoid when the red button is pressed, and holds 
;	it for a length of time determined by the A/D conversion. Altering the
; 	value of the potentiometer on the board increases the value of the 
;	A/D output, and therefore increases the time the solenoid is engaged. 
;	Pressing the red button while the solenoid is engaged resets the time 
;	it is to be engaged. The solenoid is engaged for a time roughly equal to 
;	1/4 of the AD output.
;
; 	Mode 3 engages the solenoid in the same way as Mode 2. However, after the
;	solenoid engages, the program turns on the secondary, low current transistor
;	and turns off the main, high current transistor. The red button does not 
;	reset the time. If the solenoid doensn't engage in 10 seconds after hitting
;	the main transistor is turned on, an error occurs. If the solenoid does not
;	retract after 10 seconds of the time running out, an error occuers. If the 
;	solenoid disengages before it is supposed to, it reengages once and restarts 
;	the timer. If it occurs again, an error occurs. For modes 2 and 3, if the 
;	potentiometer is set to 0, giving an AD value of 0, an error occurs. 
;
;	The modes are selected with an Octal Switch connected to ports E0, E1, and E2.
; 	The value for the selected mode is determined by complementing the input
;	from the octal switch and masking the result with 0x07. The value for mode 
;	is then stored in the State register. If the state is equal to a value other
;	than 1, 2, or 3, the program produces an error.
;
; 	The Port B LEDs are used to display what mode the program is in, turning on
;	LEDs 1, 2 or 3 depending on the mode. When an error is produced, the Port B 
; 	Port B LED 4 is turned on along with the LED for the mode the program is in
;	and the LEDs flash every second. 
;
;	Note: For some microboards, the processor runs twice as fast, so the
;	time loop only delays .5 seconds, which leads the solenoid to stay engaged
;	for 1/8 of the A/D output rather than 1/4.
;	Too keep the error counts in Mode 3 at 10 seconds, values of 20 are loaded
;	into Count and Count1 variables. If the processor ran at normal speed, values
;	of 10 would be loaded into these variables.
;	
;	Variables
;	
;	Count - stores value of A/D conversion output
; 	Count1 - used to check if solenoid stays engaged for >10 seconds in mode 3
;	Count2 - used to check if solenoid engages within 10 seconds of pressing 
;	red button
;	Count3 - used to check if solenoid is disengaged twice before time in mode 2
;	Temp - used in switch debounce delay
;
;	Inputs/outputs
;	
;	PortD, 0 - Main Transistor Output 
;	PortD, 1 - Second Transistor Output 
;	PortD, 2 - Comparator Input 
;
;	PortC, 0 - Green Button Input 
;	PortC, 1 - Red Button Input 
;	
;	PortE, 0-2 - Octal Switch Inputs 
;
; 	PortB - Mode LED Outputs
;
;	State Register
;		bit 0 - used for Mode
;		bit 1 - used for Mode
;		bit 2 - used for Mode
;		bit 3 - error bit
;		bit 4
;		bit 5
;		bit 6
;		bit 7
;
;************************************************************

	#include <P16F74.INC>				; include file for device

;	Variable declarations

Timer2	EQU		20h
Timer1	EQU		21h
Timer0	EQU		22h
Temp	EQU		23h						; temporary storage variable
Count	EQU		24h						; count storage variable
Count1	EQU 	25h						; count storage - error check
Count2	EQU		26h						; count storage - error check
Count3	EQU 	27h						; count storage - error check
State	EQU		28h						; progam state register


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

WaitPress   							; wait press when in a mode

		btfsc 		PORTC,0 			; see if green button pressed
		goto		GreenPress			; green button is pressed
		btfsc		PORTC,1 			; see if red button is pressed
		goto		RedPress
		goto 		WaitPress 			; keep checking		

GreenPressInitial 						; check for initial noise

		btfss		PORTC,0 			; check if green button still pressed
		goto 		waitGreenPress		; noise - keep checking
		goto		GreenRelease

GreenPress 								; check for noise while in modes

		btfss		PORTC,0 			; check if green button still pressed
		goto		WaitPress			; noise - keep checking

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
		goto		WaitPress			; noise - keep checking

RedRelease
	
		btfsc		PORTC,1 			; check if red button released
		goto 		RedRelease 	 		; no - keep checking for release
		call        SwitchDelay         ; let switch debounce
		return

; determines what mode octal switch is in

ModeSelect 								
		
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
		goto		Mode3 				; goto Mode 3

;
;	Mode 1 - turn engage and disengage solenoid with red button

Mode1

		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs
		call 		WaitPress	
		bsf			PORTD,0  			; turn on transistor
		call		WaitPress
		bcf 		PORTD,0 			; turn off transistor
		goto		Mode1 				; return to start of mode 1		

;
;	Mode 2 - engages solenoid for time based on potentiometer

Mode2

		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs
		call 		WaitPress
		bsf			ADCON0,GO 			; start A/D conversion
		call 		waitLoop     		; wait for A/D to finish
		bsf			PORTD,0 			; turn on transistor

Mode2Time
		
		call 		timeLoop 			; count 1 second (.5 seconds for fast proc)
		btfsc 		PORTC,1 			; check if red button pressed
		goto		TimeReset			; reset solednoid time
		decfsz 		Count  				; decrease count
		goto 		Mode2Time
		bcf 		PORTD,0 			; turn off transistor
		goto 		Mode2

TimeReset
		
		bsf 		ADCON0,GO 			; start A/D conversion
		call		waitLoop			; wait for A/D to finish
		goto		Mode2Time			; restart solenoid time

;	Loop that waits for A/D Conversion to complete and stores the value

waitLoop
		
		btfsc 		ADCON0,GO			; check if A/D finished
		goto		waitLoop			; A/D not finished, wait 
		movf 		ADRES,W 			; get A/D value
		movwf 		Count
		call		PotenCheck 			; make sure not 0
		rrf 		Count				; rotate twice right
		rrf 		Count 				; dividing by 4
		movlw 		B'00111111' 		; mask to remove possible carry bits
		andwf 		Count,F 			; store in count variable
		return							; return to mode

; Error check - checks if potentiometer is set to 0

PotenCheck								

		incfsz 		Count				; increase count
		decfsz 		Count 				; decrease count
		return 							; return with count unchanged
		goto 		ModeError			; else if count 0, error


;
;	Mode 3 - engage solenoid, switch transistors, time based on pot
; 	no red button reset timer, cannot be engaged for more than 10 seconds
;	Note - processor runs at double speed on board, so values of 20 and 21 are 
;	loaded into Count1 and Count2 variables. If the microcomputer ran at normal
;	speed, these values would be 10 and 11 respectively


Mode3

		movf 		State,W    			; store state in w register
		movwf 		PORTB 				; display state on Port B LEDs
		movlw 		D'20'			    ; for error checking - disengage check
		movwf 		Count1 				; store in count 1 variable
		movlw 		D'21'			    ; for error checking - engage timer
		movwf 		Count2 				; store in count 2 variable
		movlw 		D'2'			    ; for error checking - early disengage
		movwf 		Count3 				; store in count 3 variable					
		call 		WaitPress 			; wait for button pressed
		
		bsf 		ADCON0,GO 			; start A/D conversion
		call		waitLoop			; wait for A/D to finish
		bsf 		PORTD,0 			; turn on main transistor
		call 		SolenoidEngage		; wait for solenoid to engage
		bsf 		PORTD,1 			; turn on small transistor
		call		SwitchDelay 		; small delay
		bcf 		PORTD,0 			; turn off main transistor

Mode3Time
	
		call	 	timeLoop  			; delay 1 second (.5 of fast microcomp)
		btfss 		PORTD,2 			; check if solenoid engaged
		call 		EngageCheck 		; no - call engage time error check		
		decfsz 		Count 				; decrement counter	
		goto 		Mode3Time 			; loop
		bcf 	   	PORTD,1 			; when time done, turn off transistor
		btfss 		PORTD,2 			; is solenoid engaged
		goto		Mode3 				; no - go to mode 3 start

SolenoidExtend
		call 		timeLoop			; yes - delay 1 second (.5 of fast microcomp)
		call 		TimeCheck			; error checking - check how long been engaged
		btfss 		PORTD,2 			; check if solenoid engaged
		goto		Mode3 				; no - go to begining of mode
		goto		SolenoidExtend		; yes - repeat error check loop

;	Error check - cannot stay engaged for more than 10 seconds

TimeCheck
		
		decfsz		Count1				; decrease error check count
		return							; if not zero, return
		goto 		ModeError			; if reaches 0, error

SolenoidEngage	

		btfsc		PORTD,2 			; check if sensor on
		return							; yes - return
		call 		timeLoop 			; no - delay a second (.5 of fast microcomp)
		decfsz 		Count2 				; decrease count2 register
		goto		SolenoidEngage		; not 0 - loop
		bcf 		PORTD,0 			; turn off main transistor after 10 seconds
		goto 		ModeError			; goto error
		
EngageCheck
		
		decfsz 		Count3 				; can only disengage once
		goto 		Reengage 			; if not zero, goto reengage
		goto		ModeError 			; if count reaches 0, error
		
Reengage
		
		bsf 		ADCON0,GO 			; start A/D conversion
		call		waitLoop			; wait for A/D to finish	
		bsf 		PORTD,0 			; turns on main transistor
		call 		SolenoidEngage		; waits to engage
		bcf			PORTD,0 			; turns off main transistor
		return

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
;	1 second time loop (apppears to only delay .5 seconds)

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
;	Infinite Loop if error, only stop with reset

ModeError
		movf 		State,W 			; move state to W register
		iorlw 		B'00001000'			; and with 08h
		movwf 		PORTB 				; turn on error LEDs
		call		timeLoop			; time one second
		clrf		PORTB				; clear LEDs
		call		timeLoop 			; time one second
		goto 		ModeError 			; infinite loop



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

;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;

isrService

		goto		isrService			; error - - stay here

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		END