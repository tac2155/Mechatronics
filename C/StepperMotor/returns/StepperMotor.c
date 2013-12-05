#include "StepperMotor.h"

/***********************************************************

StepperMotor.C

***********************************************************/

void	main(void)
{
	init();

	uniStep	= 4;									// set current step to 4 
	biStep  = 4;									// for unipolar and bipolar motors

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
	
			/*case 4:									// state 4 - mode 4
				Mode4();
				break;
*/
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
	TRISD	= 0x00;									// Port D all inputs
	TRISE	= 0b00000111;							// Port E digital inputs

	synchMotors();									// synch motors to coils

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

// Motors to home position - horizontal

uint8_t	uniHome(uint8_t uHome)
{
	while(!uniHor)									// uni not at horizontal
	{									
		uHome++;								// increase step clockwise
		uHome = uniRot(uHome);					// rotate unipolar motor
		stepperDelay();
	
	}
	return uHome;
}

uint8_t	biHome(uint8_t bHome)
{
	while(!biHor)
	{								
		bHome++;								// increase step clockwise
		bHome = biRot(bHome);					// rotate bipolar motor				
		stepperDelay();								// delay 50 ms
	}	
	return bHome;
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	Mode1(void)
{
	uniStep = uniHome(uniStep);
	biStep 	= biHome(biStep);

	waitPress();

	while((1 != 42) && !green)
	{
		if(uniHor && biHor)
		{
			while(!uniVer)
			{
				uniStep--;
				uniStep = uniRot(uniStep);
				stepperDelay();
			}
		
		else if(uniVer && biHor)
		{
			while(!biVer)
			{	
				biStep++;
				biStep = biRot(biStep);
				stepperDelay();
			}
		}

		else if(uniVer && biVer)
		{
			while(!uniHor)
			{
				uniStep++;
				uniStep = uniRot(uniStep);
				stepperDelay();
			}
		}

		else if(uniHor && biVer)
		{
			while(!biHor)
			{
				biStep--;
				biStep = biRot(biStep);
				stepperDelay();
			}	
		}
		waitPress();		
	}
}

// Move motors in oppostite directions in synch

void	Mode2(void)
{
	uniStep = uniHome(uniStep);
	biStep 	= biHome(biStep);
	
	waitPress();

	while((1 != 42) && !green)
	{
		while((!uniVer || !biVer) && !green)							// while not at sensors
		{
			if(!biVer)								// if bipolar not at sensor
			{	
				biStep++;							// increment step - cw
				biStep = biRot(biStep);
			}

			if(!uniVer)								// if unipolar not at sensor
			{
				uniStep--;							// decrement step - ccw
				uniStep = uniRot(uniStep);
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button
		{
			while(redButton) {}						// motors stop, wait for release
			switchDelay();
			waitPress();
		}

		while((!uniHor || !biHor) && !green)							// while not at sensors
		{
			if(!biHor)								// if bipolar not at sensor
			{	
				biStep--;							// decrement step - ccw
				biStep = biRot(biStep);
			}

			if(!uniHor)								// if unipolar not at sensor
			{
				uniStep++;							// increment step - cw
				uniStep = uniRot(uniStep);
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button - stops motors
		{
			while(redButton) {}						// wait for release
			switchDelay();							// switch debounce
			waitPress();							// wait for button press				
		}
	}									
}

void	Mode3(void)
{
	uniStep = uniHome(uniStep);

	while(!biVer)										// bipolar to vertical
	{
		biStep++;									// increment step
		biStep = biRot(biStep);
		stepperDelay();								// motor delay
	}

	waitPress();

	while((1 != 42) && !green)
	{
		while((!uniVer || !biHor) && !green)							// while not at sensors
		{
			if(!biHor)								// if bipolar not at sensor
			{	
				biStep++;							// rotate bipolar motor
				biStep = biRot(biStep);
			}

			if(!uniVer)								// if unipolar not at sensor
			{
				uniStep++;							// rotate unipolar sensor
				uniStep = uniRot(uniStep);
			}
			stepperDelay();
		}

		if(redButton)
		{
			while(redButton) {}
			switchDelay();
			waitPress();
		}

		while((!uniHor || !biVer) && !green)				// while not at sensors
		{
			if(!biVer)								// if bipolar not at sensor
			{	
				biStep--;							// rotate bipolar motor
				biStep = biRot(biStep);
			}

			if(!uniHor)								// if unipolar not at sensor
			{
				uniStep--;							// rotate unipolar sensor
				uniStep = uniRot(uniStep);
			}
			stepperDelay();
		}

		if(redButton)
		{
			while(redButton) {}
			switchDelay();
			waitPress();			
		}
	}
}

void	Mode4(void)
{
	uniStep = uniHome(uniStep);
	biStep 	= biHome(biStep);
	
	waitPress();

	while((1 != 42) && !green)
	{
		while((!uniVer || !biVer) && !green)							// while not at sensors
		{
			if(!biVer)								// if bipolar not at sensor
			{	
				biStep++;							// increment step - cw
				biStep = biRot(biStep);
			}

			if(!uniVer)								// if unipolar not at sensor
			{
				uniStep--;							// decrement step - ccw
				uniStep = uniRot(uniStep);
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button
		{
			while(redButton) {}						// motors stop, wait for release
			switchDelay();
			waitPress();
		}

		while((!uniHor || !biHor) && !green)							// while not at sensors
		{
			if(!biHor)								// if bipolar not at sensor
			{	
				biStep--;							// decrement step - ccw
				biStep = biRot(biStep);
			}

			if(!uniHor)								// if unipolar not at sensor
			{
				uniStep++;							// increment step - cw
				uniStep = uniRot(uniStep);
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button - stops motors
		{
			while(redButton) {}						// wait for release
			switchDelay();							// switch debounce
			waitPress();							// wait for button press				
		}
	}									
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

uint8_t	uniRot(uint8_t step)						// rotating unipolar motor
{													// paramter is current *step of motor
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 4;									// reset *step count to 4
	}

	else if (step == 5)							// if *step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}

	switch(step)									// switch case to determine
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
	return step;
}

//	Bipolar rotation - full *step motion

uint8_t	biRot(uint8_t step)							// rotating bipolar motar		
{													// parameter is current *step of motor
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 4;									// reset *step count to 4
	}

	else if (step == 5)								// if *step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}

	switch(step)									// full *step - Ia and Ib = 0
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
	return step;	
}

//	Mode 4 Rotations
//	Wave Step Unipolar

uint8_t	uniWave(uint8_t step)						//	cw if ++
{
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 4;									// reset *step count to 4
	}

	else if (step == 5)							// if *step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}

	switch(step)
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
	}
	return step;
}

//	Bipolar rotation - full step

uint8_t	biWaveF(uint8_t step)						//	cw if ++		
{
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 8;									// reset *step count to 4
	}

	else if (step == 9)							// if *step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}

	switch(step)
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
	}
	return step;	
}

//	Bipolar rotation - full step

uint8_t	biWaveR(uint8_t Step)						//	cw if ++		
{
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 8;									// reset *step count to 4
	}

	else if (step == 9)							// if *step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}
	switch(step)
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
	}
	return step;	
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
	for (int j=2000; j > 0; j--) {}					// 	50 ms delay
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