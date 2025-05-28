#include <intrinsics.h>
#include <stdlib.h>
#include <iods4835.h>
#include "main.h"
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appMessageHandler.h"
#include "appWrapper.h"
#include "appGpio.h"
#include "appFlash.h"
#include "appTec.h"
#include "appApd.h"
#include "appAdc.h"
#include "appIDac.h"
#include "appApc.h"
#include "appMod.h"
#include "max5112.h"
#include "tl6100.h"
#include "sff8472.h"
#include "sff8690.h"
#include "appUart.h"
#include "appSmartTune.h"
#include "appSoa.h"

#include "appDcdc.h"
#pragma location=0x0020
const unsigned int password[16] = {0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222, 0x2222};
#pragma required = password

// 
////////////////////////////////////////////////////////////
////////////////// Define Global variable //////////////////
////////////////////////////////////////////////////////////
uint16 tempScale,tempOffset;
uint16 lddRegMap[DACNum][2];		/* upper buffer memeory */
static uint8 temperatureIndex;
uint8 avgCnt;
static sint16 temperatureAverage[AVGCNT_INDEX];

STATUS_FLAG_T statusFlag;

uint8 flashFlagByte1;
uint8 flashFlagByte2;
uint8 lasterStartFlag;
uint8 changeAspFlag = 0;

uint8 i2cSlaveReadFlagCount = 0;
#define	ALARM_ON			true
#define	ALARM_OFF			false
bool DATA_RDY_SOFT = ALARM_ON;
int EEPROM_defaultFlag;

float currentTempBuff = 0.0;
float curTemperature=0,temperatureTemp=0;
extern uint16 delaycount;
extern uint8 ttosaChannel;
extern SMART_TUNE_TX_STATE_E smartTxState;
extern SMART_TUNE_RX_STATE_E smartRxState;
extern TX_RX_CHANNEL_T smartChannel;
extern TIMING_VARIABLE_T timing;
extern MSA_TIME msaMode;
extern sint16 DdmTemp;
#ifdef ADDON_JMA_WIRELESS
extern SMART_TX_DIS_STATE_E smartDisState;
#endif
extern uint16 timer1s;
extern uint16 timer1sCounter;
extern uint8 flag00h;
extern uint8 restartFlag;
extern uint8 RSSIReceiveFlag;
extern bool ASPDisFlag;
uint16 task0, task1, task2,task3, manTransTimer = 0;
bool manTransFlag = false;
/*
 * Name         : main
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : May 21 2020
 * History      : 
 */
uint8 EEFlag = 0;
SFF_8690_T sff8690Info;
void main(void)
{   
	
    uint8 gn2146Delay = 0;
	smartTxState = SMART_TUNE_TX_INIT;
    smartRxState = SMART_TUNE_RX_INIT;
	initializeVariables();
    initializeSystem();
    
	initializeMax5112();
	memset(A2_TTOSA_PAGE_0x02,0x0,A2_TTOSA_PAGE_0x02_SIZE);
	A2_DiagData[STATUS_CONTROL_ADD] = (B1_H | B0_H);
    DATA_RDY_SOFT = ALARM_ON;
	//jdj 추가
	//첫 번째 채널의 frequency를 가져오기 위해 전체 eeprom load 전에 부분 load수행
//	eepromReadFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_EEPROM);
	FLASH_Read(A2_TTOSA_CHANNEL_01_FLASH,A2_TTOSA_MEMORY_ADD,A2_TTOSA_MEMORY_SIZE);
	// EEPROM에 data가 없을 경우
	//191400을 base frequency로 적용
	if(Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_WAVELENGTH_GHZ]) == 0xffff){
	  sff8690Info.baseFrequency = 191400;
	} else {
	  sff8690Info.baseFrequency = (uint32)(Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_WAVELENGTH_GHZ])) * 10;	  
	}	  

	externalEepromLoad();
	
	//0x02 page default write 
	if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] == 0xFF){
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] = 0x00;
	}
	if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_CHANNEL] == 0xFF){
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_CHANNEL] = 0x01;
	}
	if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] == 0xFF){
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] &= ~TUNING_ENABLE;
	}	
	if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] == 0xFF){
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] |= SUPPORTED;
	}
	if(Reg2ByteToWordLoad(&A2_8690_LIMIT[SFF_8690_A2_02_CHANNEL]) > 0xF000){
		RegWordTo2ByteSave(0x0001,&A2_8690_LIMIT[SFF_8690_A2_02_CHANNEL]);
	}
	if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]) > 0xF000){
		RegWordTo2ByteSave(0x0001,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
	}
	if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_RDT_CONTROL] == 0xFF){
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_RDT_CONTROL] = 0x00;
	}
	
	if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]) > 0xf000){
	  RegWordTo2ByteSave(0x00BF, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
	}
	if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]) > 0xf000){
	  RegWordTo2ByteSave(0x0FA0, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
	}
	if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]) > 0xf000){
	  RegWordTo2ByteSave(0x00C4, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);
	}
	if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]) > 0xf000){
	  RegWordTo2ByteSave(0x03E8, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);
	}
	if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) > 0xf000){
	  RegWordTo2ByteSave(0x03E8, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);
	}
	
	if(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add]) > 0xf000){
	  RegWordTo2ByteSave(0x0EF4, &A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add]);
	}
	
	if(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add]) >= 0xf000){
	  RegWordTo2ByteSave(0x0F52, &A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add]);
	}
	
	if((A2_8690_LIMIT[LIMIT_GRID_SPACING_Add] != GRID_SPACING_050GHz) && (A2_8690_LIMIT[LIMIT_GRID_SPACING_Add] != GRID_SPACING_100GHz)){
	  A2_8690_LIMIT[LIMIT_GRID_SPACING_Add] = GRID_SPACING_100GHz;
	}
	
	
	getFrequencyInformation(&sff8690Info);
	
	
//	loadTosaConfiguration();
	A2_DiagData[STATUS_CONTROL_ADD] = (B1_H | B0_H);	
	initializeMemory();	 			        
	loadTosaConfiguration();
	initializeApdBoost();                        // for APD1 
	initializeTecChannel(TEC_CH1);					
	initializeVeeDcDc(0xC300);					// for APD2, -5.2V
    APDCN1 = 0x0040;
	APDCN2 = 0xA040;              				// APD OFF
	DS_DAC_Config(DS_CH_5,DS_CONFIG,0x0040);
	DS_DAC_Config(DS_CH_6,DS_CONFIG,0x0040);
	initializeMemoryGN2146(A2_GN2146Sync);
   
	/*low temp test*/
	gn2146Delay = Reg2ByteToWordLoad(&A2_DEBUGTAB[GN2146_RESET_DELAY_Add]);
	swDelay(gn2146Delay);
	
	resetGN2146(A2_VendorFn);
    
	syncGN2146(A2_GN2146Sync);
  
	A2_DiagData[STATUS_CONTROL_ADD] &= B0_L;
    DATA_RDY_SOFT = ALARM_OFF;
    
	inrushForTec();
	
  	checkAlarmWarning(A2_DiagData);
//	while(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= 0x40){
//		readAdcChannels();
//		convertAdcToDdm();		
//		checkAlarmWarning(A2_DiagData);
//	}
	lasterStartFlag = LASER_START_READY;
	
	
	controlApd1Register();	 			
	APDCN1 = 0x0044;
	APDCN2 = 0xA044;    // APD ON
	DS_DAC_Config(DS_CH_5,DS_CONFIG,0x8040);
	DS_DAC_Config(DS_CH_6,DS_CONFIG,0x8040);         
	externalEepromUpdate();
//	A2_DEBUGTAB[0xD0 - 128] = EEFlag;
	A2_DiagData[FWVERS_USR] = FW_VERSION;
	A2_DiagData[PRODUCTID_Add] = PRODUCT_ID;
//    A2_DEBUGTAB[REACH_STEP_Add] = 0x40;         // LD POWER CONTROL
//    A2_DEBUGTAB[GN2146_RESET_DELAY_Add] = 0x00;         // LD POWER CONTROL
    
	if(lasterStartFlag == LASER_START_READY){
	  	memcpy(&A2_VendorFn[DAC_TEC_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET], 2);  
		memcpy(&A2_VendorFn[DAC_AMP_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET], 2);  
		startLaser(false);
       
	}
         
	//checkRS0(PIN_NOT_CHANGED, (A2_DiagData[STATUS_CONTROL_ADD] >> 4) & 0x01, (A2_DiagData[STATUS_CONTROL_ADD] >> 3) & 0x01);
	//checkRS1(PIN_NOT_CHANGED, (A2_DiagData[STATUS_CONTROL_ADD] >> 5) & 0x01, (A2_DiagData[EXT_STATUS_CONTROL_ADD] >> 3) & 0x01);
    checkRs0Rs1();     
	setStatusControlBits();
	initializeUart();
	initializeQueue(getMessageQueue());
	messageReadyFlag = ON;
    
	controlMod1Register();
	A2_DEBUGTAB[InrushDone_Add] = 0x01;    
	
	getAwsMode();
    

    
    if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & AMCC_TUNE) { 
        changeAspFlag = 0;
        EIES0_bit.EIES06 = EDGE_NEGATIVE;
        EIE0_bit.EIE06 = ON;        // UART RX interrupt Enable              
    } else if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & MSA_TUNE) {        //Smart Tune MSA Self-tuning progressing
#ifdef RSSI_LOS_TYPE
		changeAspFlag = 1;
		ADC_HQInterupt(QTCONFG_ADC02_RSSI);
		ADC_LQInterupt(QTCONFG_ADC02_RSSI);			
		RSSIReceiveFlag = 1;
#else
        changeAspFlag = 1;
        EIF0_bit.EIF03 = OFF;		// Reset Interrupt flag 
//        EIES0_bit.EIES03 = EDGE_POSITIVE; 
		EIES0_bit.EIES03 = EDGE_NEGATIVE;
        EIE0_bit.EIE03 = ON;        // Man Code RX interrupt Enable  
//        A2_UserData[MODULE_CONTROL_STATUS] &= ~AMCC_CONTROL;
#endif
    }    
    
    restartFlag = 0;
	
	/*for(i=1;i<99;i++)
	{
		//eepromReadFunction(A2_DiagData,A2_TTOSA_CHANNEL_01_EEPROM); 
		channel = i;
		eepromReadFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_EEPROM + A2_TTOSA_MEMORY_SIZE * (channel - 1));
		timerDelay(100);
		flashUpdate(A2_TTOSA_MEMORY_ADD, A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (channel - 1), A2_TTOSA_MEMORY_SIZE);
		timerDelay(100);
	}*/
    GPIO15_ExtenalInterrupt_Init(); //Tx_dis interrupt  //test
    
    pollingGpio();
    A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] &= ~TUNING;
	A2_TTOSA_PAGE_0x02[TUNING_LATCHED_STATUS] &= ~TUNING;
	A2_UserData[SFF_8690_CURRENT_STATUS_VEN] &= ~TUNING;		//Samsung req
	A2_UserData[SFF_8690_LATCHED_STATUS_VEN] &= ~TUNING;		

	A2_VendorFn[TEST_BUF_2] = 0x55;
	// Clear Timers 
//	clearTimer();
	// Register Timers 
//	registerTimer(MESSAGE_HANDLER_TIMER_ID, messageHandler);
//	registerTimer(PROCESS_9ms_TIMER_ID, startUartTx);
//	registerTimer(PROCESS_25ms_TIMER_ID, process25msTask);
//	registerTimer(PROCESS_50ms_TIMER_ID, process50msTask);
//	registerTimer(PROCESS_100ms_TIMER_ID, process100msTask);
	// Start Timers 
//	startTimer(MESSAGE_HANDLER_TIMER_ID, TIMER_1ms);
//	startTimer(PROCESS_9ms_TIMER_ID, TIMER_9ms);
//	startTimer(PROCESS_25ms_TIMER_ID, TIMER_25ms); 
//	startTimer(PROCESS_50ms_TIMER_ID, TIMER_50ms);
//	startTimer(PROCESS_100ms_TIMER_ID, TIMER_100ms); 
    
//    A2_DEBUGTAB[0xA0 - 0x80] = 0x01;
#ifdef ADDON_JMA_WIRELESS
    smartDisState = SMART_DIS_1ST_OBEY;
#endif
    
	
// Page Offset 
    A2_DEBUGTAB[PAGE_OFFSET] = 0x38; //ASCII '8'
    A2_DEBUGTAB[PAGE_OFFSET + 1] = 0x30; //ASCII '0'
    A2_DEBUGTAB[PAGE_OFFSET + 2] = 0x46; //ASCII 'F'
    A2_DEBUGTAB[PAGE_OFFSET + 3] = 0x46; //ASCII 'F'
    
	ASPDisFlag = true;
	
	A2_UserData[TX_MAX_CH_VEN] = getMsaMaxChannel(); //Samsung req
	
	while(1)
	{    
//		processTimer();
		if(manTransTimer >= 1) {
			manTransTimer = 0;
			manTransmitData();	
		}
		
		if(task0 >= TIMER_1ms) {
			task0 = 0;
			messageHandler();
		}
		if(manTransFlag == false){
			if(task1 > TIMER_25ms) {
				task1 = 0;
				process25msTask();
			}
		}
		if(task2 > TIMER_50ms) {
			task2 = 0;
			process50msTask();
		}
		
		if(task3 > TIMER_100ms) {
			task3 = 0;
			process100msTask();
	
		}
	} 
        
}

/*
 * Name         : swDelay
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
void swDelay(uint16 delay)
{   
	unsigned int i = 0, j = 0;

	for(i = 0; i < delay; i++)
	{
		/* Reset Watchdog Timer */
		WDCN_bit.RWT = 1; 
		for(j = 0; j < 1500; j++)
		{
			asm("nop");
			asm("nop");
			asm("nop");         
			asm("nop");         
		}
	}  
}


void timerDelay(uint16 delay)
{   
  delaycount = 0;
  
  while(delay > delaycount)
  {
     asm("nop");     
  }
  
} 

/*
 * Name         : initializeVariables
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
static void initializeVariables(void)
{
	messageReadyFlag = OFF;
	cdrSyncFlag = OFF;
	tecTableFlag = ON;
	apdTableFlag = ON;
	lutReadyFlag = OFF;
	i2cSlaveReadFlag = OFF;
	i2cSlaveWriteFlag = OFF;
	lasterStartFlag = OFF;      // initial laser start flag
}

/*
 * Name         : initializeMemory
 * Description  : Reads LUTs values from flash, loads into SRAM and initializes SRAM with default/saved values
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 25 2019
 * History      : 
 */
static void initializeMemory(void)
{     
	unsigned char i;
   
	/* Clear lower memory */
	temperatureIndex = 0;    
	tempScale = Info_Read(0x19);
	tempOffset = Info_Read(0x1A);
 
#if 1 	
	A2_DiagData[USER_PW_ADD + 0] = 0xFF;
	A2_DiagData[USER_PW_ADD + 1] = 0xFF;
	A2_DiagData[USER_PW_ADD + 2] = 0xFF;
	A2_DiagData[USER_PW_ADD + 3] = 0xFF;
#else
	memset(&A2_DiagData[USER_PW_ADD + 0], 0xFF, 4);
#endif
	A2_DiagData[TABLE_SEL_Add] = 0x00;
	A2_DEBUGTAB[InrushDone_Add] = 0x00;        // tec inrush done bit 
	memcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_UserData_ADD, A2_UserData_SIZE);																				
 
	memset(A2_UART_MEMORY,0x00,A2_UART_MEMORY_SIZE);
//	memset(A2_TTOSA_PAGE_0x02,0x00,A2_TTOSA_PAGE_0x02_SIZE);
	memset(A2_FAR_END_A0_0x20,0x00,A2_FAR_END_A0_0x20_SIZE);
	memset(A2_FAR_END_A2_0x22,0x00,A2_FAR_END_A2_0x22_SIZE);
	memset(A2_FAR_END_02_0x24,0x00,A2_FAR_END_02_0x24_SIZE);
        
	A2_VendorFn[TEST_BUF_2] = 0x00;  
//	A2_VendorFn[AMCC_DUMMY_DATA] = 0x04;   // UART TEST dummy 
//	A2_VendorFn[AMCC_DATALENGTH] = 0x02;   // UART TEST data length
	A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] = 0;
	A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_CHANNEL] = 1;
//	A2_UserData[AMCC_LINK_STATUS] &= ~(0x10);   
//    A2_UserData[MODULE_ID_STATUS] = A2_UserData[MODULE_ID_SET] & 0x0F;   
	
	// SFF-8690 write
	/* 해당 영역 입력할 수 있게 수정 jdj
	RegWordTo2ByteSave(LASER_FIRST_FREQENCY / 1000, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);			
	RegWordTo2ByteSave((LASER_FIRST_FREQENCY % 1000) * 10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);		
        
	RegWordTo2ByteSave(LASER_LAST_FREQENCY / 1000, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);			
	RegWordTo2ByteSave((LASER_LAST_FREQENCY % 1000) * 10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);		
    */  
    
	uint16 gridSpacing = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) / 10;

	//RegWordTo2ByteSave(LASER_GRID_SPACING*10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);			

	A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] |= 0x3;		// Add SFF-8690
	
	checkPassword(SETPW);			

#if 0	
	for(i = 0; i < 4; i++)
	{
		A2_UserData[ALARMMASK1_ADD + i] = 0;
	}
#else
	memset(&A2_UserData[ALARMMASK1_ADD], 0, 4);
#endif
#if 0	
	for(i = 111; i <= 120; i++)
	{
		A2_DiagData[i] = 0;
	}
#else
	memset(&A2_DiagData[111], 0, 10);
#endif
	
	for(i = 0; i < DACNum; i++)
	{
		lddRegMap[i][SET] = 0x0000;
	}
	for(i = 0; i < DACNum; i++)
	{
		lddRegMap[i][BUF] = 0xFFFF;
	}

	A2_VendorFn[SYNC_FLAG_Add] = 0x01;

	A2_VendorFn[MANI2C_Add] = 0x01; 
	A2_VendorFn[MANCTRL_Add] = 0x00;	
	A2_VendorFn[CTRLPIN_Add] = 0x0;		
    
   
    if(A2_DEBUGTAB[EEPROM_RESET] != 0x0A){
        memset(A2_TTOSA_MEMORY, 0xFF, 128);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_VendorFn_EEPROM);        //Vendor in all 0xFF
		
        RegWordTo2ByteSave(0x2000,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_ODD]);
        RegWordTo2ByteSave(0x166C,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_EVEN]);
        RegWordTo2ByteSave(0x0BA2,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
        RegWordTo2ByteSave(0x0000,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR_SOA]);
        RegWordTo2ByteSave(0x238E,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_GAIN]);
        RegWordTo2ByteSave(0x425A,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]);
        RegWordTo2ByteSave(0x04E0,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]);
        RegWordTo2ByteSave(0x18BA,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_IMB_LEFT]);
        RegWordTo2ByteSave(0x1E20,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_IMB_RIGHT]);
        RegWordTo2ByteSave(0xFFFF,&A2_TTOSA_MEMORY[REG_TTOSA_LOCKER_CONDITION]);
        RegWordTo2ByteSave(0x068D,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_BIAS_L]);
        RegWordTo2ByteSave(0x0651,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_BIAS_R]);
        RegWordTo2ByteSave(0x38A4,&A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET]);
        RegWordTo2ByteSave(0x00C4,&A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET]);
        RegWordTo2ByteSave(0x38A4,&A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET]);
        RegWordTo2ByteSave(0x38A4,&A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET]);
        A2_TTOSA_MEMORY[REG_TTOSA_FRONT_PAIR] = 0x01;
        RegWordTo2ByteSave(0x1591,&A2_TTOSA_MEMORY[REG_TTOSA_MON_SOA]);
        RegWordTo2ByteSave(0x0D64,&A2_TTOSA_MEMORY[REG_TTOSA_MON_REAR]);
        RegWordTo2ByteSave(0x071A,&A2_TTOSA_MEMORY[REG_TTOSA_MON_PHASE]);
        RegWordTo2ByteSave(0x06D8,&A2_TTOSA_MEMORY[REG_TTOSA_MON_FSN]);
        RegWordTo2ByteSave(0x1136,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE_HIGH]);
        RegWordTo2ByteSave(0x0749,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE_LOW]);
        
        
        RegWordTo2ByteSave(0x38A4,&A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET]);
        RegWordTo2ByteSave(0x00C4,&A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET]);
        
        A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] = 0x01;
		
        
        EEPROM_defaultFlag = 1;
        
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_GN2146Sync_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_APCLUTAB_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_MODLUTAB_EEPROM);

//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_TECLUTAB_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_APDLUTAB_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_TTOSA_PAGE_0x02_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A0_SerialID_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A0_Reserved_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_DiagData_EEPROM);

//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_UpperMEM_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_UserData_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_8690_LIMIT_EEPROM);
//        eepromWriteFunction(A2_TTOSA_MEMORY,A2_8690_LIMIT_EEPROM);
        for(char i=0;i<98;i++) {
//            eepromWriteFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_FLASH + (A2_TTOSA_MEMORY_SIZE * i) );
			FLASH_Update(A2_TTOSA_MEMORY_ADD,A2_TTOSA_CHANNEL_01_FLASH + (A2_TTOSA_MEMORY_SIZE * i),A2_TTOSA_MEMORY_SIZE);
        }
        A2_DEBUGTAB[EEPROM_RESET] = 0x0A;
        flashFlagByte2 |= A2_DEBUGTABWrFlag;
    }

	if((A2_VendorFn[FACTORYDEF_Add] != 0x0A) || (EEPROM_defaultFlag == 1)) {
        EEPROM_defaultFlag = 0;
		/* factory default */	
        A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] = 0x01;
        A2_DEBUGTAB[CHANNEL_SWITCH_TIME] = 128;
		
//        A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] = 0; //smart tune 0ff
      
		memset(A0_Reserved, 0x00, 128);
		memset(A2_MODLUTAB, 0x00, 128);
		memset(A2_MODLUTAB, 0x00, 72);
		memset(&A2_MODLUTAB[LUTHighIndex_Add], 0x00, 36);

		memset(A2_TECLUTAB, 0x00, 128);
        RegWordTo4ByteSave(158.065,&A2_TECLUTAB[TEC_LUT_CONST_0]); //-101.436
        RegWordTo4ByteSave(-4.87805,&A2_TECLUTAB[TEC_LUT_CONST_1]); //3.31325
        RegWordTo4ByteSave(-0.00233977,&A2_TECLUTAB[TEC_LUT_CONST_2]); //0.00285946
        
        RegWordTo4ByteSave(-6.10423,&A2_MODLUTAB[MOD_LUT_CONST_0]); //-10.1827
        RegWordTo4ByteSave(0.150093,&A2_MODLUTAB[MOD_LUT_CONST_1]); //0.301201
        RegWordTo4ByteSave(0.00154785,&A2_MODLUTAB[MOD_LUT_CONST_2]); //0.00160845
        
        RegWordTo4ByteSave(0.0,&A2_MODLUTAB[SOA_LUT_CONST_0]); //0
        RegWordTo4ByteSave(0.0,&A2_MODLUTAB[SOA_LUT_CONST_1]); //0
        RegWordTo4ByteSave(0.0,&A2_MODLUTAB[SOA_LUT_CONST_2]); //0
        
        
//        memset(A2_TECLUTAB, 0x00, 72);
		//memset(A2_TECLUTAB, 0x1C, 72);
		//memset(&A2_TECLUTAB[LUTHighIndex_Add], 0x22, 36);
        
 

		memset(A2_APDLUTAB, 0x00, 128);
		memset(A2_APDLUTAB, 0x60, 72);
		memset(&A2_APDLUTAB[LUTHighIndex_Add], 0x99, 36);
        
        memset(A2_UserData,0x00,119);
		memset(A2_TTOSA_PAGE_0x02,0x00,128);
		
		asm("nop");		/* To Keep Static analysis tool happy */
		
		A2_VendorFn[LDMODE_Add] = 0x80;
        A0_SerialID[RDT_FUNC] = 0x7F;
        A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_RDT_CONTROL] = 0x00;
        A2_DEBUGTAB[TX_EQ_DEF_Add] = 0x50;

		//limit fisrt frequency default 0x0EF1(191250) bit당 50Ghz
		RegWordTo2ByteSave(0x0EF1, &A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add]);
		
		//limit last frequency default 0x0F52(196100) bit당 50Ghz
		RegWordTo2ByteSave(0x0F52, &A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add]);
        
		//limit grid spacing 50Ghz
		A2_8690_LIMIT[LIMIT_GRID_SPACING_Add] = GRID_SPACING_050GHz;
		RegWordTo2ByteSave(0x0032, &A2_VendorFn[DAC_APC_SET_Add]);
		RegWordTo2ByteSave(0x00C8, &A2_VendorFn[DAC_AMP_SET_Add]);
//		RegWordTo2ByteSave(0x0208, &A2_VendorFn[DAC_TEC_SET_Add]);
		RegWordTo2ByteSave(0x36E6, &A2_VendorFn[DAC_TEC_SET_Add]);
		RegWordTo2ByteSave(0x03E8, &A2_VendorFn[DAC_APD_SET_Add]);
		RegWordTo2ByteSave(0x0000, &A2_VendorFn[DAC_AGC_SET_Add]);
                
		RegWordTo2ByteSave(0x02BC, &A2_VendorFn[INT_VCC_OFFEST_ADD]);

		RegWordTo4ByteSave(0.0, &A2_VendorFn[INT_TEMP_CAL_0_ADD]);
		RegWordTo4ByteSave(1.0, &A2_VendorFn[INT_TEMP_CAL_1_ADD]);
		RegWordTo4ByteSave(0.0, &A2_VendorFn[INT_TEMP_CAL_2_ADD]);
		memset(&A2_VendorFn[INT_TX_PWR_CAL_0_ADD], 0x00, 40);

		RegWordTo2ByteSave(0xC000, &A2_TECLUTAB[TECCN_Add]);  
        RegWordTo2ByteSave(0x0000, &A2_TECLUTAB[TECTUDY_Add]);  
		RegWordTo2ByteSave(0x0020, &A2_TECLUTAB[TECKP_Add]);  
		RegWordTo2ByteSave(0x0005, &A2_TECLUTAB[TECKI_Add]); 
		RegWordTo2ByteSave(0x0003, &A2_TECLUTAB[TECKD_Add]);  			
		RegWordTo2ByteSave(0x0000, &A2_TECLUTAB[TECSHIFT_Add]);  
		RegWordTo2ByteSave(0x0100, &A2_TECLUTAB[TECPosERRClamp_Add]);  
		RegWordTo2ByteSave(0xFF00, &A2_TECLUTAB[TECNegERRClamp_Add]); 
		RegWordTo2ByteSave(0x3A00, &A2_TECLUTAB[TECPositiveClamp_Add]);    
		RegWordTo2ByteSave(0xC600, &A2_TECLUTAB[TECNegativeClamp_Add]);   		
                    
		
		RegWordTo2ByteSave(0x0041, &A2_APDLUTAB[APD1_ErrorClamp_Add]);  
		RegWordTo2ByteSave(0x7010, &A2_APDLUTAB[APD1_DutyClamp_Add]);  
		RegWordTo2ByteSave(0x0013, &A2_APDLUTAB[APD1_InternalGain_Add]);  
		RegWordTo2ByteSave(0x000A, &A2_APDLUTAB[APD1_DutyStartClamp_Add]); 
		RegWordTo2ByteSave(0xFFFB, &A2_APDLUTAB[APD1_IntergratorClamp_Add]);
		RegWordTo2ByteSave(0x0041, &A2_APDLUTAB[APD1_NegativeErrorClamp_Add]);  
		RegWordTo2ByteSave(0x0040, &A2_APDLUTAB[APD1_PositiveErrorClamp_Add]);
		RegWordTo2ByteSave(0x1000, &A2_APDLUTAB[APD1_ProportionalGain_Add]);  
        RegWordTo2ByteSave(0x2044, &A2_APDLUTAB[APD1_CN_Add]);    
		RegWordTo2ByteSave(0x0580,&A2_GN2146Sync[APD1_FeedbackErr_Add]);
        RegWordTo2ByteSave(0x017F,&A2_GN2146Sync[APD1_ClampStatus_Add]);
        RegWordTo2ByteSave(0x1000,&A2_GN2146Sync[APD1_DutyCycle_Add]);
		
        RegWordTo2ByteSave(0x00BF, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
        RegWordTo2ByteSave(0x09C4, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
        RegWordTo2ByteSave(0x00C4, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);
        RegWordTo2ByteSave(0x03E8, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);
        RegWordTo2ByteSave(0x01F4, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);
		
		//SMART TUNE SET DEFAULT
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] |= SUPPORTED;	//bit3 is Self Tuning Support
		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] &= ~TUNING_ENABLE;	//bit2 is Self Tuning Enable
        
		
        A2_VendorFn[WAVELENGTH_LOCKING_EN] = 0; // wavelength locker disalbe
//		A2_DEBUGTAB[TEC_DELTA_TEMP_Add] = 0xA; 	// delta TOSA temp initial
		
		A2_VendorFn[MOD_OFFSEST_EN_Add] = 0;
		A2_VendorFn[MOD_OFFSEST_Add] = 0x14;
        A2_DEBUGTAB[RX_EQ_BOOST_25G_Add] = 0x28;
        
		
	
		
		memset(&A2_UserData[MODULE_ID_SET], 0, 4);
		
/*노범선 부장님 요청 사항 register default value*/		
		A2_8690_LIMIT[PHASE_OFFSET_VALUE1_Add] = 10;
		A2_8690_LIMIT[PHASE_OFFSET_VALUE2_Add] = 15;
		A2_8690_LIMIT[PHASE_OFFSET_VALUE3_Add] = 20;
		A2_8690_LIMIT[WAVELENGTH_HYS_Add] = 0xF0;
		A2_8690_LIMIT[REAR_OFFSET_VALUE1_Add] = 0x01;
		A2_8690_LIMIT[REAR_OFFSET_VALUE2_Add] = 0x02;
		A2_8690_LIMIT[REAR_OFFSET_VALUE3_Add] = 0x03;
        RegWordTo2ByteSave(500,&A2_8690_LIMIT[REAR_PHASE_mV_HYS1_Add]);
		RegWordTo2ByteSave(200,&A2_8690_LIMIT[REAR_PHASE_mV_HYS2_Add]);
		
		flashFlagByte1 |= A2_UserDataWrFlag;
		flashFlagByte1 |= A2_GN2146SyncWrFlag;
		flashFlagByte1 |= A2_APCLUTABWrFlag;
		flashFlagByte1 |= A2_VendorFnWrFlag;
		flashFlagByte1 |= A2_MODLUTABWrFlag;
		flashFlagByte1 |= A2_TECLUTABWrFlag;
		flashFlagByte1 |= A2_APDLUTABWrFlag;
        flashFlagByte2 |= A2_8690_LIMITWrFlag;
        flashFlagByte2 |= A2_TTOSA_PAGE_0x02WrFlag; 
        flashFlagByte2 |= A0_SerialIDWrFlag;
		flashFlagByte2 |= A0_ReservedWrFlag;
//		flashFlagByte2 |= A2_DEBUGTABWrFlag;
	} else {
		/* Normal */
		A2_VendorFn[LDMODE_Add] = 0x8F;
		//A2_VendorFn[LDMODE_Add] = 0x89;

		for(i = 1; i < DACNum; i++)    // mod,tec,apd dac setting
		{
			lddRegMap[i][SET] = Reg2ByteToWordLoad(&A2_VendorFn[DAC_APC_SET_Add + (i * 2)]);   
		}
//        if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & MSA_TUNE) {
//           A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] &= ~0x01;       //start Smart Tune MSA ASP start set
//           
//        } else if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & AMCC_TUNE) {
//           A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] |= 0x01;        //start AMCC ASP start set
//           A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] |= 0x03;		// Add SFF-8690           
//        }
        if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] == 0x03 || A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] == 0x00) {
            A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] = 0x02;
        }
	}	
    
    if(A2_DEBUGTAB[FACTORYDEF_Add] != 0x0A ) {
        A2_DEBUGTAB[TEC_DELTA_TEMP_Add] = 0xA; 	// delta TOSA temp initial
        
        /*Shutdown&recovery*/
        A2_DEBUGTAB[SHUTDOWN_ENABLE] = 0x00;
        RegWordTo2ByteSave(0x0061, &A2_DEBUGTAB[SHUTDOWN_TEMP]);              //shutdown default temp
        RegWordTo2ByteSave(0x005F, &A2_DEBUGTAB[SHUTDOWN_TEMP]);         //recovery default temp
        
        A2_DEBUGTAB[SOFT_RATE_SEL_DEF0_Add] = RS0_DEFAULT;
        A2_DEBUGTAB[SOFT_RATE_SEL_DEF1_Add] = RS1_DEFAULT;
        A2_DEBUGTAB[TX_EQ_DEF_Add] = TX_IN_EQ_DEFAULT;
        A2_DEBUGTAB[RX_EM_DEF_Add] = RX_OUT_EMPH_DEFAULT;
        A2_DEBUGTAB[TX_EYE_SHAPING_10G_Add] = (TX_EYE_SHAPING_10G_DEFAULT & 0x1f);
        A2_DEBUGTAB[TX_EYE_SHAPING_25G_Add] = (TX_EYE_SHAPING_25G_DEFAULT & 0x1f);
        A2_DEBUGTAB[RX_EQ_BOOST_10G_Add] = RX_EQ_BOOST_10G_DEFAULT;
        A2_DEBUGTAB[RX_EQ_BOOST_25G_Add] = RX_EQ_BOOST_25G_DEFAULT;
        A2_DEBUGTAB[RDT_BUFF_Add] = 0;
        RegWordTo2ByteSave(3000, &A2_DEBUGTAB[UART_ROD_PERIOD_Add]);
        
        RegWordTo2ByteSave(2000, &A2_DEBUGTAB[GN2146_RESET_DELAY_Add]);
        
        RegWordTo2ByteSave(0x001E, &A2_DEBUGTAB[LUTRTemp_Add]);
		RegWordTo2ByteSave(0xFFD8, &A2_DEBUGTAB[LUTLTemp_Add]);
		RegWordTo2ByteSave(0x005A, &A2_DEBUGTAB[LUTHTemp_Add]);
        
        A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] = 0x02;
        A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] = 0x02;
        RegWordTo2ByteSave(128, &A2_DEBUGTAB[CHANNEL_SWITCH_TIME]);
        RegWordTo2ByteSave(400, &A2_DEBUGTAB[T1_TIME_OUT]);
        RegWordTo2ByteSave(400, &A2_DEBUGTAB[T2_TIME_OUT]);
        A2_DEBUGTAB[T3_TIME_OUT] = 2;
		
#ifdef RSSI_LOS_TYPE
        A2_DEBUGTAB[RX_LOS_OUT_EN] = 0x01;
		RegWordTo2ByteSave(0x0023,&A2_DEBUGTAB[RSSI_TH_H]);
		RegWordTo2ByteSave(0x000F,&A2_DEBUGTAB[RSSI_TH_L]);
#endif
        
        /*inrush test*/
        A2_DEBUGTAB[CALCULATE_STEP_Add] = 10;
        A2_DEBUGTAB[REACH_STEP_Add] = 128; 
//        A2_DEBUGTAB[GN2146_RESET_DELAY_Add] = 10;
            
		A2_DEBUGTAB[FAULT_MASK] = 0x00;
		
		//BUCK delay time 
		RegWordTo2ByteSave(4000,&A2_DEBUGTAB[BUCK_delay_time]);
		
		//wavelength reporting
		A2_DEBUGTAB[WAVE_PN_REPORTING_ENABLE] = 0;
		A2_DEBUGTAB[WAVE_REPORTING_ENABLE] = 0;
		
        A2_DEBUGTAB[FACTORYDEF_Add] = 0x0A;
        flashFlagByte2 |= A2_DEBUGTABWrFlag;
	}

    
    
	A2_DEBUGTAB[SOFTWARE_RESET] = 0x00;
	if(A2_DEBUGTAB[SOFT_RATE_SEL_DEF0_Add])
		A2_DiagData[STATUS_CONTROL_ADD] |= 0x08; // RS0
	else
		A2_DiagData[STATUS_CONTROL_ADD] &= ~0x08; // RS0
    
    if(A2_DEBUGTAB[SOFT_RATE_SEL_DEF1_Add])
        A2_DiagData[EXT_STATUS_CONTROL_ADD] |= 0x08; // RS1
    else 
        A2_DiagData[EXT_STATUS_CONTROL_ADD] &= ~0x08; // RS1
	
	A2_DiagData[DIAGNOSTICS_TX_IN_EQ] = A2_DEBUGTAB[TX_EQ_DEF_Add];
	A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] = A2_DEBUGTAB[RX_EM_DEF_Add];
	A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_RDT_CONTROL] = A2_DEBUGTAB[RDT_BUFF_Add];
//	A2_DEBUGTAB[0x83-0x80] = 0x11;
	memcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_UserData_ADD, A2_UserData_SIZE);
	
	//처음 before soa offset을 0으로 초기화
	RegWordTo2ByteSave(0x0000, &A2_MODLUTAB[BEFORE_SOA_OFFSET_Add]);

}

/*
 * Name         : process25msTask
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
void process25msTask(void)
{      
  	checkRs0Rs1();
	readAdcChannels();
  	convertAdcToDdm();
	pollingGpio();
	controlLddRegMap();
	monitorDataControl();  
	
    if((A2_DEBUGTAB[DURATION_CONTROL_BIT] != 0x01) && (A2_DEBUGTAB[DURATION_CONTROL_BIT] != 0x02) && (A2_DEBUGTAB[DURATION_CONTROL_BIT] != 0x04)){
        A2_DEBUGTAB[DURATION_CONTROL_BIT] = 0x01;
    }
      
//    shutDownRecovery();
	if(i2cSlaveReadFlag == ON) {
		i2cSlaveReadFlagCount++;
		if(i2cSlaveReadFlagCount > I2C_SLAVE_READ_FLAG_COUNT_MAX) {
			i2cSlaveReadFlagCount = 0;
			i2cSlaveReadFlag = OFF;
		}
	}
	
	if(i2cSlaveWriteFlag == ON) {
		i2cSlaveWriteFlag = OFF;		
	}  
 
}

/*
 * Name         : process25msTask
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */

void process50msTask(void)
{
	controlLutAverageTemp();
	controlTecRegister();
	controlApd1Register();	
	controlMod1Register();
	controlSoaRegister();
	checkAlarmWarning(A2_DiagData);
	/*RDT function*/
	if(A0_SerialID[RDT_FUNC] & 0x80){
	  controlRDTFunc(_ON);
	}else{
	  controlRDTFunc(_OFF);
	}
	checkSumCalculateForA0Ext();
	       
	if(A2_VendorFn[MANI2C_Add] & 0x01) {
		syncGN2146(A2_GN2146Sync);
	}else {
		manualGN2146(A2_VendorFn);		  
	}
	
	if(A2_UART_MEMORY[UART_WAVELOCKER_TIMER_COUNT] > 10){
		controlWavelengthLocker();
		A2_UART_MEMORY[UART_WAVELOCKER_TIMER_COUNT]= 0;
	}else{
		A2_UART_MEMORY[UART_WAVELOCKER_TIMER_COUNT] += 1;
	}				
	checkPassword(CLRPW);
	   
}

/*
 * Name         : process100msTask
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
void process100msTask(void)
{     

  	if(A2_DEBUGTAB[SOFTWARE_RESET] == 'R'){
		A2_DEBUGTAB[SOFTWARE_RESET] = 0x00; 
		Device_Reset();
	}
	externalEepromUpdate();
	
#ifdef ADDON_JMA_WIRELESS
    load00hTo02h();
#endif 
	checkSumCalculateForA0Base();
}

/*
 * Name         : controlLddRegMap
 * Description  : Scheduled every 25ms in process25msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
void controlLddRegMap(void)
{
	unsigned char i, j;
  
	if(i2cSlaveWriteFlag == ON) {
		return;
	}
        
	A2_VendorFn[LDMODE_Add] &= ~0x00;
       
	for(i = 0; i < DACNum; i++)
	{
		j = (0x01 << i);
		
		if((A2_VendorFn[LDMODE_Add] & j) != j) {              
			lddRegMap[i][SET] = Reg2ByteToWordLoad(&A2_VendorFn[DAC_APC_SET_Add + (i * 2)]);	/* LUT MODE OFF */                          
		}else{
			lddRegMap[i][SET] = lut12BitsIndex(i);							/* LUT MODE ON */
		}		
	}
           
}

/*
 * Name         : monitorDataControl
 * Description  : Scheduled every 25ms in process25msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 27 2019
 * History      : 
 */
static void monitorDataControl(void)
{
	if((A2_VendorFn[LDMODE_Add] & TEMP_INDEX) != TEMP_INDEX) {
		temperatureIndex = A2_VendorFn[TINDEX_Add];
	} else {
		A2_VendorFn[TINDEX_Add] = temperatureIndex;
	}
}

/*
 * Name         : softTxDisableControl
 * Description  : soft tx disable control
 * Return Value : void
 * Parameter    : void
 * Author       : Kyungsoo Park, pks@lightron.co.kr
 * Date         : June 22 2020
 * History      : 
 */

void softTxDisableControl(void)
{ 
    uint8 data=0;
    A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1; 
    if( A2_DiagData[STATUS_CONTROL_ADD] & 0x40 ){
      data = 0x1;    
      writeMax5112(DAC_GAIN_CODE, 0);
      writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, 0);
    }else{
      data = 0x0;
//      writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]));
	if(A2_VendorFn[LDMODE_Add] & 0x01){
		writeMax5112(DAC_FRONT_SOA_SOURCE_CODE,lddRegMap[APCL_REG][SET]);
	} else {
		writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]));
	}
      writeMax5112(DAC_GAIN_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_GAIN]));
    }
	I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TXDSBL_SOFT_REG_0, &data, 1);
	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
}

/*
 * Name         : checkPassword
 * Description  : 
 * Return Value : void
 * Parameter    : uint8
 * Author       : 
 * Date         : March 27 2019
 * History      : 
 */
static void checkPassword(uint8 mode)
{
	uint8 i, pwCheck;
	
	pwCheck = 0;
	
	for(i = 0; i < 4; i++)
	{
		if(A2_VendorFn[VENDOR_PW_ADD + i] != A2_DiagData[USER_PW_ADD + i]) {
			pwCheck++;
		}
	}
	if(mode == CLRPW) {
		if(pwCheck == 0) {
			A2_DiagData[PROTECTION_ADD] = OFF;
			DEV_NUM &= ~0x80;   // boot mode i2c address enable
		}
	} else {
		if(pwCheck == 0) {
			A2_DiagData[PROTECTION_ADD] = OFF;
		} else {
			A2_DiagData[PROTECTION_ADD] = ON;
		}
	}
}

/*
 * Name         : lut12BitsIndex
 * Description  : 
 * Return Value : uint16
 * Parameter    : uint8
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
static uint16 lut12BitsIndex(uint8 lutSelect)
{
	uint8 highByte, lowByte, *p;
	uint16 result = 0x0000;
    float tecLutOffset=0,tecLutIndex=0;
	float modLutOffset=0,modLutIndex=0;
	float soaLutOffset=0,soaLutIndex=0;
	uint16 beforeSoaLutOffset = 0;
	uint8 modOffset=0;
	
	switch(lutSelect)
	{   
		case APCL_REG :
		  	//변수명 A2_MOD이지만 MOD LUT을 수식으로 사용하여 MOD LUT 영역이 남아 해당 page 사용
			p = &A2_MODLUTAB[SOA_LUT_CONST_0];			
//			soaLutIndex = Reg2ByteToWordLoad(&A2_DiagData[DIAGNOSTICS_TEC_TEMP]) / 256;
			soaLutIndex = (temperatureIndex * 2) - 40;
			//tempToReferenceAdc함수는 float형 인자를 받지 않으므로 tempToReferenceAdc2함수 생성 하였음
			//추후 테스트를 하여 tempToReferenceAdc함수 하나로 합치는 것이 좋아 보임			
			soaLutOffset = tempToReferenceAdc2(soaLutIndex,p);	
			
			//계속 값이 증가하는 버그로 인해 이전 offset은 빼주고 현재 offset을 적용함
//			beforeSoaLutOffset = Reg2ByteToWordLoad(&A2_MODLUTAB[BEFORE_SOA_OFFSET_Add]);			
//			RegWordTo2ByteSave((uint16)soaLutOffset, &A2_MODLUTAB[BEFORE_SOA_OFFSET_Add]);
//			result = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA])+(sint16)soaLutOffset - beforeSoaLutOffset;
			result = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA])+(sint16)soaLutOffset;
			RegWordTo2ByteSave(result, &A2_8690_LIMIT[SOA_LUT_RESULT_Add]);
//			RegWordTo2ByteSave(result, &A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]);
			break;
		case AMPU_REG :
			p = A2_MODLUTAB;
			if( (A2_DiagData[EXT_STATUS_CONTROL_ADD]&0x08) == 0x08 || RS1_HOST == _ON) 
			{ 
			}
			else
			{ // TX Rate select off -> 10G, CDR Bypass			  			  
			  if(A2_VendorFn[MOD_OFFSEST_EN_Add] & 0x1){
				modOffset = A2_VendorFn[MOD_OFFSEST_Add];
				//RegWordTo2ByteSave(modValue-modOffset, &A2_VendorFn[DAC_AMP_SET_Add]);
			  }
			}
			modLutIndex = (temperatureIndex * 2) - 40;
			modLutOffset = tempToReferenceAdc(modLutIndex,p);	
			result = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET])+(sint16)modLutOffset - modOffset;
			break;			
	  	  
		case WAVE_REG :
			p = A2_TECLUTAB;
			tecLutIndex = (temperatureIndex * 2) - 40;
		  	tecLutOffset = tempToReferenceAdc(tecLutIndex,p);	
			result = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET])+(sint16)tecLutOffset;
			break;			
		case APDS_REG :
			p = A2_APDLUTAB;
			lowByte = *(p + temperatureIndex);
			highByte = *(p + LUT_HIGH_Add + (temperatureIndex / 2));
			if((temperatureIndex % 2) == 0) {	
         		result = (highByte << 8) & 0x0F00;	/* Low Nibble */
			} else {
          		result = (highByte << 4) & 0x0F00;	/* High Nibble */
			}
			result |= lowByte;
			break;	
		default :
			break;
	}
	
	return result;
}

/*
 * Name         : setTemperatureIndex
 * Description  : 
 * Return Value : void
 * Parameter    : sint16
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
void setTemperatureIndex(sint16 data)
{
	float floatData;
	
	floatData = ((data / 255) + 40) / 2;				
	if(floatData >= 71.0 ) {
		temperatureIndex = 71;
	} else if(floatData <= 0.0) {
		temperatureIndex = 0;
	} else {
		temperatureIndex = (uint8)floatData;	
	}
}

uint16 tempToReferenceAdc(sint16 temp, uint8* lockingAddr)
{
	float p;
	float f_temp[3];
	uint8 i, buff[4];
	uint16 data;
	p = temp;
        
	for(i=0; i<(WAVELENGTH_LOCKING_EQUATION+1); i++){
		memcpy(&buff[0], &lockingAddr[0+(i*4)],sizeof(unsigned long));
		swap_uint32(&buff[0]);
		memcpy(&f_temp[i], &buff[0], sizeof(float));	
	}
  
	data = (f_temp[2]*p*p) + (f_temp[1]*p) + f_temp[0];
        
	return data;
}

uint16 tempToReferenceAdc2(float temp, uint8* lockingAddr)
{
	float p;
	float f_temp[3];
	uint8 i, buff[4];
	uint16 data;
	p = temp;
        
	for(i=0; i<(WAVELENGTH_LOCKING_EQUATION+1); i++){
		memcpy(&buff[0], &lockingAddr[0+(i*4)],sizeof(unsigned long));
		swap_uint32(&buff[0]);
		memcpy(&f_temp[i], &buff[0], sizeof(float));	
	}
  
	data = (f_temp[2]*p*p) + (f_temp[1]*p) + f_temp[0];
        
	return data;
}

void controlLutAverageTemp(void)
{
  	uint8 i;
	sint16 s16Data;
	sint32 s32Data;
        
	if(avgCnt == 0) {
		avgCnt = AVGCNT_INDEX;
		s32Data = 0;		
		if(lutReadyFlag == ON) {
			for(i = 0; i < AVGCNT_INDEX ; i++)
			{
				s32Data += temperatureAverage[i];
			}
			s16Data = (s32Data / AVGCNT_INDEX);	
			curTemperature = (float)s16Data / 256;
			if( fabs(temperatureTemp-curTemperature) > 1.0 ){
				setTemperatureIndex(s16Data);			
				temperatureTemp = curTemperature;
			}
		}else{
			s16Data = (sint16)Reg2ByteToWordLoad(&A2_DiagData[DIAGNOSTICS_TEMP]);
			lutReadyFlag = ON;
			setTemperatureIndex(s16Data);
			temperatureTemp = (float)s16Data / 256;
		}			
	}else {
		if(avgCnt > 0) {
			avgCnt--;
		}
		temperatureAverage[avgCnt] = (sint16)Reg2ByteToWordLoad(&A2_DiagData[DIAGNOSTICS_TEMP]);
	}
}

void checkSumCalculateForA0Base(void)
{
    uint8 i=0;
    uint32 checkSum=0;
     
    for(i=0;i<CC_BASE;i++){
        checkSum += A0_SerialID[i];
    }
    
    A0_SerialID[CC_BASE] = (checkSum & 0xff);
}

void checkSumCalculateForA0Ext(void)
{
    uint8 i=0;
    uint32 checkSum=0;
     
    for(i=64;i<CC_EXT;i++){
        checkSum += A0_SerialID[i];
    }
    
    A0_SerialID[CC_EXT] = (checkSum & 0xff);
}



void externalEepromUpdate(void)
{
	uint8 i = 0, j = 0x00;

	for(i = 0; i < 8; i++)
	{
	  
		j = (0x01 << i);
		if(flashFlagByte1 & j) {
			flashFlagByte1 &= ~j;	// flashFlagByte1 can be updated by I2C ISR during updating the flash memory
			
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
#ifdef USE_EXT_EEPROM
			switch(j)
			{
				case A2_UserDataWrFlag :
				  	eepromWriteFunction(A2_UserData,A2_UserData_EEPROM);
                    flag00h = 1;
					break;
				case A2_VendorFnWrFlag :
					 eepromWriteFunction(A2_VendorFn,A2_VendorFn_EEPROM);
					break;
				case A2_GN2146SyncWrFlag :
					eepromWriteFunction(A2_GN2146Sync,A2_GN2146Sync_EEPROM);
					break;
				case A2_APCLUTABWrFlag :
					eepromWriteFunction(A2_APCLUTAB,A2_APCLUTAB_EEPROM);
					break;
				case A2_MODLUTABWrFlag :
					eepromWriteFunction(A2_MODLUTAB,A2_MODLUTAB_EEPROM);
					break;
				case A2_TECLUTABWrFlag :
					eepromWriteFunction(A2_TECLUTAB,A2_TECLUTAB_EEPROM);
					break;
				case A2_APDLUTABWrFlag :
					eepromWriteFunction(A2_APDLUTAB,A2_APDLUTAB_EEPROM);
					break;
				default :
					break;
			}
#else
           switch(j)
			{
				case A2_UserDataWrFlag :
				  	flashUpdate(A2_UserData_ADD, A2_UserData_FLASH, A2_UserData_SIZE);
					break;
				case A2_VendorFnWrFlag :
					 flashUpdate(A2_VendorFn_ADD, A2_VendorFn_FLASH, A2_VendorFn_SIZE);
					break;
				case A2_GN2146SyncWrFlag :
					flashUpdate(A2_GN2146Sync_ADD, A2_GN2146Sync_FLASH, A2_GN2146Sync_SIZE);
					break;
				case A2_APCLUTABWrFlag :
					flashUpdate(A2_APDLUTAB_ADD, A2_APDLUTAB_FLASH, A2_APDLUTAB_SIZE);
					break;
				case A2_MODLUTABWrFlag :
					flashUpdate(A2_MODLUTAB_ADD, A2_MODLUTAB_FLASH, A2_MODLUTAB_SIZE);
					break;
				case A2_TECLUTABWrFlag :
					flashUpdate(A2_TECLUTAB_ADD, A2_TECLUTAB_FLASH, A2_TECLUTAB_SIZE);
					break;
				case A2_APDLUTABWrFlag :
					flashUpdate(A2_APDLUTAB_ADD, A2_APDLUTAB_FLASH, A2_APDLUTAB_SIZE);
					break;
				default :
					break;
			}
#endif
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
		
	}

       
	for(i = 0; i < 8; i++)
	{
		j = (0x01 << i);
		if(flashFlagByte2 & j) {        // flashFlagByte2 can be updated by I2C ISR during updating the flash memory 
			flashFlagByte2 &= ~j;	
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
#ifdef USE_EXT_EEPROM
			switch(j)
			{
				case A0_SerialIDWrFlag :
					eepromWriteFunction(A0_SerialID,A0_SerialID_EEPROM);
					break;
				case A0_ReservedWrFlag :
					eepromWriteFunction(A0_Reserved,A0_Reserved_EEPROM);
					break;
				case A2_DiagDataWrFlag :
					eepromWriteFunction(A2_DiagData,A2_DiagData_EEPROM);
					break;
				case A2_DEBUGTABWrFlag :                    
				  	flashUpdate(A2_DEBUGTAB_ADD, A2_DEBUGTEAB_FLASH, A2_DEBUGTAB_SIZE);
					break;
				case A2_8690_LIMITWrFlag :	
				 	 eepromWriteFunction(A2_8690_LIMIT,A2_8690_LIMIT_EEPROM);
					break;
				case A2_TTOSAMemoryWrFlag :		
					eepromWriteFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_EEPROM + (A2_TTOSA_MEMORY_SIZE * (ttosaChannel - 1)) );				
					break;
				case TTOSAChannelWrFlag :	  	
					I2CM_16WriteNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS, (TTOSA_CHANNEL_NUMBER_EEPROM + SFF_8690_A2_02_CHANNEL),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL], 4);
					break;
                case A2_TTOSA_PAGE_0x02WrFlag :
                    eepromWriteFunction(A2_TTOSA_PAGE_0x02, A2_TTOSA_PAGE_0x02_EEPROM);
                    break;
                    
				default :
					break;
			}                   
	
#else
			switch(j)
			{
				case A0_SerialIDWrFlag :
					flashUpdate(A0_SerialID_ADD, A0_SerialID_FLASH, A0_SerialID_SIZE);
					break;
				case A0_ReservedWrFlag :
					flashUpdate(A0_Reserved_ADD, A0_Reserved_FLASH, A0_Reserved_SIZE);
					break;
				case A2_DiagDataWrFlag :
                    flashUpdate(A2_DiagData_ADD, A2_DiagData_FLASH, A2_DiagData_SIZE);
					break;
				case A2_DEBUGTABWrFlag :
				  	flashUpdate(A2_DEBUGTAB_ADD, A2_DEBUGTEAB_FLASH, A2_DEBUGTAB_SIZE);
					break;
				case A2_8690_LIMITWrFlag :	
					flashUpdate(A2_8690_LIMIT_ADD, A2_8690_LIMIT_0x40_FLASH, A2_8690_LIMIT_SIZE);
					break;
				case A2_TTOSAMemoryWrFlag :				
					flashUpdate(A2_TTOSA_MEMORY_ADD, A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (ttosaChannel - 1), A2_TTOSA_MEMORY_SIZE);
                    break;
				case TTOSAChannelWrFlag :	
                    flashUpdate(A2_TTOSA_PAGE_0x02_ADD+16, A2_TTOSA_PAGE_0x02_FLASH+16, 4);
                    break;
				case A2_TTOSA_PAGE_0x02WrFlag :
                    flashUpdate(A2_TTOSA_PAGE_0x02_ADD, A2_TTOSA_PAGE_0x02_FLASH,A2_TTOSA_PAGE_0x02_SIZE);
                    break;
					
				default :
					break;
			}
#endif
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
	}
        
}



void externalEepromLoad(void)
{

#ifdef USE_EXT_EEPROM
  eepromReadFunction(A2_UserData,A2_UserData_EEPROM);
  eepromReadFunction(A2_VendorFn,A2_VendorFn_EEPROM);
  eepromReadFunction(A2_GN2146Sync,A2_GN2146Sync_EEPROM);
  eepromReadFunction(A2_APCLUTAB,A2_APCLUTAB_EEPROM);
  eepromReadFunction(A2_MODLUTAB,A2_MODLUTAB_EEPROM);
  eepromReadFunction(A2_TECLUTAB,A2_TECLUTAB_EEPROM);
  eepromReadFunction(A2_APDLUTAB,A2_APDLUTAB_EEPROM);
  eepromReadFunction(A2_TTOSA_PAGE_0x02,A2_TTOSA_PAGE_0x02_EEPROM);
  eepromReadFunction(A0_SerialID,A0_SerialID_EEPROM);
  eepromReadFunction(A0_Reserved,A0_Reserved_EEPROM);
  eepromReadFunction(A2_DiagData,A2_DiagData_EEPROM); 
  eepromReadFunction(A2_8690_LIMIT,A2_8690_LIMIT_EEPROM); 
  FLASH_Read(A2_DEBUGTEAB_FLASH, A2_DEBUGTAB_ADD, A2_DEBUGTAB_SIZE);
  
#else
  FLASH_Read(A2_UserData_FLASH, A2_UserData_ADD, A2_UserData_SIZE);
  FLASH_Read(A2_VendorFn_FLASH, A2_VendorFn_ADD, A2_VendorFn_SIZE);
  FLASH_Read(A2_TTOSA_PAGE_0x02_FLASH, A2_TTOSA_PAGE_0x02_ADD, A2_TTOSA_PAGE_0x02_SIZE);
  FLASH_Read(A2_GN2146Sync_FLASH, A2_GN2146Sync_ADD, A2_GN2146Sync_SIZE);
  FLASH_Read(A2_APDLUTAB_FLASH, A2_APCLUTAB_ADD, A2_APCLUTAB_SIZE);
  FLASH_Read(A2_MODLUTAB_FLASH, A2_MODLUTAB_ADD, A2_MODLUTAB_SIZE);
  FLASH_Read(A2_TECLUTAB_FLASH, A2_TECLUTAB_ADD, A2_TECLUTAB_SIZE);
  FLASH_Read(A2_APDLUTAB_FLASH, A2_APDLUTAB_ADD, A2_APDLUTAB_SIZE);
  FLASH_Read(A0_SerialID_FLASH, A0_SerialID_ADD, A0_SerialID_SIZE);
  FLASH_Read(A0_Reserved_FLASH, A0_Reserved_ADD, A0_Reserved_SIZE);
  FLASH_Read(A2_DiagData_FLASH, A2_DiagData_ADD, A2_DiagData_SIZE);
  FLASH_Read(A2_DEBUGTEAB_FLASH, A2_DEBUGTAB_ADD, A2_DEBUGTAB_SIZE);
  FLASH_Read(A2_8690_LIMIT_0x40_FLASH, A2_8690_LIMIT_ADD, A2_8690_LIMIT_SIZE);
#endif
}


void checkRs0Rs1(void)
{
	uint8 temp_a, temp, temp_b, Emph[8] = {0, 3, 5, 7, 9, 11, 13, 15}, EQ[11] = {0, 4, 7, 10, 13, 16, 19, 22, 25, 28, 31};
	//uint16 modValue=0;
	//uint8 modOffset=0;
	uint8 lolValue=0;
	
	temp = A2_GN2146Sync[RX_CDR_BYPASS_CTRL_REG_0_Add];
	
	temp_b = (A2_GN2146Sync[RX_DRV_DEEMPH_CTRL_REG_0_Add] & 0x0F);
	
 
        if(A2_UserData[AMCC_LINK_STATUS] & 0x01) {		// Return while ASP is proccessing
          return;
        }

	if		(temp_b == 0)  	temp_b = 0;
	else if (temp_b == 3)   temp_b = 1;
	else if (temp_b == 5)   temp_b = 2;
	else if (temp_b == 7)   temp_b = 3;
	else if (temp_b == 9)   temp_b = 4;
	else if (temp_b == 11)  temp_b = 5;
	else if (temp_b == 13)  temp_b = 6;
	else if (temp_b == 15)  temp_b = 7;
	else 					temp_b = 99;
	
	if( (A2_DiagData[STATUS_CONTROL_ADD]&0x08) == 0x08 || RS0_HOST == _ON) 
	{ // RX Rate select on -> 25G, CDR ON Locking
	  	I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, LOSL_STATUS_REG_0, &lolValue, 1);	
		if(lolValue & 0x08)	A2_DiagData[EXT_STATUS_CDR_UNLOCKED_Add] |= 0x01;
		else				A2_DiagData[EXT_STATUS_CDR_UNLOCKED_Add] &= ~0x01;

		if(temp&0x01) 
		{
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			A2_GN2146Sync[RX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_CDR_BYPASS_CTRL_REG_0, &A2_GN2146Sync[RX_CDR_BYPASS_CTRL_REG_0_Add], 1);
#if 1			
			A2_GN2146Sync[RX_EQ_BOOST_REG_0_Add] = A2_DEBUGTAB[RX_EQ_BOOST_25G_Add];
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_EQ_BOOST_REG_0, &A2_GN2146Sync[RX_EQ_BOOST_REG_0_Add], 1);
#endif			
			//A2_DiagData[DIAGNOSTICS_TX_IN_EQ] = A2_DEBUGTAB[TX_EQ_DEF_Add];
			//A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] = A2_DEBUGTAB[RX_EM_DEF_Add];
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
		//RX emphasis
		temp = (A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] & 0xF0) >> 4;
		if(temp != temp_b)
		{
		  	
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			A2_GN2146Sync[RX_DRV_DEEMPH_CTRL_REG_0_Add] = Emph[temp];
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_DRV_DEEMPH_CTRL_REG_0, &A2_GN2146Sync[RX_DRV_DEEMPH_CTRL_REG_0_Add], 1);
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
	}
	else
	{ // RX Rate select off -> 10G, CDR Bypass
		A2_DiagData[EXT_STATUS_CDR_UNLOCKED_Add] &= ~0x01;
		if(!(temp&0x01)) 
		{	
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			A2_GN2146Sync[RX_CDR_BYPASS_CTRL_REG_0_Add] |= 0x01;
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_CDR_BYPASS_CTRL_REG_0, &A2_GN2146Sync[RX_CDR_BYPASS_CTRL_REG_0_Add], 1);
#if 1
			A2_GN2146Sync[RX_EQ_BOOST_REG_0_Add] = A2_DEBUGTAB[RX_EQ_BOOST_10G_Add];
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_EQ_BOOST_REG_0, &A2_GN2146Sync[RX_EQ_BOOST_REG_0_Add], 1);
#endif			
			//A2_DiagData[DIAGNOSTICS_TX_IN_EQ] = A2_DEBUGTAB[TX_EQ_DEF_Add];
			//A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] = A2_DEBUGTAB[RX_EM_DEF_Add];
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}	
		//RX emphasis		
		temp = A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] & 0x0F;
		if(temp != temp_b)
		{
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			A2_GN2146Sync[RX_DRV_DEEMPH_CTRL_REG_0_Add] = (Emph[temp]);
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_DRV_DEEMPH_CTRL_REG_0, &A2_GN2146Sync[RX_DRV_DEEMPH_CTRL_REG_0_Add], 1);	
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}		
	}
//	RS0 -> RX Rate select

//    if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & MSA_TUNE) {            //smart_tune 일 때 동작 
//      return;
//    }
    
	temp = A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add];
	temp_b = (A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add]) >> 1;

	if		(temp_b == 0)	temp_b = 0;
	else if (temp_b == 4)   temp_b = 1;
	else if (temp_b == 7)   temp_b = 2;
	else if (temp_b == 10)  temp_b = 3;
	else if (temp_b == 13)  temp_b = 4;
	else if (temp_b == 16)  temp_b = 5;
	else if (temp_b == 19)  temp_b = 6;
	else if (temp_b == 22)  temp_b = 7;
	else if (temp_b == 25)  temp_b = 8;
	else if (temp_b == 28)  temp_b = 9;
	else if (temp_b == 31)  temp_b = 10;
	else 					temp_b = 99;

	if( (A2_DiagData[EXT_STATUS_CONTROL_ADD]&0x08) == 0x08 || RS1_HOST == _ON) 
	{ // TX Rate select on -> 25G, CDR ON Locking
	  	I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, LOSL_STATUS_REG_0, &lolValue, 1);	
		RS1_HOST == _ON;
		if(lolValue & 0x04)	A2_DiagData[EXT_STATUS_CDR_UNLOCKED_Add] |= 0x02;
		else				A2_DiagData[EXT_STATUS_CDR_UNLOCKED_Add] &= ~0x02;

		if(A2_VendorFn[MOD_OFFSEST_EN_Add] & 0x1){
		  /* jdj
			modValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET]);
			RegWordTo2ByteSave(modValue, &A2_VendorFn[DAC_AMP_SET_Add]);
		  */
		}
		if(temp&0x01) 
		{	
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
#if 1			
			A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add] &= ~(0x3f);
			A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add] |= 0x20;
			A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add] |= A2_DEBUGTAB[TX_EYE_SHAPING_25G_Add];	
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_EYE_SHAPING_CTRL_REG_0, &A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add], 1);
#endif			
			//A2_DiagData[DIAGNOSTICS_TX_IN_EQ] = A2_DEBUGTAB[TX_EQ_DEF_Add];
			//A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] = A2_DEBUGTAB[RX_EM_DEF_Add];
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
		//TX Equalization
		temp = (A2_DiagData[DIAGNOSTICS_TX_IN_EQ] & 0xF0) >> 4;
		if(temp != temp_b)
		{
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			temp_a = A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add] &= 0x01;
			A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add] = temp_a;
			A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add] |= (EQ[temp] * 2);
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CTLE_BOOST_OVRD_REG_0, &A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add], 1);
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
	}
	else
	{ // TX Rate select off -> 10G, CDR Bypass
		A2_DiagData[EXT_STATUS_CDR_UNLOCKED_Add] &= ~0x02;
		
		/* jdj
		if(A2_VendorFn[MOD_OFFSEST_EN_Add] & 0x1){
			modValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET]);
			modOffset = A2_VendorFn[MOD_OFFSEST_Add];
			RegWordTo2ByteSave(modValue-modOffset, &A2_VendorFn[DAC_AMP_SET_Add]);
		}
		*/
		if(!(temp&0x01)) 
		{
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] |= 0x01;
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
#if 1			
			A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add] &= ~(0x3f);
			A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add] |= A2_DEBUGTAB[TX_EYE_SHAPING_10G_Add];	
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_EYE_SHAPING_CTRL_REG_0, &A2_GN2146Sync[TX_EYE_SHAPING_CTRL_REG_0_Add], 1);
#endif
			//A2_DiagData[DIAGNOSTICS_TX_IN_EQ] = A2_DEBUGTAB[TX_EQ_DEF_Add];
			//A2_DiagData[DIAGNOSTICS_RX_OUT_EMPH] = A2_DEBUGTAB[RX_EM_DEF_Add];
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}	
		//TX Equalization
		temp = A2_DiagData[DIAGNOSTICS_TX_IN_EQ] & 0x0F;
		if(temp != temp_b)
		{	
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
			temp_a = A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add] &= 0x01;
			A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add] = temp_a;
			A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add] |= (EQ[temp] * 2);
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CTLE_BOOST_OVRD_REG_0, &A2_GN2146Sync[TX_CTLE_BOOST_OVRD_REG_0_Add], 1);
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
		}
	}
//	RS1 -> TX Rate select	
}

void shutDownRecovery(void)
{
    float currentTemp = 0;
//    float tempL, tempH = 0;
    float shutDownTemp = 0;
    
    currentTemp  = DdmTemp/256.0; 
//    tempL = (sint16)Reg2ByteToWordLoad(&A2_DEBUGTAB[RECOVERY_LOW_TEMP]);
//    tempL = tempL/256.0;
//    tempH = (sint16)Reg2ByteToWordLoad(&A2_DEBUGTAB[RECOVERY_HIGH_TEMP]);
//    tempH = tempH/256.0;
    
    shutDownTemp = (sint16)Reg2ByteToWordLoad(&A2_DEBUGTAB[SHUTDOWN_TEMP]);
    shutDownTemp = shutDownTemp/256.0;
    
    currentTempBuff = (sint16)Reg2ByteToWordLoad(&A2_DEBUGTAB[RECOVERY_TEMP]);
    currentTempBuff = currentTempBuff/256.0;
    
    if((A2_DEBUGTAB[SHUTDOWN_ENABLE] & 0x01) == 0x01) {
      if(currentTemp > shutDownTemp){
        A2_DEBUGTAB[SHUTDOWN_FLAG] = _ON;
        TX_FAULT_HOST = FAULT_ON;
        APDCN1 = 0x0040;            				// APD OFF
        currentTempBuff = currentTemp;
        RegWordTo2ByteSave((sint16)(currentTempBuff*256),&A2_DEBUGTAB[RECOVERY_TEMP]);        
      }else if(currentTemp < currentTempBuff){
        A2_DEBUGTAB[SHUTDOWN_FLAG] = _OFF;
        TX_FAULT_HOST = FAULT_OFF;
        APDCN1 = 0x0044;              				// APD ON          
      }
    }    
}
