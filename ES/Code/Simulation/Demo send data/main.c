#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h"

__CONFIG(HS & WDTDIS& LVPDIS);

//    U22222222222222
void main()
{
	PORTB = 0;
	TRISB = 0;
	TRISB0 = 1;
	TRISC = 0;
	PORTC = 0;
	
	// INT interrupt
	INTE = 1;
	INTEDG = 1;
	GIE = 1;

	uart_init();
	RB2 = 0;	
	
//	int a[] = {0x04, 0x12, 0x30, 0x39, 0x33, 0x30, 0x31, 0x32, 0x32, 0x34, 0x36, 0x30, 0x39, 0x35, 0x35, 0x35, 0x31, 0x32, 0x31, 0x32, 0x51};
//	int length = sizeof(a)/sizeof(a[0]);
	SLEEP();
while (1){
		uart_puts("demo soft uart\r\n");
		__delay_ms(10);
	}
}
void interrupt isr(){
	if (INTF){
		INTF =0;
		RB2 = 1;
//		SLEEP();
	}
}