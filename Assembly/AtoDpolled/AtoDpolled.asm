		LIST P=16F74
		title "A/D Demo Program"
		__CONFIG B'11111110110010'

;************************************************************
;
;	Demonstrates PIC16F74's Analog to Digital (A/D) converter.
;
;	The A/D is configured in ADCON0 and ADCON1 as follows:
;		Vred = +5V internal
;		A/D Oscillator = 8 * oscillator period (of processor)
;		A/D Channel = AN0 (RA0)
;
;	The program converts the potentiometer value on RA0 and
;	displays it as an 8 bit binary value on Port D
;
;	When the indicator reads 0 (full scale ccw), the reading
;	on the LEDs connected to Port D should be 0. When the indicator
;	reads 10 (full scale cw), the readings on the LEDs connected to
;	Port D should be FF. When in between, the reading on the LEDs
;	will be scaled proportionately.
;
;	State register (not used)
;		bit 0
;		bit 1
;		bit 2
;		bit 3
;		bit 4
;		bit 5
;		bit 6
;		bit 7
;
;************************************************************

	#include <P16F74.INC>					; include file for device

;	Variable declarations

Temp	equ		20h							; temporary storage variable
State	equ		21h							; program state register


		org		00h							; reset vector

		goto	initPort					; goto start of routine

		org		04h							; interrupt vector
		goto	isrService					; jump to interrupt routine


		org		10h

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;	Port Initialization

initPort

		clrf		PORTD					; set Port D low
		bsf			STATUS,RP0				; select register bank 1
		clrf		TRISD					; set Port D as outputs
		bcf			STATUS,RP0				; select register bank 0
		call		initAD					; call to initialize A/D
		call		SetupDelay				; delay for Tad (see data sheet) prior to A/D start
		bsf			ADCON0,GO 				; start A/D conversion

;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


;
;	Wait loop

waitLoop
	
		btfsc		ADCON0,GO 				; check if A/D is finished
		goto		waitLoop				; loop until A/D finished


;		Get value and display on LEDs
		
		btfsc		ADCON0,GO 				; make sure A/D finished
		goto		waitLoop				; A/D not finished, continue to loop
		movf		ADRES,W 				; get A/D value
		movwf		PORTD					; display on LEDs
		bsf			ADCON0,GO 				; restart A/D conversion
		goto		waitLoop 				; return to loop


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

delay
		
		decfsz		Temp,F 					; Delay loop
		goto		delay
		return

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;

isrService

		goto		isrService				; error - - stay here

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		END









