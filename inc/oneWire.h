/*
 * oneWire.h
 *
 *  Created on: Apr 19, 2015
 *      Author: james
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include "system.h"
#include <xc.h>
#include "user.h"
#include <stdint.h>
#include <stdbool.h>        /* For true/false definition */

#define TEMP_RES              0x100 /* temperature resolution => 1/256°C = 0.0039°C */


/* -------------------------------------------------------------------------- */
/*                         DS1820 Timing Parameters                           */
/* -------------------------------------------------------------------------- */

#define DS1820_RST_PULSE       480   /* master reset pulse time in [us] */
#define DS1820_MSTR_BITSTART   2     /* delay time for bit start by master */
#define DS1820_PRESENCE_WAIT   40    /* delay after master reset pulse in [us] */
#define DS1820_PRESENCE_FIN    480   /* dealy after reading of presence pulse [us] */
#define DS1820_BITREAD_DLY     5     /* bit read delay */
#define DS1820_BITWRITE_DLY    100   /* bit write delay */


/* -------------------------------------------------------------------------- */
/*                            DS1820 Registers                                */
/* -------------------------------------------------------------------------- */

#define DS1820_REG_TEMPLSB    0
#define DS1820_REG_TEMPMSB    1
#define DS1820_REG_CNTREMAIN  6
#define DS1820_REG_CNTPERSEC  7
#define DS1820_SCRPADMEM_LEN  9     /* length of scratchpad memory */

#define DS1820_ADDR_LEN       8



/* -------------------------------------------------------------------------- */
/*                            DS1820 Commands                                 */
/* -------------------------------------------------------------------------- */

#define DS1820_CMD_SEARCHROM     0xF0
#define DS1820_CMD_READROM       0x33
#define DS1820_CMD_MATCHROM      0x55
#define DS1820_CMD_SKIPROM       0xCC
#define DS1820_CMD_ALARMSEARCH   0xEC
#define DS1820_CMD_CONVERTTEMP   0x44
#define DS1820_CMD_WRITESCRPAD   0x4E
#define DS1820_CMD_READSCRPAD    0xBE
#define DS1820_CMD_COPYSCRPAD    0x48
#define DS1820_CMD_RECALLEE      0xB8


#define DS1820_FAMILY_CODE_DS18B20      0x28
#define DS1820_FAMILY_CODE_DS18S20      0x10


/* -------------------------------------------------------------------------- */
/*                            static variables                                */
/* -------------------------------------------------------------------------- */

static bool bDoneFlag[DS1820_DEVICE_PINS];
static uint8_t nLastDiscrepancy_u8[DS1820_DEVICE_PINS];
static uint8_t nRomAddr_au8[DS1820_DEVICE_PINS][DS1820_ADDR_LEN];

/* -------------------------------------------------------------------------- */
/*                           Low-Level Functions                              */
/* -------------------------------------------------------------------------- */

// Function Prototypes
bool DS1820_Reset(uint8_t busNum);
bool DS1820_ReadBit(uint8_t busNum);
void DS1820_WriteBit(bool bBit, uint8_t busNum);
uint8_t DS1820_ReadByte(uint8_t busNum);
void DS1820_WriteByte(uint8_t val_u8, uint8_t busNum);

/* -------------------------------------------------------------------------- */
/*                             API Interface                                  */
/* -------------------------------------------------------------------------- */


void DS1820_AddrDevice(uint8_t nAddrMethod, uint8_t busNum);
bool DS1820_FindNextDevice(uint8_t busNum);
bool DS1820_FindFirstDevice(uint8_t busNum);
void DS1820_WriteEEPROM(uint8_t nTHigh, uint8_t nTLow, uint8_t busNum);
int16_t DS1820_GetTempRaw(uint8_t busNum);
float DS1820_GetTempFloat(uint8_t busNum);
void DS1820_GetTempString(int16_t tRaw_s16, char *strTemp_pc);


#endif /* ONEWIRE_H_ */
