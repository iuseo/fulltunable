#ifndef _MAIN_H_
#define _MAIN_H_

//efine TTOSA_CHANNEL_MAX_98			1// channel 98 define
#define ON								1
#define OFF								0

#define NULL_PTR 						((void*)0)

typedef unsigned char					uint8; 
typedef signed char						sint8; 
typedef unsigned short					uint16; 
typedef signed short					sint16; 
typedef unsigned long					uint32;
typedef signed long						sint32;     

typedef enum _bool 
{ 
	false = 0,
	true = 1 
} bool;

typedef volatile struct 
{
	unsigned int bit0	: 1;
	unsigned int bit1	: 1;
	unsigned int bit2	: 1;
	unsigned int bit3	: 1;
	unsigned int bit4	: 1; 
	unsigned int bit5	: 1;
	unsigned int bit6	: 1;
} STATUS_FLAG_T;
extern STATUS_FLAG_T statusFlag;
#define messageReadyFlag				statusFlag.bit0
#define cdrSyncFlag						statusFlag.bit1
#define tecTableFlag					statusFlag.bit2
#define apdTableFlag					statusFlag.bit3
#define lutReadyFlag					statusFlag.bit4
#define i2cSlaveReadFlag				statusFlag.bit5
#define i2cSlaveWriteFlag				statusFlag.bit6
//#define lasterStartFlag				statusFlag.bit7

#define I2C_SLAVE_READ_FLAG_COUNT_MAX	4		/* 4 * 25ms = 100ms, TIMER_25ms */

extern uint8 lasterStartFlag;
#define LASER_START_NOT_READY           1
#define LASER_START_READY               0
#define WAVELENGTH_LOCKING_EQUATION     2
#define	AVGCNT_INDEX		        	10

#define TX_IN_EQ						0x00
#define RX_OUT_EMPH						0x22

#define RS0                             0
#define RS1                             1
#define PIN_CHANGED                     1
#define PIN_NOT_CHANGED                 0
#define CDR_ENABLE_RX                   0
#define CDR_ENABLE_TX                   1
#define SPEED_OF_LIGHT                  299792458
#define round(x)                        (long)floor(x+0.5555555555)


static void initializeVariables(void);
void swDelay(uint16 delay);
static void initializeMemory(void);
void process25msTask(void);
void process50msTask(void);
void process100msTask(void);
void process150msTask(void);
void controlLddRegMap(void);
static void monitorDataControl(void);
static void checkPassword(uint8 mode);
static uint16 lut12BitsIndex(uint8 lutSelect);
float calculatePower(float data, uint8 power);
//uint16 voltageToLdTemperature(float adcValue);
void setTemperatureIndex(sint16 data);
static void checkPassword(uint8 mode);
uint16 tempToReferenceAdc(sint16 temp, uint8* lockingAddr);
void controlLutAverageTemp(void);
void timerDelay(uint16 delay);
void softTxDisableControl(void);
void checkSumCalculateForA0Base(void);
void checkSumCalculateForA0Ext(void);
void externalEepromUpdate(void);
void externalEepromLoad(void);
void checkRs0Rs1(void);
void shutDownRecovery(void);

uint16 tempToReferenceAdc2(float temp, uint8* lockingAddr);
#endif
