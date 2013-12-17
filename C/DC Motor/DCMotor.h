#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

Header file for DC Motor Case Study

Tim Cousins
Chi Yung Fung
Jamie Collier

Contrains configuration and pic.h. Define the various pins with 
names that make code easier to read. Contains function declartions
for code to compile easier.

***********************************************************/

//	Pin Defines

//	Port A
#define tach 		RA1
#define blackButton	RA4

//	Port C
#define dir			RC0
#define motorBrake	RC1
#define PWM			RC2
#define eddy		RC3
#define LSBHex		RC4
#define MSBHex		RC5
#define enc			RC6


//	Type Definitions
typedef	unsigned char	uint8_t;
typedef	unsigned int	uint16_t;

//	Variables
unsigned long  timer;							// Holds value for 1 sec timer

static uint8_t eddyCount;		
static uint16_t	count;

static uint8_t readings;			
static uint8_t speed;
static uint8_t motorRef;
static uint8_t desVel;

static uint8_t error;

//	Gains
static uint8_t kp; 	= 15;
static uint8_t kd;	= 5;
static double  ki;	= 0.005;

//	Function Declarations

void	main(void);

void	motorControl(void);
void	brake(void);
uint8_t	speedRead(void);

//	Initialization

void	init(void);
void	initAtoD(void);
void 	initPWM(void);
void	initISR(void);
void	refVoltage(void);

//	Delays
void	delay(void);
void	longTimer(void);


void	interrupt ISR_AtoD(void);