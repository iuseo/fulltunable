#ifndef _APPUART_H_
#define _APPUART_H_

typedef enum
{
	UART_RX_STATE_IDLE = 0,
	UART_RX_STATE_START,
	UART_RX_STATE_DATA,
	UART_RX_STATE_STOP,
	UART_RX_STATE_MAX
} UART_RX_STATE_E;

typedef enum
{
	UART_TX_STATE_IDLE = 0,
	UART_TX_STATE_START,
	UART_TX_STATE_DATA,
	UART_TX_STATE_STOP,
	UART_TX_STATE_MAX
} UART_TX_STATE_E;


typedef enum
{
	UART_ROD_IDLE = 0,
	UART_AWS_REQUEST_WAIT,
	UART_AWS_REQUEST,
	UART_AWS_REQUEST_LASER_ON,
	UART_AWS_REQUEST_TX_REQUEST,
	UART_AWS_RESPONSE,
	UART_AWS_CH_SET,
	UART_AWS_CH_SET_RESPONSE,
	UART_AWS_COMM_SUCCESS,
	UART_AWS_COMM_COMPLETE,
	UART_COMM_TRY,
	UART_COMM_TRY_RESPONSE,
	UART_AWS_MASTER_MODE,
	UART_AWS_SLAVE_SET,
	UART_AWS_MASTER_CH_REQUEST,
	UART_AWS_MASTER_SET,
	UART_AWS_MASTER_RESPONSE,
	UART_REQ_ROD_DDM,
	UART_ROD_DDM,
	UART_ROD_RX_DDM,
	UART_ROD_CH_REQ,
	UART_ROD_CH_RES,
	UART_ROD_MEM_READ_REQ,
	UART_ROD_MEM_READ_RECV,
	UART_ROD_MEM_READ_RES,
	UART_TEST
} UART_TX_AMCC_STATE_E;

typedef enum
{
	HEADER_1ST_PREAMBLE = 0,
	HEADER_2ND_PREAMBLE,
	HEADER_STX_CODE,
	DATA_CODE,
} UART_RX_CHECK_STATE_E;

#ifdef ADDON_JMA_WIRELESS
typedef enum
{
	TX_DIS_IDLE = 0,
    TX_DIS_1ST_OBEY,
	TX_DIS_1ST_OBEY_CNT,
	TX_DIS_IGNORE,
	TX_DIS_IGNORE_CNT,
	TX_DIS_2ND_OBEY,
	TX_DIS_MAX
} UART_TX_DIS_STATE_E;
#endif



#define TX_DUMMY_OFFSET 4

#define UART_DUMMY_0   0 
#define UART_DUMMY_1   1 
#define UART_DUMMY_2   2 
#define UART_DUMMY_3   3 

#define UART_PRE_0   (0) 
#define UART_PRE_1   (1)
#define UART_STX     (2) 
#define UART_DLEN    (3)
#define UART_CMD     (4)
#define UART_DATA    (5)
//#define UART_CC      30
//#define UART_ETX     31

#define UART_FRAME_LENGTH   32
#define PREAMBLE_CODE       0xAA
#define STX_CODE            0xF0
#define ETX_CODE            0x55

#define AWS_MASTER          0
#define AWS_SLAVE           2
#define AWS_RANDOM          1

#define AMCC_ENABLE         0x1
#define AWS_ENABLE          0x2

#define AMCC_MASTER         0x10
#define AMCC_SLAVE          0x20

#define SLAVE_CHANNEL_OFFSET 47


#define RX_CRC_INDEX       (5) // PREAMblE(3) + CMD + "CRC"
#define RX_ETX_INDEX       (6) // PREAMblE(3) + CMD + CRC + "ETX"
#define RX_CRC_LEGNTH      (2) // datlength + CMD
 
#define CMD_TXO             0x07	// TX ON
#define CMD_LBE             0x08	// Loop back Enable
#define CMD_DDM             0x0C	// DDM 
#define CMD_REQ_DDM         0x0D	// DDM request
#define CMD_CH_REQ          0x0B	// CH change request
#define CMD_MER_RES         0x0E	// Memory Read response
#define CMD_MER             0x0F	// Memory Read

/* bi-direction mode */
#define CMD_AWS_REQ         0x10	// AWS request
#define CMD_AWS_CH_REQ      0x11	// AWS channel set request
#define CMD_AWS_CH_RES      0x12	// AWS channel set response
#define CMD_AWS_COMM_CHECK  0x13	// AWS channel commmunication check
#define CMD_COMM_CHECK      0x14        // AWS retry commmunication check
#define CMD_COMM_CHECK_RES  0x15        // AWS retry commmunication response

/* master/slave mode */
#define CMD_AWS_MASTER_REQ  0x16        // AWS MASTER MODE request
#define CMD_AWS_MASTER_SET  0x17        // AWS MASTER channel set
#define CMD_AWS_MASTER_RES  0x18        // AWS MASTER channel set

#define ROD_DDM_LOOP_LENGTH     7
#define ROD_DDM_BUFFER_INDEX    2

#define UART_ADDRESS_OFFSET            0x80
#define UART_ROD_STATE				   0x80-0x80
#define UART_AWS_SUCCESS_VARIABLE      0x81-0x80  // AWS success(tuning:1,done:0)
#define UART_AWS_FLASH_WRITE_FLAG      0x82-0x80  // AWS flag write(enable:1,disable:0)
#define UART_AMCC_RETRY_COUNT          0x83-0x80
#define UART_AWS_TX_LOCAL_CHANNEL      0x84-0x80  // AWS Tx local data
#define UART_AWS_SEND_COUNT            0x85-0x80  // AWS packet send count
#define UART_RX_LOCAL_CHANNEL          0x86-0x80
#define UART_RX_REMOTE_CHANNEL         0x87-0x80  // AWS remote channel(for AWS bi-direction mode)
#define UART_RX_RESPONSE_CHANNEL       0x88-0x80  // AWS remote channel(for AWS bi-direction mode)
#define UART_RX_INDEX_COUNT            0x89-0x80  // UART Rx index count
#define UART_RX_ERROR_COUNT            0x8A-0x80  // UART Rx error count
#define UART_RX_SUCCESS_COUNT          0x8C-0x80  // UART Rx success count
#define UART_TX_PACKET_PERIOD          0x8E-0x80  // UART Tx packet period(AWS:30ms,ROD:1000ms)
#define UART_TX_PACKET_COUNTER         0x90-0x80  // UART Tx packet counter(bit 1 per 10ms)
#define UART_RX_LOS_COUNTER 		   0x92-0x80


#define UART_WAVELOCKER_TIMER_COUNT    0x94-0x80	
#define UART_TX_PROCESS_FLAG 		   0x95-0x80

#ifdef ADDON_JMA_WIRELESS
#define UART_TX_DIS_COUNTER 		   0x96-0x80  // processing Tx_Disable after ASP (default value : 0x1E=3s)
#define UART_TX_DIS_FLAG 		   	   0x98-0x80  // processing Tx_Disable after ASP 
//(bit 0 : ASP Done, bit 1 : 1st obey Tx_Disable, bit 2 : ignore Tx_Disable, bit 3 : 2nd obey Tx_Disable)
#define UART_TX_DIS_COUNTER1 		   0xAC-0x80  // processing Tx_Disable after ASP (default value : 0x1E=3s)
#define UART_TX_DIS_COUNTER2		   0xAE-0x80  // processing Tx_Disable after ASP (default value : 0x1E=3s)
#endif

#if 0
#define UART_AMCC_SLOPE_Add            0x92-0x80  // UART AMCC channel slope
#define UART_AMCC_OFFSET_Add           0x96-0x80  // UART AMCC channel offset
#define UART_AMCC_SLOPE_ENABLE_Add     0x9A-0x80  // UART AMCC Slope/Offset function enalbe
#define UART_AMCC_SLOPE_CHANNEL_Add    0x9B-0x80  // UART AMCC Slope/Offset function channel define
#endif



#define AWS_CHANNEL_OFFSEST			   0xDF-0x80

#define UART_RX_BUF_START_INDEX        0xE0-0x80

#define UART_RX_BUF_DATA_0_IDX         0xE5-0x80
#define UART_RX_BUF_DATA_1_IDX         0xE6-0x80
#define UART_RX_BUF_DATA_2_IDX         0xE7-0x80
#define UART_RX_BUF_DATA_3_IDX         0xE8-0x80




// A2h 0x22 table index define
#define UART_ROD_DDM_TEMP              0xE0-0x80
#define UART_ROD_DDM_VCC               0xE2-0x80
#define UART_ROD_DDM_TX_BIAS           0xE4-0x80
#define UART_ROD_DDM_TX_POWER          0xE6-0x80
#define UART_ROD_DDM_RX_POWER          0xE8-0x80
#define UART_ROD_DDM_TEC_TEMP          0xEA-0x80
#define UART_ROD_DDM_TEC_CURR          0xEC-0x80

void uartReceiveData(void);
void uartTransmitData(void);
void startUartTx();
void startUartRx(void);


void initializeUart();
uint8 makeCRC8code(uint8* data, uint16 length);
void uartPacketParsing(uint8 dataLength, uint8 command, uint8* data);
void uartStateManager(void);
void getAwsMode(void);
void changeTxChannel(uint8 channel);
void masterState(void);
extern uint8 changeAspFlag;
#endif
