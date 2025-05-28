
#ifndef _TL6100_H_
#define _TL6100_H_

#include "max5112.h"
#include "appMessageQueue.h"

#define DAC_FRONT_ODD_CONFIG				DAC1_CONFIG_ADDRESS
#define DAC_FRONT_EVEN_CONFIG				DAC2_CONFIG_ADDRESS

#define DAC_FRONT_ODD_CODE					DAC1_CODE_ADDRESS
#define DAC_FRONT_EVEN_CODE					DAC2_CODE_ADDRESS
#define DAC_PHASE_CODE						DAC3_CODE_ADDRESS
#define DAC_REAR_SOA_CODE					DAC4_CODE_ADDRESS
#define DAC_GAIN_CODE						DAC5_CODE_ADDRESS
#define DAC_FRONT_SOA_SOURCE_CODE			DAC6_SOURCE_MODE_CODE_ADDRESS
#define DAC_REAR_CODE						DAC7_CODE_ADDRESS
#define DAC_IMB_LEFT_CODE					DAC8_CODE_ADDRESS
#define DAC_IMB_RIGHT_CODE					DAC9_CODE_ADDRESS
#define DAC_FRONT_SOA_SINK_CODE				DAC6_SINK_MODE_CODE_ADDRESS
#define DAC_FRONT_SOA_SHUTTER_CODE			DAC6_SHUTTER_MODE_CODE_ADDRESS
#define DAC_FRONT_SOA_POLARITY_CODE			DAC6_POLARITY_CTRL_ADDRESS

#define WAVELENGTH_LOCKING_MAX_CONDITION        15
#define WAVELENGTH_LOCKING_MIN_CONDITION        15
#define WAVELENGTH_LOCKING_HYSTERISYS           0
//#define PHASE_OFFSET                            10
//#define REAR_OFFSET								1
#define PHASE_LIMIT_OFFSET                      10

#define DAC_OUTPUT_MUX_A					0x0400
#define DAC_OUTPUT_MUX_B					0x0480
#define DAC_OUTPUT_MUX_C					0x0500
#define DAC_OUTPUT_MUX_D					0x0580

/* A2_TOSA_MEMORY => Address : 0xA2, Page : 0x30 */
#define REG_TTOSA_CTRL_FRONT_ODD			0x80 - 0x80
#define REG_TTOSA_CTRL_FRONT_EVEN			0x82 - 0x80
#define REG_TTOSA_CTRL_PHASE				0x84 - 0x80
#define REG_TTOSA_CTRL_REAR_SOA				0x86 - 0x80
#define REG_TTOSA_CTRL_GAIN					0x88 - 0x80
#define REG_TTOSA_CTRL_FRONT_SOA			0x8A - 0x80
#define REG_TTOSA_CTRL_REAR					0x8C - 0x80
#define REG_TTOSA_CTRL_IMB_LEFT				0x8E - 0x80
#define REG_TTOSA_CTRL_IMB_RIGHT			0x90 - 0x80
#define REG_TTOSA_LOCKER_CONDITION      	0x92 - 0x80
#define REG_TTOSA_CTRL_BIAS_L				0x94 - 0x80
#define REG_TTOSA_CTRL_BIAS_R				0x96 - 0x80
#define REG_TTOSA_TEMPERATURE_SET			0x98 - 0x80
#define REG_TTOSA_MODULATION_SET			0x9A - 0x80
                          
#define REG_TTOSA_FRONT_PAIR				0x9F - 0x80

#define REG_TTOSA_MON_SOA			        0xA0 - 0x80
#define REG_TTOSA_MON_REAR			        0xA2 - 0x80
#define REG_TTOSA_MON_PHASE			        0xA4 - 0x80
#define REG_TTOSA_MON_FSN			        0xA6 - 0x80
#define REG_TTOSA_CTRL_PHASE_HIGH		  	0xA8 - 0x80
#define REG_TTOSA_CTRL_PHASE_LOW			0xAA - 0x80

#define REG_TTOSA_POWER_CAL					0xAC - 0x80
#define REG_TTOSA_POWER_CAL_END				0xBF - 0x80

//#define REG_LOCKER_RATIO					0xB8 - 0x80
//#define REG_POWER_CAL						0xC4 - 0x80

#define REG_EOL_TH_HI						0xD0 - 0x80
#define REG_EOL_TH_LO						0xD2 - 0x80
#define REG_TARGET_REFL						0xD4 - 0x80

#define REG_WAVELENGTH						0xE0 - 0x80
#define REG_WAVELENGTH_GHZ					0xE4 - 0x80

#define REG_WAVELENGTH_UNLOCK				0xF0 - 0x80
#define REG_WAVELENGTH_CHANNEL				0xF1 - 0x80
#define REG_WAVELENGTH_COMMAND				0xF2 - 0x80

#define REG_WAVELENGTH_CONSTATNT_FACTOR    	0xF3 - 0x80
#define REG_WAVELENGTH_CONSTATNT_FIRST    	0xF7 - 0x80
#define REG_WAVELENGTH_CONSTATNT_SECOND   	0xFB - 0x80

#define REG_TTOSA_SAVE						0xFF - 0x80

#define REAR_SOA_MINIMUM						1000

#define FS_PAIR_MINIMUM							1
#define FS_PAIR_MAXIMUM							7

#define SET_LEFT                         		0
#define SET_RIGHT                              	1  
#define IndexToTemp(x) 					     	(x*2)-40

#define VREF                            		2400
#define BIT_RESOLUTION                  		65535

void processTable0x30(uint8 startAddress, uint8 numberOfBytes);
#if 0	/* jhRoh */
void processTL6100(EVENT_LIST event, uint16 data);
#else
void processTL6100(EVENT_LIST event);
#endif
void startLaser(bool isFirstBit);
void writeLaserInformation(void);
void setBias(uint8 option);
void controlWavelengthLocker(void);
void stopLaser(void);
#endif

