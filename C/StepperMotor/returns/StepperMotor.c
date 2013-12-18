#include "StepperMotor.h"

/***********************************************************

StepperMotor.C

Tim Cousins
Chi Yung Fung
Jamie Collier

Case Study 5

Four modes to move stepper motor in multiple ways.
Initiation clears the Ports and sets the directions of the ports.
Synchs the motors by running each motor through each step slowly.

Mode selected by octal switch, Port E, pins 0-2. The mode is stored
in the State variable and displayed on the Port B pins 0-3 LEDs. 
Pin 3 is turned hi when an error occurs.

Modes 1-3 move the motors in Full Step Drive, given by the 
uniRot and biRot functions.

Mode 4 moves the motors in Wave drice, given by uniWave, biWaveF and 
biWaveR functions.

Mode 1
Starts with motors in horizontal home position. Pressing red
button moves the unipolar to vertical sensor by shortest route, ccw.
Another red button moves the bipolar motor to the vertical sensor
by the shortest route, cw. Another redbutton press moves the unipolar
motor back to start posistion by shortest path, cw. A red button press
moves the bipolar motor back to its start position by shortest path, ccw.
This pattern repeats. Pressing the greenbutton when ever the motors are
not moving changes to mode selected by octal switch.

Mode 2
Starts with motors in horizontal position. Pressing red button will make
the motors rotate 90 degrees to vertical sensor. Once both reach vertical,
the motors rotate back to horizontal by 90 degree path. This repeats continuously.
If the red button is held down when the motors reach a sensor position, 
they stop and wait for a button press. If the red button is pressed, the
pattern continues. If the green button is pressed, the program goes to the 
mode indicated by the octal switch.

Mode 3
Starts with unipolar in horizontal position and the bipolar in vertical position.
Pressing red button will make the motors rotate 270 degrees to the other sensor. 
Once both reach their respective sensors, the motors rotate back to the opposite
sensor by 270 degree path. This repeats continuously. If the red button is held 
down when the motors reach a sensor position, they stop and wait for a button 
press. If the red button is pressed, the pattern continues. If the green button 
is pressed, the program goes to the selected mode indicated by the octal switch.

Mode 4
Same as mode 2, except with Wave drive control.

Other References:
The C Programming Language, Second Edtion - Brian Kernichan and Dennis Ritchie
www.stackoverflow.com - various syntax questions

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
	TRISD	= 0x00;									// Port D all outputs
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

uint8_t	uniHome(uint8_t uHome)						// takes unipolar to home horizontal position
{
	while(!uniHor)									// uni not at horizontal
	{									
		uHome++;									// increase step clockwise
		uHome = uniRot(uHome);						// rotate unipolar motor
		stepperDelay();								// delat for stepper
	
	}
	return uHome;									// return unipolar step count
}

uint8_t	biHome(uint8_t bHome)						// takes bipolar to home horizontal position
{
	while(!biHor)
	{								
		bHome++;									// increase step clockwise
		bHome = biRot(bHome);						// rotate bipolar motor				
		stepperDelay();								// delay for stepper
	}	
	if(!biHor)										// make sure bipolar at sensor
	{	
		bHome++;									// increase bipolar step count
		bHome = biRot(bHome);						// rotate bipolar motor
	}	
	return bHome;									// return bipolar step count
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void	Mode1(void)
{
	uniStep = uniHome(uniStep);						// moves unipolar to home
	biStep 	= biHome(biStep);						// moves bipolar to home

	waitPress();									// wait for button press

	while(!green)									// infinite loop if green
	{												// button not pressed
		if(!uniVer && !biVer)							// if both at horizontal
		{
			while(!uniVer)							// while unipolar not at vertical
			{
				uniStep--;							// decrement step for unipolar ccw
				uniStep = uniRot(uniStep);			// rotate unipolar
				stepperDelay();						// delay for stepper motor
			}
		}
		else if(!uniHor && !biVer)					// if unipolar vertical and 
		{											// bipolar horizontal
			while(!biVer)							// while bipolar not at vertical
			{			
				biStep++;							// increment bipolar step - cw
				biStep = biRot(biStep);				// rotate bipolar
				stepperDelay();						// delay for stepper
			}
		}

		else if(!uniHor && !biHor)					// if both at vertical
		{
			while(!uniHor)							// while unipolar not at horizontal
			{
				uniStep++;							// increment unipolar step - cw
				uniStep = uniRot(uniStep);			// rotate unipolar motor
				stepperDelay();						// delay for stepper
			}
		}

		else if(!uniVer && !biHor)					// if unipolar at horizontal and 
		{											// bipolar vertical
			while(!biHor)							// while bipolar not at horizontal
			{
				biStep--;							// decrement bipolar step - ccw
				biStep = biRot(biStep);				// rotate bipolar
				stepperDelay();						// delay for stepper
			}	
		}
		waitPress();								// wait for button press	
	}
}

// Move motors in oppostite directions in synch

void	Mode2(void)
{
	uniStep = uniHome(uniStep);						// moves unipolar to start position
	biStep 	= biHome(biStep);						// moves bipolar to start position
	
	waitPress();									// wait for button press

	while(!green)									// red button starts mode
	{
		while((!uniVer || !biVer) && !green)		// while not at sensors and not green button
		{
			if(!biVer)								// if bipolar not at sensor
			{	
				biStep++;							// increment step - cw
				biStep = biRot(biStep);				// rotate bipolar motor
			}

			if(!uniVer)								// if unipolar not at sensor
			{
				uniStep--;							// decrement step - ccw
				uniStep = uniRot(uniStep);			// rotate unipolar motor
			}
			stepperDelay();							// motor delay
		}

		if(redButton)								// if red button
		{
			while(redButton) {}						// motors stop, wait for release
			switchDelay();							// switch debounce
			waitPress();							// wait for a button press
		}

		while((!uniHor || !biHor) && !green)		// while not at sensors and not green button
		{
			if(!biHor)								// if bipolar not at sensor
			{	
				biStep--;							// decrement step - ccw
				biStep = biRot(biStep);				// rotate bipolar motor
			}

			if(!uniHor)								// if unipolar not at sensor
			{
				uniStep++;							// increment step - cw
				uniStep = uniRot(uniStep);			// rotate unipolar motor
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

	while(!biVer)									// bipolar to vertical
	{
		biStep++;									// increment step
		biStep = biRot(biStep);
		stepperDelay();								// motor delay
	}

	waitPress();

	while(!green)									// when a green button hasn't been pressed
	{
		while((!uniVer || !biHor) && !green)		// while not at sensors
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

		if(redButton)								// if red button presed
		{
			while(redButton) {}						// wait for release
			switchDelay();							// switch debounce
			waitPress();							// wait for button press
		}

		while((!uniHor || !biVer) && !green)		// while not at sensors and not green button
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

		if(redButton)								// if red button pressed
		{
			while(redButton) {}						// wait for release
			switchDelay();							// switch debounce
			waitPress();							// wait for button press
		}
	}
}

// Wave drive
void	Mode4(void)
{
	uniStep = uniHome(uniStep);
	biStep 	= biHome(biStep);
	
	waitPress();
	biStep *= 2;

	while(!green)									// infinite loop when no green press
	{
		while(!uniVer || !biVer)					// while not at sensors
		{
			if(!biVer)								// if bipolar not at sensor
			{	
				biStep++;							// increment step - cw
				biStep = biWaveF(biStep);
			}

			if(!uniVer)								// if unipolar not at sensor
			{
				uniStep--;							// decrement step - ccw
				uniStep = uniWave(uniStep);
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
				biStep = biWaveR(biStep);
			}

			if(!uniHor)								// if unipolar not at sensor
			{
				uniStep++;							// increment step - cw
				uniStep = uniWave(uniStep);
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
	biStep /= 2;									
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
	if(step == 0)									// if step is 0,
	{												// ccw motion
		step = 4;									// reset step count to 4
	}

	else if (step == 5)								// if step is 5
	{												// cw motion
		step = 1;									// reset step count to 1
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
	if(step <= 0)									// if step is 0,
	{												// ccw motion
		step = 4;									// reset *step count to 4
	}

	else if (step == 5)								// if step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}

	switch(step)									// full step - Ia and Ib = 0
	{												// Port D 4-7 determine bipolar
		case 1:										// 0100
			RD4	= 0;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 2:										// 0000
			RD4	= 0;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;	
			break;

		 case 3:									// 1000
			RD4	= 1;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;

		case 4:										// 1010
			RD4	= 1;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
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

	else if (step == 5)								// if *step is 5
	{												// cw motion
		step = 1;									// reset step count to 1
	}

	switch(step)									// Port D, pins 0-3
	{
		case 1:										// 1000
			RD0	= 0;
			RD1	= 0;
			RD2	= 0;
			RD3 = 1;
			break;

		case 2:										// 0001
			RD0	= 1;
			RD1	= 0;
			RD2	= 0;
			RD3 = 0;
			break;

		 case 3:									// 0010
			RD0	= 0;
			RD1	= 1;
			RD2	= 0;
			RD3 = 0;
			break;

		case 4:										// 0100
			RD0	= 0;
			RD1	= 0;
			RD2	= 1;
			RD3 = 0;
			break;
	}
	return step;									// return curent step for unipolar
}

//	Bipolar rotation - wave drive

/* 	Bipolar motor takes 8 steps to complete rotation, and the forward is not 
	simply the opposite of the reverse.
*/

uint8_t	biWaveF(uint8_t step)						//	cw if ++		
{
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 8;									// reset *step count to 4
	}

	else if (step == 9)								// if *step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}

	switch(step)									// Port D, pins 4-7
	{
		case 1:										//	0101
			RD4	= 1;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 2:										// 0110
			RD4	= 0;
			RD5 = 1;
			RD6	= 1;
			RD7 = 0;	
			break;

		 case 3:									// 0100
			RD4	= 0;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 4:										// 1000
			RD4	= 0;
			RD5 = 0;
			RD6	= 0;
			RD7 = 1;
			break;

		case 5:										// 0000
			RD4	= 0;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;
		
		case 6:										// 0011
			RD4	= 1;
			RD5 = 1;
			RD6	= 0;
			RD7 = 0;
			break;

		case 7:										// 0001
			RD4	= 1;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;

		case 8:										// 1101
			RD4	= 1;
			RD5 = 0;
			RD6	= 1;
			RD7 = 1;
			break;
	}
	return step;	
}

//	Bipolar rotation - full step

uint8_t	biWaveR(uint8_t step)						//	cw if ++		
{
	if(step == 0)									// if *step is 0,
	{												// ccw motion
		step = 8;									// reset *step count to 4
	}

	else if (step == 9)								// if step is 5
	{												// cw motion
		step = 1;									// reset *step count to 1
	}
	switch(step)									// Port D, pins 4-7
	{
		case 1:										//	0101
			RD4	= 1;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 2:										// 0111
			RD4	= 1;
			RD5 = 1;
			RD6	= 1;
			RD7 = 0;	
			break;

		 case 3:									// 0100
			RD4	= 0;
			RD5 = 0;
			RD6	= 1;
			RD7 = 0;
			break;

		case 4:										// 1100
			RD4	= 0;
			RD5 = 0;
			RD6	= 1;
			RD7 = 1;
			break;

		case 5:										// 0000
			RD4	= 0;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;
		
		case 6:										// 0010
			RD4	= 0;
			RD5 = 1;
			RD6	= 0;
			RD7 = 0;
			break;

		case 7:										// 0001
			RD4	= 1;
			RD5 = 0;
			RD6	= 0;
			RD7 = 0;
			break;

		case 8:										// 1001
			RD4	= 1;
			RD5 = 0;
			RD6	= 0;
			RD7 = 1;
			break;
	}
	return step;									// return bipolar step count	
}

//	Timers

void	longTimer(void)						
{
	for(Timer = 38461; Timer > 0; Timer--) {}		// 1s delay loop

}

void	switchDelay(void)							//	Waits for Switch to debounce
{
	for (uint8_t i=200; i > 0; i--) {}				// 	1200 us delay
}


void	stepperDelay(void)							//	delay for stepper windings
{
	for (int j=2000; j > 0; j--) {}					
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~