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



/******************************************************************************/
/* User Functions                                                             */
/******************************************************************************/

/*******************************************************************************
 * FUNCTION:   DS1820_DelayUs
 * PURPOSE:    Delay for the given number of micro seconds.
 *
 * INPUT:      dly_us      number of micro seconds to delay
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
//#define DS1820_DelayUs(dly_us)       __delay_us(dly_us)

//// 11200 max value for _delay function
//void DS1820_DelayUs(unsigned long dly_us) {
//  unsigned long int i = 0;
//
//  for(; i<(16*dly_us); i++) {
//    NOP();
////    _delay(dly_us);
//  }
//}

/*******************************************************************************
 * FUNCTION:   DS1820_DelayMs
 * PURPOSE:    Delay for the given number of milliseconds.
 *
 * INPUT:      dly_ms      number of milliseconds to delay
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
//#define DS1820_DelayMs(dly_ms)   __delay_ms(dly_ms)
void DS1820_DelayMs(unsigned long dly_ms) {
//  dly_ms = dly_ms * 16;
  do {
    DS1820_DelayUs(999);
  } while(--dly_ms);
}


// Used in printf()
void putch(char data) {
  while(Busy2USART());
  Write2USART(data);
}



void InitApp(void)
{
    // Initialize heartbeat led as output
    TRISDbits.TRISD2 = 0;

    // temperature sensor
    //    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC6 = 0;
    
    LATCbits.LATC5 = 1;
    LATCbits.LATC6 = 0;
    

    /******************** ADC Setup ************************************/
    // use RA0:3, RA5, RE0 as analog inputs
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA2 = 1;
    TRISAbits.TRISA3 = 1;    
    TRISAbits.TRISA5 = 1;
    TRISEbits.TRISE0 = 1;
    TRISE = 255; // all inputs
    REPU = 0;   // disable PORT E pull-ups

    // configure ADC to use AN0,DONE, and turn on ADC
    ADCON0 = 0b00000001;
    // configure ctmu "special trigger", Internal VREF 4.1, AVss, AVss
    ADCON1 = 0b01000000;
    
    // right justified, 0 TAD, F RC (clock derived from A/D RC oscillator)
    ADCON2 = 0b10000111;
    // set AN0-5 as an analog inputs
    ANSEL0 = 1;
    ANSEL1 = 1;
    ANSEL2 = 1;
    ANSEL3 = 1;
    ANSEL4 = 1;
    ANSEL5 = 1;
    // clear ADC interrupt
    ADIF = 0;
    ADIP = 0;  //A/D Converter Interrupt Priority Low
    // enable ADC interrupt
    ADIE = 1;
  
      //----Configure Timers----
    WriteTimer0(0); //clear timer if previously contains any value
    OpenTimer0(TIMER_INT_ON & T0_16BIT & T0_SOURCE_INT & T0_PS_1_256);
  
    // configure UART2  
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
                BAUDRATEREG);
    
    
    /* Enable interrupts */
    IPEN = 1;

    PEIE = 1;               // enable peripheral interrupts
    GIE = 1;

    unhandledIRQ = 0;
    characterReceived = 0;
}

void doHeartBeat(void) {
    heartbeat = ~heartbeat;
}
