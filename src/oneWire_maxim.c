/*
 * oneWire.c
 *
 *  Created on: Apr 11, 2015
 *      Author: james
 */

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#include "oneWire_maxim.h"
//#include "user.h"


/*******************************************************************************
* Generate a 1-Wire reset
*
* return 0 if no presence detect was found
* return 1 otherwise
*
*******************************************************************************/
uint8_t OWReset(void) {
  uint8_t result;
  output_temp_sensors(0x00);      // Drives DQ low
  DelayUs(DELAY_H);
  output_temp_sensors(0x01);      // Releases the bus
  DelayUs(DELAY_I);
  result = input_temp_sensors();  // Sample for presence pulse from slave
  DelayUs(DELAY_J);               // Complete the reset sequence recovery
  return result;                  // Return sample presence pulse result
}

/*******************************************************************************
* Send a 1-Wire write bit. Provide 10us recovery time.
*******************************************************************************/
void OWWriteBit(uint8_t data) {

  // Write '1' bit
  if (data) {
    output_temp_sensors(0x00);  // Drives DQ low
    DelayUs(DELAY_A);
    output_temp_sensors(0x01);  // Releases the bus
    DelayUs(DELAY_B);           // Complete the time slot and 10us recovery

  // Write '0' bit
  } else {
    output_temp_sensors(0x00);  // Drives DQ low
    DelayUs(DELAY_C);
    output_temp_sensors(0x01);  // Releases the bus
    DelayUs(DELAY_D);
  }
}

/*******************************************************************************
* Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
*******************************************************************************/
uint8_t OWReadBit(void) {
  uint8_t result;
  output_temp_sensors(0x00);      // Drives DQ low
  DelayUs(DELAY_A);
  output_temp_sensors(0x01);      // Releases the bus
  DelayUs(DELAY_E);
  result = input_temp_sensors();  // Sample the bit value from the slave
  DelayUs(DELAY_F);               // Complete the time slot and 10us recovery
  return result;
}
/*******************************************************************************
* Write 1-Wire data byte
*******************************************************************************/
void OWWriteByte(uint8_t data) {
  uint8_t loop;

  // Loop to write each bit in the byte, LS-bit first
  for (loop = 0; loop < 8; loop++) {
    OWWriteBit(data & 0x01);

    // shift the data byte for the next bit
    data >>= 1;
  }
}
/*******************************************************************************
* Read 1-Wire data byte and return it
*******************************************************************************/
uint8_t OWReadByte(void) {
  uint8_t loop, result = 0;

  for (loop = 0; loop < 8; loop++) {
    // shift the result to get it ready for the next bit
    result >>= 1;

    // if result is one, then set MS bit
    if (OWReadBit())
      result |= 0x80;
  }
  return result;
}

/*******************************************************************************
* Find the 'first' devices on the 1-Wire bus
*
* Return TRUE if device found; ROM number in ROM_NO buffer
*******************************************************************************/
uint8_t OWFirst() {
  // reset the search state
  LastDiscrepancy = 0;
  LastDeviceFlag = FALSE;
  LastFamilyDiscrepancy = 0;

  return OWSearch();
}
/*******************************************************************************
* Find the 'next' devices on the 1-Wire bus
*
* Return TRUE if device found; ROM number in ROM_NO buffer
*******************************************************************************/
uint8_t OWNext() {
  // leave the search state alone
  return OWSearch();
}

/*******************************************************************************
* Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
*
* Return TRUE if device found; ROM number in ROM_NO buffer
*******************************************************************************/
uint8_t OWSearch() {
  uint8_t id_bit_number;
  uint8_t last_zero;
  uint8_t rom_byte_number;
  uint8_t search_result;
  uint8_t id_bit;
  uint8_t cmp_id_bit;
  uint8_t rom_byte_mask;
  uint8_t search_direction;

  // initialize for search
  id_bit_number = 1;
  last_zero = 0;
  rom_byte_number = 0;
  rom_byte_mask = 1;
  search_result = 0;
  crc8 = 0;

  // if the last call was not the last one
  if (!LastDeviceFlag) {
    // 1-Wire reset
    if (OWReset()) {
      // reset the search
      LastDiscrepancy = 0;
      LastDeviceFlag = FALSE;
      LastFamilyDiscrepancy = 0;
      return FALSE;
    }

    // issue the search command
    OWWriteByte(0xF0);

    // loop to do the search
    do {
      // read a bit and its complement
      id_bit = OWReadBit();
      cmp_id_bit = OWReadBit();

      // check for no devices on 1-wire
      if ((id_bit == 1) && (cmp_id_bit == 1))
        break;
      else {
        // all devices coupled have 0 or 1
        if (id_bit != cmp_id_bit)
          search_direction = id_bit;  // bit write value for search
        else {
          // if this discrepancy if before the Last Discrepancy
          // on a previous next then pick the same as last time
          if (id_bit_number < LastDiscrepancy)
            search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
          else
            // if equal to last pick 1, if not then pick 0
            search_direction = (id_bit_number == LastDiscrepancy);

          // if 0 was picked then record its position in LastZero
          if (search_direction == 0) {
            last_zero = id_bit_number;

            // check for Last discrepancy in family
            if (last_zero < 9)
              LastFamilyDiscrepancy = last_zero;
          }
        }

        // set or clear the bit in the ROM byte rom_byte_number
        // with mask rom_byte_mask
        if (search_direction == 1)
          ROM_NO[rom_byte_number] |= rom_byte_mask;
        else
          ROM_NO[rom_byte_number] &= ~rom_byte_mask;

        // serial number search direction write bit
        OWWriteBit(search_direction);

        // increment the byte counter id_bit_number
        // and shift the mask rom_byte_mask
        id_bit_number++;
        rom_byte_mask <<= 1;

        // if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
        if (rom_byte_mask == 0) {
          docrc8(ROM_NO[rom_byte_number]);  // accumulate the CRC
          rom_byte_number++;
          rom_byte_mask = 1;
        }
      }
    } while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

    // if the search was successful then
    if (!((id_bit_number < 65) || (crc8 != 0))) {
      // search successful so set LastDiscrepancy,LastDeviceFlag,search_result
      LastDiscrepancy = last_zero;

      // check for last device
      if (LastDiscrepancy == 0)
        LastDeviceFlag = TRUE;

      search_result = TRUE;
    }
  }

  // if no device found then reset counters so next 'search' will be like a first
  if (!search_result || !ROM_NO[0]) {
    LastDiscrepancy = 0;
    LastDeviceFlag = FALSE;
    LastFamilyDiscrepancy = 0;
    search_result = FALSE;
  }

  return search_result;
}

/*******************************************************************************
* Verify the device with the ROM number in ROM_NO buffer is present.
*
* Return TRUE if device verified present
*******************************************************************************/
uint8_t OWVerify() {
  uint8_t rom_backup[8];
  uint8_t i, returnValue, ld_backup, ldf_backup, lfd_backup;

  // keep a backup copy of the current state
  for (i = 0; i < 8; i++)
    rom_backup[i] = ROM_NO[i];
  ld_backup = LastDiscrepancy;
  ldf_backup = LastDeviceFlag;
  lfd_backup = LastFamilyDiscrepancy;

  // set search to find the same device
  LastDiscrepancy = 64;
  LastDeviceFlag = FALSE;

  if (OWSearch()) {
    // check if same device found
    returnValue = TRUE;
    for (i = 0; i < 8; i++) {
      if (rom_backup[i] != ROM_NO[i]) {
        returnValue = FALSE;
        break;
      }
    }
  } else
    returnValue = FALSE;

  // restore the search state
  for (i = 0; i < 8; i++)
    ROM_NO[i] = rom_backup[i];
  LastDiscrepancy = ld_backup;
  LastDeviceFlag = ldf_backup;
  LastFamilyDiscrepancy = lfd_backup;

  // return the result of the verify
  return returnValue;
}

/*******************************************************************************
* Calculate the CRC8 of the byte value provided with the current
*  global 'crc8' value.
*  Returns current global crc8 value
*******************************************************************************/
uint8_t docrc8(uint8_t value) {
  crc8 = dscrc_table[crc8 ^ value];
  return crc8;
}


/*******************************************************************************
* Address all DS1820 devices on bus, send start temp conversion command
*******************************************************************************/
void startTempConversion() {
  OWReset();
  OWWriteByte(DS1820_CMD_SKIPROM);
  output_temp_sensors(1);
  OWWriteByte(DS1820_CMD_CONVERTTEMP);
  DelayMs(750);
}
/*******************************************************************************
 * Get temperature raw value from single DS1820 device.
 ******************************************************************************/
float ReadTemp(uint8_t *address)
{
  uint8_t i;
  uint16_t temp = 0;
  float highres;
  int8_t scratchPad[DS1820_SCRPADMEM_LEN];

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
  if(scratchPad[DS1820_REG_TEMPMSB]) {
    highres = (float)scratchPad[DS1820_REG_TEMPLSB] * - 0.5;
  } else {
    highres = (float)scratchPad[DS1820_REG_TEMPLSB] * 0.5;
  }
  printf("temp C: %2.2f \r\n", highres);

  temp = 0;
  temp = (uint16_t)((uint16_t)scratchPad[DS1820_REG_TEMPMSB] << 8);
  temp |= (uint16_t)(scratchPad[DS1820_REG_TEMPLSB]);
  printf("other_temp C: %d ", (int16_t)temp*0.5);

  if (address[0] == DS1820_FAMILY_CODE_DS18S20) {
    printf("DS1820_GetTempRaw(): DS1820 Sensor Type\r\n");

    /* Temp = Temp_read - 0.25 + ((Count_per_C - Count_Remain)/Count_per_C)
     * Count_per_C = 16
     * temp = temp_read - 0.25 + ((16-count_remain)*0.0625)
    */
    highres = temp - 0.25 + ((16.0 - scratchPad[DS1820_REG_CNTREMAIN])*0.0625);

  } else {
    printf("Not DS1820_GetTempRaw(): DS1820 Sensor Type\r\n");
    // 12 bit temperature value has 0.0625°C resolution
    highres = temp*0.0625;
  }

  return (highres);
}
