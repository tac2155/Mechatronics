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
	while(1 != 42)									// infinite loop
	{
		State	= ((~PORTE) & 0b00000111);			// complement and mask
		PORTB	= State;							// set to state LEDs

		switch(State)								// switch case to determine mode
		{
			case 1:									// if state 1, mode 1
				Mode1();
				break;

			case 2:									// state 2 - mode 2
				Mode2();
				break;

			case 3:									// state 3 - mode 3
				Mode3();
				break;
	
			case 4:									// state 4 - mode 4
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

	synchMotors();									// synch motors to coils

	uniStep	= 4;									// set current step to 4 
	biStep  = 4;									// for unipolar and bipolar motors
}

void	synchMotors(void)							// syncronizes motors
{
	for(uint8_t synch = 1; synch < 5; synch++)		// for each of the 4 activations
	{
		uniRot(synch);								// rotate unipolar
		biRot(synch);								// rotate bipolar
		longTimer();								// wait one second
	}
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	motorsHome(uint8_t *uniHome, uint8_t *biHome)
{
	while(!RB4 || !RB7)								// while not at horizontal sensors
	{
		if(!RB4)									// if unipolar not at horizonal sensor
		{
			*uniHome++;								// increase step clockwise
			uniRot(*uniHome);						// rotate unipolar motor
		}

		if(!RB7)									// if bipolar not at horizontal sensor
		{												
			*biHome++;								// increase step clockwise
			biRot(*biHome);							// rotate bipolar motor
		}					
		
		stepperDelay();								// delay 50 ms
	}		
}


void	Mode1(void)
{
	motorsHome(&uniStep, &biStep);

	while(1 != 42)
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

	while(1 != 42)
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

	while(1 != 42)
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
		}

		while(!RB4 || !RB6)							// while not at sensors
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
	motorsHome(&uniStep, &biStep);
	waitPress();
	if(green)
	{
		goto modeEnd;

	}

	while(1 != 42)
	{
		while(!RB5 || !RB6)							// while not at sensors
		{
			if(!RB6)								// if bipolar not at sensor
			{	
				*biStep++;							// increment step - cw
				biWave(&biStep);					// rotate bipolar - wavedrive
			}

			if(!RB5)								// if unipolar not at sensor
			{
				*uniStep--;							// decrement step - ccw
				uniWave(&uniStep);					// rotate unipolar motor
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
		}

		while(!RB4 || !RB7)							// while not at sensors
		{
			if(!RB7)								// if bipolar not at sensor
			{	
				*biStep--;							// decrement step - ccw
				biWave(&biStep);						// rotate bipolar motor
			}

			if(!RB4)								// if unipolar not at sensor
			{
				*uniStep++;							// increment step - cw
				uniWave(&uniStep);					// rotate unipolar
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

void	waitPress(void)								// wait for button press
{
	while(1 != 42)									// infinite loop
	{
		if(greenButton)								// if green button pressed
		{
			while(greenButton)	{}					// wait for release
			switchDelay();							// switch debounce
			green 	= 1;							// set green bit hi
			break;									// break out of infinite loop
		}

		else if(redButton)							// if red button pressed
		{
			while(redButton)	{}					// wait for release
			switchDelay();							// switch debounce
			green	= 0;							// set green bit lo
			break;									// break out of inifinite loop
		}
	}
}

//	Stepper Motor Rotations

//	Unipolar rotation - full step motion

void	uniRot(uint8_t *step)						// rotating unipolar motor
{													// paramter is current step of motor
	if(*step == 0)									// if step is 0,
	{												// ccw motion
		*step == 4;									// reset step count to 4
	}

	else if (*step == 5)							// if step is 5
	{												// cw motion
		*step == 1;									// reset step count to 1
	}

	switch(*step)									// switch case to determine
	{												// Port D 0-3 determine unipolar
		case 1:										// 1001
			RD0	= 1;								
			RD1	= 0;
			RD2	= 0;
			RD3 = 1;
			break;

		case 2:										// 0011
			RD0	= 1;
			RD1	= 1;
			RD2	= 0;
			RD3 = 0;
			break;

		 case 3:									// 0110
			RD0	= 0;
			RD1	= 1;
			RD2	= 1;
			RD3 = 0;
			break;

		case 4:										// 1100
			RD0	= 0;
			RD1	= 0;
			RD2	= 1;
			RD3 = 1;
			break;
	}
}

//	Bipolar rotation - full step motion

void	biRot(uint8_t *step)						// rotating bipolar motar		
{													// parameter is current step of motor
	if(*step == 0)									// if step is 0,
	{												// ccw motion
		*step == 4;									// reset step count to 4
	}

	else if (*step == 5)							// if step is 5
	{												// cw motion
		*step == 1;									// reset step count to 1
	}

	switch(*step)									// full step - Ia and Ib = 0
	{												// Port D 4-7 determine bipolar
		case 1:										// 0100
			RD4	= 0;
			RD6	= 1;
			break;

		case 2:										// 0000
			RD4	= 0;
			RD6	= 0;	
			break;

		 case 3:									// 1000
			RD4	= 1;
			RD6	= 0;
			break;

		case 4:										// 1010
			RD4	= 1;
			RD6	= 1;
			break;
	}	
}
/*
void	uniWave(uint8_t *step)						//	cw if ++
{
	stepReset:
	switch(*step)
	{
		case 1:
			RD0	= 1;
			RD1	= 0;
			RD2	= 0;
			RD3 = 0;
			break;

		case 2:
			RD0	= 0;
			RD1	= 1;
			RD2	= 0;
			RD3 = 0;
			break;

		 case 3:
			RD0	= 0;
			RD1	= 0;
			RD2	= 1;
			RD3 = 0;
			break;

		case 4:
			RD0	= 0;
			RD1	= 0;
			RD2	= 0;
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

void	biWave(uint8_t *step)						//	cw if ++		
{
	stepReset:	
	switch(*step)
	{
		case 1:										//	step through phases
			RD4	= 1;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 2:
			RD4	= 0;
			RD5 = 1;
			RD6	= 1;
			RD7 = 0;	
			break;

		 case 3:
			RD4	= 0;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 4:
			RD4	= 0;
			RD5 = 0;
			RD6	= 0;
			RD7 = 1;
			break;

		case 5:
			RD4	= 0;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;
		
		case 6:
			RD4	= 1;
			RD5 = 1;
			RD6	= 0;
			RD7 = 0;
			break;

		case 7:
			RD4	= 1;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;

		case 8:
			RD4	= 1;
			RD5 = 0;
			RD6	= 1;
			RD7 = 1;
			break;
		
		case 9:
			*step = 1;
			goto stepReset;
		case 0:
			*step = 8;
			goto stepReset;
	}	
}
*/
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
	while(1 != 42)									//	Infinite loop
	{
		PORTB = State;								//	Flash mode error every second
		longTimer();
		PORTB = 0x00;
		longTimer();
	}	
}