//**********************************************************
//**********************************************************
//**********************************************************
// 					Home Automation
//**********************************************************
//**********************************************************
//**********************************************************
// This program controls a Remote Home Automation System
// that has a ATtiny2313 uP at its core. The code is 
// intended to be easily ported to other products in the 
// ATMEL AVR family & so all port names are given in the 
// #define section.
//
// This code is based on 'Phone.asm' written by me durring
// the summer of 2007, but with Portability in mind. And is
// interfaced with a Phone through a CM8870 DTMF decoder 
// circuit as well as various subsystems of my own design. 
//
// Written By:  David McDougall
// Date: 		2/29/08
// 
//
//
//**********************************************************
//**********************************************************
//**********************************************************




//**********************************************************
//		#Includes
//**********************************************************

#include <avr/interrupt.h>    	// Defines pins, ports, etc to make programs easier to read
#define F_CPU 8000000UL	    	// Sets up the default speed for delay.h
#include <util/delay.h>			// Allows for delay to be used
#include <avr/io.h>				// Enables IO
//#include "lcd.h"				// Header file for controlling a HD44780 compatible LCD on a ATtiny2313
//#include "lcdor.h"


//**********************************************************
//		#defines	for porting to new uP	
//**********************************************************

//#define B1    _BV(PB0)

#define Bit1	1
#define Bit2	2
#define Bit3	3
#define Bit4	4



#define STD (PIND & 0x10)			// Pin D5 is connected to STD on CM8870
#define TOE 0b01000000				// Pin D6 is connected to TOE on CM8870
#define CodeMask 0x0F				// Mask for the lower nibble on PORTD to obtain validated code

#define Subs (PORTB)				// For portable code, PORTB on Attiny2313
#define DDR_Subs	(DDRB)			// Data Direction Register for subsystems

#define Phone	(PORTD)				// For portable code, PortD on Attiny2313
#define DDR_Phone	(DDRD)			// Data direction register for 

#define DTMF ((PIND & CodeMask))	// Decoded DTMF tone input on PortD

#define true 1
#define false 0 

//**********************************************************
//		Function Prototypes
//**********************************************************

void Reset_CM8870(void);			// enable next DTMF tone
int validate(int*);					// Compare entered sequence & code return 1 if valid, 0 if invalid
void Lockdown(void); 				// 3 minute sleeper mode to discourage unauthorized entry
void RESET_uP(void);				// Reset the Microprocessor by grounding RST pin through NPN transistor


//**********************************************************
//		Main Program
//**********************************************************

int main()
{
// Define I/O

	DDR_Subs = 0xFF;                  	// PortB is an OUTPORT for triggering Subsystems
	DDR_Phone = 0b01010000;				// PortD is an INPORT, Pin D6 is an OUTPUT for reseting the DTMF decoder chip
										// Pin D4 is an OUTPUT for restting the Tiny2313 uP
	
	Subs = 0x00;                 		// Set PortB = 0; so that no Subsystems will trigger
	Phone = 0b01010000; 		// Set TOE high, set PD4 (RESET Control) high
	

// Initializations

	int CODE[4] = {0,0,0,0};			// holds the bit sequence that is entered by the user
	int CODE_digit = 0; 				// tells what digit in the CODE array is being addressed
	int Invalid = 0;					// Counts the number of invalid attempts to enter code, if > 3 enter 3 min lockdown
	

/******************************************************************************************
	// Wait for valid code signal from STD pin on CM8870
	// If STD == 1 decode which button was pressed. 
	// If invalid code (A,B,C,D, noisy line) 
******************************************************************************************/

	
	DDRB = 0xff;	//output
	DDRD = 0x00;	//input


	while(1)									// Endless loop, same as "for(;;)"
	{ 
		
	//	if(CODE_digit >= 4)						// If array pointer is out of bounds Reset it
	//	{										// This will over-write old data.
	//		CODE_digit = 0; 
	//	}
		PORTB = (PIND & 0x0f);
		/*
		if(STD)									// IS there a valid code? IF yes STD == 1
		{
		
		
		
			if( DTMF == 0x01 )					// was 1 pressed
			{
				Subs = 0x01;
				
				
				CODE[CODE_digit] = 0x01;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
				
			}
			else if(DTMF == 0x02)							// was 2 pressed
			{
			
				Subs = 0x02;
				
				CODE[CODE_digit] = 0x02;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			
			}
			else if(DTMF == 0x03)							// was 3 pressed
			{
				Subs = 0x03;
				CODE[CODE_digit] = 0x03;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			
			}
			else if(DTMF == 0x04)							// was 4 pressed
			{
				Subs = 0x04;
				CODE[CODE_digit] = 0x04;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			
			}
			else if(DTMF == 0x05)							// was 5 pressed
			{
				Subs = 0x05;
				CODE[CODE_digit] = 0x05;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			}
			else if(DTMF == 0x06)							// was 6 pressed
			{
				Subs = 0x06;
				CODE[CODE_digit] = 0x06;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
				
			}
			else if(DTMF == 0x07)							// was 7 pressed
			{
				Subs = 0x07;
				CODE[CODE_digit] = 0x07;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
				
			}
			else if(DTMF == 0x08)							// was 8 pressed
			{
				Subs = 0x08;
				CODE[CODE_digit] = 0x08;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			}
			else if(DTMF == 0x09)							// was 9 pressed
			{
				Subs = 0x09;
				CODE[CODE_digit] = 0x09;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			
			}
			else if(DTMF == 0x0A)							// was 0 pressed
			{
				Subs = 0x0A;
				CODE[CODE_digit] = 0x0A;		// Store Which digit was entered
				CODE_digit += 1; 				// increment Code_digit	
			
			}
			else if(DTMF == 0x0B)							// was * pressed, if so clear entries
			{
			
				Subs = 0x0B;
				for(int i = 0; i < 4; i++)
				{
					CODE[i] = 0x00;				// Clear the Contents of CODE	
				}
				CODE_digit = 0; 				// Reset Code_digit to point at the first block of the CODE array
				
			}
			else if(DTMF == 0x0C)							// was # pressed, If so Check for valid Code
			{
				// Check to see if the input sequence matches the stored code
				
				
				for(int i = 0; i < 4 ; i++)
				{
					Subs = (CODE[i] & 0x0f);			// Display each digit for 1 second Then display the next
					
					_delay_ms(250);	
					_delay_ms(250);
					_delay_ms(250);
					_delay_ms(250);
					_delay_ms(250);	
					_delay_ms(250);
					_delay_ms(250);
					_delay_ms(250);
				}
				
				
				
				/*if(true == validate(CODE))						// If Code matches call subroutines		
				{
				
				
				
				}
				else										// Invalid Code record # & enter lockdown if > 3;						
				{
				
					Invalid += 1; 				// Increment Invalid
					
					if(Invalid >= 3)			// Too Many Invalid Characters entered, Enter Lockdown Mode
					{
					
						Lockdown();				// Call the Lockdown Function
					
					}				
				}
				
			}
			else					// No valid entry (A,B,C,D or a short occoured, reset Decoder by toggling TOE)
			{
				//Reset_CM8870();
			} // END if	
			
			//Reset_CM8870();		// Get next digit
			
		} // END If
		*/
		
 
	} // END While



	return(0);			// Never reached	
}


/******************************************************************************************
******************************************************************************************
******************************************************************************************

	FUNCTIONS
	
******************************************************************************************
******************************************************************************************	
******************************************************************************************/


// ************************************************************
//		Function Brings TOE low to reset CM8870 and enable
//		the input of new DTMF tones
// ************************************************************
void Reset_CM8870()
{
	Phone = !(TOE);
	_delay_ms(10);
	Phone = (TOE);
}

// ************************************************************
//		Function Checks for a valid code entry
//		If valid returns 1 else returns 0 
// ************************************************************
int validate(int* code)
{
	
	int Cypher[4] = {Bit1, Bit2, Bit3, Bit4};
	
	for(int i=0; i < 4 ; i++)		// Look at all bits in the sequence
	{
		if(code[i] != Cypher[i])	// Sequence entered doesn't match Code			
		{
			return(0);				// Invalid Code
		}
		
	}	
	
	return(1);						// Valid Code
}

// ************************************************************
//		Function Checks for a valid code entry
//		If valid returns 1 else returns 0 
// ************************************************************

void Lockdown() 				// 3 minute sleeper mode to discourage unauthorized entry
{
	for(int i = 0; i < 2; i++)	// wait 1 second on each loop
	{
		_delay_ms(250);
		_delay_ms(250);
		_delay_ms(250);
		_delay_ms(250);
	}
	
	//RESET_uP();					// Reset the Microprocessor & reinitialize the system

}
// ************************************************************
//		Function Checks for a valid code entry
//		If valid returns 1 else returns 0 
// ************************************************************
void RESET_uP()				// Reset the Microprocessor by grounding RST pin (done by bringing pinD4 low)
{

	Subs = 0x00; 			// Make sure that no subsystems are active
	Phone = 0x00; 			// Reset the microprocessor
	
	for(int i = 0; i < 5; i++)	// wait 1 second on each loop
	{
		_delay_ms(250);
		_delay_ms(250);
		_delay_ms(250);
		_delay_ms(250);
	}
	

}

