/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
#ifndef SYSTEM_H_INCLUDE
#define SYSTEM_H_INCLUDE

/* Microcontroller MIPs (FCY) */
#define SYS_FREQ        64000000L
#define FCY             SYS_FREQ/4

#define FOSC (64000000L)
#define FCYC (FOSC/4L)
#define _XTAL_FREQ FOSC

#define BAUDRATE1    115200L
//#define BAUDRATE2    57600L
#define BAUDRATE2    115200L
#define USE_HI_SPEED_BRG            //Use BRGH=1, UART high speed mode

//UART Baud Rate Calculation *******************************************************
#ifdef USE_HI_SPEED_BRG
    #define BRG_DIV 4L
#else
    #define BRG_DIV 16L
#endif

#define BAUDRATEREG1    ( ((FCYC + (BRG_DIV * BAUDRATE1 / 2L)) / (BRG_DIV * BAUDRATE1)) - 1L)
#define BAUD_ACTUAL1    (FCYC / BRG_DIV / (BAUDRATEREG1+1))

#define BAUD_ERROR1          ((BAUD_ACTUAL1 > BAUDRATE1) ? BAUD_ACTUAL1-BAUDRATE1 : BAUDRATE1-BAUD_ACTUAL1)
#define BAUD_ERROR_PRECENT1  ((BAUD_ERROR1 * 100L + (BAUDRATE1 / 2L)) / BAUDRATE1)

#if (BAUD_ERROR_PRECENT1 > 3)
    #error "UART frequency error is worse than 3%"
#elif (BAUD_ERROR_PRECENT1 > 2)
    #warning "UART frequency error is worse than 2%"
#endif

#define BAUDRATEREG2    ( ((FCYC + (BRG_DIV * BAUDRATE2 / 2L)) / (BRG_DIV * BAUDRATE2)) - 1L)
#define BAUD_ACTUAL2    (FCYC / BRG_DIV / (BAUDRATEREG2+1))

#define BAUD_ERROR2          ((BAUD_ACTUAL2 > BAUDRATE2) ? BAUD_ACTUAL2-BAUDRATE2 : BAUDRATE2-BAUD_ACTUAL2)
#define BAUD_ERROR_PRECENT2  ((BAUD_ERROR2 * 100L + (BAUDRATE2 / 2L)) / BAUDRATE2)

#if (BAUD_ERROR_PRECENT2 > 3)
    #error "UART frequency error is worse than 3%"
#elif (BAUD_ERROR_PRECENT2 > 2)
    #warning "UART frequency error is worse than 2%"
#endif
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */
#endif
