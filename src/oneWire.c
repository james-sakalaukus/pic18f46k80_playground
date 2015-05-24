/*
 * oneWire.c
 *
 *  Created on: Apr 19, 2015
 *      Author: james
 */

#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "user.h"
#include "oneWire.h"
#include <stdbool.h>        /* For true/false definition */



/*******************************************************************************
 * FUNCTION:   DS1820_Reset
 * PURPOSE:    Initializes the DS1820 device.
 *
 * INPUT:      -
 * OUTPUT:     -
 * RETURN:     FALSE if at least one device is on the 1-wire bus, TRUE otherwise
 ******************************************************************************/
uint8_t DS1820_Reset(uint8_t busNum)
{
  bool bPresPulse;

  // reset pulse
  output_temp_sensors(0, busNum);

  DelayUs(DS1820_RST_PULSE);
  output_temp_sensors(1, busNum);

  // wait until pullup pull 1-wire bus to high
  DelayUs(DS1820_PRESENCE_WAIT);

  // get presence pulse
  bPresPulse = input_temp_senosrs(busNum);

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
bool DS1820_ReadBit(char busNum)
{
  output_temp_sensors(0, busNum);
  DelayUs(DS1820_MSTR_BITSTART);
  input_temp_senosrs(busNum);
  DelayUs(DS1820_BITREAD_DLY);

  return input_temp_senosrs(busNum);
}


/*******************************************************************************
 * FUNCTION:   DS1820_WriteBit
 * PURPOSE:    Writes a single bit to the DS1820 device.
 *
 * INPUT:      bBit        value of bit to be written
 * OUTPUT:     -
 * RETURN:     -
 ******************************************************************************/
void DS1820_WriteBit(bool bBit, uint8_t busNum)
{
  output_temp_sensors(0, busNum);
  DelayUs(DS1820_MSTR_BITSTART);

  if (bBit != FALSE) {
    output_temp_sensors(1, busNum);
  }

  DelayUs(DS1820_BITWRITE_DLY);
  output_temp_sensors(1, busNum);
}


/*******************************************************************************
 * FUNCTION:   DS1820_ReadByte
 * PURPOSE:    Reads a single byte from the DS1820 device.
 *
 * INPUT:      -
 * OUTPUT:     -
 * RETURN:     uint8_t          byte which has been read from the DS1820
 ******************************************************************************/
uint8_t DS1820_ReadByte(uint8_t busNum)
{
  uint8_t i;
  uint8_t value = 0;

  for (i=0 ; i < 8; i++) {
    if(DS1820_ReadBit(busNum)) {
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
void DS1820_WriteByte(uint8_t val_u8, uint8_t busNum)
{
  uint8_t i;
  uint8_t temp;

  /* writes byte, one bit at a time */
  for (i=0; i < 8; i++) {
    temp = val_u8 >> i;     /* shifts val right 'i' spaces */
    temp &= 0x01;           /* copy that bit to temp */
    DS1820_WriteBit(temp, busNum);  /* write bit in temp into */
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
void DS1820_AddrDevice(uint8_t nAddrMethod, uint8_t busNum)
{
  uint8_t i;

  // address single devices on bus
  if (nAddrMethod == DS1820_CMD_MATCHROM) {
    DS1820_WriteByte(DS1820_CMD_MATCHROM, busNum);
    for (i = 0; i < DS1820_ADDR_LEN; i ++) {
       DS1820_WriteByte(nRomAddr_au8[busNum][i], busNum);
    }

  // address all devices on bus
  } else {
    DS1820_WriteByte(DS1820_CMD_SKIPROM, busNum);
  }
}


/*******************************************************************************
 * FUNCTION:   DS1820_FindNextDevice
 * PURPOSE:    Finds next device connected to the 1-wire bus.
 *
 * INPUT:      -
 * OUTPUT:     nRomAddr_au8[]       ROM code of the next device
 * RETURN:     uint8_t                 TRUE if there are more devices on the 1-wire
 *                                  bus, FALSE otherwise
 ******************************************************************************/
bool DS1820_FindNextDevice(uint8_t busNum)
{
//  printf("DS1820_FindNextDevice(): bus number: %d\r\n", busNum);
  uint8_t state_u8;
  uint8_t byteidx_u8;
  uint8_t mask_u8 = 1;
  uint8_t bitpos_u8 = 1;
  uint8_t nDiscrepancyMarker_u8 = 0;
  bool bit_b;
  bool bStatus;
  bool next_b = FALSE;

  // reset the ROM addresses to 0
  for (byteidx_u8=0; byteidx_u8 < 8; byteidx_u8 ++) {
      nRomAddr_au8[busNum][byteidx_u8] = 0x00;
  }

  // reset 1-wire bus
  bStatus = DS1820_Reset(busNum);

  // no device found, reset the search
  if (bStatus || bDoneFlag[busNum]) {
    nLastDiscrepancy_u8[busNum] = 0;
//    printf("DS1820_FindNextDevice(): bus number: %d; no device found!\r\n", busNum);
    return FALSE;
  }

  // send search rom command
  DS1820_WriteByte(DS1820_CMD_SEARCHROM, busNum);

  byteidx_u8 = 0;

  do {
    state_u8 = 0;

    // read bit
    if ( DS1820_ReadBit(busNum) != 0 ) {
      state_u8 = 2;
    }
    DelayUs(120);

    // read bit complement
    if ( DS1820_ReadBit(busNum) != 0 ) {
      state_u8 |= 1;
    }
    DelayUs(120);

    /* description for values of state_u8: */
    /* 00    There are devices connected to the bus which have conflicting */
    /*       bits in the current ROM code bit position. */
    /* 01    All devices connected to the bus have a 0 in this bit position. */
    /* 10    All devices connected to the bus have a 1 in this bit position. */
    /* 11    There are no devices connected to the 1-wire bus. */

    // if there are no devices on the bus
    if (state_u8 == 3) {
      break;
    } else {

      // devices have the same logical value at this position
      if (state_u8 > 0) {
        // get bit value
        bit_b = (bool)(state_u8 >> 1);

      // devices have confilcting bits in the current ROM code
      } else {
        // if there was a conflict on the last iteration
        if (bitpos_u8 < nLastDiscrepancy_u8[busNum]) {
          // take same bit as in last iteration
          bit_b = ( (nRomAddr_au8[busNum][byteidx_u8] & mask_u8) > 0 );

        } else {
          bit_b = (bitpos_u8 == nLastDiscrepancy_u8[busNum]);
        }

        if (bit_b == 0) {
          nDiscrepancyMarker_u8 = bitpos_u8;
        }
      }

      /* store bit in ROM address */
      if (bit_b != 0) {
         nRomAddr_au8[busNum][byteidx_u8] |= mask_u8;
      } else {
         nRomAddr_au8[busNum][byteidx_u8] &= ~mask_u8;
      }

      DS1820_WriteBit(bit_b, busNum);

      // increment bit position
      bitpos_u8 ++;

      // calculate next mask value
      mask_u8 = mask_u8 << 1;

      // check if this byte has finished
      if (mask_u8 == 0) {
         byteidx_u8 ++;  /* advance to next byte of ROM mask */
         mask_u8 = 1;    /* update mask */
      }
    }
  } while (byteidx_u8 < DS1820_ADDR_LEN);


  // if search was unsuccessful then, reset the last discrepancy to 0
  if (bitpos_u8 < 65) {
    nLastDiscrepancy_u8[busNum] = 0;

  // search was successful
  } else {

    nLastDiscrepancy_u8[busNum] = nDiscrepancyMarker_u8;
    bDoneFlag[busNum] = (nLastDiscrepancy_u8[busNum] == 0);

    for(int i=0; i< DS1820_ADDR_LEN; i++) {
      if(nRomAddr_au8[busNum][i] != 0) {
        next_b = TRUE;
        break;
      }
    }
    if(next_b) {
      char address[DS1820_ADDR_LEN];
      for(int i=0; i< DS1820_ADDR_LEN; i++) {
        sprintf(&address[i], "%X", nRomAddr_au8[busNum][i]);
      }
//      printf("DS1820_FindNextDevice(): Search successful bus: %d, address is: %s\r\n", busNum, address);
    } else {
      printf("DS1820_FindNextDevice(): Search NOT successful bus: %d, addres was 0's\r\n", busNum);
    }

//    /* indicates search is not complete yet, more parts remain */
//    next_b = TRUE;

  }

    return next_b;
}


/*******************************************************************************
 * FUNCTION:   DS1820_FindFirstDevice
 * PURPOSE:    Starts the device search on the 1-wire bus.
 *
 * INPUT:      -
 * OUTPUT:     nRomAddr_au8[]       ROM code of the first device
 * RETURN:     uint8_t                 TRUE if there are more devices on the 1-wire
 *                                  bus, FALSE otherwise
 ******************************************************************************/
bool DS1820_FindFirstDevice(uint8_t busNum)
{
//  printf("DS1820_FindFirstDevice(): bus number: %d\r\n", busNum);
  nLastDiscrepancy_u8[busNum] = 0;
  bDoneFlag[busNum] = FALSE;

  return (DS1820_FindNextDevice(busNum));
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
void DS1820_WriteEEPROM(uint8_t nTHigh, uint8_t nTLow, uint8_t busNum)
{
  /* --- write to scratchpad ----------------------------------------------- */
  DS1820_Reset(busNum);
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, busNum);
  DS1820_WriteByte(DS1820_CMD_WRITESCRPAD, busNum); /* start conversion */
  DS1820_WriteByte(nTHigh, busNum);
  DS1820_WriteByte(nTLow, busNum);

  DelayUs(10);

  DS1820_Reset(busNum);
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, busNum);
  DS1820_WriteByte(DS1820_CMD_COPYSCRPAD, busNum); /* start conversion */

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
int16_t DS1820_GetTempRaw(uint8_t busNum)
{
  uint8_t i;
  uint16_t temp_u16;
  uint16_t highres_u16;
  uint8_t scrpad[DS1820_SCRPADMEM_LEN];

  /* --- start temperature conversion -------------------------------------- */
  DS1820_Reset(busNum);
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, busNum);     /* address the device */
  output_temp_sensors(1, busNum);
  DS1820_WriteByte(DS1820_CMD_CONVERTTEMP, busNum);   /* start conversion */
  DelayMs(750);

  /* --- read sratchpad ---------------------------------------------------- */
  DS1820_Reset(busNum);
  DS1820_AddrDevice(DS1820_CMD_MATCHROM, busNum);   /* address the device */
  DS1820_WriteByte(DS1820_CMD_READSCRPAD, busNum);  /* read scratch pad */

  /* read scratch pad data */
  for (i=0; i < DS1820_SCRPADMEM_LEN; i++) {
    scrpad[i] = DS1820_ReadByte(busNum);
  }


  /* --- calculate temperature --------------------------------------------- */
  /* Formular for temperature calculation: */
  /* Temp = Temp_read - 0.25 + ((Count_per_C - Count_Remain)/Count_per_C) */

  /* get raw value of temperature (0.5°C resolution) */
  temp_u16 = 0;
  temp_u16 = (uint16_t)((uint16_t)scrpad[DS1820_REG_TEMPMSB] << 8);
  temp_u16 |= (uint16_t)(scrpad[DS1820_REG_TEMPLSB]);

  if (nRomAddr_au8[busNum][0] == DS1820_FAMILY_CODE_DS18S20) {
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
float DS1820_GetTempFloat(uint8_t busNum)
{
    return ((float)DS1820_GetTempRaw(busNum) / (float)TEMP_RES);
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
