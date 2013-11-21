#include "StepperMotor.h"

/***********************************************************

StepperMotor.C

***********************************************************/

void	main(void)
{
	init();
	while(!greenButton) {}						// wait for initial press
	while(greenButton)	{}						// wait for release
	switchDelay();								// switch debounce
	while(1 != 2)								// infinite loop
	{
		modeSelect();							// select mode
	}
}													

void	init(void)
{
	PORTB	= 0x00;								// clear ports
	PORTC	= 0x00;
	PORTD	= 0x00;
	PORTE	= 0x00;
	
	ADCON1	= 0b00000100;						// set port E as digital
	TRISC 	= 0xff;								// Port C all inputs
	TRISB	= 0b11110000;						// PORTB pins 0-3 out, 4-7 in
	TRISD	= 0xff;								// Port D all inputs
	TRISE	= 0b00000111;						// Port E digital inputs
}

void 	modeSelect(void)
{
	State	= ((~PORTE) & 0b00000111);			// complement and mask
	PORTB	= State;							// set to state LEDs

	switch(State)								// switch case to determine mode
	{
		case 1:
			Mode1();
			break;

		case 2:
			Mode2();
			break;

		case 3:
			Mode3();
			break;

		case 4:
			Mode4();
			break;

		default:								// if not modes 1-4, error
			error();
	}
}

void	Mode1(void)
{
	while(1 =! 2)
	{
		waitPress();
		while(!RB5)
		{
			PORTD	= 0b00000011;
			PORTD	= 0b00000110;
			PORTD	= 0b00001100;
			PORTD	= 0b00001001;
		}
		waitPress();
	}
}

void	Mode2(void)
{
	waitPress();
	while(1 =! 2)
	{
		error();
	}
}

void	Mode3(void)
{
	waitPress();
	while(1 =! 2)
	{
		error();
	}
}

void	Mode4(void)
{
	waitPress();
	while(1 =! 2)
	{
		error();
	}
}

void	waitPress(void)
{
	while(1 != 2)
	{
		if(greenButton)
		{
			while(greenButton)	{}
			switchDelay();
			modeSelect();
		}

		else if(redButton)
		{
			while(redButton)	{}
			switchDelay();
			break;
		}
	}
}

void	longTimer(void)
{
	for(Timer = 38461; Timer > 0; Timer--) {}	// 1s delay loop

}

void	switchDelay(void)					//	Waits for Switch to debounce
{
	for (char i=200; i > 0; i--) {}				// 1200 us delay
}

void	error()
{
	State |= (1 << 3);									// set error LED high
	while(1 != 2)
	{
		PORTB = State;
		longTimer();
		PORTB = 0x00;
		longTimer();
	}	
}