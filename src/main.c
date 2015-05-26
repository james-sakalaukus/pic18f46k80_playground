/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <plib.h>
#include <delays.h>
#include <adc.h>
#include <string.h>
#include <pic18f46k80.h>

#include "system.h"
#include "user.h"
#include "oneWire_maxim.h"

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/

static float currentTemps[DS1820_DEVICES];

void setLcdBaud(void);
void clearScreen(void);
void Init_Sensors(void);
void readTempAll(void);

uint8_t tempSensorAddress[DS1820_DEVICES][DS1820_ADDR_LEN];


/******************************************************************************
* 20x4 LCD
*
* |xx.y xx.y xx.y xx.y |
* |Hold Temp: xx.y     |
* |Heater Status: OFF  |
* |Max: xx.y  Min: xx.y|
*
******************************************************************************/


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{

  int i = 0;
  int j = 0;


  // Configure the oscillator for the device
  ConfigureOscillator();

  // Initialize I/O and Peripherals for application
  InitApp();

  clearScreen();
  printf("Oscillator and Peripherals initialized!\r\n");
  DelayMs(3000);

  Init_Sensors();

  while(1) {
    

    if(unhandledIRQ == 1) {
      printf("A high priority IRQ was ignored\n");
      unhandledIRQ = 0;
    } else if(unhandledIRQ == 2) {
      printf("A low priority IRQ was ignored\n");
      unhandledIRQ = 0;
    }

    if(updateDisplay){
      readTempAll();
      printf("%c%c", 254, 88);
      for(i=0; i<DS1820_DEVICES;i++) {
        printf("%2.1f ", currentTemps[i]);
      }
      updateDisplay = 0;
    }
  }
}

void Init_Sensors(void) {

  uint8_t returnValue = 0;
  uint8_t sensorCount = 0;
  int8_t i = 0;

  clearScreen();
  printf("Init_Sensors(): Checking for DS1820 Devices\r\n");

  // find ALL devices
  returnValue = OWFirst();
  while (returnValue) {

    // print device found
    printf("%d ", sensorCount);
    for (i = 7; i >= 0; i--) {
      printf("%02X", ROM_NO[i]);
      tempSensorAddress[sensorCount][i] = ROM_NO[i];
    }
    printf("\r\n");

    if(++sensorCount > DS1820_DEVICES) {
      break;
    } else {
      returnValue = OWNext();
    }
  }
}


void readTempAll() {
  uint8_t sensorCount = 0;
  uint8_t i = 0;
  startTempConversion();
  DelayMs(1000);
  for(i=0;i<DS1820_DEVICES; i++) {
    currentTemps[i] = ReadTemp(tempSensorAddress[i]);
  }
}

void clearScreen(void) {
  // Command bytes to clear screen
  unsigned char clear_screen[] = {254, 88};

  while(Busy1USART());
  Write1USART(clear_screen[0]);
  while(Busy1USART());
  Write1USART(clear_screen[1]);
}

void setLcdBaud(void) {
  // set to 115200
  unsigned char set_baudrate[] = {254, 57, 8};

  while(Busy1USART());
  Write1USART(set_baudrate[0]);
  while(Busy1USART());
  Write1USART(set_baudrate[1]);
  while(Busy1USART());
  Write1USART(set_baudrate[2]);
}


