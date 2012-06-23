#include <htc.h>
#include "uart.h"

void uart_init()
{
	SPBRG=129;
	BRGH=0;	  //data rate for sending/receiving
	SYNC=0;						//asynchronous
	SPEN=1;						//enable serial port pins
	CREN=1;						//enable reception
	SREN=0;						//no effect
	TXIE=0;						//disable tx interrupts
	RCIE=0;						//disable rx interrupts
	TX9=0;						//8-bit transmission
	RX9=0;						//8-bit reception
	TXEN=0;						//reset transmitter
	TXEN=1;						//enable the transmitter
	TX_TRIS = 0;
	RX_TRIS = 1;
}

void uart_putc(char c){
	while(!TXIF);
	TXREG = c;
}

void uart_puts(const char* s){
	while(*s != '\0'){
		uart_putc(*s);
		s++;
	}
}
void uart_puthex(unsigned char c){
	unsigned char temp;

	// transmits in hex

	temp=c;

	c=(c >> 4);
	if (c<10) c+=48; else c+=55;
	uart_putc(c);

	c=temp;

	c=(c & 0x0F);
	if (c<10) c+=48; else c+=55;
	uart_putc(c);
}
void uart_putn(unsigned char c){
uart_putc(c+48);
}