#include <htc.h>
#include "uart.h"

void uart_init()
{
	#ifndef _XTAL_FREQ
		#error "Please define _XTAL_FREQ in uart.h"
	#endif
	#define FOSC _XTAL_FREQ      // <<< clock freq (ie: 4000000 Hz)
	#ifndef	BAUD
		#error "Please define BAUD in uart.h"
	#endif
	#define	BAUD_ERROR	4 	    // <<< set max. baud rate error (ie: 4%)

  //generate settings
	#if ((FOSC/(16UL * BAUD))-1UL) < 256UL
		#define SPBRG_SET (FOSC/(16UL * BAUD))-1
		#define BRGH_SET 1
	#else
		#define SPBRG_SET (FOSC/(64UL * BAUD))-1
		#define BRGH_SET 0
	#endif

  //check that error < N% (see manual for USART baud rates for async. mode)
	#define	BAUD_REAL (FOSC/((64UL-(BRGH_SET*48UL))*(SPBRG_SET+1)))
	#if BAUD_REAL > BAUD
		#if (((BAUD_REAL - BAUD)*100UL)/BAUD) > BAUD_ERROR
			#error	"baud rate error percentage is too high"
		#endif
	#else
		#if (((BAUD - BAUD_REAL)*100UL)/BAUD) > BAUD_ERROR
			#error	"baud rate error percentage is too high"
		#endif
	#endif

	/* relates crystal freq to baud rate - see above and PIC16F87x data sheet under async 'USART'

	BRGH=1, Fosc=3.6864MHz		BRGH=1, Fosc=4MHz      BRGH=1, Fosc=8MHz    BRGH=1, Fosc=16MHz
	----------------------	  -----------------      -----------------    ------------------
	Baud		SPBRG             Baud		SPBRG          Baud		SPBRG         Baud		SPBRG
	1200		191               1200		207.3          1200		415.7         9600		103
	2400		95                2400		103.2          2400		207.3         19200		51
	4800		47                4800		51.1           4800		103.2         38400		25
	9600		23                9600		25.0           9600		51.1          57600		16
	19200		11                19200		12.0           19200	25.0          115200	8
	38400		5                 38400		5.5            38400	12.0
	57600		3                 57600		3.3            57600	7.7
	115200		1                 115200	1.2            115200	3.3

	*/

	//you can comment these #assert statements out if you dont want error checking
	#if FOSC==3686400 && BAUD==19200
		#assert SPBRG_SET==11
	#elif FOSC==4000000 && BAUD==19200
		#assert SPBRG_SET==12
	#elif FOSC==10000000 && BAUD==19200
		#if BRGH_SET==0
  		#assert SPBRG_SET==7
    #elif BRGH_SET==1
  		#assert SPBRG_SET==31
    #else
      #error
    #endif
	#elif FOSC==10000000 && BAUD==33600
		#if BRGH_SET==0
  		#assert SPBRG_SET==4
    #elif BRGH_SET==1
  		#assert SPBRG_SET==18 //NOTE: due to rounding, this should be 18, not 17! See manual.
    #else
      #error
    #endif
	#elif FOSC==16000000 && BAUD==19200
		#assert SPBRG_SET==51
	#elif FOSC==20000000 && BAUD==19200
		#assert SPBRG_SET==64
	#elif FOSC==20000000 && BAUD==57600
		#if BRGH_SET==0
  		#assert SPBRG_SET==4
    #elif BRGH_SET==1
  		#assert SPBRG_SET==20
    #else
      #error
    #endif
  #else
    #error could not #assert SPRBG and BRGH for specified crystal freq. and baud rate
	#endif

	SPBRG=SPBRG_SET;
	BRGH=BRGH_SET;	  //data rate for sending/receiving
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
char uart_getc(){
	while(!RCIF);
	return RCREG;
}
char uart_data_ready(){
	if(RCIF) return 1;
	else return 0;
}
void uart_gets(char *s){
	char g;
	*s = uart_getc();
	while(*s!='\0'){
		s++;
		*s = uart_getc();
	}
}

void uart_putn(unsigned char c){
	uart_putc(c+48);
}
void uart_puthex(unsigned char c){
	
//first number	
	if (c/16<10)
		uart_putc(c/16 + 48);
		else uart_putc(c/16+ 55);			
//second number
		c = c %16;	
	if (c <10)
		uart_putc(c + 48);
		else uart_putc(c + 55);
	uart_puts(" ");
}