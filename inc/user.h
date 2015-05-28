#ifndef USER_H
#define USER_H

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */

#include "system.h"
#include "pic18f46k80.h"

/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define heartbeat LATDbits.LATD2
#define heaterPin LATCbits.LATC4

#define DelayUs(dly_us) __delay_us(dly_us)

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/
volatile uint8_t uart1CharacterReceived;
volatile uint8_t uart2CharacterReceived;
volatile uint8_t unhandledIRQ;
volatile uint8_t receivedCharacter;
volatile uint8_t updateDisplay;

/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void putch(uint8_t data);
void InitApp(void);
void delay();
void doHeartBeat();
void DelayMs(uint32_t dly_ms);

void output_temp_sensors(uint8_t value);
uint8_t input_temp_sensors();

#endif
