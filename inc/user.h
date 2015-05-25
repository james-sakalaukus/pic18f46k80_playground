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
#define DS1820_DEVICES    4     // number of 1-wire buses

/******************************************************************************/
/* Global Variables                                                           */
/******************************************************************************/

volatile unsigned char uart1CharacterReceived;
volatile unsigned char uart2CharacterReceived;
volatile unsigned char unhandledIRQ;
volatile unsigned char receivedCharacter;
volatile unsigned char updateDisplay;


// 6 16-bit temperature values
volatile uint8_t temperature[12];


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void putch(char data);
void InitApp(void);
void delay();
void doHeartBeat();

void DelayMs(unsigned long dly_ms);
#define DelayUs(dly_us) __delay_us(dly_us)

void output_temp_sensors(uint8_t value);
uint8_t input_temp_senosrs();
#endif
