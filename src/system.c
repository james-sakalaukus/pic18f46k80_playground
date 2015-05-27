/******************************************************************************/
/*Files to Include                                                            */
/******************************************************************************/
#include <xc.h>         /* XC8 General Include File */

#include <stdint.h>         /* For uint8_t definition */
#include <stdbool.h>        /* For true/false definition */

#include "system.h"

/* Refer to the device datasheet for information about available
oscillator configurations. */

/* NOTE: Microchip debugger hangs on HFIOFS */

void ConfigureOscillator(void)
{
    // idle on sleep
    // 16MHz internal oscillator
    // Use default oscillator defined in CONFIG1H (HF w/ PLL)
    OSCCON = 0b11110000;    // 0xF2
    
#ifndef __DEBUG
    while(!HFIOFS);
#endif
    // LF clock comes from HF clock
    // Tune RC Oscillator to calibrated frequency
    // PLL enable to 64MHz
    OSCTUNE = 0b11000000;
    // NO PLL
//    OSCTUNE = 0b10000000;
#ifndef __DEBUG
    while(!HFIOFS);
#endif

}
