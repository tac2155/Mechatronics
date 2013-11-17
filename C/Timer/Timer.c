#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

Timer.c

***********************************************************/

//	Variable declarations

	unsigned long Timer;							// Holds value for 1 sec timer
	unsigned char Count;							// holds value for Count

void	main(void)
{

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	Intialization of port D & counter

	PORTD	= 0x00;									// clear port D
	TRISD	= 0x00;									// config port D as outputs
	Count 	= 0;									// clear counter

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

	while(1 != 2)
	{
		for(Timer = 38461; Timer > 0; Timer--) {}	// 1s delay loop
		Count++; 									// increment count
		PORTD = Count;								// display count on Port D

//		Could also use PORTD++ to display values on Port D	
	}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
}