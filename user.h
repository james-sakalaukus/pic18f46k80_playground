/******************************************************************************/
/* User Level #define Macros                                                  */
/******************************************************************************/
#define heartbeat LATDbits.LATD2
#define DS1820_DATAPIN  LATCbits.LATC5 

#define uint8 uint8_t
#define sint16 int16_t
#define uint16 uint16_t
#define sint8 int8_t

#define output_low(name) name = 0;
#define output_high(name) name = 1;
#define input(name) name
/******************************************************************************/
/* User Function Prototypes                                                   */
/******************************************************************************/
void putch(char data);
void InitApp(void);         /* I/O and Peripheral Initialization */
void delay();
void doHeartBeat();

char characterReceived;

// 6 16-bit temperature values
uint8_t temperature[12];