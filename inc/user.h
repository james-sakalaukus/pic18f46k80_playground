/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
//#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
//#include <stdio.h>          /* For printf definition */

//#include "user.h"
//#include "system.h"
//
// #include <plib.h>

#define heartbeat LATDbits.LATD2


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void putch(char data);
void InitApp(void);         /* I/O and Peripheral Initialization */
void delay();
void doHeartBeat();

char characterReceived;

// 6 16-bit temperature values
uint8_t temperature[12];
