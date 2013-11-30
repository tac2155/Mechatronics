#include "StepperMotor.h"

/***********************************************************

StepperMotor.C

***********************************************************/

void	main(void)
{
	init();
	while(!greenButton) {}							// wait for initial press
	while(greenButton)	{}							// wait for release
	switchDelay();									// switch debounce
	while(1 != 2)									// infinite loop
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
}													

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

//	Initialization

void	init(void)
{
	PORTB	= 0x00;									// clear ports
	PORTC	= 0x00;
	PORTD	= 0x00;
	PORTE	= 0x00;
	
	ADCON1	= 0b00000100;							// set port E as digital
	TRISC 	= 0xff;									// Port C all inputs
	TRISB	= 0b11110000;							// PORTB pins 0-3 out, 4-7 in
	TRISD	= 0xff;									// Port D all inputs
	TRISE	= 0b00000111;							// Port E digital inputs

	synchMotors();

	uniStep	= 4;
	biStep  = 4;
}

void	synchMotors(void)
{
	for(uint8_t synch = 1; synch < 5; synch++)
	{
		uniRot(synch);
		biRot(synch);
		longTimer();
	}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	motorsHome(uint8_t *uniHome, uint8_t *biHome)
{
	while(!RB4)										// unipolar to home, cw
	{
		*uniHome++;
		uniRot(*uniHome);
		stepperDelay();
	}

	while(!RB7)										//	bipolar to home, cw
	{
		*biHome++;
		biRot(*biHome);
		stepperDelay();
	}		
}


void	Mode1(void)
{
	motorsHome(&uniStep, &biStep);

	while(1 != 2)
	{
		waitPress();
		if(green)
		{
			break;
		}

		if(RB4 && RB7)
		{
			while(!RB5)
			{
				*uniStep--;
				uniRot(&uniStep);
				stepperDelay();
			}

			while(!RB6)
			{
				*biStep++;
				biRot(&biStep);
				stepperDelay();
			}
		}
		else if (RB5 && RB6)
		{
			while(!RB4)
			{
				*uniStep++;
				uniRot(&uniStep);
				stepperDelay();
			}

			while(!RB7)
			{
				*biStep--;
				biRot(&biStep);
				stepperDelay();
			}			
		}
	}
}

// Move motors in oppostite directions in synch

void	Mode2(void)
{
	motorsHome(&uniStep, &biStep);
	waitPress();
	if(green)
	{
		goto modeEnd;

	}

	while(1 != 2)
	{
		while(!RB5 || !RB6)							// while not at sensors
		{
			if(!RB6)								// if bipolar not at sensor
			{	
				*biStep++;							// increment step - cw
				biRot(&biStep);						// rotate bipolar motor
			}

			if(!RB5)								// if unipolar not at sensor
			{
				*uniStep--;							// decrement step - ccw
				uniRot(&uniStep);					// rotate unipolar motor
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button
		{
			while(redButton) {}						// motors stop, wait for release
			switchDelay();
			waitPress();							// wait for next press
			if(green)								// green press to switch modes
			{
				goto modeEnd;
			}
		
		while(!RB4 || !RB7)							// while not at sensors
		{
			if(!RB7)								// if bipolar not at sensor
			{	
				*biStep--;							// decrement step - ccw
				biRot(&biStep);						// rotate bipolar motor
			}

			if(!RB4)								// if unipolar not at sensor
			{
				*uniStep++;							// increment step - cw
				uniRot(&uniStep);					// rotate unipolar
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button - stops motors
		{
			while(redButton) {}						// wait for release
			switchDelay();							// switch debounce
			waitPress();							// wait for button press
			if(green)								// if green, switch modes
			{
				goto modeEnd;
			}				
		}
	}
	modeEnd: ;										// label to switch modes
}

void	Mode3(void)
{
	while(!RB4)										// unipolar to horizontal
	{
		*uniHome++;
		uniRot(*uniHome);
		stepperDelay();
	}

	while(!RB6)										// bipolar to vertical
	{
		*biHome++;									// increment step
		biRot(*biHome);								// rotate bipolar
		stepperDelay();								// motor delay
	}

	waitPress();
	if(green)
	{
		goto modeEnd;

	}

	while(1 != 2)
	{
		while(!RB5 || !RB7)							// while not at sensors
		{
			if(!RB7)								// if bipolar not at sensor
			{	
				*biStep++;							// rotate bipolar motor
				biRot(&biStep);
			}

			if(!RB5)								// if unipolar not at sensor
			{
				*uniStep++;							// rotate unipolar sensor
				uniRot(&uniStep);
			}
			stepperDelay();
		}

		if(redButton)
		{
			while(redButton) {}
			switchDelay();
			waitPress();
			if(green)
			{
				goto modeEnd;
			}
		
		while(!RB4 || !RB6)							// while not at sensores
		{
			if(!RB6)								// if bipolar not at sensor
			{	
				*biStep--;							// rotate bipolar motor
				biRot(&biStep);
			}

			if(!RB4)								// if unipolar not at sensor
			{
				*uniStep--;							// rotate unipolar sensor
				uniRot(&uniStep);
			}
			stepperDelay();
		}

		if(redButton)
		{
			while(redButton) {}
			switchDelay();
			waitPress();
			if(green)
			{
				goto modeEnd;
			}				
		}
	}
	modeEnd: ;
}

void	Mode4(void)
{
	while(1 != 2)
	{
		waitPress();
		if(green)
		{
			break;
		}		
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
			green 	= 1;
			break;
		}

		else if(redButton)
		{
			while(redButton)	{}
			switchDelay();
			green	= 0;
			break;
		}
	}
}

//	Stepper Motor Rotations

//	Unipolar rotation - full step

void	uniRot(uint8_t *step)						//	cw if ++
{
	stepReset:
	switch(*step)
	{
		case 1:
			RD0	= 1;
			RD1	= 0;
			RD2	= 0;
			RD3 = 1;
			break;

		case 2:
			RD0	= 1;
			RD1	= 1;
			RD2	= 0;
			RD3 = 0;
			break;

		 case 3:
			RD0	= 0;
			RD1	= 1;
			RD2	= 1;
			RD3 = 0;
			break;

		case 4:
			RD0	= 0;
			RD1	= 0;
			RD2	= 1;
			RD3 = 1;
			break;
		case 5:
			*step = 1;
			goto stepReset;
		case 0:
			*step = 4;
			goto stepReset;
	}
}

//	Bipolar rotation - full step

void	biRot(uint8_t *step)						//	cw if ++		
{
	stepReset:	
	switch(*step)
	{
		case 1:										//	step through phases
			RD4	= 0;
			RD6	= 1;
			break;

		case 2:
			RD4	= 0;
			RD6	= 0;	
			break;

		 case 3:
			RD4	= 1;
			RD6	= 0;
			break;

		case 4:
			RD4	= 1;
			RD6	= 1;
			break;

		case 5:
			*step = 1;
			goto stepReset;
		case 0:
			*step = 4;
			goto stepReset;
	}	
}

//	Timers

void	longTimer(void)						
{
	for(Timer = 38461; Timer > 0; Timer--) {}	// 1s delay loop

}

void	switchDelay(void)							//	Waits for Switch to debounce
{
	for (uint8_t i=200; i > 0; i--) {}				// 	1200 us delay
}


void	stepperDelay(void)							//	delay for stepper windings
{
	for (int j=8350; j > 0; j--) {}					// 	50 ms delay
}

//	Error - only end with reset

void	error()
{
	PORTD =	0x00;									//	Turn off motors
	State |= (1 << 3);								// 	set error LED high
	while(1 != 2)									//	Infinite loop
	{
		PORTB = State;								//	Flash mode error every second
		longTimer();
		PORTB = 0x00;
		longTimer();
	}	
}