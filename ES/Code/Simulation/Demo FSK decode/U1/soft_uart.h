#ifndef __SOFT_UART_H_
#define	__SOFT_UART_H_

//*********************************************
 /*   Transmit and Receive port bits */ 
 #define SERIAL_PORT   PORTB 
 #define SERIAL_TRIS     TRISB 
 #define   TX_PIN      3 
 #define   RX_PIN      1 
 
 /*   Xtal frequency */ 
 #define   XTAL   10000000 

 /*   Baud rate   */ 
 #define   BRATE   1200 

//*********************************************
 /*   Don't change anything else */ 
 #define SCALER      10000000 
 #define ITIME      4*SCALER/XTAL   /* Instruction cycle time */ 
 #if BRATE > 1200 
  #define   DLY      3      /* cycles per null loop */ 
  #define   TX_OHEAD 13      /* overhead cycles per loop */ 
 #else 
  #define   DLY      9      /* cycles per null loop */ 
  #define TX_OHEAD  14 
 #endif 
 #define   RX_OHEAD   12      /* receiver overhead per loop */ 
 
 #define   DELAY(ohead)   (((SCALER/BRATE)-(ohead*ITIME))/(DLY*ITIME)) 
 
 static bit   TxData @ (unsigned)&SERIAL_PORT*8+TX_PIN;   /* Map TxData to pin */ 
 static bit   RxData @ (unsigned)&SERIAL_PORT*8+RX_PIN;   /* Map RxData to pin */ 
 #define   INIT_PORT   SERIAL_TRIS = 1<<RX_PIN            /* set up I/O direction */ 
 
//*********************************************
 char getche(void) ;
 char getch(void)  ;
 void putch(char c) ;
 int gethex(void);
//*********************************************

#endif