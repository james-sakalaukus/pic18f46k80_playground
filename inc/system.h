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

#define BAUDRATE    57600L
#define USE_HI_SPEED_BRG            //Use BRGH=1, UART high speed mode

//UART Baud Rate Calculation *******************************************************
#ifdef USE_HI_SPEED_BRG
    #define BRG_DIV 4L
#else
    #define BRG_DIV 16L
#endif

#define BAUDRATEREG    ( ((FCYC + (BRG_DIV * BAUDRATE / 2L)) / (BRG_DIV * BAUDRATE)) - 1L)
#define BAUD_ACTUAL    (FCYC / BRG_DIV / (BAUDRATEREG+1))

#define BAUD_ERROR          ((BAUD_ACTUAL > BAUDRATE) ? BAUD_ACTUAL-BAUDRATE : BAUDRATE-BAUD_ACTUAL)
#define BAUD_ERROR_PRECENT  ((BAUD_ERROR * 100L + (BAUDRATE / 2L)) / BAUDRATE)

#if (BAUD_ERROR_PRECENT > 3)
    #error "UART frequency error is worse than 3%"
#elif (BAUD_ERROR_PRECENT > 2)
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
