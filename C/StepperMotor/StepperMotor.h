#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

Header file for Stepper Motor Case Study

***********************************************************/

//	Variable defines

#define	greenButton	RC0
#define	redButton	RC1

unsigned char	State;							// Holds state of function
unsigned long 	Timer;							// Holds value for 1 sec timer


void	main(void);

void	init(void);

void	waitPress(void);
void	modeSelect(void);
void	error(void);

//	Modes
void	Mode1(void);
void	Mode2(void);
void	Mode3(void);
void 	Mode4(void);

//	Timers
void	switchDelay(void);
void	longTimer(void);