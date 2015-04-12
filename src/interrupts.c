/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

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
      
  if (RC2IF) {
    
      RC2IF=0; // Clear Interrupt Flag
      characterReceived = 1;
      Read2USART();
  
  } else if(TMR0IF) {
    TMR0IF = 0;
    if(!heartbeat) {
      WriteTimer0(32768);
      pulseLed = 0;
    } else {
      pulseLed = 1;
    }

    doHeartBeat();
  
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
    
    char index;
    index = adcIndex*2;    

    temperature[index] = ADRESH;
    temperature[index+1] = ADRESL;
    if(adcIndex==5) {
        ADCON0 = 1;
        adcIndex = 0;
    } else {
        ADCON0 = 4 + ADCON0;
        adcIndex++;
    }
    GODONE = 1;
    
  } else {
    unhandledIRQ = 2;
  }
}
