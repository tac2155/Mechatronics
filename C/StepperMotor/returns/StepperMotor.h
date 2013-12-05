#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

Header file for Stepper Motor Case Study

***********************************************************/

//	Variable defines

// Define Button Presses
#define	greenButton	RC0							// green button Port C, pin 0
#define	redButton	RC1							// red button Port C, pin 1

// Define Sensor Ports
#define uniHor		RB4							// unipolar horizontal sensor Port B, pin 4
#define uniVer		RB5							// unipolar vertical sensor Port B, pin 5
#define biVer		RB6							// bipolar horizontal Sensor Port B, pin 6
#define biHor		RB7							// bipolar vertical sensore Port B, pin 7


typedef	unsigned char	uint8_t;

uint8_t			State;							// Holds state of function
unsigned long 	Timer;							// Holds value for 1 sec timer

static uint8_t 		uniStep;
static uint8_t 		biStep;

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
uint8_t	    uniRot(uint8_t);
uint8_t 	biRot(uint8_t);
uint8_t		uniHome(uint8_t);
uint8_t		biHome(uint8_t);

//	Wave Step
uint8_t		uniWave(uint8_t);
uint8_t 	biWaveF(uint8_t);
uint8_t 	biWaveR(uint8_t);
 
//	Timers
void	switchDelay(void);
void 	stepperDelay(void);
void	longTimer(void);