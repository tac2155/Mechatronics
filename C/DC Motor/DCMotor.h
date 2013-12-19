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
#define dir			RC0					// Motor Direction
#define motorBrake	RC1					// Motor Brake
#define PWM			RC2					// PWM set
#define eddy		RC3					// Eddy Sensor
#define LSBHex		RC4					// lower bit of mode switch
#define MSBHex		RC5					// upper bit of mode switch
#define enc			RC6					// encoder sensor


//	Type Definitions
typedef	unsigned char	uint8_t;		// unsigned, 8 bit integer
typedef	unsigned int	uint16_t;		// unsigned, 16 bit integer

//	Variables
unsigned long  timer;					// Holds value for 1 sec timer
uint8_t i;								// unsigned char for timer	

uint8_t mode;							// unsigned char for mode

uint8_t eddyCount;						// unsined char for eddy sensor count
static uint16_t	count;					// unsigned int for encoder count
static uint8_t readings;				// unsigned char for tach readings counts

//	Errors and Controller Output	
static int error;						// unsigned char for error
static int errorD;						// usinged char for previous error
static int errorI;						// unsinged char for total error
int input;								// signed integer for inputed duty cycle
	

static uint16_t speed;					// unsinged int for tachometer speed
static uint16_t tachSpeed;				// unsigned int for total of tachometer readings
uint8_t velRef;							// unsigned char for desired voltage

//	Gains
uint8_t kp;								// unsigned char for proportional gain
uint8_t kd;								// unsigned char for derivative gain
float  ki;								// float for integer gain
static uint8_t bias;					// unsigned char for bias voltage

//	Function Declarations

//	Function declartations for Motor Contry, PWM duty cycle set, and motor brake
void	motorControl(uint8_t, uint8_t, float, uint8_t);
void 	setPWM(uint8_t, uint8_t, float, uint8_t);
void	brake(void);

//	Initialization

void	init(void);						// main initialization function
void	initAtoD(void);					// A/D initialization
void 	initPWM(void);					// PWM initialization
void	initISR(void);					// ISR initialization
void	refVoltage(void);				// set reference voltage

//	Delays
void	delay(uint8_t);					// delay for max of i = 255
void	longTimer(void);				// delay for 1 second timer
