/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/
#include <xc.h>        /* XC8 General Include File */
#include <stdint.h>        /* For uint8_t definition */
#include <stdbool.h>       /* For true/false definition */
#include <stdio.h>

#include <plib.h>
#include <delays.h>

#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */

/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
char endLine[3] = "\n";
char openString[] = "Hello from PIC18";

/******************************************************************************/
/* Main Program                                                               */
/******************************************************************************/

void main(void)
{
  uint8_t index = 0;
  int i = 0;
  uint16_t sensorValue;
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
  printf("\n");
  printf("\n");
  printf("Oscillator and Peripherals initialized!\n");


  

    // start a conversion
    GODONE = 1;
    
    while(1)
    {
      if(characterReceived) {
        characterReceived = 0;         
        printf("\n");
        for(i=0; i<6; i++) {
          index = i*2;
          sensorValue = ((uint8_t)temperature[index] * 256) + (uint8_t)temperature[index+1];
          printf("ADC %d is: %u\n", i, sensorValue);
        }
      }
    }
}

/*
 * Begin Copied data from: http://www.picprojects.net/ds1820/
 */

//int16_t temperature_raw;     /* temperature raw value (resolution 1/256?C) */
//float temperature_float;
//char temperature[8];        /* temperature as string */
//uint8_t sensor_count;         /* sensor counter */
//
///* main loop */
//while (1)
//{
///* set LED on measurement start */
//LED1_On();
//sensor_count = 0;
//if ( DS1820_FindFirstDevice() ) {
//    do {
//        /* get temperature raw value (resolution 1/256?C) */
//        temperature_raw = DS1820_GetTempRaw();
//        /* convert raw temperature to string for output */
//        DS1820_GetTempString(temperature_raw, temperature);
//        /* get temperature value as float */
//        temperature_float = DS1820_GetTempFloat();
//        /* print result to RS232 interface */
//        printf("Sensor %d: %s?C (temperature_float = %f), temperature_raw = %ld)\n\r",
//        sensor_count,
//        temperature,
//        temperature_float,
//        temperature_raw);
//        sensor_count ++;
//    } while ( DS1820_FindNextDevice() );
//
//sensor_count = 0;
//}
