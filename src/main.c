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
//#include <pic18f46k80.h>
#include "pic18f46k80.h"
#include "configuration_bits.h"
#include "system.h"
#include "user.h"
#include "oneWire_maxim.h"
#include "ds18b20.h"

/******************************************************************************/
/* Function Prototypes                                                        */
/******************************************************************************/



void setLcdBaud(void);
void clearScreen(void);
uint8_t Init_Sensors(void);
void readTempAll(void);
// DS1820 Specific
float ReadTemp(uint8_t *address);
void startTempConversion();
void writeDisplay();

uint8_t updateHeater();

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
float currentTemps[DS1820_DEVICES];
uint8_t tempSensorAddress[DS1820_DEVICES][DS1820_ADDR_LEN];
uint8_t sensorsFound[DS1820_DEVICES];
float averageTemp;
float maxTemp;
float minTemp;
int sensorCount;

#define HOLD_TEMP 78.00

/******************************************************************************
* 20x4 LCD
*
* |xx.y xx.y xx.y xx.y |
* |Avg xx.y Hold: xx.y |
* |Heater Status: OFF  |
* |Max: xx.y  Min: xx.y|
*
******************************************************************************/


/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
  sensorCount = 0;
  maxTemp = -1000;
  minTemp = 1000;
  // Configure the oscillator for the device
  ConfigureOscillator();

  // Initialize I/O and Peripherals for application
  InitApp();

  clearScreen();
  printf("\r\n\r\nOscillator and Peripherals initialized!\r\n");

  sensorCount = Init_Sensors();
  printf("Found %d sensors\r\n", sensorCount);

  while(1) {
    

    if(unhandledIRQ == 1) {
      printf("A high priority IRQ was ignored\n");
      unhandledIRQ = 0;
    } else if(unhandledIRQ == 2) {
      printf("A low priority IRQ was ignored\n");
      unhandledIRQ = 0;
    }

    if(updateDisplay == 20) {
      if(sensorCount) {
        writeDisplay();
      } else {
        sensorCount = Init_Sensors();
      }
      updateDisplay = 0;
    }


  }
}

uint8_t updateHeater() {
  if(averageTemp < (float)HOLD_TEMP-0.5) {
    heaterPin = 1;
  } else if(averageTemp > (float)HOLD_TEMP+0.5) {
    heaterPin = 0;
  } else {
//    heaterPin = heaterPin;
  }
  return heaterPin;
}


void writeDisplay() {

  int i;

  readTempAll();
  clearScreen();

  averageTemp = 0;
  for(i=0; i<DS1820_DEVICES;i++) {
    if(sensorsFound[i]) {
      printf("%2.1f ", currentTemps[i]);
      averageTemp += currentTemps[i];
      if(currentTemps[i] > maxTemp) {
        maxTemp = currentTemps[i];
      } else if(currentTemps[i] < minTemp) {
        minTemp = currentTemps[i];
      }
    }
  }
  averageTemp = averageTemp / sensorCount;
  printf("Avg %2.1f Hold %2.1f\n", averageTemp, (float)HOLD_TEMP);
  printf("Heater Status: ");
  if(updateHeater()) {
    printf("ON\n");
  } else {
    printf("OFF\n");
  }
  printf("Max %2.1f Min %2.1f", maxTemp, minTemp);
}



void clearScreen(void) {
  // Command bytes to clear screen
  unsigned char clear_screen[] = {254, 88};

  while(Busy2USART());
  Write2USART(clear_screen[0]);
  while(Busy2USART());
  Write2USART(clear_screen[1]);
}

void setLcdBaud(void) {
  // set to 115200
  unsigned char set_baudrate[] = {254, 57, 8};

  while(Busy2USART());
  Write2USART(set_baudrate[0]);
  while(Busy2USART());
  Write2USART(set_baudrate[1]);
  while(Busy2USART());
  Write2USART(set_baudrate[2]);
}


uint8_t Init_Sensors(void) {

  uint8_t returnValue = 0;
  uint8_t sensorCount = 0;
  int8_t i = 0;

  for(i=0;i<DS1820_DEVICES; i++) {
    sensorsFound[i] = 0;
  }
  printf("Init_Sensors(): Checking for DS1820 Devices\r\n");

  // find ALL devices
  returnValue = OWFirst();
  while (returnValue) {
    sensorsFound[sensorCount] = 1;
    // print device found
    printf("%d ", sensorCount);
    for (i = 7; i >= 0; i--) {

      tempSensorAddress[sensorCount][i] = ROM_NO[i];
      printf("%02X", tempSensorAddress[sensorCount][i]);
    }
    printf("\r\n");
    sensorCount++;
    if(sensorCount == DS1820_DEVICES) {
      break;
    } else {
      returnValue = OWNext();
    }
  }
  return sensorCount;
}


void readTempAll() {
  uint8_t sensorCount = 0;
  uint8_t i = 0;
//  printf("\r\n");
  startTempConversion();
  for(i=0;i<DS1820_DEVICES; i++) {
    if(sensorsFound[i]) {
      currentTemps[i] = ReadTemp(tempSensorAddress[i]);
    }
  }
//  printf("\r\n");
}

/*******************************************************************************
* Address all DS1820 devices on bus, send start temp conversion command
*******************************************************************************/
void startTempConversion() {
  OWReset();
  OWWriteByte(DS1820_CMD_SKIPROM);
  output_temp_sensors(1);
  OWWriteByte(DS1820_CMD_CONVERTTEMP);
  DelayMs(2000);
}
/*******************************************************************************
 * Get temperature raw value from single DS1820 device.
 ******************************************************************************/
float ReadTemp(uint8_t *address)
{
  uint8_t i;
//  uint16_t temp = 0;
  float highres;
  uint8_t scratchPad[DS1820_SCRPADMEM_LEN];

  // read scratch pad memory for given address
  OWReset();
  OWWriteByte(DS1820_CMD_MATCHROM);
  for (i = 0; i < DS1820_ADDR_LEN; i ++) {
    OWWriteByte(address[i]);
  }
  OWWriteByte(DS1820_CMD_READSCRPAD);
  for (i=0; i < DS1820_SCRPADMEM_LEN; i++) {
    scratchPad[i] = OWReadByte();
  }

  // Print temp for Debug
  if((int8_t)scratchPad[DS1820_REG_TEMPMSB] > 0) {
    highres = ((scratchPad[DS1820_REG_TEMPMSB] & 0x07)*16) + (scratchPad[DS1820_REG_TEMPLSB] * 0.0625);
  } else {
    highres = ((scratchPad[DS1820_REG_TEMPMSB] & 0x07)*16) + (scratchPad[DS1820_REG_TEMPLSB] * -0.0625);
  }
  // convert C to F
  highres = highres * (9/2) + 32;
//  printf("ReadTemp() MSB: %u, LSB: %u, highres: %2.2f \r\n", scratchPad[DS1820_REG_TEMPMSB], scratchPad[DS1820_REG_TEMPLSB], highres);

  return (highres);
}
