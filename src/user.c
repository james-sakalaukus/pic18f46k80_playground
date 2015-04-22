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
//bool input_func(void) {
//  TRISCbits.TRISC5 = 1;
//    return (PORTCbits.RC5);
//}

bool input_func(void) {
  TRISAbits.TRISA0 = 1;
    return (PORTAbits.RA0);
}

void DS1820_DelayMs(unsigned long dly_ms) {
  do {
    DS1820_DelayUs(999);
  } while(--dly_ms);
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
  config0 = 0;
  config1 = 0;

  /******************** Hearbeat LED Setup ************************************/
    // Initialize heartbeat led as output
    TRISDbits.TRISD2 = 0;

    /******************** Temperature Sensor Setup ************************************/
    // 3-pins: Sensor, VDD, GND
    // RC5, RC4, RD3
    // RC5 is set with macros defined in user.h; used by ds1820.h functions

    // Sensor VDD Output
    TRISCbits.TRISC4 = 0;
    LATCbits.LATC4 = 1;
    
    // Sensor GND Output
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;
    
    // 3-pins: Sensor, VDD, GND
    // RA0:2
    // RA0 is set with macros defined in user.h; used by ds1820.h functions
    // make sure to make pins digital
    ADCON0 = 0xFC;
    ADCON1 = 0;
    TRISAbits.TRISA1 = 0;
    LATAbits.LATA1 = 1;
    TRISAbits.TRISA2 = 0;
    LATAbits.LATA2 = 0;

//    /******************** ADC Setup ************************************/
//    // use RA0:3, RA5, RE0 as analog inputs
//    TRISAbits.TRISA0 = 1;
//    TRISAbits.TRISA1 = 1;
//    TRISAbits.TRISA2 = 1;
//    TRISAbits.TRISA3 = 1;
//    TRISAbits.TRISA5 = 1;
//    TRISEbits.TRISE0 = 1;
//    TRISE = 255; // all inputs
//    REPU = 0;   // disable PORT E pull-ups
//
//    // configure ADC to use AN0,DONE, and turn on ADC
//    ADCON0 = 0b00000001;
//    // configure ctmu "special trigger", Internal VREF 4.1, AVss, AVss
//    ADCON1 = 0b01000000;
//
//    // right justified, 0 TAD, F RC (clock derived from A/D RC oscillator)
//    ADCON2 = 0b10000111;
//
//    // set AN0-5 as an analog inputs
//    ANSEL0 = 1;
//    ANSEL1 = 1;
//    ANSEL2 = 1;
//    ANSEL3 = 1;
//    ANSEL4 = 1;
//    ANSEL5 = 1;
//    // clear ADC interrupt
//    ADIF = 0;
//    ADIP = 0;  //A/D Converter Interrupt Priority Low
//    // enable ADC interrupt
//    ADIE = 1;
  
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
    
    
    /* Enable interrupts */
    IPEN = 1;

    PEIE = 1;               // enable peripheral interrupts
    GIE = 1;

    // initialize global variables
    unhandledIRQ = 0;
    uart1CharacterReceived = 0;
    uart2CharacterReceived = 0;
    updateDisplay = 0;
    DS1820_FOUND = 0;
}

void doHeartBeat(void) {
    heartbeat = ~heartbeat;
}


