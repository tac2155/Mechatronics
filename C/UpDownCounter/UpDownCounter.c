#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

UpDownCounter.c 

***********************************************************/

/* Variable declarations */

	static bit		greenButton		RC0
	static bit		redButton		RC1

	char Count, i;

void	SwitchDelay(void)					//	Waits for Switch to debounce
{
	for (i=200; 1 > 0; i--) {}				// 1200 us delay
}

void	main(void)
{

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	PORTD	= 0b00000000;					// Clear Port D outputs
	TRISD 	= 0b00000000;					// Config Port D as all output
	TRISC	= 0xff;							// Config Port C as all input

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	while(1 != 2)							// Infinite loop
	{

		if(greenButton) 					// If green press
		{
			while(greenButton) {} 			// Wait for release
			SwitchDelay();					// Let Switch debounce
			Count++;						// Increment Count
			PORTD = Count;					// Display count on PORTD LEDs
		}

		else if(redButton) 					// If red press
		{
			while(redButton){}				// wait for release		
			SwitchDelay();					// let switch debounce
			Count--;						// decrement count
			PORTD = Count;					// display on LEDs
		}
	}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
}