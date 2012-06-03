#ifndef _UART_H_
#define	_UART_H_

#ifndef _XTAL_FREQ
#define _XTAL_FREQ	10000000
#endif

#define	H_BAUD	19200
#define	S_BAUD	1200

#define	TX_TRIS	TRISC6
#define	RX_TRIS	TRISC7

#define SERIAL_PORT	PORTB
#define SERIAL_TRIS	TRISB
#define RX_PIN		2

#define ON 1
#define OFF 0
void h_uart_init();
void h_uart_putc(char char_out);
void h_uart_puts(const char* str_out);

char s_uart_getc();
void s_uart_gets(char* str_in);
#endif