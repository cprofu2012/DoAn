#include	<pic.h>
#include    "uart.h"

#define SCALER		10000000
#define ITIME		4*SCALER/_XTAL_FREQ	/* Instruction cycle time */

#define	DLY		9		/* cycles per null loop */
#define TX_OHEAD  14

#define	RX_OHEAD	12		/* receiver overhead per loop */

#define	DELAY(ohead)	(((SCALER/S_BAUD)-(ohead*ITIME))/(DLY*ITIME))

static bit	RxData @(unsigned)&SERIAL_PORT*8+RX_PIN;	/* Map RxData to pin */

#define	INIT_PORT	SERIAL_TRIS = 1<<RX_PIN				/* set up I/O direction */


char s_uart_getc()
{
	unsigned char	c, bitno;
	unsigned int	dly;

	for(;;) {
		while(RxData)
			continue;	/* wait for start bit */
		dly = DELAY(3)/2;
		do
			/* waiting in delay loop */ ;
		while(--dly);
		if(RxData)
			continue;	/* twas just noise */
		bitno = 8;
		c = 0;
		do {
			dly = DELAY(RX_OHEAD);
			do
			/* waiting in delay loop */ ;
			while(--dly);
			c = (c >> 1) | (RxData << 7);
		} while(--bitno);
		return c;
	}
}

void s_uart_gets(char *str_in){
	char g;
	*str_in = s_uart_getc();
	while(*str_in!='\0'){
		str_in++;
		*str_in = s_uart_getc();
	}
}
