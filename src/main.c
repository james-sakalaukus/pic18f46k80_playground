/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>        /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
//#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>

#include <plib.h>
#include <delays.h>
#include <adc.h>
#include <string.h>
#include <pic18f46k80.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */


//#include "oneWire.h"
#include <oneWire_maxim.h>
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
char endLine[3] = "\n";
char openString[] = "Hello from PIC18";

static float currentTemps[DS1820_DEVICES];

void writeDisplay(void);
void readAnalogs(void);
void clearScreen(void);
void Init_Sensors(void);
void readTempAll(void);

//uint8_t tempSensorAddress[DS1820_DEVICES][DS1820_ADDR_LEN];


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
//  DelayMs(10000);

  printf("\r\n");
  printf("\r\n");
  printf("Oscillator and Peripherals initialized!\r\n");


  Init_Sensors();

  // start a conversion
  GODONE = 1;

  while(1) {
    
//    if(uart2CharacterReceived) {
//      uart2CharacterReceived = 0;
//      switch (receivedCharacter) {
//        case 'd':
//          writeDisplay();
//          break;
//        case 'v':
//          readAnalogs();
//          break;
//        case 'a':
////          printTempAddress();
//          break;
//        default: break;
//      }
//    }

    if(unhandledIRQ == 1) {
      printf("A high priority IRQ was ignored\n");
      unhandledIRQ = 0;
    } else if(unhandledIRQ == 2) {
      printf("A low priority IRQ was ignored\n");
      unhandledIRQ = 0;
    }

    if(updateDisplay){
//      readTempAll();
//      printf("%c%c", 254, 88);
//      for(i=0; i<DS1820_DEVICES;i++) {
//        printf("%2.1f ", currentTemps[i]);
//      }
      updateDisplay = 0;
    }
  }
}

//--------------------------------------------------------------------------
// TEST BUILD MAIN
//
void Init_Sensors(void) {

  uint8_t returnValue = 0;
  int8_t i = 0;
  uint8_t sensorCount = 0;

  // find ALL devices
  printf("%c%cInit_Sensors(): Checking for DS1820 Devices\r\n", 254, 88);
  returnValue = OWFirst();
  while (returnValue) {

    // print device found
    for (i = 7; i >= 0; i--) {
      printf("%02X", ROM_NO[i]);
    }

    printf("  %d\n", ++sensorCount);
    returnValue = OWNext();
  }
}

//void Init_Sensors_Old() {
//  uint8_t sensorCount;
//  sensorCount = 0;
//
//  printf("%c%cInit_Sensors(): Checking for DS1820 Devices\r\n", 254, 88);
//
//  if ( DS1820_FindFirstDevice(tempSensorAddress[sensorCount]) ) {
//    char address[DS1820_ADDR_LEN+10];
//    do {
//      printf("Found Sensor %d: \r\n", sensorCount);
//      for(int i=0; i< DS1820_ADDR_LEN; i++) {
//        sprintf(&address[i], "%X", tempSensorAddress[sensorCount][i]);
//      }
//      printf("Search successful address is: %s\r\n", address);
//      sensorCount ++;
//      if(sensorCount > DS1820_DEVICES) {
//        break;
//      }
//    } while ( DS1820_FindNextDevice(tempSensorAddress[sensorCount]) );
//    sensorCount = 0;
//  }
//}

//void readTempAll() {
//  uint8_t sensorCount = 0;
//  uint8_t i = 0;
//  if ( DS1820_FindFirstDevice() ) {
//    do {
//      currentTemps[sensorCount] = DS1820_GetTempFloat();
//      currentTemps[sensorCount] = ((currentTemps[sensorCount] * 9)/5)+32;
//      sensorCount ++;
//    } while ( DS1820_FindNextDevice() );
//    sensorCount = 0;
//  } // findFirstDevice()
//}

//void readTempAll() {
//  uint8_t sensorCount = 0;
//  uint8_t i = 0;
//  for(i=0;i<DS1820_DEVICES;i++) {
//    currentTemps[i] = DS1820_GetTempFloat(tempSensorAddress[i]);
//    currentTemps[i] = ((currentTemps[i] * 9)/5)+32;
//  }
//}

void clearScreen(void) {
  unsigned char clear_screen[] = {254, 88};    //Command bytes to clear screen
  while(Busy1USART());
  Write1USART(clear_screen[0]);
  while(Busy1USART());
  Write1USART(clear_screen[1]);
}

void writeDisplay(void) {

  // clear screen - "X"
  // go home - "H"
//  unsigned char unlock[] = {0xFE, 0xCA, 0xF5, 0xA0, 0x00};
//  unsigned char display_on[] = {0xFE, 0x42, 0x00};  //Command bytes to turn display on
  unsigned char clear_screen[] = {254, 88};    //Command bytes to clear screen
  unsigned char message[] = "Hello World!";     //Message string
  unsigned char set_baudrate[] = {254, 57, 8};    // set to 115200

//  for(int i=0; i<5; i++) {
//    while(Busy1USART());
//    Write1USART(unlock[i]);
//  }
//
//  while(Busy1USART());
//  Write1USART(set_baudrate[0]);
//  while(Busy1USART());
//  Write1USART(set_baudrate[1]);
//  while(Busy1USART());
//  Write1USART(set_baudrate[2]);
//
  while(Busy1USART());
  Write1USART(clear_screen[0]);
  while(Busy1USART());
  Write1USART(clear_screen[1]);

  printf("Message length is : %d\r\n", strlen(message));

  for(int i=0; i<strlen(message); i++) {
    while(Busy1USART());
    Write1USART(message[i]);
    while(Busy2USART());
    Write2USART(message[i]);
  }
}

void readAnalogs(void) {
  uint16_t sensorValue;
  uint8_t index = 0;
  for(int i=0; i<6; i++) {
    index = i*2;
    sensorValue = ((uint8_t)temperature[index] * 256) + (uint8_t)temperature[index+1];
    printf("ADC %d is: %u\r\n", i, sensorValue);
  }
}

