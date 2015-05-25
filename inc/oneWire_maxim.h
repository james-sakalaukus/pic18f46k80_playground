/*
 * oneWire_maxim.h
 *
 *  Created on: May 25, 2015
 *      Author: james
 */

#ifndef ONEWIRE_MAXIM_H_
#define ONEWIRE_MAXIM_H_

#include <stdbool.h>


// definitions
//#define FALSE 0
//#define TRUE  1

// method declarations
int  OWFirst();
int  OWNext();
int  OWVerify();
void OWTargetSetup(unsigned char family_code);
void OWFamilySkipSetup();
int  OWReset();
void OWWriteByte(unsigned char byte_value);
void OWWriteBit(unsigned char bit_value);
unsigned char OWReadBit();
int  OWSearch();
unsigned char docrc8(unsigned char value);

// global search state
unsigned char ROM_NO[8];
int LastDiscrepancy;
int LastFamilyDiscrepancy;
int LastDeviceFlag;
unsigned char crc8;

// Pause for exactly 'tick' number of ticks = 0.25us
void tickDelay(int tick); // Implementation is platform specific

// 'tick' values
int A,B,C,D,E,F,G,H,I,J;

#endif /* ONEWIRE_MAXIM_H_ */
