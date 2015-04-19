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
#include <string.h>
#include <pic18f46k80.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */


#include "ds1820.h"
/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
char endLine[3] = "\n";
char openString[] = "Hello from PIC18";



void writeDisplay(void);
void readTempSensor(void);
void readAnalogs(void);


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{

  int i = 0;


  /* Configure the oscillator for the device */
  ConfigureOscillator();

  /* Initialize I/O and Peripherals for application */
  InitApp();


  DS1820_DelayMs(1000);

  printf("\r\n");
  printf("\r\n");
  printf("Oscillator and Peripherals initialized!\r\n");

  // start a conversion
  GODONE = 1;

  while(1) {
    
    if(uart2CharacterReceived) {
      uart2CharacterReceived = 0;
      switch (receivedCharacter) {
        case 'd':
          writeDisplay();
          break;
        case 'a':
          readAnalogs();
          break;

        default:
          readTempSensor();
      }
    }

    if(unhandledIRQ == 1) {
      printf("A high priority IRQ was ignored\n");
      unhandledIRQ = 0;
    } else if(unhandledIRQ == 2) {
      printf("A low priority IRQ was ignored\n");
      unhandledIRQ = 0;
    }

  }
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

void readTempSensor(void) {

  int16_t temperature_raw;
  float temperature_float;
  char temperature_string[8];
  bool DS1820_FOUND;

  if(DS1820_FindFirstDevice()) {
    DS1820_FOUND = true;
//        printf("Found first DS1820 device\n");
    temperature_raw = DS1820_GetTempRaw();
//        DS1820_GetTempString(temperature_raw, temperature_string);
    temperature_float = DS1820_GetTempFloat();
    temperature_float = ((temperature_float * 9)/5)+32;

//        printf("DS1820 Sensor Temperature: %s?C \n\r", temperature_string);
    printf("DS1820 Sensor Temperature F: %f \r\n", temperature_float);
//        printf("DS1820 Sensor Temperature Raw: %ld \n\r", temperature_raw);
  } else {
    DS1820_FOUND = false;
    printf("Could not find first DS1820 device\r\n");
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
