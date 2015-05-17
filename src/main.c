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


#include "oneWire.h"
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
char endLine[3] = "\n";
char openString[] = "Hello from PIC18";

static char temperatureSensorAddress[DS1820_DEVICE_PINS][DS1820_ADDR_LEN];

void writeDisplay(void);
float readTempSensor(uint8_t);
void readAnalogs(void);
void clearScreen(void);
void printTempAddress(void);
void printTemps(void);
void Init_Sensors(void);

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
  DelayMs(10000);

  printf("\r\n");
  printf("\r\n");
  printf("Oscillator and Peripherals initialized!\r\n");


  Init_Sensors();

  // start a conversion
  GODONE = 1;

  while(1) {
    
    if(uart2CharacterReceived) {
      uart2CharacterReceived = 0;
      switch (receivedCharacter) {
        case 'd':
          writeDisplay();
          break;
        case 'v':
          readAnalogs();
          break;
        case 'a':
          printTempAddress();
          break;
        default: break;
      }
    }

    if(unhandledIRQ == 1) {
      printf("A high priority IRQ was ignored\n");
      unhandledIRQ = 0;
    } else if(unhandledIRQ == 2) {
      printf("A low priority IRQ was ignored\n");
      unhandledIRQ = 0;
    }
    if(updateDisplay){
//      printTempAddress();
      printTemps();
      updateDisplay = 0;
    }
  }
}

void Init_Sensors() {
  uint8_t i, j;
  printf("Init_Sensors(): Checking for DS1820 Device on %d bus pins\r\n", DS1820_DEVICE_PINS);

  for(i=0; i< DS1820_DEVICE_PINS; i++) {

    if(DS1820_FindFirstDevice(i)) {
      DS1820_FOUND[i] = TRUE;

      for(j=0; j< DS1820_ADDR_LEN; j++) {
        sprintf(&temperatureSensorAddress[i][j], "%X", nRomAddr_au8[i][j]);
      }
      printf("Init_Sensors(): Found a DS1820 Device on bus: %d with address: %s\r\n", i, temperatureSensorAddress[i][0]);
    } else {
      DS1820_FOUND[i] = FALSE;
      printf("Init_Sensors(): No DS1820 Device on bus: %d\r\n", i);
    }
  }
}



void printTemps() {
  uint8_t i = 0;
  for(i=0; i< DS1820_DEVICE_PINS; i++) {
    if(DS1820_FOUND[i]) {
      printf("%c%cDS1820_%d: F: %2.2f \r\n", 254, 88, i, readTempSensor(i));
    }
  }
}
void printTempAddress() {
  uint8_t i = 0;
//  printf("printTempAddress()\r\n");
  for(i=0; i< DS1820_DEVICE_PINS; i++) {
    if(DS1820_FOUND[i]) {
      printf("DS1820 Device on bus: %d with address: %s\r\n", i, temperatureSensorAddress[i][0]);
    }
  }
}


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

float readTempSensor(uint8_t busNum) {

  float temperature_float;

  if(DS1820_FOUND[busNum]) {
    temperature_float = DS1820_GetTempFloat(busNum);
    temperature_float = ((temperature_float * 9)/5)+32;
    return temperature_float;

  } else if(DS1820_FindFirstDevice(busNum)) {
    DS1820_FOUND[busNum] = TRUE;
    temperature_float = DS1820_GetTempFloat(busNum);
    temperature_float = ((temperature_float * 9)/5)+32;
    return temperature_float;

  } else {
    DS1820_FOUND[busNum] = FALSE;
    printf("Could not find first DS1820 device on bus: %d\r\n", busNum);
    return 0;
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
