#include <pic.h>
	__CONFIG(11111110110010);

/***********************************************************

OnOffControl.h

***********************************************************/

#define	greenButton	RC0
#define	redButton	RC1

#define	mainTran	RD0
#define secTran		RD1
#define engaged		RD2

unsigned char	State
unsigned char	Temp
unsigned char	Count
unsigned char	error1
unsigned char	error2
unsigned char 	error3

static	bit		green
unsigned long	Timer

void	main(void);


// initializations
void	init(void);
void	initAtoD(void);

// state modes
void	Mode1(void);
void	Mode2(void);
void	Mode3(void);

// functions
void	waitPress(void);
void	AtoD(void);
void	error(void);

// timer
void	SwitchDelay(void);
void	longTimer(void);