
#ifndef _SFF8690_H_
#define _SFF8690_H_

/* Dithering */
#define SFF_8690_A2_02_DITHERING						0x80 - 0x80			/* Dithering, 1 byte */
#define SFF_8690_A2_02_FUNCTION_CONTROL					0x81 - 0x80			/* bit0: remote loopbacak enable, bit1: self-loopback enable, bit2-6: reserved, bit7: force tx disable at remote side, 1 byte  */
#define SFF_8690_A2_02_REMOTE_CHANNEL					0x82 - 0x80         /* remote request channel, 1 byte */
#define SFF_8690_A2_02_RDT_CONTROL						0x83 - 0x80			/* RDT(receiver decision threshold) , 1 byte */

#ifdef ADDON_JMA_WIRELESS
/*0x00 page*/
#define SFF_8690_A2_00_LFL1								0xED - 0x80			/* Laser's First Frequency(THz), 2 bytes */
#define SFF_8690_A2_00_LFL2								0xEF - 0x80			/* Laser's First Frequency(GHz * 10), in units of 0.1GHz, 2 bytes */
#define SFF_8690_A2_00_LFH1								0xF1 - 0x80			/* Laser's Last Frequency(THz), 2 bytes */
#define SFF_8690_A2_00_LFH2								0xF3 - 0x80			/* Laser's Last Frequency(GHz * 10), in units of 0.1GHz, 2 bytes */
#define SFF_8690_A2_00_LGRID							0x9B - 0x80			/* Laser's minimum supported grid spacing(GHz * 10), i.e., in units of 0.1GHz, LGrid : positive or negative number, 2 bytes */
#define SFF_8690_A2_00_CHANNEL							0xF6 - 0x80			/* User input of wavelength channel, 1 ~ n, 2 bytes */

#endif

/* Module Capabilities */
#define SFF_8690_A2_02_LFL1								0x84 - 0x80			/* Laser's First Frequency(THz), 2 bytes */
#define SFF_8690_A2_02_LFL2								0x86 - 0x80			/* Laser's First Frequency(GHz * 10), in units of 0.1GHz, 2 bytes */
#define SFF_8690_A2_02_LFH1								0x88 - 0x80			/* Laser's Last Frequency(THz), 2 bytes */
#define SFF_8690_A2_02_LFH2								0x8A - 0x80			/* Laser's Last Frequency(GHz * 10), in units of 0.1GHz, 2 bytes */
#define SFF_8690_A2_02_LGRID							0x8C - 0x80			/* Laser's minimum supported grid spacing(GHz * 10), i.e., in units of 0.1GHz, LGrid : positive or negative number, 2 bytes */
/* Frequency and Wavelength Control Commands */
#define SFF_8690_A2_02_CHANNEL							0x90 - 0x80			/* User input of wavelength channel, 1 ~ n, 2 bytes */
#define SFF_8690_A2_02_CHANNEL_ADDR						0x90
#define SFF_8690_A2_02_CHANNEL_LSB                      0x91 - 0x80
#define SFF_8690_A2_02_WAVELENGTH						0x92 - 0x80			/* User input of wavelength setpoint, in units of 0.05nm, 2 bytes */
#define SFF_8690_A2_02_WAVELENGTH_ADDR					0x92
#define SFF_8690_A2_02_TX_DITHER						0x97 - 0x80			/* bit[0] : 1 to Disable Tx Dither, 0 to Enable Tx Dither, bit_0 of 1 byte */
/* Frequency and Wavelength Errors */
#define SFF_8690_A2_02_FREQUENCY_ERROR		        	0x98 - 0x80			/* Frequency error reported in 16 bit signed integer with LSB = 0.1GHz, 2 bytes */
#define SFF_8690_A2_02_WAVELENGTH_ERROR		        	0x9A - 0x80			/* Wavelength error reported in 16 bit signed integer with LSB = 0.005nm, 2 bytes */
/* Current Status */
#define SFF_8690_A2_02_CURRENT_STATUS		        	0xA8 - 0x80			/* bit[6] : TEC Fault, bit[5] : Wavelength Unlocked, bit[4] : Tx Tune(Tx is not ready due to tuning), 1 byte */
/* Latched Status */
#define SFF_8690_A2_02_LATCHED_STATUS		        	0xAC - 0x80			/* bit[6] : L-TEC Fault, bit[5] : L-Wavelength Unlocked, bit[4] : L-Bad Channel, bit[3] : L-New Channel, bit[2] : L-Unsupported Tx Dither, 1 byte */
#define SFF_8690_LATCHED_STATUS_ADDRESS                 0xAC

#define SFF_8690_A2_02_REMOTE_DEVICE_ADDRESS            0xB0 - 0x80
#define SFF_8690_A2_02_REMOTE_TABLE_ADDRESS             0xB1 - 0x80
#define SFF_8690_A2_02_REMOTE_START_ADDRESS             0xB2 - 0x80
#define SFF_8690_A2_02_REMOTE_BYTE_NUMBER               0xB3 - 0x80

/* GUI Command @REG_WAVELENGTH_COMMAND(0xF2 - 0x80 = 0x72 = 114) */
#define GUI_TOSA_CONFIG_SAVE							0x01
#define GUI_TOSA_CONFIG_LOAD							0x02

#define GRID_SPACING_050GHz					50
#define GRID_SPACING_100GHz					100

#define BASE_FREQUENCY_50GHZ							191250
#define BASE_FREQUENCY_100GHZ							191400

#if 0
#ifdef TTOSA_CHANNEL_MAX_98
	#define LASER_FIRST_FREQENCY						191250		/* GHz */
	#define LASER_LAST_FREQENCY							196100		/* GHz */
	#define LASER_GRID_SPACING							50			/* GHz */
	#define MAX_WAVELENGTH_CHANNEL						98	        /* 01 ~ 98 */
#else
	#define LASER_FIRST_FREQENCY						191400		/* GHz */
	#define LASER_LAST_FREQENCY							196100		/* GHz */
	#define LASER_GRID_SPACING							100			/* GHz */
	#define MAX_WAVELENGTH_CHANNEL						48			/* 01 ~ 48*/
#endif
#endif
typedef enum
{
	LASER_FIRST_FREQUENCY = 0,
	LASER_LAST_FREQUENCY,
	LASER_FREQUENCY_MAX
} LASER_FREQUENCY_E;

typedef struct
{
  	uint32 baseFrequency;
	uint32 firstFrequency;
	uint32 lastFrequency;
	int gridFrequency;
	uint16 tTosaFirstChannel;
	uint16 tTosaLastChannel;
	uint16 msaMaxChannel;
} SFF_8690_T;

void processTable0x02(uint8 startAddress, uint8 numberOfBytes);
#if 0	/* jhRoh */
void processSFF8690(EVENT_LIST event, uint16 data);
#else
void processSFF8690(EVENT_LIST event);
#endif
unsigned char charToDec(unsigned char bDec);
void loadTosaConfiguration(void);
void saveTosaChannelNumber(void);
void wavelengthMonitoring(uint16 channel);
void getFrequencyInformation(SFF_8690_T* sff8690);
uint16 getMsaMaxChannel(void);
uint16 getTunableTosaChannel(uint16 sff8690Channel);
#endif
