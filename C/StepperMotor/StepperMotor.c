#include "StepperMotor.h"

/***********************************************************

StepperMotor.C

***********************************************************/

void	main(void)
{
	init();
	while(!greenButton) {}									// wait for initial press
	while(greenButton)	{}									// wait for release
	switchDelay();											// switch debounce

	while(1 != 2)
	{
		State	= ((~PORTE) & 0b00000111);					// complement and mask
		PORTB	= State;									// set to state LEDs

		switch(State)
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

			default:
				error();
		}
	}
}
													

void	init(void)
{
	PORTB	= 0x00;
	PORTC	= 0x00;
	PORTD	= 0x00;
	PORTE	= 0x00;
	
	TRISC 	= 0xff;
	TRISB	= 0x00;
	TRISE	= 0b00000111;
}

void	switchDelay(void)					//	Waits for Switch to debounce
{
	for (char i=200; i > 0; i--) {}				// 1200 us delay
}

void	Mode1(void)
{
	waitPress();
	error();
}

void	Mode2(void)
{
	waitPress();
	error();
}

void	Mode3(void)
{
	waitPress();
	error();
}

void	Mode4(void)
{
	waitPress();
	error();
}

void	waitPress(void)
{
	while(1 != 2)
	{
		if(greenButton)
		{
			while(greenButton)	{}
			switchDelay();
			green = 1;
			break;
		}

		else if(redButton)
		{
			while(redButton)	{}
			switchDelay();
			green = 0;
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