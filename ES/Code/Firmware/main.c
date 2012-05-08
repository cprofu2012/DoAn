#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"

__CONFIG(XT & WDTDIS& LVPDIS);

//==========Prototype ==================
void init_all();
void process();
void receive_data();
void sleep_all();
bit signal_mode = 0;
//======================================
//========== Main Program ==============
void main()
{
	init_all();
	while(1){ 	
		RC2 = 3;			
		uart_puts("0987654321");
		__delay_ms(10);
		RC2 = 0;
		__delay_ms(10);
	}
}

//=========== Initialize Board ===========
void init_all(){
	uart_init();		// initialize uart

	TRISB = 0xFF;		// Set Port B input
	TRISC = 0x00;		// Set Port C output
	
	RBIE = 1;			// Enable Interrupt On Change Port B
	RBIF = 0;

	INTE = 1;			// Enable External Interrupt
	INTEDG  = 1;

	GIE = 1;			// Global Interrupt Enable
}

//========== Process data and send to PC ====
void process(){
	char start = '$';
//	char* s = "0987654321";
	uart_putc(start);
	__delay_ms(100);
}

//========== Recieve data ===================
void receive_data(){
}

//=========== Sleep mode Enable =============
void sleep_all(){
	RC2 = 1;		// RC2 pin control power mode of 8223
	SLEEP();
}

//=========== Interrupt Function=============
static void interrupt isr(){
	if (RBIF){			// Port B interrupt change to wake up PIC
		RB4 = 1;		// Read or write Port B pin to enable clear RBIF register
		RBIF = 0;		
		RC2 = 0;		// wake up 8223
	}

	if(INTF) {			// start recieve data
		INTF = 0;
		signal_mode = RB2;	// determine if signal is FSK (1) or DTMF (0)
		RC0 = !RC0;		
	}	
}