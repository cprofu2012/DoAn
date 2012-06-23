#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"
#include "soft_uart.h"
//**************************************************
#define     PDWN   		RC2
#define     RDET     		RB4
#define     F_D		       RB2
#define     DATA		RB1
#define     DV			RB0
#define		MLENGTH		64
//**************************************************
void delay(unsigned int n)
{
unsigned int m , i ;
for(i=0;i<n;i++){m = 1000 ;
				 while(m)m--;
				}							  
}
//**************************************************
interrupt void isr (void)
{
	if(INTF&&INTE ) {
					
					
					}			
	if (RBIF&&RBIE){			// Port B interrupt change to wake up PIC
	//	putch('b');
		RB4 = 0;		// Read or write Port B pin to enable clear RBIF register
		RBIF = 0;
		RC2 = !RC2;
	}
}
	
//**************************************************
void sys_init()
{
TRISB3 = 0  ;		// Chan TX 
TRISB1 = 1  ;		// Chan RX

TRISC3  = 0 ;            //chan led
TRISC2  = 0 ;            //pin PDWN
TRISC = 0;
RC2 =0;
TRISB4  = 1 ;            //pin RDET
TRISB2  = 1 ;            //pin F/D
//TRISB1  = 1 ;            //pin DATA
TRISB0  = 1 ;            //pin DV

RBPU = 1 ;
INTEDG = 1 ;

RBIE =1;

INTE = 1 ;
GIE = 1 ; 
}
//**************************************************
void main()
{
char data ;
int checksum;
unsigned char len;
unsigned char message[MLENGTH];
sys_init();
uart_init();

unsigned char k;
unsigned char l;
SLEEP();
while(1){
			data = getch();

			 if (data != 0x80 && data !=0x04) 			// error
				{ 
//uart_puthex(data);
					continue;
                 }								 //while(1){uart_putc('z');}
			checksum = data;
//uart_puthex(data);

			data = getch();
			 len = data +1;						// length of date time, number and checksum
//uart_puthex(data);
//SLEEP();
				checksum += data;
			 for (unsigned char i=0;i<len;i++){
				data = getch();
//uart_puthex(data);
				message[i] = data;
				checksum += data;
			}
//SLEEP();
//uart_puts("\r\n");
		int   res= (256 - (checksum & 0xff)) ;	
		unsigned char i = 8;
		switch(message[i]){ 
			case 0x4F:	uart_puts("Out of area");
						break;
			case 0x50:	uart_puts("Private Call");	
						break;
			default:	while(i<len -1){
							uart_putc(message[i]);
							i++;
						}
					break;
		}
uart_puts("\r\n");
//uart_puthex(checksum);
//uart_puts("\r\n");
SLEEP();
	}
}
