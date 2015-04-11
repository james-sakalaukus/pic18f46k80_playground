   /*** FILEHEADER ****************************************************************
 *
 *    FILENAME:    ds1820.h
 *    DATE:        25.02.2005
 *    AUTHOR:      Christian Stadler
 *
 *    DESCRIPTION: Driver for DS1820 1-Wire Temperature sensor (Dallas)
 *
 ******************************************************************************/


#ifndef _DS1820_H
#define _DS1820_H

#include "user.h"
#include <xc.h>

#define uint8 uint8_t
#define sint16 int16_t
#define uint16 uint16_t
#define sint8 int8_t

/* --- configure DS1820 temparture sensor pin --- */
#define DS1820_DATAPIN      LATCbits.LATC5
bool input_func(void)
{
    TRISCbits.TRISC5 = 1;
    return (PORTCbits.RC5);
}

//#define DS1820_DATAPIN      PORTBbits.RB3
#define output_low(pin)     TRISCbits.TRISC5 = 0;(PORTCbits.RC5 = 0)
#define output_high(pin)    TRISCbits.TRISC5 = 0;(PORTCbits.RC5 = 1)
#define input(pin)          input_func()

/* check configuration of driver */
#ifndef DS1820_DATAPIN
#error DS1820 data pin not defined!
#endif


#define TEMP_RES              0x100 /* temperature resolution => 1/256�C = 0.0039�C */


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

static bool bDoneFlag;
static uint8_t nLastDiscrepancy_u8;
static uint8_t nRomAddr_au8[DS1820_ADDR_LEN];


#define DS1820_DelayUs(dly_us)       __delay_us(dly_us)
#define DS1820_DelayMs(dly_ms)   __delay_ms(dly_ms)
#ifdef DS1820_INTERRUPT_LOCK
#define DS1820_DisableInterrupts()  disable_interrupts(GLOBAL)
#else
#define DS1820_DisableInterrupts()
#endif
#ifdef DS1820_INTERRUPT_LOCK
#define DS1820_EnableInterrupts()   enable_interrupts(GLOBAL)
#else
#define DS1820_EnableInterrupts()
#endif

bool DS1820_Reset(void);
bool DS1820_ReadBit(void);
void DS1820_WriteBit(bool bBit);
uint8 DS1820_ReadByte(void);
void DS1820_WriteByte(uint8 val_u8);
void DS1820_AddrDevice(uint8 nAddrMethod);
bool DS1820_FindNextDevice(void);
bool DS1820_FindFirstDevice(void);
void DS1820_WriteEEPROM(uint8 nTHigh, uint8 nTLow);
sint16 DS1820_GetTempRaw(void);
float DS1820_GetTempFloat(void);
void DS1820_GetTempString(sint16 tRaw_s16, char *strTemp_pc);

#endif /* _DS1820_H */

