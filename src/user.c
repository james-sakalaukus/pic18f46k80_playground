/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>          /* For printf definition */

#include "user.h"
#include "system.h"

#include <plib.h>

#include <pic18f46k80.h>


/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/


void DelayMs(unsigned long dly_ms) {
  do {
    DelayUs(999);
  } while(--dly_ms);
}

void output_temp_sensors(uint8_t value) {
  TRISCbits.TRISC3 = 0;
  LATCbits.LATC3 = value;
}

uint8_t input_temp_sensors() {
  TRISCbits.TRISC3 = 1;
  return PORTCbits.RC3;
}



// Used in printf()
void putch(char data) {
  while(Busy2USART());
  Write2USART(data);
  while(Busy1USART());
  Write1USART(data);
}

void InitApp(void)
{
  uint8_t config0, config1;
  uint8_t i;

  config0 = 0;
  config1 = 0;

  /******************** Hearbeat LED Setup ************************************/
    // Initialize heartbeat led as output
    TRISDbits.TRISD2 = 0;

    // Initialize heater relay as output
    TRISCbits.TRISC4 = 0;

    /******************** Timer 0 Setup ************************************/

    WriteTimer0(0); //clear timer if previously contains any value
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_256);

    /******************** Timer 3 Setup ************************************/

    WriteTimer3(0); //clear timer if previously contains any value
    // use interrupt, 16-bit, 1:8 prescaler, instruction clock source, oscillator off,

    config0 = TIMER_INT_ON & T3_16BIT_RW & T3_SOURCE_FOSC_4 & T3_OSC1EN_ON & T3_PS_1_8 & T3_SYNC_EXT_OFF;
    config1 = TIMER_GATE_OFF;
    OpenTimer3(config0, config1);

    /******************** UART1 Setup ************************************/
    TRISCbits.TRISC6 = 0;   // TX
    TRISCbits.TRISC7 = 1;   // RX
    RC1IP = 1;              // high priority interrupt
    RC1IF = 1;              // clear RX interrupt
    RC1IE = 1;              // enable RX interrupt

    //Configure the serial port
    Open1USART( USART_TX_INT_OFF &
                USART_RX_INT_ON &
                USART_ASYNCH_MODE &
                USART_EIGHT_BIT &
                USART_CONT_RX &
                USART_BRGH_HIGH,
                BAUDRATEREG1);

    /******************** UART2 Setup ************************************/
    TRISDbits.TRISD6 = 0;   // TX
    TRISDbits.TRISD7 = 1;   // RX
    RC2IP = 1;              // high priority interrupt
    RC2IF = 1;              // clear RX interrupt
    RC2IE = 1;              // enable RX interrupt

        //Configure the serial port
    Open2USART( USART_TX_INT_OFF &
                USART_RX_INT_ON &
                USART_ASYNCH_MODE &
                USART_EIGHT_BIT &
                USART_CONT_RX &
                USART_BRGH_HIGH,
                BAUDRATEREG2);
    
    
    // Enable interrupt priorities
    IPEN = 1;
    // enable peripheral interrupts
    PEIE = 1;
    // enable global interrupts
    GIE = 1;

    // initialize global variables
    unhandledIRQ = 0;
    uart1CharacterReceived = 0;
    uart2CharacterReceived = 0;
    updateDisplay = 0;
}

void doHeartBeat(void) {
    heartbeat = ~heartbeat;
}


