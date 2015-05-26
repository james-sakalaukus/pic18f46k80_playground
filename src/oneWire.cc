/*
 * oneWire.c
 *
 *  Created on: Apr 19, 2015
 *      Author: james
 */

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "user.h"
#include "oneWire.h"




/*******************************************************************************
 * FUNCTION:   DS1820_Reset
 * PURPOSE:    Initializes the DS1820 device.
 *
 * INPUT:      -
 * OUTPUT:     -
 * RETURN:     FALSE if at least one device is on the 1-wire bus, TRUE otherwise
 ******************************************************************************/
uint8_t DS1820_Reset()
{
  bool bPresPulse;

  // reset pulse
  output_temp_sensors(0);

  DelayUs(DS1820_RST_PULSE);
  output_temp_sensors(1);

  // wait until pullup pull 1-wire bus to high
  DelayUs(DS1820_PRESENCE_WAIT);

  // get presence pulse
  bPresPulse = input_temp_senosrs();

  DelayUs(424);

  return bPresPulse;
}


/*******************************************************************************
 * FUNCTION:   DS1820_ReadBit
 * PURPOSE:    Reads a single bit from the DS1820 device.
 *
 * INPUT:      -
 * OUTPUT:     -
 * RETURN:     uint8_t        value of the bit which as been read form the DS1820
 ******************************************************************************/
bool DS1820_ReadBit()
{
  output_temp_sensors(0);
  DelayUs(DS1820_MSTR_BITSTART);
  input_temp_senosrs();
  DelayUs(DS1820_BITREAD_DLY);

  return input_temp_senosrs();
}


/*******************************************************************************
 * FUNCTION:   DS1820_WriteBit
 * PURPOSE:    Writes a single bit to the DS1820 device.
 *
 * INPUT:      bBit        value of bit to be written
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
void DS1820_WriteBit(bool bBit)
{
  output_temp_sensors(0);
  DelayUs(DS1820_MSTR_BITSTART);

  if (bBit != FALSE) {
    output_temp_sensors(1);
  }

  DelayUs(DS1820_BITWRITE_DLY);
  output_temp_sensors(1);
}


/*******************************************************************************
 * FUNCTION:   DS1820_ReadByte
 * PURPOSE:    Reads a single byte from the DS1820 device.
 *
 * INPUT:      -
 * OUTPUT:     -
 * RETURN:     uint8_t          byte which has been read from the DS1820
 ******************************************************************************/
uint8_t DS1820_ReadByte()
{
  uint8_t i;
  uint8_t value = 0;

  for (i=0 ; i < 8; i++) {
    if(DS1820_ReadBit()) {
      value |= (1 << i);
    }
    DelayUs(120);
  }
  return(value);
}


/*******************************************************************************
 * FUNCTION:   DS1820_WriteByte
 * PURPOSE:    Writes a single byte to the DS1820 device.
 *
 * INPUT:      val_u8         byte to be written
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
void DS1820_WriteByte(uint8_t val_u8)
{
  uint8_t i;
  uint8_t temp;

  /* writes byte, one bit at a time */
  for (i=0; i < 8; i++) {
    temp = val_u8 >> i;     /* shifts val right 'i' spaces */
    temp &= 0x01;           /* copy that bit to temp */
    DS1820_WriteBit(temp);  /* write bit in temp into */
  }

  DelayUs(105);
}



/* -------------------------------------------------------------------------- */
/*                             API Interface                                  */
/* -------------------------------------------------------------------------- */


/*******************************************************************************
 * FUNCTION:   DS1820_AddrDevice
 * PURPOSE:    Addresses a single or all devices on the 1-wire bus.
 *
 * INPUT:      nAddrMethod       use DS1820_CMD_MATCHROM to select a single
 *                               device or DS1820_CMD_SKIPROM to select all
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
void DS1820_AddrDevice(uint8_t nAddrMethod, uint8_t *address)
{
  uint8_t i;

  // address single devices on bus
  if (nAddrMethod == DS1820_CMD_MATCHROM) {
    DS1820_WriteByte(DS1820_CMD_MATCHROM);
    for (i = 0; i < DS1820_ADDR_LEN; i ++) {
//       DS1820_WriteByte(nRomAddr[i]);
      DS1820_WriteByte(address[i]);
    }

  // address all devices on bus
  } else {
    DS1820_WriteByte(DS1820_CMD_SKIPROM);
  }
}


/*******************************************************************************
 * FUNCTION:   DS1820_FindNextDevice
 * PURPOSE:    Finds next device connected to the 1-wire bus.
 *
 * INPUT:      -
 * OUTPUT:     nRomAddr[]       ROM code of the next device
 * RETURN:     uint8_t                 TRUE if there are more devices on the 1-wire
 *                                  bus, FALSE otherwise
 ******************************************************************************/
bool DS1820_FindNextDevice(uint8_t *address)
{
  uint8_t state;
  uint8_t byteid;
  uint8_t mask = 1;
  uint8_t bitpos = 1;
  uint8_t nDiscrepancyMarker = 0;
  bool bit;
  bool bStatus;
  bool next = FALSE;

  // reset the ROM addresses to 0
  for (byteid=0; byteid < 8; byteid ++) {
      nRomAddr[byteid] = 0x00;
      address[byteid] = 0x00;
  }

  // reset 1-wire bus
  bStatus = DS1820_Reset();

  // no device found, reset the search
  if (bStatus || bDoneFlag) {
    nLastDiscrepancy = 0;
    return FALSE;
  }

  // send search rom command
  DS1820_WriteByte(DS1820_CMD_SEARCHROM);

  byteid = 0;

  do {
    state = 0;

    // read bit
    if ( DS1820_ReadBit() != 0 ) {
      state = 2;
    }
    DelayUs(120);

    // read bit complement
    if ( DS1820_ReadBit() != 0 ) {
      state |= 1;
    }
    DelayUs(120);

    /* description for values of state: */
    /* 00    There are devices connected to the bus which have conflicting */
    /*       bits in the current ROM code bit position. */
    /* 01    All devices connected to the bus have a 0 in this bit position. */
    /* 10    All devices connected to the bus have a 1 in this bit position. */
    /* 11    There are no devices connected to the 1-wire bus. */

    // if there are no devices on the bus
    if (state == 3) {
      break;
    } else {

      // devices have the same logical value at this position
      if (state > 0) {
        // get bit value
        bit = (bool)(state >> 1);

      // devices have confilcting bits in the current ROM code
      } else {
        // if there was a conflict on the last iteration
        if (bitpos < nLastDiscrepancy) {
          // take same bit as in last iteration
          bit = ( (nRomAddr[byteid] & mask) > 0 );

        } else {
          bit = (bitpos == nLastDiscrepancy);
        }

        if (bit == 0) {
          nDiscrepancyMarker = bitpos;
        }
      }

      /* store bit in ROM address */
      if (bit != 0) {
         nRomAddr[byteid] |= mask;
      } else {
         nRomAddr[byteid] &= ~mask;
      }

      DS1820_WriteBit(bit);

      // increment bit position
      bitpos ++;

      // calculate next mask value
      mask = mask << 1;

      // check if this byte has finished
      if (mask == 0) {
         byteid ++;  /* advance to next byte of ROM mask */
         mask = 1;    /* update mask */
      }
    }
  } while (byteid < DS1820_ADDR_LEN);


  // if search was unsuccessful then, reset the last discrepancy to 0
  if (bitpos < 65) {
    nLastDiscrepancy = 0;
    printf("DS1820_FindNextDevice(): Search NOT successful\r\n");
  // search was successful
  } else {

    nLastDiscrepancy = nDiscrepancyMarker;
    bDoneFlag = (nLastDiscrepancy == 0);
    next = TRUE;

    char laddress[DS1820_ADDR_LEN];
    for(int i=0; i< DS1820_ADDR_LEN; i++) {
      sprintf(&laddress[i], "%X", nRomAddr[i]);
//      sprintf(&address[i], "%X", nRomAddr[i]);
      address[i] = nRomAddr[i];
    }
    printf("DS1820_FindNextDevice(): Search successful address is: %s\r\n", laddress);
//    printf("DS1820_FindNextDevice(): Search successful address is: %s\r\n", address);

  }

    return next;
}


/*******************************************************************************
 * FUNCTION:   DS1820_FindFirstDevice
 * PURPOSE:    Starts the device search on the 1-wire bus.
 *
 * INPUT:      -
 * OUTPUT:     nRomAddr[]       ROM code of the first device
 * RETURN:     uint8_t                 TRUE if there are more devices on the 1-wire
 *                                  bus, FALSE otherwise
 ******************************************************************************/
bool DS1820_FindFirstDevice(uint8_t *address)
{
  nLastDiscrepancy = 0;
  bDoneFlag = FALSE;

  return (DS1820_FindNextDevice(address));
}


/*******************************************************************************
 * FUNCTION:   DS1820_WriteEEPROM
 * PURPOSE:    Writes to the DS1820 EEPROM memory (2 bytes available).
 *
 * INPUT:      nTHigh         high byte of EEPROM
 *             nTLow          low byte of EEPROM
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
void DS1820_WriteEEPROM(uint8_t nTHigh, uint8_t nTLow, uint8_t *address)
{
  /* --- write to scratchpad ----------------------------------------------- */
  DS1820_Reset();
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, address);
  DS1820_WriteByte(DS1820_CMD_WRITESCRPAD); /* start conversion */
  DS1820_WriteByte(nTHigh);
  DS1820_WriteByte(nTLow);

  DelayUs(10);

  DS1820_Reset();
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, address);
  DS1820_WriteByte(DS1820_CMD_COPYSCRPAD); /* start conversion */

  DelayMs(10);
}


/*******************************************************************************
 * FUNCTION:   DS1820_GetTempRaw
 * PURPOSE:    Get temperature raw value from single DS1820 device.
 *
 *             Scratchpad Memory Layout
 *             Byte  Register
 *             0     Temperature_LSB
 *             1     Temperature_MSB
 *             2     Temp Alarm High / User Byte 1
 *             3     Temp Alarm Low / User Byte 2
 *             4     Reserved
 *             5     Reserved
 *             6     Count_Remain
 *             7     Count_per_C
 *             8     CRC
 *
 *             Temperature calculation for DS18S20 (Family Code 0x10):
 *             =======================================================
 *                                             (Count_per_C - Count_Remain)
 *             Temperature = temp_raw - 0.25 + ----------------------------
 *                                                     Count_per_C
 *
 *             Where temp_raw is the value from the temp_MSB and temp_LSB with
 *             the least significant bit removed (the 0.5C bit).
 *
 *
 *             Temperature calculation for DS18B20 (Family Code 0x28):
 *             =======================================================
 *                      bit7   bit6   bit5   bit4   bit3   bit2   bit1   bit0
 *             LSB      2^3    2^2    2^1    2^0    2^-1   2^-2   2^-3   2^-4
 *                      bit15  bit14  bit13  bit12  bit3   bit2   bit1   bit0
 *             MSB      S      S      S      S      S      2^6    2^5    2^4
 *
 *             The temperature data is stored as a 16-bit sign-extended twos
 *             complement number in the temperature register. The sign bits (S)
 *             indicate if the temperature is positive or negative: for
 *             positive numbers S = 0 and for negative numbers S = 1.
 *
 * RETURN:     int16_t         raw temperature value with a resolution
 *                            of 1/256°C
 ******************************************************************************/
int16_t DS1820_GetTempRaw(uint8_t *address)
{
  uint8_t i;
  uint16_t temp_u16;
  uint16_t highres_u16;
  uint8_t scrpad[DS1820_SCRPADMEM_LEN];

  /* --- start temperature conversion -------------------------------------- */
  DS1820_Reset();
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, address);     /* address the device */
  output_temp_sensors(1);
  DS1820_WriteByte(DS1820_CMD_CONVERTTEMP);   /* start conversion */
  DelayMs(750);

  /* --- read sratchpad ---------------------------------------------------- */
  DS1820_Reset();
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, address);   /* address the device */
  DS1820_WriteByte(DS1820_CMD_READSCRPAD);  /* read scratch pad */

  /* read scratch pad data */
  for (i=0; i < DS1820_SCRPADMEM_LEN; i++) {
    scrpad[i] = DS1820_ReadByte();
  }


  /* --- calculate temperature --------------------------------------------- */
  /* Formular for temperature calculation: */
  /* Temp = Temp_read - 0.25 + ((Count_per_C - Count_Remain)/Count_per_C) */

  /* get raw value of temperature (0.5°C resolution) */
  temp_u16 = 0;
  temp_u16 = (uint16_t)((uint16_t)scrpad[DS1820_REG_TEMPMSB] << 8);
  temp_u16 |= (uint16_t)(scrpad[DS1820_REG_TEMPLSB]);

  if (nRomAddr[0] == DS1820_FAMILY_CODE_DS18S20) {
    /* get temperature value in 1°C resolution */
    temp_u16 >>= 1;

    /* temperature resolution is TEMP_RES (0x100), so 1°C equals 0x100 */
    /* => convert to temperature to 1/256°C resolution */
    temp_u16 = ((uint16_t)temp_u16 << 8);

    /* now substract 0.25°C */
    temp_u16 -= ((uint16_t)TEMP_RES >> 2);

    /* now calculate high resolution */
    highres_u16 = scrpad[DS1820_REG_CNTPERSEC] - scrpad[DS1820_REG_CNTREMAIN];
    highres_u16 = ((uint16_t)highres_u16 << 8);
    if (scrpad[DS1820_REG_CNTPERSEC]) {
      highres_u16 = highres_u16 / (uint16_t)scrpad[DS1820_REG_CNTPERSEC];
    }
    /* now calculate result */
    highres_u16 = highres_u16 + temp_u16;
  } else {
    /* 12 bit temperature value has 0.0625°C resolution */
    /* shift left by 4 to get 1/256°C resolution */
    highres_u16 = temp_u16;
    highres_u16 <<= 4;
  }
  return (highres_u16);
}


/*******************************************************************************
 * FUNCTION:   DS1820_GetTempFloat
 * PURPOSE:    Converts internal temperature value to string (physical value).
 *
 * INPUT:      none
 * OUTPUT:     none
 * RETURN:     float          temperature value with as float value
 ******************************************************************************/
float DS1820_GetTempFloat(uint8_t *address)
{
    return ((float)DS1820_GetTempRaw(address) / (float)TEMP_RES);
}


/*******************************************************************************
 * FUNCTION:   DS1820_GetTempString
 * PURPOSE:    Converts internal temperature value to string (physical value).
 *
 * INPUT:      tRaw_s16       internal temperature value
 * OUTPUT:     strTemp_pc     user string buffer to write temperature value
 * RETURN:     int16_t         temperature value with an internal resolution
 *                            of TEMP_RES
 ******************************************************************************/
void DS1820_GetTempString(int16_t tRaw_s16, char *strTemp_pc)
{
    int16_t tPhyLow_s16;
    int8_t tPhy_s8;

    /* convert from raw value (1/256°C resolution) to physical value */
    tPhy_s8 = (int8_t)(tRaw_s16/TEMP_RES);

    /* convert digits from raw value (1/256°C resolution) to physical value */
    /*tPhyLow_u16 = tInt_s16 % TEMP_RES;*/
    tPhyLow_s16 = tRaw_s16 & 0xFF;      /* this operation is the same as */
                                        /* but saves flash memory tInt_s16 % TEMP_RES */
    tPhyLow_s16 = tPhyLow_s16 * 100;
    tPhyLow_s16 = (uint16_t)tPhyLow_s16 / TEMP_RES;

    /* write physical temperature value to string */
    sprintf(strTemp_pc, "%d.%02d", tPhy_s8, (int8_t)tPhyLow_s16);
}
