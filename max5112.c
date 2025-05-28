/*
 * FILE NAME            : max5112.c
 * DATE                 : April 18 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : April 18 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <iods4835.h>
#include "main.h"
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appWrapper.h"
#include "appGpio.h"
#include "max5112.h"
#include "tl6100.h"

const uint16 max5112Default[MAX5112_ADDRESS_NUMBER] = 
{
	0x8000,		/* 0x00, GENERAL_CONFIG_ADDRESS */
	0x8800,		/* 0x01, DAC1_CONFIG_ADDRESS */
	0x8800,		/* 0x02, DAC2_CONFIG_ADDRESS */
	0x8000,		/* 0x03, DAC3_CONFIG_ADDRESS */
	0x8800,		/* 0x04, DAC4_CONFIG_ADDRESS */
	0xC800,		/* 0x05, DAC5_CONFIG_ADDRESS */
	0xB800,		/* 0x06, DAC6_CONFIG_ADDRESS */
	0x8800,		/* 0x07, DAC7_CONFIG_ADDRESS */
	0x8000,		/* 0x08, DAC8_CONFIG_ADDRESS */
	0x8000,		/* 0x09, DAC9_CONFIG_ADDRESS */
	0x0000,		/* 0x0A, RESERVED_0_ADDRESS */
	0x0000,		/* 0x0B, RESERVED_1_ADDRESS */
	0x0000,		/* 0x0C, RESERVED_2_ADDRESS */
	0x0000,		/* 0x0D, RESERVED_3_ADDRESS */	
	0x0140,		/* 0x0E, STATUS_FB_PART_ID_ADDRESS */		
	0x0000,		/* 0x0F, SW_RST_STDBY_CLR_ADDRESS */
	0x0000,		/* 0x10, DAC1_9_CODE_ADDRESS */
	0x0000,		/* 0x11, DAC1_CODE_ADDRESS */
	0x0000,		/* 0x12, DAC2_CODE_ADDRESS */
	0x0000,		/* 0x13, DAC3_CODE_ADDRESS */
	0x0000,		/* 0x14, DAC4_CODE_ADDRESS */
	0x0000,		/* 0x15, DAC5_CODE_ADDRESS */
	0x0000,		/* 0x16, DAC6_SOURCE_MODE_CODE_ADDRESS */
	0x0000,		/* 0x17, DAC7_CODE_ADDRESS */
	0x0000,		/* 0x18, DAC8_CODE_ADDRESS */
	0x0000,		/* 0x19, DAC9_CODE_ADDRESS */
	0x0000,		/* 0x1A, DAC6_SINK_MODE_CODE_ADDRESS */
	0x0000,		/* 0x1B, DAC6_SHUTTER_MODE_CODE_ADDRESS */
	0x0000,		/* 0x1C, RESERVED_4_ADDRESS */
	0x0000,		/* 0x1D, RESERVED_5_ADDRESS */
	0x0000,		/* 0x1E, RESERVED_6_ADDRESS */
	0x0000      /* DAC6_POLARITY_CTRL_ADDRESS */
};
	
/****************************************************************************/
/*			F1		F2		F3		F4		F5		F6		F7		F8		*/
/*	Pair1	I		I		GND		GND		GND		GND		GND		GND		*/
/*	Pair2	GND		I		I   	GND 	GND 	GND 	GND 	GND		*/
/*	Pair3	GND		GND		I		I		GND		GND		GND		GND		*/
/*	Pair4	GND		GND		GND		I		I		GND		GND		GND		*/
/*	Pair5	GND		GND		GND		GND		I		I		GND		GND		*/
/*	Pair6	GND		GND		GND		GND		GND		I		I		GND		*/
/*	Pair7	GND		GND		GND		GND		GND		GND		I		I		*/
/****************************************************************************/
/*	OP1A : FRONT 1															*/
/*	OP1B : FRONT 3															*/
/*	OP1C : FRONT 5															*/
/*	OP1D : FRONT 7															*/
/*	OP2A : FRONT 2															*/
/*	OP2B : FRONT 4															*/
/*	OP2C : FRONT 8															*/
/*	OP2D : FRONT 6															*/
/****************************************************************************/
/* pair1 : F1, F2 => OP1A, OP2A => A for Odd, A for Even					*/
/* pair2 : F2, F3 => OP2A, OP1B => B for Odd, A for Even					*/
/* pair3 : F3, F4 => OP1B, OP2B => B for Odd, B for Even					*/
/* pair4 : F4, F5 => OP2B, OP1C => C for Odd, B for Even					*/
/* pair5 : F5, F6 => OP1C, OP2D => C for Odd, D for Even					*/
/* pair6 : F6, F7 => OP2D, OP1D => D for Odd, D for Even					*/
/* pair7 : F7, F8 => OP1D, OP2C => D for Odd, C for Even					*/
/****************************************************************************/
const unsigned short max5112MuxOdd[7] = 
{
	DAC_OUTPUT_MUX_A,	/* 1 2 */
	DAC_OUTPUT_MUX_B,	/* 3 2 */
	DAC_OUTPUT_MUX_B,	/* 3 4 */
	DAC_OUTPUT_MUX_C,	/* 5 4 */
	DAC_OUTPUT_MUX_C,	/* 5 6 */
	DAC_OUTPUT_MUX_D,	/* 7 6 */
	DAC_OUTPUT_MUX_D	/* 7 8 */
};

const unsigned short max5112MuxEven[7] = 
{
	DAC_OUTPUT_MUX_A,	/* 1 2 */
	DAC_OUTPUT_MUX_A,	/* 3 2 */
	DAC_OUTPUT_MUX_B,	/* 3 4 */
	DAC_OUTPUT_MUX_B,	/* 5 4 */
	DAC_OUTPUT_MUX_D,	/* 5 6 */
	DAC_OUTPUT_MUX_D,	/* 7 6 */
	DAC_OUTPUT_MUX_C	/* 7 8 */
};

/*
 * Name         : initializeMax5112
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 18 2019
 * History      : 
 */
void initializeMax5112(void)
{
	uint8 i = 0;
	uint16 data;

	writeMax5112(SW_RST_STDBY_CLR_ADDRESS, 0x8000);		/* SW_RST_STDBY_CLR.bit.RST = 1 */
	for(i = 0; i < MAX5112_ADDRESS_NUMBER; i++)
	{
		data = max5112Default[i];
		writeMax5112(i, data);
	}
	writeMax5112(DAC6_SHUTTER_MODE_CODE_ADDRESS, 0x0000);
}

/*
 * Name         : writeMax5112
 * Description  : 
 * Return Value : void
 * Parameter    : uint8, uint16
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 19 2019
 * History      : 
 */

void writeMax5112(uint8 address, uint16 value)
{
	uint8 dataBuffer[3];
	
	if(address == DAC_REAR_SOA_CODE) {
		return;
	}
          
	if((address >= DAC1_9_CODE_ADDRESS) && (address <= DAC6_SHUTTER_MODE_CODE_ADDRESS)) {
		value = (value << 2) & 0xFFFC;	/* 0x10 ~ 0x1B => [15:2] : B, [1:0] : reserved */
	}
	address = address << 1;
	dataBuffer[0] = (uint8)(value >> 8);
	dataBuffer[1] = (uint8)(value & 0x00FF);
	dataBuffer[2] = (uint8)0x38;	 	// stop byte for max5112 i2c conflict issue 
	//i2cMWriteBytes(I2CM_MASTER_1, MAX5112_DEVICE_ADDRESS, address, dataBuffer, 3);
	I2CM_WriteNBytes(I2CM_MASTER_1, MAX5112_DEVICE_ADDRESS, address, dataBuffer, 3);
}
/*
 * Name         : setOutputMuxMax5112
 * Description  : 
 * Return Value : void
 * Parameter    : uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 23 2019
 * History      : 
 */
void setOutputMuxMax5112(uint8 pair)
{
	uint16 data = 0x0000;

	data = max5112Default[DAC_FRONT_ODD_CONFIG];
	data |= max5112MuxOdd[pair];
	writeMax5112(DAC1_CONFIG_ADDRESS, data);
   
	data = max5112Default[DAC_FRONT_EVEN_CONFIG];
	data |= max5112MuxEven[pair];
	writeMax5112(DAC2_CONFIG_ADDRESS, data);
     
}

void max5112StopCondition(void)
{
  	uint8 dataBuffer[3];

	dataBuffer[0] = (uint8)0x00;
	dataBuffer[1] = (uint8)0x00;
	dataBuffer[2] = (uint8)0x38;
	I2CM_WriteNBytes(I2CM_MASTER_1, MAX5112_DEVICE_ADDRESS, DAC4_CODE_ADDRESS, dataBuffer, 3);
}
