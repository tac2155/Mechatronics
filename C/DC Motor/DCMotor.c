#include "DCMotor.h"

/***********************************************************

Case Study 6 - DC Motor

Tim Cousins
Jamie Collier
Chi Yung Fung

***********************************************************/

void main(void)
{
	init();
	
	while(!blackButton) {} 					// wait for button press
	while(blackButton)  {}					// wait for release
	delay(200);	
	
	motorControl(kp, 0, 0, 0);							// delay for switch to debounce, 1200us
/*
	if(MSBHex)
	{
		if(LSBHex)
		{
			mode = 3;
		}
		else
		{
			mode = 2;
		}
	}
	else if(LSBHex)
	{
		mode = 1;
	}
	else
	{
		mode = 0;
	}
	
	switch(mode)
	{
		case 0:
			motorControl(kp, 0, 0, 0);
			break;

		case 1:
			motorControl(kp, 0, 0, bias);
			break;

		case 2:
			motorControl(kp, kd, 0, 0);
			break;

		case 3:
			motorControl(kp, kd, ki, 0);
			break;
	}
*/
	while(1 != 42)
	{
		PORTB = 0xff;						//	Flash mode error every second
		longTimer();
		PORTB = 0x00;
		longTimer();
	}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	Initialization

void	init(void)
{
	PORTA	= 0x00;							// Clear Ports
	PORTB 	= 0x00;
	PORTC	= 0x00;

	TRISA4	= 1;							// Set Port A, pin 4 as input
	TRISC	= 0b11111000;					// set Port C inputs and outputs
	TRISB	= 0x00;							// Set Port B as all outputs

	initISR();
	initAtoD();								// AtoD Initialization
	initPWM();								// PWM initialization

	refVoltage();

	kp  	= 15;
	kd 		= 5;
	ki		= 0.005;

	readings  = 0;
	speed	  = 0;
}

void	initISR(void)						// initialize interrupts				
{
	INTCON	= 0b11000000;					// enable global and peripheral interrupts
	ADIE	= 1;
	ADIF	= 0;
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
	T2CON 	= 0b00000101;					// set up Timer 2
}

void	refVoltage(void)
{
	desVel	= 150;
	bias 	= desVel;

	dir 	= 1;							// Motor cw
	CCPR1L 	= desVel;						// set reference voltage at 150
	PWM 	= 1;							// start PWM

	for (eddyCount = 0; eddyCount < 4; eddyCount++)
	{
		while(!eddy) {}
	}

	brake();
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	motorControl(uint8_t kP, uint8_t kD, double kI, uint8_t b)
{
	dir 	= 1;
	motorBrake = 0;
	CCPR1L	= 150;
	PWM 	= 1;

	ADGO 	= 1;
	while(readings < 64) {}
	readings = 0;

	velRef	= speed;	
	
	errorP = 0;
	error = velRef - speed;
	PORTB = error;

	while(velRef < 190)
	{
		delay(1);
		ADGO = 1;
		input = error*kP + (error - errorP)*kD + (error + errorP)*kI + b;
		
		if(input > 255)
		{
			CCPR1L = 255;
		}
		else if(input < 0)
		{
			CCPR1L = 0;
		}		
		else if(input == 0)
		{
			CCPR1L = velRef;
		}

		else
		{
			CCPR1L = input;
		}

		for(count = 0; count < 100; count++)
		{
			while(!enc) {}
		}

		while(readings < 64) {}
		readings = 0;

		errorP = error;
		error = velRef - speed;

		velRef++;
		PORTB = error;
	}
	
	for(count = 0; count < 6000; count++)
	{
		while(!enc) {}
	}

	count = 0;

	while(velRef > 140)
	{
		delay(1);
		ADGO = 1;
		input = error*kP + (error - errorP)*kD + (error + errorP)*kI + b;
		
		if(input > 255)
		{
			CCPR1L = 255;
		}
		else if(input < 0)
		{
			CCPR1L = 0;
		}		
		else if(input == 0)
		{
			CCPR1L = velRef;
		}

		else
		{
			CCPR1L = input;
		}

		for(count = 0; count < 100; count++)
		{
			while(!enc) {}
		}

		while(readings < 64) {}
		readings = 0;

		errorP = error;
		error = velRef - speed;

		velRef--;
		PORTB = error;
	}

	brake();
}

void	brake(void)
{
	motorBrake = 1;
	CCPR1L = 0x00;
}

void	delay(uint8_t t)							//	delay for loop
{
	for (i = t; i > 0; i--) {}				
}

void	longTimer(void)						
{
	for(timer = 38461; timer > 0; timer--) {}		// 1s delay loop
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void interrupt	isr(void)
{
	ADIF	= 0;
	if(readings < 64)
	{
		readings++;
		tachSpeed = tachSpeed + ADRES;
		for (i = 1; i > 0; i--) {}	
		ADGO = 1;
	}

	else
	{
		speed = tachSpeed / 64;
		tachSpeed = 0;
	}
}
