/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>        /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>

#include <plib.h>
#include <delays.h>
#include <adc.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

#include <pic18f46k80.h>

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
char endLine[3] = "\n";
char openString[] = "Hello from PIC18";

/* --- configure DS1820 temperature sensor pin --- */
#define DS1820_DATAPIN      PORTBbits.RB3
#define output_low(pin)     TRISBbits.TRISB3 = 0;(PORTBbits.RB3 = 0)
#define output_high(pin)    TRISBbits.TRISB3 = 0;(PORTBbits.RB3 = 1)
#define input(pin)          input_func()
bool input_func(void)
{
    TRISBbits.TRISB3 = 1;
    return (PORTBbits.RB3);
}

#include "ds1820.h"

/* --- end configure DS1820 temperature sensor pin --- */

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
  uint8_t index = 0;
  int i = 0;
  uint16_t sensorValue;

  int16_t temperature_raw;
  float temperature_float;
  char temperature_string[8];


  /* Configure the oscillator for the device */
  ConfigureOscillator();

  /* Initialize I/O and Peripherals for application */
  InitApp();


  Delay10KTCYx(128);
  Delay10KTCYx(128);
  Delay10KTCYx(128);
  Delay10KTCYx(128);
  Delay10KTCYx(128);
  Delay10KTCYx(128);
  printf("\r\n");
  printf("\r\n");
  printf("Oscillator and Peripherals initialized!\r\n");



  // start a conversion
  GODONE = 1;

  bool DS1820_FOUND;

  // populate sensor ID
  if(DS1820_FindFirstDevice()) {
    DS1820_FOUND = true;
    printf("Found first DS1820 device\n");
  } else {
    DS1820_FOUND = false;
    printf("Could not find first DS1820 device\n");
  }

  while(1) {
    
    if(characterReceived) {
      characterReceived = 0;
      printf("\n");
      for(i=0; i<6; i++) {
        index = i*2;
        sensorValue = ((uint8_t)temperature[index] * 256) + (uint8_t)temperature[index+1];
        printf("ADC %d is: %u\r\n", i, sensorValue);
      }
    }

    if(unhandledIRQ == 1) {
      printf("A high priority IRQ was ignored\n");
      unhandledIRQ = 0;
    } else if(unhandledIRQ == 2) {
      printf("A low priority IRQ was ignored\n");
      unhandledIRQ = 0;
    }

    if ( DS1820_FOUND ) {
      temperature_raw = DS1820_GetTempRaw();
      DS1820_GetTempString(temperature_raw, temperature_string);
      temperature_float = DS1820_GetTempFloat();

      printf("DS1820 Sensor Temperature: %s?C \n\r", temperature_string);
      printf("DS1820 Sensor Temperature Float: %f \n\r", temperature_float);
      printf("DS1820 Sensor Temperature Raw: %ld \n\r", temperature_raw);
    }

  }
}
