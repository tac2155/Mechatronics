#include	"OnOffControl.h"

/***********************************************************

OnOffControl.c

***********************************************************/

void	main(void)
{
	init();

	while(!greenButton)	{}
	while(greenButton)	{}
	switchDelay();
	
	while(1 != 2)
	{
		State	= ((~PORTE) & 0b00000111);
		PORTB	= State;

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

			default:
				error();
		}
	}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// initialization for ports

void	init(void)
{
	PORTB	= 0x00;								// clear ports
	PORTC	= 0x00;
	PORTD	= 0x00;
	PORTE	= 0x00;

	initAtoD();									// initializes A/D

	TRISB	= 0x00;								// port B outputs
	TRISD	= 0b11111100;						// pins 0 and output on PortD
	TRISC	= 0xff;								// port C inputs
	TRISE	= 0b00000111;						// port E digital inputs

	Count 	= 0;
	State 	= 0;
}


void	initAtoD(void)						// initialize A/D
{
	ADCON1	= 0b00000100;					// RA0, RA1, RA3 analog inputs
	ADCON0	= 0b01000001;					// select 8* oscillator, analog input 0, turn on 
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	Mode1(void)
{
	while(1 != 2)
	{
		waitPress();
		if(green)
		{
			break;
		}
		mainTran	= 1;
		waitPress();
		if(green)
		{
			break;
		}
		mainTran	= 0;
	}
}

void	Mode2(void)
{
	while(1 != 2)						// infinite loop
	{
		waitPress();					// wait for button press
		Count	= AtoD();				// get timer value
		mainTran	= 1;				// turn on main transistor
		while(Count > 0)
		{
			if (redButton)
			{
				Count = AtoD();
			}
			longTimer();				// 1s delay loop
			Count--;
		}
		mainTran	= 0;
	}
}

void	Mode3(void)
{
	while(1 != 2)						// infinte loop
	{
		error1 	= 10;
		error2	= 10;
		error3	= 2;
		waitPress();
		Count	= AtoD();
		mainTran	= 1;
		while(!engaged)
		{
			longTimer();
			error1--;
			if(error1 == 0)
			{
				error();
			}
		}
		secTran	 = 1;
		switchDelay();
		mainTran = 0;
		while(Count > 0)
		{
			longTimer(); 				// 1s delay loop
			if(!engaged)
			{
				error3--;
				if(error3 == 0)
				{
					error();
				}
				mainTran	= 1;
				SwitchDelay();
				mainTran	= 0;
			}
			Count--;
		}
		secTran	= 0;
		while(engaged)
		{
			longTimer();
			error2--;
			if(error2 == 0)
			{
				error();
			}
		}
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
			green 	= 1;
			break;
		}

		else if(redButton)
		{
			while(redButton)	{}
			switchDelay();
			green 	= 0;
			break;
		}
	}
}

unsigned char	AtoD(void)
{

	ADGO	= 1;
	while(ADGO) {}						// wait until A/D conversion done
	while(ADGO)	{}						// make Sure A/D done

	if (ADRES == 0)
	{
		error();
	}

	return (ADRES / 4);
}

void	switchDelay(void)					//	Waits for Switch to debounce
{
	for (char i=200; i > 0; i--) {}				// 1200 us delay
}

void	longTimer(void)
{
	for(Timer = 38461; Timer > 0; Timer--) {}	// 1s delay loop

}

void	error(void)
{
	while(1 != 2)
	{
		State |= (1 << 3);						// set error LED high
		PORTB = State;
		longTimer();
		PORTB = 0x00;
		longTImer();
	}	
}

