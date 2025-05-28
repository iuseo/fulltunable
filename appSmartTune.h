
#include "main.h"
/*DEBUG_TAB*/
#define SMART_TUNE_SELF_TUNING                  0xA0 - 0x80        // select tuning mode
#define OPTIONAL_HOLD_LASER_TIME                0xA1 - 0x80       
#define CHANNEL_SWITCH_TIME                     0xA2 - 0x80
#define T1_TIME_OUT                             0xA4 - 0x80
#define T2_TIME_OUT                             0xA6 - 0x80
#define T3_TIME_OUT                             0xA8 - 0x80
#define TIME_OUT_SEC							0xA9 - 0x80
#define TX_STATE								0xAB - 0x80
#define RX_STATE								0xAC - 0x80


/*02h_PAGE*/
#define TUNING_CURRENT_STATUS                   0xA8 - 0x80
#define TUNING_LATCHED_STATUS                   0xAC - 0x80

/*0x80 PAGE*/
#define SMART_TUNE_TX_MC_CHANNEL                0xA1-0x80  // SMART Tx local data
#define SMART_TUNE_TX_YC_CHANNEL                0xA2-0x80  // SMART Tx REMOTE data
#define SMART_TUNE_RX_MC_CHANNEL                0xA3-0x80  // SMART Rx local data
#define SMART_TUNE_RX_YC_CHANNEL                0xA4-0x80  // SMART Rx REMOTE data

#define AMCC_TUNE                      0x01
#define MSA_TUNE                       0x02
#define AMCC_CONTROL                   0x03

#define SUPPORTED                    0x08
#define TUNING_ENABLE                0x02

#define TUNING                       0x80

#define TUNING_READ_HOST             0x01
#define TUNE_RESTART                 0x10

#define SWITCH_TIME                   270           // 360ms - ticktimer delay 
#define T1_T2_OUT                     400000
#define T3_OUT                        60000   

#define TX_MC_MAX                     98

#define SWITCH_TIME_MAX                 3200
#define SWITCH_TIME_MIN                 128
#define SET_TIME_OUT_1_2                400
#define SET_TIME_OUT_3                  57

#ifdef ADDON_JMA_WIRELESS
typedef enum
{
	SMART_DIS_IDLE = 0,
    SMART_DIS_1ST_OBEY,
	SMART_DIS_1ST_OBEY_CNT,
	SMART_DIS_IGNORE,
	SMART_DIS_IGNORE_CNT,
	SMART_DIS_2ND_OBEY,
	SMART_DIS_MAX
} SMART_TX_DIS_STATE_E;
#endif

typedef enum
{
	MAN_RX_STATE_IDLE = 0,
	MAN_RX_STATE_START,
	MAN_RX_STATE_DATA,
	MAN_RX_STATE_STOP,
	MAN_RX_STATE_MAX
} MAN_RX_STATE_E;

typedef enum
{
	MAN_TX_STATE_IDLE = 0,
	MAN_TX_STATE_START,

	MAN_TX_STATE_DATA_LOW_LOW,
	MAN_TX_STATE_DATA_LOW_HIGH,
	MAN_TX_STATE_DATA_HIGH_HIGH,
	MAN_TX_STATE_DATA_HIGH_LOW,

	MAN_TX_STATE_STOP,
	MAN_TX_STATE_TX_HOLD,
	MAN_TX_STATE_MAX
} MAN_TX_STATE_E;


typedef enum
{
	SMART_TUNE_TX_INIT = 0,
	SMART_TUNE_TX_LASER,
	SMART_TUNE_TX_CHECK_YC,
	SMART_TUNE_TX_SEND_MSG,
    SMART_TUNE_TX_HOLD_LSR,
    SMART_TUNE_TX_NEXT_CHAN,
    SMART_TUNE_TX_SELFTUNE_CHAN,
    SMART_TUNE_TX_MISSION
} SMART_TUNE_TX_STATE_E;


typedef enum
{
	SMART_TUNE_RX_INIT = 0,
	SMART_TUNE_RX_DEC_00,
	SMART_TUNE_RX_DEC_X0,
	SMART_TUNE_RX_DEC_XY,
    SMART_TUNE_RX_MISSION
} SMART_TUNE_RX_STATE_E;

typedef enum
{
  START = 0,
  SETING,
  STOP
} FLAG_TIMING;

typedef enum
{
  HOLD_START = 0,
  HOLD_SETING,
  HOLD_STOP
} HOLD_TIMING;

typedef struct {
	uint8 TX_MC; 
    uint8 TX_YC;
    uint8 RX_MC;
    uint8 RX_YC;
}TX_RX_CHANNEL_T;

typedef struct {   
    FLAG_TIMING sendFlag;
    HOLD_TIMING holdFlag;
    bool bitFlag;
    uint8 bitCount;
	uint8 holdBit;
    uint8 bitNum;
    uint16 smartTxTimeOut;
    uint32 smartRxTimeOut;
    bool changeAspFlagTx;
    bool changeAspFlagRx;
}TIMING_VARIABLE_T;

typedef struct {
    uint16 manRxData;
    uint16 manTxData;
    uint8 manMcData;
    uint8 manYcData;
    uint8 manRxDataCount;
    uint8 manTxDataCount;
    bool isLaserOn;
}MAN_DATA_T;


typedef struct {
    uint16 holdLaserBit;
    uint16 switchTime;
    uint32 t1TimeOut;
    uint32 t2TimeOut;
    uint16 t3TimeOut;
    uint8 selfTuningSupported;
    uint8 enableSelfTuning;
    uint8 selfTuningCurrentStatus;
    uint8 selfTuningLatchedStatus;
    bool timeOutStarted;
}MSA_TIME;

void smartTxData(void);
void smartRxData(void);
void smartSendMsg(uint8 dataLength, uint8 command,uint8* data);
void smartReceive(uint8 myChannel, uint8 yourChannel);
void startSmartTuneRx(void);
void manReceiveData(void);
void manTransmitData(void);
void setManTxData(uint8 dataMc, uint8 dataYc, bool holdFlag);
void gn2146TxSoftDisable(uint8 onOff);
extern uint8 changeAspFlag;
void changeTxChannel(uint8 channel);

#ifdef ADDON_JMA_WIRELESS
void txDisable(void);
#endif
