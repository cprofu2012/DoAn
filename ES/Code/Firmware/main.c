#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"

__CONFIG(HS & WDTDIS& LVPDIS);

					// U11111111111111111111111
//==========Prototype ==================
void init_all();
void process();
void receive_data();
void sleep_all();
void turn_led(unsigned char led_status);
bit signal_mode = 0;
//======================================
//========== Main Program ==============
void main()
{
	init_all();				// initial uart, enable interrupt
	
	h_uart_puts("start\r\n");
	__delay_ms(10);
	
	sleep_all();			// finish initial, enter power down mode for 8223 and PIC
	h_uart_puts("wakeup\r\n");

	while(1){
		char c = s_uart_getc();
		h_uart_putc(c);
//	SLEEP();
	}
}
//=========== Interrupt Function=============
static void interrupt isr(){
	if (RBIF){			// Port B interrupt change to wake up PIC
		RB4 = 1;		// Read or write Port B pin to enable clear RBIF register
		RBIF = 0;		
		RC2 = 0;		// wake up 8223
		h_uart_puts("rb interrupt \r\n");
	}

	if(INTF) {			// start receive data
		INTF = 0;
		signal_mode = RB2;	// determine signal is FSK (1) or DTMF (0)
		h_uart_puts("int interrupt \r\n");
	}	
}

//=========== Sleep mode Enable =============
void sleep_all(){
	RC2 = 1;		// RC2 = 1: 8223 enter power down mode
	SLEEP();
}

//=========== Turn led ON/OFF ===============
void turn_led(unsigned char led_status){
	RC3 = led_status;
}

//=========== Initialize Board ===========
void init_all(){	
	TRISB = 0xFF;		// Set Port B input
	TRISC = 0x00;		// Set Port C output
	h_uart_init();		// initialize hard uart
	
	RBIE = 1;			// Enable Interrupt On Change Port B
	RBIF = 0;

	INTE = 1;			// Enable External Interrupt
	INTEDG  = 1;

	GIE = 1;			// Global Interrupt Enable
	
	turn_led(ON);		// turn led ON -> Finish initial
}