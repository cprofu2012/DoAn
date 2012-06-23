#include <htc.h>
#include <stdio.h>
#include "soft_uart.h"


//**************************************************
//printf("Call id restar \n"); 		//printf string ---> soft uart
//**************************************************
 void putch(char c) 
 { 
    unsigned char   bitno; 
 #if BRATE > 1200 
    unsigned char   dly; 
 #else 
    unsigned int   dly; 
 #endif 
 
    INIT_PORT; 
    TxData = 0;         /* start bit */ 
    bitno = 12; 
    do { 
       dly = DELAY(TX_OHEAD);   /* wait one bit time */ 
       do 
          /* waiting in delay loop */ ; 
       while(--dly); 
       if(c & 1) 
          TxData = 1; 
       if(!(c & 1)) 
          TxData = 0; 
       c = (c >> 1) | 0x80; 
    } while(--bitno); 
 NOP(); 
 } 
//**************************************************
 char getch(void) 
 { 
    unsigned char   c, bitno; 
 #if BRATE > 1200 
    unsigned char   dly; 
 #else 
    unsigned int   dly; 
 #endif 
 
    for(;;) { 
       while(RxData) 
          continue;   /* wait for start bit */ 
       dly = DELAY(3)/2; 
       do 
          /* waiting in delay loop */ ; 
       while(--dly); 
       if(RxData) 
          continue;   /* twas just noise */ 
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
//**************************************************
 char getche(void) 
 { 
    char c; 
    putch(c = getch()); 
    return c; 
 } 
//**************************************************
int gethex(void){
	char c;
	c = getch();
	c = c << 4;
//if (c<58) c-=48; else c-=55;
	char temp = getch();
//if (c<58) c-=48; else c-=55;
	return (c&temp);
}