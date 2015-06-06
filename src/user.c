/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#include "system.h"
#include "user.h"
#include "pic18f46k80.h"

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */
#include <stdio.h>          /* For printf definition */
#include <timers.h>
#include <usart.h>

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
  /*********************************************
   * Initialize Global Variables
   ********************************************/
  unhandledIRQ = 0;
  uart1CharacterReceived = 0;
  uart2CharacterReceived = 0;
  updateDisplay = 0;

  /*********************************************
   * Output Tristate Setup
   ********************************************/
  TRISDbits.TRISD2 = 0;   // heart beat led
  TRISCbits.TRISC4 = 0;   // heater relay

  /*********************************************
   * Timer 0 Setup
   ********************************************/
  WriteTimer0(0);
  OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_256);

  /*********************************************
   * Timer 3 Setup
   ********************************************/
  WriteTimer3(0);
  // use interrupt, 16-bit, 1:8 prescaler, instruction clock source, oscillator off,
  OpenTimer3( TIMER_INT_ON & T3_16BIT_RW & T3_SOURCE_FOSC_4 &
              T3_OSC1EN_ON & T3_PS_1_8 & T3_SYNC_EXT_OFF,
              TIMER_GATE_OFF);

  /*********************************************
   * UART1 Setup
   ********************************************/
  TRISCbits.TRISC6 = 0;   // TX
  TRISCbits.TRISC7 = 1;   // RX


  // Configure the serial port
  Open1USART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE &
              USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH,
              BAUDRATEREG1);

  /*********************************************
   * UART2 Setup
   ********************************************/
  TRISDbits.TRISD6 = 0;   // TX
  TRISDbits.TRISD7 = 1;   // RX

  //Configure the serial port
  Open2USART( USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE &
              USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH,
              BAUDRATEREG2);


  /*********************************************
   * IRQ Setup
   ********************************************/
  IPR1bits.RC1IP = 1;  // UART1 high priority interrupt
  PIR1bits.RC1IF = 1;  // clear UART1 RX interrupt
  PIE1bits.RC1IE = 1;  // enable UART1 RX interrupt

  IPR3bits.RC2IP = 1;  // UART2 high priority interrupt
  PIR3bits.RC2IF = 1;  // clear UART2 RX interrupt
  PIE3bits.RC2IE = 1;  // enable UART2 RX interrupt

  RCONbits.IPEN = 1;   // Enable interrupt priorities
  INTCONbits.PEIE = 1;   // enable peripheral interrupts
  INTCONbits.GIE = 1;    // enable global interrupts


}

void doHeartBeat(void) {
    heartbeat = ~heartbeat;
}


