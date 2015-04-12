#ifndef USER_H
#define USER_H

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <pic18f46k80.h>

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/

#define heartbeat LATDbits.LATD2

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

volatile char characterReceived;
volatile char unhandledIRQ;

// 6 16-bit temperature values
volatile uint8_t temperature[12];

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void putch(char data);
void InitApp(void);         /* I/O and Peripheral Initialization */
void delay();
void doHeartBeat();


#endif
