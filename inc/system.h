/******************************************************************************/
/* System Level #define Macros                                                */
/******************************************************************************/
#ifndef SYSTEM_H_INCLUDE
#define SYSTEM_H_INCLUDE

// Microcontroller MIPs (FCY)
#define SYS_FREQ        64000000L
#define FCY             SYS_FREQ/4

#define FOSC (SYS_FREQ)
#define FCYC (FOSC/4L)
#define _XTAL_FREQ FOSC

// 2 UARTS on Deivce
#define BAUDRATE1    115200L
#define BAUDRATE2    115200L

// Use BRGH=1, UART high speed mode
#define USE_HI_SPEED_BRG

/*############################################################*/
/*          Configuration for device =  'PIC18F46K80'         */
/*############################################################*/

/* ADC */
#define ADC_V13_3

/* ECC */
#define ECC_V8_2

/* CC */
#define CC_V8_2

/* EPWM */
#define EPWM_V14_2

/* PWM */
#define PWM_V14_2

/* PCPWM */
/*No configuration chosen for this peripheral*/

/* USART */
#define EAUSART_V11

/* SPI */
#define SPI_V1

/* I2C */
#define I2C_V1

/* TIMERS */
#define TMR_V7_2

/* EEPROM */
#define EEP_V3

/* PORT_B */
#define PTB_V2

/* ANCOMP */
#define ANCOM_V10_2

/* MWIRE */
#define MWIRE_V1

/* CTMU */
#define CTMU_V1_1

/* PPS */
/*No configuration chosen for this peripheral*/

/* RTCC */
/*No configuration chosen for this peripheral*/

/* DPSLP */
#define DPSLP_V1_2

/* PMP */
/*No configuration chosen for this peripheral*/

/* FLASH */
#define FLASH_V1_4
/******************************************************************************/
/* System Function Prototypes                                                 */
/******************************************************************************/

/* Custom oscillator configuration funtions, reset source evaluation
functions, and other non-peripheral microcontroller initialization functions
go here. */

void ConfigureOscillator(void); /* Handles clock switching/osc initialization */


/******************************************************************************/
/* UART Baud Rate Calculation                                                 */
/******************************************************************************/
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

#endif
