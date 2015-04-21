/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */
#include <pic18f46k80.h>
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>
#include <stdio.h>

#include "user.h"        /* For true/false definition */

/******************************************************************************/
/* Interrupt Routines                                                         */
/******************************************************************************/

/* High-priority service */
int adcIndex = 0;
char pulseLed = 0;

void interrupt high_isr(void) {

  /* Determine which flag generated the interrupt */
  if (RC1IF) {

      RC1IF=0; // Clear Interrupt Flag
      uart1CharacterReceived = 1;
      Read1USART();

  } else if (RC2IF) {
    
      RC2IF=0; // Clear Interrupt Flag
      uart2CharacterReceived = 1;
      receivedCharacter = Read2USART();
  
  } else if(TMR0IF) {
    TMR0IF = 0;
    if(!heartbeat) {
      WriteTimer0(32768);
//      pulseLed = 0;
    } else {
//      pulseLed = 1;
      updateDisplay = 1;
    }

    doHeartBeat();


  } else if(TMR3IF) {
    TMR3IF = 0;

  } else {
    unhandledIRQ = 1;
  }
}

/* Low-priority interrupt routine */
void low_priority interrupt low_isr(void)
{
  if(ADIF) {
    
    // Clear ADC Interrupt Flag            
    ADIF = 0;     
    
//    char index;
//    index = adcIndex*2;
//
//    temperature[index] = ADRESH;
//    temperature[index+1] = ADRESL;
//    if(adcIndex==5) {
//        ADCON0 = 1;
//        adcIndex = 0;
//    } else {
//        ADCON0 = 4 + ADCON0;
//        adcIndex++;
//    }
//    GODONE = 1;
    
  } else {
    unhandledIRQ = 2;
  }
}
