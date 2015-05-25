/*
 * oneWire.c
 *
 *  Created on: Apr 11, 2015
 *      Author: james
 */

#include <user.h>
#include <pic18f46k80.h>
#include <oneWire_maxim.h>
#include <stdint.h>
#include <stdbool.h>

#define PORT_WRITE LATBbits.LATB7
#define PORT_TRIS
#define PORT_READ


// send 'databyte' to 'port'
void outp(unsigned port, uint8_t databyte) {
  TRISBbits.TRISB7 = 0;
  PORT_WRITE = databyte;
}

// read byte from 'port'
uint8_t inp(unsigned port) {
  TRISBbits.TRISB7 = 1;
  return PORTBbits.PORTB7;
}


//-----------------------------------------------------------------------------
// Generate a 1-Wire reset, return 1 if no presence detect was found,
// return 0 otherwise.
uint8_t OWReset(void) {
  uint8_t result;

  tickDelay(DELAY_G);
  outp(0x00); // Drives DQ low
  tickDelay(DELAY_H);
  outp(0x01); // Releases the bus
  tickDelay(DELAY_I);
  result = inp() ^ 0x01; // Sample for presence pulse from slave
  tickDelay(DELAY_J); // Complete the reset sequence recovery
  return result; // Return sample presence pulse result
}

//-----------------------------------------------------------------------------
// Send a 1-Wire write bit. Provide 10us recovery time.
//
void OWWriteBit(uint8_t bit) {
  if (bit) {
    // Write '1' bit
    outp(0x00); // Drives DQ low
    tickDelay(DELAY_A);
    outp(0x01); // Releases the bus
    tickDelay(DELAY_B); // Complete the time slot and 10us recovery
  } else {
    // Write '0' bit
    outp(0x00); // Drives DQ low
    tickDelay(DELAY_C);
    outp(0x01); // Releases the bus
    tickDelay(DELAY_D);
  }
}

//-----------------------------------------------------------------------------
// Read a bit from the 1-Wire bus and return it. Provide 10us recovery time.
//
uint8_t OWReadBit(void) {
  uint8_t result;

  outp(0x00); // Drives DQ low
  tickDelay(DELAY_A);
  outp(0x01); // Releases the bus
  tickDelay(DELAY_E);
  result = inp() & 0x01; // Sample the bit value from the slave
  tickDelay(DELAY_F); // Complete the time slot and 10us recovery

  return result;
}

//-----------------------------------------------------------------------------
// Write 1-Wire data byte
//
void OWWriteByte(uint8_t data) {
  uint8_t loop;

  // Loop to write each bit in the byte, LS-bit first
  for (loop = 0; loop < 8; loop++) {
    OWWriteBit(data & 0x01);

    // shift the data byte for the next bit
    data >>= 1;
  }
}

//-----------------------------------------------------------------------------
// Read 1-Wire data byte and return it
//
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


//--------------------------------------------------------------------------
// Find the 'first' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : no device present
//
uint8_t OWFirst() {
  // reset the search state
  LastDiscrepancy = 0;
  LastDeviceFlag = FALSE;
  LastFamilyDiscrepancy = 0;

  return OWSearch();
}

//--------------------------------------------------------------------------
// Find the 'next' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
uint8_t OWNext() {
  // leave the search state alone
  return OWSearch();
}

//--------------------------------------------------------------------------
// Perform the 1-Wire Search Algorithm on the 1-Wire bus using the existing
// search state.
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
uint8_t OWSearch() {
  uint8_t id_bit_number;
  uint8_t last_zero, rom_byte_number, search_result;
  uint8_t id_bit, cmp_id_bit;
  uint8_t rom_byte_mask, search_direction;

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
    if (!OWReset()) {
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

//--------------------------------------------------------------------------
// Verify the device with the ROM number in ROM_NO buffer is present.
// Return TRUE  : device verified present
//        FALSE : device not present
//
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


//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current
// global 'crc8' value.
// Returns current global crc8 value
//
uint8_t docrc8(uint8_t value) {
  // See Application Note 27

  // TEST BUILD
  crc8 = dscrc_table[crc8 ^ value];
  return crc8;
}

//--------------------------------------------------------------------------
// TEST BUILD MAIN
//
int main(short argc, char **argv) {

  uint8_t returnValue = 0;
  uint8_t i = 0;
  uint8_t sensorCount = 0;

  // find ALL devices
  printf("\nFIND ALL\n");
  returnValue = OWFirst();
  while (returnValue) {

    // print device found
    for (i = 7; i >= 0; i--) {
      printf("%02X", ROM_NO[i]);
    }

    printf("  %d\n", ++sensorCount);
    returnValue = OWNext();
  }
}
