#include "DCMotor.h"

/***********************************************************

Case Study 6 - DC Motor

Tim Cousins
Jamie Collier
Chi Yung Fung

This case study controls a DC Motor with a trapezoidal voltage
profile with 4 different modes of feedback. After the ports and
registers are initialized, an intial voltage to overcome the 
Coulumb friction of the motor is determined and used as the 
reference voltage for the profile. The gains and bias are set,
and the program waits for the user to press the button. Once the
button is pressed, the mode is read and the motor is controlled 
with the feedback specified by the mode. The A/D converter is 
started to read the speed of the motor from the tachometer, and 
an interrupt is used for whenever the conversion is finished.
Whenver 64 readings have been made, the average is taken and used 
as the speed of the motor to determine the required input voltage.

For the profile, the reference voltage is incremented every full
rotation until it reaches 190. Once at 190, it holds for 600
revolutions before decrementing every revolution to 140. The revoltions
are determined by an encoder attached to the motor. Every 100 passes
of the encoder is equal to one revolution. During the encoder checks, 
the program checks to see if 64 tachometer readings have been made, 
and if they have, it determines the average speed and the required duty
cycle.

Mode 0 - Proportional Control
Duty Cycle = Kp * error

Mode 1 - Proportional + Bias Control
Duty Cycle = Kp * error + Bias

Mode 2 - Proportional - Derivative Control
Duty Cycle = Kp * error + Kd * (error - (Previous Error))

Mode 3 - Proportional - Integral - Derivative Control
Duty Cycle = Kp * error + Kd * (error - (Previous Error)) + Ki * (total error)

Outside References:
C Programming Language, 2nd Edition
stackoverflow.com
***********************************************************/

void main(void)
{
	init();									// Run initialization routine
	kp  	= 15;							// set proportional gain
	kd 		= 5;							// set derivative gain
	ki		= 0.005;						// set integral gain

	while(!blackButton) {} 					// wait for button press
	while(blackButton)  {}					// wait for release
	delay(200);	
		
	if(MSBHex)								// if upper bit of switch hi
	{
		if(LSBHex)							// and lower bit hi
		{
			mode = 3;						// then mode 3
		}
		else
		{
			mode = 2;						// else mode 2
		}
	}
	else if(LSBHex)							// if lower bit hi and upper bit lo
	{
		mode = 1;							// then mode 1 
	}
	else
	{	
		mode = 0;							// else mode 0
	}
	
	switch(mode)							// switch case for mode
	{
		case 0:
			motorControl(kp, 0, 0, 0);		// mode 0 - proportional control
			break;

		case 1:
			motorControl(kp, 0, 0, bias);	// mode 1 - proportional control with bias
			break;

		case 2:								// mode 2 - proportional derivative control
			motorControl(kp, kd, 0, 0);
			break;

		case 3:								// mode 3 - PID control
			motorControl(kp, kd, ki, 0);	
			break;
	}

	while(1 != 42)							// Inifinite loop
	{
		PORTB = 0xff;						//	Flash Port B every second
		longTimer();
		PORTB = 0x00;
		longTimer();
	}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	Initialization

void	init(void) 							// Initialize Ports and Registers
{
	PORTA	= 0x00;							// Clear Ports
	PORTB 	= 0x00;
	PORTC	= 0x00;

	TRISA4	= 1;							// Set Port A, pin 4 as input
	TRISC	= 0b11111000;					// set Port C inputs and outputs
	TRISB	= 0x00;							// Set Port B as all outputs

	initISR();								// ISR initialization
	initAtoD();								// AtoD Initialization
	initPWM();								// PWM initialization

	refVoltage();							// determine reference voltage
}

void	initISR(void)						// initialize interrupts				
{
	INTCON	= 0b11000000;					// enable global and peripheral interrupts
	ADIE	= 1;							// enable A/D interrupt bit
	ADIF	= 0;							// set A/D flag to zero
}

void	initAtoD(void)						// initialize A/D
{
	ADCON1	= 0b00000100;					// RA0, RA1, RA3 analog inputs
	ADCON0	= 0b01001001;					// select 8* oscillator, analog input 1, turn on 
	delay(1);								// small delay
}

void	initPWM(void)
{
	CCP1CON	= 0b00001100;					// Set PWM in CCP1
	PR2		= 0xff;							// set PR2
	T2CON 	= 0b00000100;					// set up Timer 2
}

void	refVoltage(void)					// determine reference voltage
{
	velRef	= 160;							// set refefence voltage at 160
	bias 	= velRef;						// set bias equal to this value

	dir 	= 1;							// Motor cw
	CCPR1L 	= velRef;						// set PWM duty cycle at 160
	PWM 	= 1;							// start PWM

	//	Wait for four passes of the eddy sensor

	for (eddyCount = 0; eddyCount < 4; eddyCount++)
	{
		while(!eddy) {}						// while sensor not activated
		while(eddy)  {}						// wait to no sensor
	}

	brake();								// stop the motor
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	Function to control the motor - paramters are gains and bias

void	motorControl(uint8_t kP, uint8_t kD, float kI, uint8_t b)
{
	dir 		= 1;						// set motor to cw
	motorBrake 	= 0;						// turn of motor brake
	CCPR1L		= velRef;					// set duty cycle to reference voltage
	ADGO 		= 1;						// start A/D conversion

	while(velRef < 190)						// while reference voltage less than 190
	{
		for(count = 0; count < 100; count++) // count 100 counts of the encoder
		{
			while(!enc) 					// while not at encoder
			{
				setPWM(kP, kD, kI, b);		// set PWM
			}

			while(enc) 						// wait till off encoder sensor
			{
				setPWM(kP, kD, kI, b);		// set PWM
			}
		}

		velRef++;							// increment reference voltage
	}
	
	for(count = 0; count < 6000; count++)	// hold voltage for 6000 counts of the encoder
	{
		while(!enc) 						// while not at encoder
		{
			setPWM(kP, kD, kI, b);			// set PWM
		}
		while(enc)  						// wait till off encoder sensor
		{
			setPWM(kP, kD, kI, b);			// set PWM
		}
	}

	while(velRef > 140)						// while reference voltage greater than 140
	{
		for(count = 0; count < 100; count++) // count 100 counts of the encoder
		{
			while(!enc) 					// while not at encoder
			{
				setPWM(kP, kD, kI, b);		// set PWM
			}

			while(enc) 						// wait till off encoder sensor
			{
				setPWM(kP, kD, kI, b);		// set PWM
			}
		}
		velRef--;							// decrement reference voltage
	}

	brake();								// turn on brake
}

//	Function to set PWM Duty cyle

void	setPWM(uint8_t KP, uint8_t KD, float KI, uint8_t B)
{
	if(readings == 64)						// check if 64 tachometer readings
	{										// if yes
		speed = tachSpeed / 64;				// average the readings
		tachSpeed = 0;						// set tachometer readings to zero

		errorD = error;						// set previous error
		errorI += error;					// add previous error to total error
		error = velRef - speed;				// find new error
		
		PORTB = error;						// display error on Port B

		//	Equation for input with all gains and bias
		//	Proportional - Kp * error
		//	Derivative - Kd * (error - (previous error))
		// 	Integral - Ki * (total error)
		//	Bias - add on bias

		input = (int) error*KP + (error - errorD)*KD + (errorI)*KI + B;	


		if(input > 255)						// if duty cycle above 255
		{
			CCPR1L = 255;					// set at 255
		}
		else if(input < 0)					// if duty cycle negative
		{
			CCPR1L = 0;						// set at 0
		}		

		else
		{
			CCPR1L = input;					// set duty cycle to command input
		}

		readings = 0; 						// set readings to 0
		ADGO = 1;							// start A/D conversion
	}
}

void	brake(void)							// Function to brake motors
{
	motorBrake = 1;							// turn on brake
	CCPR1L = 0x00;							// set duty cycle to 0
}

void	delay(uint8_t t)					//	delay loop - parameter unsigned char
{
	for (i = t; i > 0; i--) {}				
}

void	longTimer(void)						// 1 sec delay loop			
{	
	for(timer = 38461; timer > 0; timer--) {}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//	Interrupt Function - occurs when A/D flag set

void interrupt	isr(void)
{
	ADIF	= 0;							// reset A/D interrupt flag
	if(readings < 64)						// check if at 64 readigns
	{
		readings++;							// increment readings count
		tachSpeed = tachSpeed + ADRES;		// add current output to tachmoter readings
		for (i = 1; i > 0; i--) {}			// delay loop for A/D
		ADGO = 1;							// start A/D conversion
	}
}
