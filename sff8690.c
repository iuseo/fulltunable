/*
 * FILE NAME            : sff8690.c
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
#include <math.h>
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appWrapper.h"
#include "sff8690.h"
#include "tl6100.h"
#include "appGpio.h"
#include "appUart.h"
#include "appSmartTune.h"
/*
 * Name         : processTable0x02
 * Description  : 
 * Return Value : void
 * Parameter    : uint8, uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 23 2019
 * History      : 
 */

static uint16 preChannel=0;
extern SFF_8690_T sff8690Info;
uint8 flag02h = 0;
uint8 flag00h = 0;




void processTable0x02(uint8 startAddress, uint8 numberOfBytes)
{
	uint8 i;
	uint8 address;
//    A2_DEBUGTAB[0x8E - 128] = startAddress;
	for(i = 0; i < numberOfBytes; i++)
	{
		address = startAddress + i;
	
        
        switch(address - 0x80)
		{   
			case SFF_8690_A2_02_FUNCTION_CONTROL:
				sendMessage(getMessageQueue(), EVENT_SFP_8690_LOOPBACK, 0);
		  		break;
			case (SFF_8690_A2_02_CHANNEL + 1) :
				sendMessage(getMessageQueue(), EVENT_SFF_8690_CHANNEL, 0);
#ifdef ADDON_JMA_WIRELESS
                flag02h = 1;
#endif
				break;
            case (SFF_8690_A2_02_WAVELENGTH + 1) :
                sendMessage(getMessageQueue(), EVENT_SFF_8690_FREQUENCY, 0);
				break;
			case SFF_8690_A2_02_DITHERING : 
			case SFF_8690_A2_02_TX_DITHER :
            case SFF_8690_A2_02_LFL1 :
			case SFF_8690_A2_02_LFL1 + 1 :
            case SFF_8690_A2_02_LFL2 :
			case SFF_8690_A2_02_LFL2 + 1 :
            case SFF_8690_A2_02_LFH1 :
			case SFF_8690_A2_02_LFH1 + 1 :
            case SFF_8690_A2_02_LFH2 :
			case SFF_8690_A2_02_LFH2 + 1:
            case SFF_8690_A2_02_LGRID :
			case SFF_8690_A2_02_LGRID  + 1:
			  flashFlagByte2 |= A2_TTOSA_PAGE_0x02WrFlag; 
              flag02h = 1;
			default :
				break;
		}
	}
}

/*
 * Name         : processSFF8690
 * Description  : 
 * Return Value : void
 * Parameter    : EVENT_LIST, uint16
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 23 2019
 * History      : 
 */
#if 0	/* jhRoh */
void processSFF8690(EVENT_LIST event, uint16 data)
#else
void processSFF8690(EVENT_LIST event)
#endif
{
	uint16 channel=0,wavelength_data=0;
    float frequency=0,wavelength=0,calculationData=0;
    sint32 dataTempBuffer = 0;
    uint16 channelData=0;
    uint8 data = 0;
	uint16 msaMaxChannel = getMsaMaxChannel();
	//SFF_8690_T sff8690Info;
	
	switch(event)
	{
#if 1	  
	  	case EVENT_SFP_8690_LOOPBACK:
		  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
		  	if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] & 0x02){
			  	data = 0x00;
			  	I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_PD_DRV_REG_1, &data, 1);
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_DRV_MUTE_CTRL_REG_0, &data, 1);
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_PD_EQ_REG_0, &data, 1);
			}else if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] & 0x01){
				data = 0x01;
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_PD_DRV_REG_1, &data, 1);
				data = 0x01;
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_DRV_MUTE_CTRL_REG_0, &data, 1);
				data = 0x18;
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_PD_EQ_REG_0, &data, 1);
				data = 0x00;
			  	I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_PD_DRV_REG_1, &data, 1);
			  
			}else if( (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] & 0x3) == 0 ){
			  	data = 0x01;
                I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_PD_DRV_REG_1, &data, 1);
				data = 0x01;
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_DRV_MUTE_CTRL_REG_0, &data, 1);
				data = 0x18;
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_PD_EQ_REG_0, &data, 1);
				data = 0x08;
			  	I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_PD_DRV_REG_1, &data, 1);
			}
			A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
#endif			
		 	break;
		case EVENT_SFF_8690_CHANNEL :
			channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
			//A2_DEBUGTAB[0x20] = channel;
			//if((channel == 0) || (channel > MAX_WAVELENGTH_CHANNEL)) {
			if((channel == 0) || (channel > msaMaxChannel)) {
			  A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] |= 0x10;    // SFF-8690 Bad Channel flag set
              if(channel > msaMaxChannel) {
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL] = 0x00;
                A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL + 1] = 0x30;
#ifdef ADDON_JMA_WIRELESS
                A2_UserData[SFF_8690_A2_00_CHANNEL + 1] = 0x30;
#endif
              }
              if(channel == 0) {
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL] = 0x00;
                A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL + 1] = 0x01;
#ifdef ADDON_JMA_WIRELESS                
                 A2_UserData[SFF_8690_A2_00_CHANNEL + 1] = 0x01;
#endif
              }
			  break;
			}
			if(preChannel != channel){
//				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] |= 0x10;     // SFF-8690 Tx Tune flag set 
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] |= 0x20;     // SFF-8690 New Channel flag set
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] |= 0x8;      // SFF-8690 Wavlength Unlock flag set
				wavelengthMonitoring(channel);
			}              
			//A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] = channel;
			A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] = getTunableTosaChannel(channel);
			preChannel = channel;
			flashFlagByte2 |= TTOSAChannelWrFlag;
			sendMessage(getMessageQueue(), EVENT_TL6100_TOSA_CONFIG_LOAD, 0);
			break;
		case EVENT_SFF_8690_FREQUENCY :
			wavelength_data = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_WAVELENGTH]);
			wavelength = (float)wavelength_data*0.05;
			frequency = SPEED_OF_LIGHT / wavelength;
			dataTempBuffer = (sint32)frequency;
			//getFrequencyInformation(&sff8690Info);
			//calculationData = ((float)(dataTempBuffer-LASER_FIRST_FREQENCY)) / LASER_GRID_SPACING;
			calculationData = ((float)(dataTempBuffer-sff8690Info.firstFrequency)) / sff8690Info.gridFrequency;
			channelData = 1 + round(calculationData);
#ifdef ADDON_JMA_WIRELESS
            RegWordTo2ByteSave(channelData, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
            RegWordTo2ByteSave(channelData, &A2_UserData[SFF_8690_A2_00_CHANNEL]);
#else
			RegWordTo2ByteSave(channelData, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
#endif
            sendMessage(getMessageQueue(), EVENT_SFF_8690_CHANNEL, 0);
			break;
		default :
			break;
	}
	
	if(A2_DiagData[TABLE_SEL_Add] == 0x02){
		memcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_TTOSA_PAGE_0x02_ADD, A2_TTOSA_PAGE_0x02_SIZE); // Change upper memory for 0x2 table 
	}
}

/*
 * Name         : loadTosaConfiguration
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 23 2019
 * History      : 
 */
void loadTosaConfiguration(void)
{
	uint16 channel = 0;
	/* Copy to A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL] */
          
#ifdef USE_EXT_EEPROM
    I2CM_16ReadNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS, TTOSA_CHANNEL_NUMBER_EEPROM+SFF_8690_A2_02_CHANNEL ,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL], 4);
#else
    FLASH_Read(A2_TTOSA_PAGE_0x02_FLASH+16, A2_TTOSA_PAGE_0x02_ADD + SFF_8690_A2_02_CHANNEL, 4);  
#endif
    channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
	//추가	
	
	//channel = 2;
	wavelengthMonitoring(channel);
	//if((channel == 0) || (channel > MAX_WAVELENGTH_CHANNEL)) {
	if((channel == 0) || (channel > sff8690Info.msaMaxChannel)) {
		lasterStartFlag = LASER_START_NOT_READY;
		RegWordTo2ByteSave(0x36E6, &A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET]);
	}else{
		channel = getTunableTosaChannel(channel);
#ifdef USE_EXT_EEPROM
    eepromReadFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_EEPROM + A2_TTOSA_MEMORY_SIZE * (channel - 1));
#else
    FLASH_Read(A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (channel - 1),A2_TTOSA_MEMORY_ADD, A2_TTOSA_MEMORY_SIZE); 
#endif
	}

	preChannel = channel;
	/* Copy to A2_TTOSA_MEMORY[] */
	//FLASH_Read(A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (channel - 1), A2_TTOSA_MEMORY_ADD, A2_TTOSA_MEMORY_SIZE);
    
}

/*
 * Name         : saveTosaChannelNumber
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 25 2019
 * History      : 
 */
#if 0
void saveTosaChannelNumber(void)
{ 
	/* Write to flash memory from A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL] */
	flashUpdate(A2_TTOSA_PAGE_0x02_ADD + SFF_8690_A2_02_CHANNEL, TTOSA_CHANNEL_NUMBER_FLASH, 4);
}
#endif
/*
 * Name         : writeLaserInformation
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 25 2019
 * History      : 
 */
#if 0
//사용하지 않는 함수 jdj
void writeLaserInformation(void)
{
	RegWordTo2ByteSave(LASER_FIRST_FREQENCY / 1000, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);			/* THz */
	RegWordTo2ByteSave((LASER_FIRST_FREQENCY % 1000) * 10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);	/* 0.1 GHz */
        
	RegWordTo2ByteSave(LASER_LAST_FREQENCY / 1000, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);			/* THz */
	RegWordTo2ByteSave((LASER_LAST_FREQENCY % 1000) * 10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);	/* 0.1 GHz */
        
	RegWordTo2ByteSave(LASER_GRID_SPACING*10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);				/* LGrid */
}
#endif

void wavelengthMonitoring(uint16 channel)
{
    
    float wavelength=0.0, wavelengthDec=0.0;
    uint16 wavelengthData=0;
	
#if defined(AXIOM) || defined(ADDON_JMA_WIRELESS)
	float wavelengthPN=0.0;
	uint8 i = 0, j = 0;
	uint16 wavelengthDataPN = 0;
	uint16 wavelengthArr[5] = {0, };
    uint16 PnData[8] = {0, }; 
#endif 
    float wavelengthDataNm = 0.0;
    wavelength = (float)SPEED_OF_LIGHT / (float)(((channel-1)*sff8690Info.gridFrequency)+sff8690Info.firstFrequency);
    wavelengthData = wavelength;
#if defined(ADDON_JMA_WIRELESS) 
    if(A2_DEBUGTAB[WAVE_REPORTING_ENABLE] == 0) {
        RegWordTo2ByteSave(wavelengthData, &A0_SerialID[A0_WAVELENGTH]);
    } else if(A2_DEBUGTAB[WAVE_REPORTING_ENABLE] == 1) {
        wavelengthDataNm = (wavelength / 0.05);  
        RegWordTo2ByteSave(wavelengthDataNm, &A0_SerialID[A0_WAVELENGTH]);
    }
#elif defined(SAMSUNG)
//	float wavelengthSamsung = 0.0;
//	uint16 wavelengthDataSamsung;
//	wavelengthDataSamsung = (wavelength / 0.05);
//	wavelengthDataSamsung *= 0.05;
//	wavelengthSamsung = (wavelengthSamsung - (wavelengthSamsung/1))*100;
//	RegWordTo2ByteSave(wavelengthData, &A0_SerialID[A0_WAVELENGTH]);
//	A0_SerialID[A0_WAVELENGTH + 2] = wavelengthSamsung;

#else	
	RegWordTo2ByteSave(wavelengthData, &A0_SerialID[A0_WAVELENGTH]);
#endif
    
#ifdef AXIOM
	wavelengthDataPN = wavelength;
	wavelengthDataPN = (wavelengthDataPN / 100) * 100;
	wavelengthPN = wavelength - wavelengthDataPN; 			//remain xx.xxx
	
	wavelengthPN = (wavelengthPN * 100) + 0.5;
	wavelengthDataPN = floor(wavelengthPN);
	wavelengthArr[0] = '-';

	for(i=0;i<16;i++){										//add wavelength data to PN
	  if(A0_SerialID[A0_PN + i] == 0x20){
		for(j=0;j<5;j++){
			if(j == 0){
				A0_SerialID[A0_PN + i + j] = wavelengthArr[j];
			}else{
				wavelengthArr[j] = wavelengthDataPN / (pow(10, 4 - j));
				A0_SerialID[A0_PN + i + j] = wavelengthArr[j] + 0x30;
				wavelengthDataPN = wavelengthDataPN - (wavelengthArr[j] * pow(10, 4 - j));
			}
		}
	  }else if(A0_SerialID[A0_PN + i] == 0xFF){
		;
	  }else{
		;
	  }
	}
#endif   
    
#ifdef ADDON_JMA_WIRELESS
    if(A2_DEBUGTAB[WAVE_PN_REPORTING_ENABLE] == 1) {
        wavelengthDataPN =(uint16)wavelength;    
        PnData[0] = (wavelengthDataPN / 1000);
        A0_SerialID[48] = charToDec(PnData[0]);
        PnData[1] = (wavelengthDataPN / 100) - (PnData[0] * 10);
        A0_SerialID[49] = charToDec(PnData[1]);
        PnData[2] = (wavelengthDataPN / 10) - (PnData[0] * 100) - (PnData[1] * 10);
        A0_SerialID[50] = charToDec(PnData[2]);
        PnData[3] = (wavelengthDataPN % 10); 
        A0_SerialID[51] = charToDec(PnData[3]);
        PnData[4] = 0x2E;
        A0_SerialID[52] = PnData[4];
    } else {
        ;
    }
#endif
	wavelengthDec = wavelength;
	wavelengthDec -= wavelengthData;
	wavelengthDec *= 100;
	wavelengthDec = floor(wavelengthDec + 0.5);
#ifdef ADDON_JMA_WIRELESS
    if(A2_DEBUGTAB[WAVE_REPORTING_ENABLE] == 0) {
	    A0_SerialID[A0_WAVELENGTH + 2] = wavelengthDec; //파장 소수부 표시
    } else if(A2_DEBUGTAB[WAVE_REPORTING_ENABLE] == 1) {
        A0_SerialID[A0_WAVELENGTH + 2] = 0x00;      //default 
    }
#elif defined(SAMSUNG)
	
#else 
    A0_SerialID[A0_WAVELENGTH + 2] = wavelengthDec; //파장 소수부 표시
#endif
    
#ifdef ADDON_JMA_WIRELESS
    if(A2_DEBUGTAB[WAVE_PN_REPORTING_ENABLE] == 1) {
        wavelengthDataPN = (int)wavelengthDec;
        PnData[5] = (wavelengthDataPN / 10);
        A0_SerialID[53] = charToDec(PnData[5]);
        PnData[6] = (wavelengthDataPN % 10);
        A0_SerialID[54] = charToDec(PnData[6]);
        PnData[7] = 0x20;
        A0_SerialID[55] = charToDec(PnData[7]);
    } else {
        ;
    }

#endif
    wavelength = wavelength / 0.05;
    wavelengthData = wavelength;
    RegWordTo2ByteSave(wavelengthData, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_WAVELENGTH]);
	
#ifdef SAMSUNG	
	double wavelengthSamsung = 0.000;
	double wavelengthSamsung2 = 0.000;
	double wavelengthSamsung3 = 0.000;

	uint32 checkSum;
	uint8 i;
	RegWordTo2ByteSave(wavelengthData * 0.05, &A0_SerialID[A0_WAVELENGTH]);	
	wavelengthSamsung = wavelengthData*0.05;
//	wavelengthSamsung = (wavelengthSamsung - (uint16)(wavelengthSamsung));
	
//	wavelengthSamsung *= 100.0;
//	wavelengthDecimal = wavelengthSamsung *100;
//	wavelengthtest = (wavelengthData/20);
//	wavelengthData -= wavelengthtest*20;
//	wavelengthData *= 5;
	
	wavelengthSamsung2 = wavelengthSamsung;
	wavelengthSamsung3 = (uint16)(wavelengthSamsung);
	wavelengthSamsung = (wavelengthSamsung2*100) - (wavelengthSamsung3*100);
	A0_SerialID[A0_WAVELENGTH+2] = wavelengthSamsung;
	
	for(i=0;i<CC_BASE;i++){		//checksum update
    	checkSum += A0_SerialID[i];
    }
    A0_SerialID[CC_BASE] = (checkSum & 0xff);
	
#endif
}

void getFrequencyInformation(SFF_8690_T* sff8690)
{
#ifdef ADDON_JMA_WIRELESS
  //data가 없다면
  if((Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) == 0xffff) || (Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LGRID]) == 0xffff)) {
	//default 100GHz
	sff8690->gridFrequency = GRID_SPACING_100GHz;
  } else {
	sff8690->gridFrequency = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) / 10;	
  }
  
  //data가 없다면
  
  if((Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]) == 0xffff) || (Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LFL1]) == 0xffff)) {
	//firtst frequency 191400으로 적용
	sff8690->firstFrequency = 191400;
	RegWordTo2ByteSave(0x00BF, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
    RegWordTo2ByteSave(0x00BF, &A2_UserData[SFF_8690_A2_00_LFL1]);
	RegWordTo2ByteSave(0x0FA0, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
    RegWordTo2ByteSave(0x0FA0, &A2_UserData[SFF_8690_A2_00_LFL2]);
  } else {
	sff8690->firstFrequency = (uint32)(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1])) * 1000;
	sff8690->firstFrequency += Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]) * 0.1;
  }

  
  //limit first frequency 보다 작은 first frequency가 들어가 있다면  
  if((uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add])) * 50 > sff8690->firstFrequency) {
	//limit first frequency로 적용
	sff8690->firstFrequency = (uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add])) * 50;
	RegWordTo2ByteSave((uint16)(sff8690->firstFrequency / 1000), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
    RegWordTo2ByteSave((uint16)(sff8690->firstFrequency / 1000), &A2_UserData[SFF_8690_A2_00_LFL1]);
	RegWordTo2ByteSave((uint16)((sff8690->firstFrequency - (sff8690->firstFrequency /1000) * 1000) * 10), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
    RegWordTo2ByteSave((uint16)((sff8690->firstFrequency - (sff8690->firstFrequency /1000) * 1000) * 10), &A2_UserData[SFF_8690_A2_00_LFL2]);
  }
 
  //data가 없다면
  if((Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]) == 0xffff) || (Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LFH1]) == 0xffff)) {
	//last frequency 196100으로 적용
	sff8690->lastFrequency = 196100;
	RegWordTo2ByteSave(0x00C4, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);
    RegWordTo2ByteSave(0x00C4, &A2_UserData[SFF_8690_A2_00_LFH1]);
	RegWordTo2ByteSave(0x03e8, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);
    RegWordTo2ByteSave(0x03e8, &A2_UserData[SFF_8690_A2_00_LFH2]);
  } else {
	sff8690->lastFrequency = (uint32)(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1])) * 1000; 
	sff8690->lastFrequency += Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]) * 0.1;	
  }

  
  //limit last frequency 보다 큰 last frequency가 들어가 있다면  
  if((uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add])) * 50 < sff8690->lastFrequency) {
	//limit last frequency로 적용
	sff8690->lastFrequency = (uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add])) * 50;
	RegWordTo2ByteSave((uint16)(sff8690->lastFrequency / 1000), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);
    RegWordTo2ByteSave((uint16)(sff8690->lastFrequency / 1000), &A2_UserData[SFF_8690_A2_00_LFH1]);
	RegWordTo2ByteSave((uint16)((sff8690->lastFrequency - (sff8690->lastFrequency /1000) * 1000) * 10), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);
    RegWordTo2ByteSave((uint16)((sff8690->lastFrequency - (sff8690->lastFrequency /1000) * 1000) * 10), &A2_UserData[SFF_8690_A2_00_LFH2]);
  }
  
  if((sff8690->gridFrequency != GRID_SPACING_050GHz) && (sff8690->gridFrequency != GRID_SPACING_100GHz)) {
	sff8690->gridFrequency = GRID_SPACING_100GHz;
  }
  
  
  if(A2_8690_LIMIT[LIMIT_GRID_SPACING_Add] == GRID_SPACING_100GHz) {
	sff8690->gridFrequency = GRID_SPACING_100GHz;
	RegWordTo2ByteSave(sff8690->gridFrequency * 10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);
    RegWordTo2ByteSave(sff8690->gridFrequency * 10, &A2_UserData[SFF_8690_A2_00_LGRID]);
  }
  
  //sff8690->tTosaFirstChannel = 1 + (sff8690->firstFrequency - sff8690->baseFrequency) / sff8690->gridFrequency;
  sff8690->tTosaFirstChannel = 1 + (sff8690->firstFrequency - sff8690->baseFrequency) / GRID_SPACING_050GHz;
  sff8690->tTosaLastChannel = sff8690->tTosaFirstChannel + (sff8690->lastFrequency - sff8690->firstFrequency) / GRID_SPACING_050GHz;
  
  sff8690->msaMaxChannel = 1 + (sff8690->lastFrequency - sff8690->firstFrequency) / sff8690->gridFrequency;
  if(sff8690->gridFrequency == GRID_SPACING_100GHz) {
	//sff8690->tTosaFirstChannel *= 2;	
	/*                                                                                        
	if((sff8690->firstFrequency % GRID_SPACING_100GHz) != 0) {
	  sff8690->tTosaFirstChannel -= 1;
	}
	*/
  }  
#else 
  //data가 없다면
  if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) == 0xffff) {
	//default 100GHz
	sff8690->gridFrequency = GRID_SPACING_100GHz;
  } else {
	sff8690->gridFrequency = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) / 10;	
  }
  
  //data가 없다면
  if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]) == 0xffff) {
	//firtst frequency 191400으로 적용
	sff8690->firstFrequency = 191400;
	RegWordTo2ByteSave(0x00BF, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
	RegWordTo2ByteSave(0x0FA0, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
  } else {
	sff8690->firstFrequency = (uint32)(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1])) * 1000;
	sff8690->firstFrequency += Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]) * 0.1;
  }

  
  //limit first frequency 보다 작은 first frequency가 들어가 있다면  
  if((uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add])) * 50 > sff8690->firstFrequency) {
	//limit first frequency로 적용
	sff8690->firstFrequency = (uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_FISRT_FREQUENCY_Add])) * 50;
	RegWordTo2ByteSave((uint16)(sff8690->firstFrequency / 1000), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
	RegWordTo2ByteSave((uint16)((sff8690->firstFrequency - (sff8690->firstFrequency /1000) * 1000) * 10), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
  }
 
  //data가 없다면
  if(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]) == 0xffff) {
	//last frequency 196100으로 적용
	sff8690->lastFrequency = 196100;
	RegWordTo2ByteSave(0x00C4, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
	RegWordTo2ByteSave(0x03e8, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
  } else {
	sff8690->lastFrequency = (uint32)(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1])) * 1000; 
	sff8690->lastFrequency += Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]) * 0.1;	
  }

  
  //limit last frequency 보다 큰 last frequency가 들어가 있다면  
  if((uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add])) * 50 < sff8690->lastFrequency) {
	//limit last frequency로 적용
	sff8690->lastFrequency = (uint32)(Reg2ByteToWordLoad(&A2_8690_LIMIT[LIMIT_LAST_FREQUENCY_Add])) * 50;
	RegWordTo2ByteSave((uint16)(sff8690->lastFrequency / 1000), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);
	RegWordTo2ByteSave((uint16)((sff8690->lastFrequency - (sff8690->lastFrequency /1000) * 1000) * 10), &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);
  }
  
  if((sff8690->gridFrequency != GRID_SPACING_050GHz) && (sff8690->gridFrequency != GRID_SPACING_100GHz)) {
	sff8690->gridFrequency = GRID_SPACING_100GHz;
  }
  
  
  if(A2_8690_LIMIT[LIMIT_GRID_SPACING_Add] == GRID_SPACING_100GHz) {
	sff8690->gridFrequency = GRID_SPACING_100GHz;
	RegWordTo2ByteSave(sff8690->gridFrequency * 10, &A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);
  }
  
  //sff8690->tTosaFirstChannel = 1 + (sff8690->firstFrequency - sff8690->baseFrequency) / sff8690->gridFrequency;
  sff8690->tTosaFirstChannel = 1 + (sff8690->firstFrequency - sff8690->baseFrequency) / GRID_SPACING_050GHz;
  sff8690->tTosaLastChannel = sff8690->tTosaFirstChannel + (sff8690->lastFrequency - sff8690->firstFrequency) / GRID_SPACING_050GHz;
  
  sff8690->msaMaxChannel = 1 + (sff8690->lastFrequency - sff8690->firstFrequency) / sff8690->gridFrequency;
  if(sff8690->gridFrequency == GRID_SPACING_100GHz) {
	//sff8690->tTosaFirstChannel *= 2;	
	/*                                                                                        
	if((sff8690->firstFrequency % GRID_SPACING_100GHz) != 0) {
	  sff8690->tTosaFirstChannel -= 1;
	}
	*/
  }  
#endif
}

uint16 getMsaMaxChannel()
{
  uint16 gridFrequency = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]) / 10;
  
  uint32 firstFrequency = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]) * 1000;
  firstFrequency += Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]) * 0.1;
  
  uint32 lastFrequency = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]) * 1000; 
  lastFrequency += Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]) * 0.1;
  
  return (1 + (lastFrequency - firstFrequency) / gridFrequency);
}

uint16 getTunableTosaChannel(uint16 sff8690Channel)
{
  uint16 tTosaChannel = 0;
  //RegWordTo2ByteSave(sff8690Info.firstFrequency, &A2_DEBUGTAB[0x20]);
  //RegWordTo2ByteSave(sff8690Info.baseFrequency, &A2_DEBUGTAB[0x30]);
  //tTosaChannel = 1 + (sff8690->firstFrequency - sff8690->baseFrequency) / sff8690->gridFrequency;
   
  if(sff8690Info.firstFrequency < sff8690Info.baseFrequency)
  {
	return 1;
  }
//  RegWordTo2ByteSave(sff8690Info.firstFrequency >> 16, &A2_DEBUGTAB[0xC8 - 128]);
//  RegWordTo2ByteSave(sff8690Info.firstFrequency & 0xffff, &A2_DEBUGTAB[0xCA - 128]);
//  RegWordTo2ByteSave(sff8690Info.baseFrequency >> 16, &A2_DEBUGTAB[0xCC - 128]);
//  RegWordTo2ByteSave(sff8690Info.baseFrequency & 0xffff, &A2_DEBUGTAB[0xCE - 128]);

  //모든 data를 넣으므로 50Ghz를 default로 잡아 채널 계산 수행  
  tTosaChannel = 1 + (sff8690Info.firstFrequency - sff8690Info.baseFrequency) / GRID_SPACING_050GHz;
  
  if(sff8690Info.gridFrequency == GRID_SPACING_100GHz){
	tTosaChannel += (sff8690Channel - 1) * 2;
  } else {
	tTosaChannel += sff8690Channel - 1;
  }

  /*
  if(sff8690->gridFrequency == GRID_SPACING_100GHz){
	tTosaChannel *= 2;
	
	if((sff8690->firstFrequency % GRID_SPACING_100GHz) != 0){
	  tTosaChannel -= 1;
	}
  }*/
  return tTosaChannel;
}

unsigned char charToDec(unsigned char bDec)
{
    bDec += 0x30;
	return bDec;
}

#ifdef ADDON_JMA_WIRELESS
void load00hTo02h(void)
{  
  if(flag02h == 1) {
    flag02h = 0;
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]),&A2_UserData[SFF_8690_A2_00_LGRID]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]),&A2_UserData[SFF_8690_A2_00_LFL1]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]),&A2_UserData[SFF_8690_A2_00_LFL2]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]),&A2_UserData[SFF_8690_A2_00_LFH1]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]),&A2_UserData[SFF_8690_A2_00_LFH2]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]),&A2_UserData[SFF_8690_A2_00_CHANNEL]);  
  }

  if(flag00h == 1) {
    flag00h = 0;
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LGRID]),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LGRID]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LFL1]),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL1]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LFL2]),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFL2]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LFH1]),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH1]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_LFH2]),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LFH2]);
    RegWordTo2ByteSave(Reg2ByteToWordLoad(&A2_UserData[SFF_8690_A2_00_CHANNEL]),&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
    sendMessage(getMessageQueue(), EVENT_SFF_8690_CHANNEL, 0);   
  }
}
#endif
