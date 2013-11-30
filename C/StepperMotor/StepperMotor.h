#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

Header file for Stepper Motor Case Study

***********************************************************/

//	Variable defines

#define	greenButton	RC0
#define	redButton	RC1


typedef	unsigned char	uint8_t;

uint8_t			State;							// Holds state of function
unsigned long 	Timer;							// Holds value for 1 sec timer

uint8_t 		uniStep;
uint8_t 		biStep;

static bit 		green;

void	main(void);

//	Initiation
void	init(void);
void	synchMotors(void);

//	Functions
void	waitPress(void);
void	error(void);

//	Modes
void	Mode1(void);
void	Mode2(void);
void	Mode3(void);
void 	Mode4(void);

//	Rotation
void	uniRot(void);
void	biRot(void);
void	motorsHome(void);

//	Timers
void	switchDelay(void);
void 	stepperDelay(void);
void	longTimer(void);