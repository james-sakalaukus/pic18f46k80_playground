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

volatile unsigned char uart1CharacterReceived;
volatile unsigned char uart2CharacterReceived;
volatile unsigned char unhandledIRQ;
volatile unsigned char receivedCharacter;
volatile unsigned char updateDisplay;
volatile unsigned char DS1820_FOUND;

// 6 16-bit temperature values
volatile uint8_t temperature[12];

/* --- configure DS1820 temperature sensor pin --- */
#define DS1820_DATAPIN      PORTCbits.RC5
#define output_low()     TRISCbits.TRISC5 = 0;(LATCbits.LATC5 = 0)
#define output_high()    TRISCbits.TRISC5 = 0;(LATCbits.LATC5 = 1)
//#define DS1820_DATAPIN      PORTAbits.RA0
//#define output_low()     TRISAbits.TRISA0 = 0;(LATAbits.LATA0 = 0)
//#define output_high()    TRISAbits.TRISA0 = 0;(LATAbits.LATA0 = 1)

#define input()          input_func()
bool input_func(void);


/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void putch(char data);
void InitApp(void);         /* I/O and Peripheral Initialization */
void delay();
void doHeartBeat();

void DS1820_DelayMs(unsigned long dly_ms);
//void DS1820_DelayUs(unsigned long dly_us);
#define DS1820_DelayUs(dly_us)       __delay_us(dly_us)

#endif
