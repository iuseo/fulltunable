#ifndef _MAX5112_H_
#define _MAX5112_H_

#define MAX5112_DEVICE_ADDRESS			0x58

typedef enum
{
	GENERAL_CONFIG_ADDRESS = 0,		/* 0x00, General Configuration */
	DAC1_CONFIG_ADDRESS,			/* 0x01, DAC 1 Configuration */
	DAC2_CONFIG_ADDRESS,			/* 0x02, DAC 2 Configuration */
	DAC3_CONFIG_ADDRESS,			/* 0x03, DAC 3 Configuration */
	DAC4_CONFIG_ADDRESS,			/* 0x04, DAC 4 Configuration */
	DAC5_CONFIG_ADDRESS,			/* 0x05, DAC 5 Configuration */
	DAC6_CONFIG_ADDRESS,			/* 0x06, DAC 6 Configuration */
	DAC7_CONFIG_ADDRESS,			/* 0x07, DAC 7 Configuration */
	DAC8_CONFIG_ADDRESS,			/* 0x08, DAC 8 Configuration */
	DAC9_CONFIG_ADDRESS,			/* 0x09, DAC 9 Configuration */
	RESERVED_0_ADDRESS,				/* 0x0A, Reserved */
	RESERVED_1_ADDRESS,				/* 0x0B, Reserved */
	RESERVED_2_ADDRESS,				/* 0x0C, Reserved */
	RESERVED_3_ADDRESS,				/* 0x0D, Reserved */
	STATUS_FB_PART_ID_ADDRESS,		/* 0x0E, Status Feedback and Part ID */
	SW_RST_STDBY_CLR_ADDRESS,		/* 0x0F, Software Reset/Standby/Clear */
	DAC1_9_CODE_ADDRESS,			/* 0x10, DAC 1-9 Code */
	DAC1_CODE_ADDRESS,				/* 0x11, DAC 1 Code */
	DAC2_CODE_ADDRESS,				/* 0x12, DAC 2 Code */
	DAC3_CODE_ADDRESS,				/* 0x13, DAC 3 Code */
	DAC4_CODE_ADDRESS,				/* 0x14, DAC 4 Code */
	DAC5_CODE_ADDRESS,				/* 0x15, DAC 5 Code */
	DAC6_SOURCE_MODE_CODE_ADDRESS,	                /* 0x16, DAC 6 Source Mode Code */
	DAC7_CODE_ADDRESS,				/* 0x17, DAC 7 Code */
	DAC8_CODE_ADDRESS,				/* 0x18, DAC 8 Code */
	DAC9_CODE_ADDRESS,				/* 0x19, DAC 9 Code */
	DAC6_SINK_MODE_CODE_ADDRESS,	/* 0x1A, DAC 6 Sink Mode Code */
	DAC6_SHUTTER_MODE_CODE_ADDRESS,	/* 0x1B, DAC 6 Shutter Mode Code */
	RESERVED_4_ADDRESS,				/* 0x1C, Reserved */
	RESERVED_5_ADDRESS,				/* 0x1D, Reserved */
	RESERVED_6_ADDRESS,				/* 0x1E, Reserved */
	DAC6_POLARITY_CTRL_ADDRESS,		/* 0x1F, DAC 6 Polarity Control */
	MAX5112_ADDRESS_NUMBER			/* 32 */
} MAX5112_REGISTER_ADDRESS;

typedef struct
{
	/* 0x00 : General Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 14;	/* [13:0] */
			uint16 GSWG						: 2;	/* [15:14] : GSWG[1:0] */
		} bit;
	} GENERAL_CONFIG;
	/* 0x01 : DAC1 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC1_CONFIG;
	/* 0x02 : DAC2 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC2_CONFIG;
	/* 0x03 : DAC3 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC3_CONFIG;
	/* 0x04 : DAC4 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC4_CONFIG;
	/* 0x05 : DAC5 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC5_CONFIG;
	/* 0x06 : DAC6 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC6_CONFIG;
	/* 0x07 : DAC7 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC7_CONFIG;
	/* 0x08 : DAC8 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC8_CONFIG;
	/* 0x09 : DAC9 Configuration Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 7;	/* [6:0] */
			uint16 MUX						: 4;	/* [10:7] : MUX[3:0] */
			uint16 RNG						: 1;	/* [11] : Range(DAC 3, 8 and 9) */
			uint16 T_H_EN					: 1;	/* [12] : Track and Hold Enable */
			uint16 CLR_CFG					: 2;	/* [14:13] : Clear Configuration Settings */
			uint16 GSW						: 1;	/* [15] : Ground Switch Control */
		} bit;
	} DAC9_CONFIG;

	/* 0x0E : Status/Revision Readback Command Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 4;	/* [3:0] */
			uint16 REV_ID					: 4;	/* [7:4] : Revision Code */
			uint16 PART_ID					: 4;	/* [11:8] : Part ID Code */
			uint16							: 1;	/* [12] */
			uint16 HI_TEMP					: 1;	/* [13] : High Temperature Warning Indicator */
			uint16 OVR_TEMP					: 1;	/* [14] : Overtemperature Warning Indicator */
			uint16 PRO_TEMP					: 1;	/* [15] : Overtemperature Protection Indicator */
		} bit;
	} STATUS_FB_PART_ID;

	/* 0x0F : Software Reset Command Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 13;	/* [12:0] */
			uint16 SW_CLR					: 1;	/* [13] : Software Clear */
			uint16 STDBY					: 1;	/* [14] : Global Standby(identical to a global power-down) */
			uint16 RST						: 1;	/* [15] : Global Reset(identical to a POR) */
		} bit;
	} SW_RST_STDBY_CLR;

	/* 0x10 : Group DAC(1 to 9) Code Command Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : Group DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC1_9_CODE;

	/* 0x11 : Individual DAC1 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC1_CODE;

	/* 0x12 : Individual DAC2 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC2_CODE;

	/* 0x13 : Individual DAC3 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC3_CODE;

	/* 0x14 : Individual DAC4 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC4_CODE;

	/* 0x15 : Individual DAC5 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC5_CODE;

	/* 0x16 : Individual DAC6 Source Mode Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC6_SOURCE_MODE_CODE;

	/* 0x17 : Individual DAC7 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC7_CODE;

	/* 0x18 : Individual DAC8 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC8_CODE;

	/* 0x19 : Individual DAC9 Code Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC9_CODE;

	/* 0x1A : Individual DAC6 Sink Mode Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC6_SINK_MODE_CODE;

	/* 0x1B : Individual DAC6 Shutter Mode Setting Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 2;	/* [1:0] */
			uint16 B						: 14;	/* [15:2] : DAC Code Setting in Straight Binary Format */
		} bit;
	} DAC6_SHUTTER_MODE_CODE;

	/* 0x1F : DAC6 Polarity Command Register */
	volatile union
	{
		uint16 word;
		struct
		{
			uint16							: 15;	/* [14:0] */
			uint16 SW_POL					: 1;	/* [15] : Software Polarity Control(to DAC6 only) */
		} bit;
	} DAC6_POLARITY_CTRL;
} MAX5112_REGISTER_T;

void initializeMax5112(void);
void writeMax5112(uint8 address, uint16 value);
void setOutputMuxMax5112(uint8 pair);
void max5112StopCondition(void);
#endif
