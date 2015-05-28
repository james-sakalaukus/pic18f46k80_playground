/*
 * ds18b20.h
 *
 *  Created on: May 27, 2015
 *      Author: james
 */

#ifndef INC_DS18B20_H_
#define INC_DS18B20_H_

#include <xc.h>             /* XC8 General Include File */
#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#define DS1820_DEVICES    4     // number of 1-wire buses

// DS1820 Registers
#define DS1820_REG_TEMPLSB    0
#define DS1820_REG_TEMPMSB    1
#define DS1820_REG_CNTREMAIN  6
#define DS1820_REG_CNTPERSEC  7
#define DS1820_SCRPADMEM_LEN  9

// DS1820 Commands
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

// DS1820 Settings
#define DS1820_ADDR_LEN       8
#define TEMP_RES              0x100     /* temperature resolution => 1/256°C = 0.0039°C */
#define DS1820_FAMILY_CODE_DS18B20      0x28
#define DS1820_FAMILY_CODE_DS18S20      0x10

#endif /* INC_DS18B20_H_ */
