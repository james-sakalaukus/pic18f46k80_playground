/*
 * oneWire_maxim.h
 *
 *  Created on: May 25, 2015
 *      Author: james
 */

#ifndef ONEWIRE_MAXIM_H_
#define ONEWIRE_MAXIM_H_

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>
#include <stdbool.h>
#include "user.h"

// method declarations
uint8_t OWFirst();
uint8_t OWNext();
uint8_t OWVerify();
uint8_t OWReset();
void OWWriteByte(uint8_t data);
void OWWriteBit(uint8_t value);
uint8_t OWReadBit();
uint8_t OWReadByte();
uint8_t OWSearch();
uint8_t docrc8(uint8_t value);

// DS1820 Specific
float ReadTemp(uint8_t *address);
void startTempConversion();

// global search state
uint8_t ROM_NO[8];
uint8_t LastDiscrepancy;
uint8_t LastFamilyDiscrepancy;
uint8_t LastDeviceFlag;
uint8_t crc8;


// Standard Speed - uS delay values
#define DELAY_A 6
#define DELAY_B 64
#define DELAY_C 60
#define DELAY_D 10
#define DELAY_E 9
#define DELAY_F 55
#define DELAY_H 480
#define DELAY_I 70
#define DELAY_J 410

// TEST BUILD
static uint8_t dscrc_table[] = {
        0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
      157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
       35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
      190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
       70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
      219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
      101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
      248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
      140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
       17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
      175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
       50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
      202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
       87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
      233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
      116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};

#endif /* ONEWIRE_MAXIM_H_ */
