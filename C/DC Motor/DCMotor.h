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
unsigned long  timer;					// Holds value for 1 sec timer
uint8_t i;			

uint8_t mode;

static uint8_t eddyCount;		
static uint16_t	count;
static uint8_t readings;			

//	Errors and Controller Output
static int error;
static int errorD;
static int errorI;
int input;

static uint8_t desVel;
static uint16_t speed;
static uint16_t tachSpeed;
uint8_t velRef;

//	Gains
static uint8_t kp;
static uint8_t kd;
static float  ki;
static uint8_t bias;

//	Function Declarations

void	motorControl(uint8_t, uint8_t, float, uint8_t);
void 	setPWM(uint8_t, uint8_t, float, uint8_t);
void	brake(void);

//	Initialization

void	init(void);
void	initAtoD(void);
void 	initPWM(void);
void	initISR(void);
void	refVoltage(void);

//	Delays
void	delay(uint8_t);
void	longTimer(void);
