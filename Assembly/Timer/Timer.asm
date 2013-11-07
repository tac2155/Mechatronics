		LIST P=16F74
		title "Long Timer Program"
		__CONFIG B'11111110110010'

;************************************************************
;
;	This program uses 3 registers to form a timer with a one second
;	period. Note: and extra count has to be added to the most significant
;	register. This is because once the register counts to zero, the delay
;	loop stops.
;
;	A short delay loop is 3uS, therefore need approximately 333,333 loops
; 	to equal 1 second, 0x51615 in hex. Therefore, need 3 8-bit registers
;	to store this number: 0x06(0x05 + 1) in MSR, 0x16 in middle register,
;	and 0x15 in LSR.
;
;	Program counts the seconds and displays the count as 80bit binary value
;	 on port D. Therefore, LEDs on Port D should increment by one every sec.
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

	#include <P16F74.INC>				; include file for the device

;	Variable declarations

Timer2	EQU 	20h						; 
Timer1	EQU 	21h						; timer storage variables
Timer0	EQU 	22h						;
Count 	EQU 	23h						; count storage variable
State	EQU 	24h						; program state register


		ORG		00h						; reset vector

		goto 	initPort				; goto start of routine

		ORG		04h						; interrupt vector
		goto 	isrService				; jump to interrupt routine


		ORG		10h

;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;	Port Initialization

initPort

		clrf		PORTD				; clear Port D
		bsf			STATUS,RP0			; select register bank 1
		clrf		TRISD				; set Port D as all outputs
		bcf			STATUS,RP0			; select register bank 0
		clrf		Count 				; clear counter register

;
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

;
;	Count loop

timeLoop

		movlw		06h					; get most significant hex value + 1
		movwf		Timer2				; store it in count register
		movlw 		16h					; get next most significant hex value
		movwf		Timer1				; store in count register
		movlw 		15h					; get least significant hex value
		movwf		Timer0				; store in count register

delay

		decfsz		Timer0,F 			; Delay loop;
		goto 		delay
		decfsz		Timer1,F 			; delay loop
		goto		delay
		decfsz		Timer2,F 			; delay loop
		goto		delay

;		Increment counter and display on LEDs

		incf		Count,F 			; increment count - store in Count register
		movf 		Count,W 			; move the count to the W register
		movwf 		PORTD 				; display count on port D 

		goto		timeLoop			; time another second

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
;

isrService

		goto		isrService			; error - - stay here

;~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		END