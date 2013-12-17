#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

AtoDpolled.c 

***********************************************************/

// Variable Declarations

	char Temp;								// Variable for delay loop

void	SetupDelay(void)					// delay loop
{
	for (uint8_t Temp = 1; Temp > 0; Temp--) {}		// 17 us delay
}

void	initAtoD(void)						// initialize A/D
{
	ADCON1	= 0b00000100					// RA0, RA1, RA3 analog inputs
	ADCON0	= 0b01000001					// select 8* oscillator, analog input 0, turn on 
	SetupDelay();							// small delay
	ADGO	= 1; 							// Start A/D
}

//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void	main(void)
{

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	Initialization of ports and A/D

	PORTD	= 0x00;							// clear port D
	TRISD	= 0x00;							// configure port D as all output

	initAtoD();

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	while(1 != 2)							// infinite loop
	{
		while(ADGO) {}						// wait until A/D conversion done
		while(ADGO)	{}						// make Sure A/D done
		PORTD	= ADRES;					// Display A/D value on Port D LED's
		ADGO 	= 1;						// restart A/D
	}

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
	
}
