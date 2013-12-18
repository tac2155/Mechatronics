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
	delay(200);								// delay for switch to debounce, 1200us
	
	motorControl();

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
	delay(1);							// small delay
}

void	initPWM(void)
{
	CCP1CON	= 0b00001100;					// Set PWM in CCP1
	PR2		= 0xff;							// set PR2
	T2CON 	= 0b00000101;					// set up Timer 2
}

void	refVoltage(void)
{
	eddyCount = 0;
	desVel	  = 150;

	dir = 1;								// Motor cw
	CCPR1L 	 = desVel;						// set reference voltage at 140
	PWM 	 = 1;							// start PWM

	while(eddyCount < 4)
	{
		if(eddy)
		{
			eddyCount++;
		}
	}
	brake();
	eddyCount = 0;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	motorControl(void)
{
	dir 	= 1;
	motorBrake = 0;
	CCPR1L	= desVel;
	PWM 	= 1;	

	motorRef	= speedRead();
	error = desVel - motorRef;
	PORTB = error;

	while(motorRef < 190)
	{
		if((error * kp) > 255)
		{
			CCPR1L = 255;
		}
		else if((error * kp) < 0)
		{
			CCPR1L = 0;
		}		
		else
		{
			CCPR1L = (error * kp);
		}

		while(count < 100)
		{
			if(enc)
			{
				count++;
			}
		}
		motorRef = speedRead();
		error = desVel - motorRef;
		desVel++;
		PORTB = error;
	}
	count = 0;
	
	while(count < 6000)
	{
		if(enc)
		{
			count++;
		}
	}
	count = 0;

	while(desVel > 140)
	{
		if((error * kp) > 255)
		{
			CCPR1L = 255;
		}
		else if((error * kp) < 0)
		{
			CCPR1L = 0;
		}
		else
		{
			CCPR1L = (error * kp);
		}

		while(count < 100)
		{
			if(enc)
			{
				count++;
			}
		}
		motorRef = speedRead();
		error = desVel - motorRef;
		desVel--;
		PORTB = error;
	}

	brake();
}

void	brake(void)
{
	motorBrake = 1;
	CCPR1L = 0x00;
}

uint8_t	speedRead(void)
{
	uint8_t vel;
	ADGO = 1;
	while(readings <= 64) {}
	vel = tachSpeed;
	readings = 0;
	return vel;
}

void	delay(uint8_t t)							//	delay for loop
{
	uint8_t i;
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
	if(readings <= 64)
	{
		readings++;
		tachSpeed = (tachSpeed + ADRES)/ readings;
		ADGO = 1;
	}
}
