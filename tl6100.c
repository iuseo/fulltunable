/*
 * FILE NAME            : tl6100.c
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
#include "tl6100.h"
#include "sff8690.h"
#include "appUart.h"
#include "appSmartTune.h"
#include "stdlib.h"

uint8 ttosaChannel = 0;
extern uint8 changeAspFlag;

extern uint16 lddRegMap[DACNum][2];
/*
 * Name         : initializeTL6100
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 22 2019
 * History      : 
 */
#if 0
//사용하지 않는 코드
void initializeTL6100(void)
{
	if((A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] == 0) || (A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] > MAX_WAVELENGTH_CHANNEL)) {
		A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] = MAX_WAVELENGTH_CHANNEL;
	}
}
#endif
/*
 * Name         : processTable0x30
 * Description  : 
 * Return Value : void
 * Parameter    : uint8, uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 23 2019
 * History      : 
 */
void processTable0x30(uint8 startAddress, uint8 numberOfBytes)
{
	uint8 i;
	uint8 address;
	uint8 data;

	for(i = 0; i < numberOfBytes; i++)
	{
		address = startAddress + i;
		switch(address - 0x80)
		{
			case REG_TTOSA_FRONT_PAIR :
				sendMessage(getMessageQueue(), EVENT_TL6100_FRONT_PAIR, 0);
				break;
			case (REG_TTOSA_CTRL_FRONT_ODD + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_FRONT_ODD, 0);
				break;
			case (REG_TTOSA_CTRL_FRONT_EVEN + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_FRONT_EVEN, 0);
				break;
			case (REG_TTOSA_CTRL_PHASE + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_PHASE, 0);
				break;
			case (REG_TTOSA_CTRL_REAR + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_REAR, 0);
				break;
			case (REG_TTOSA_CTRL_GAIN + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_GAIN, 0);
				break;
			case (REG_TTOSA_CTRL_FRONT_SOA + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_FRONT_SOA, 0);
				break;
			case (REG_TTOSA_CTRL_IMB_LEFT + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_IMB_LEFT, 0);
				break;
			case (REG_TTOSA_CTRL_IMB_RIGHT + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_IMB_RIGHT, 0);
				break;
			case (REG_TTOSA_CTRL_BIAS_L + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_BIAS_LEFT, 0);     
				break;
			case (REG_TTOSA_CTRL_BIAS_R + 1) :
				sendMessage(getMessageQueue(), EVENT_TL6100_BIAS_RIGHT, 0);
				break;		
			case REG_WAVELENGTH_COMMAND :
				data = A2_TTOSA_MEMORY[REG_WAVELENGTH_COMMAND];
				if(data == GUI_TOSA_CONFIG_SAVE) {
					sendMessage(getMessageQueue(), EVENT_TL6100_TOSA_CONFIG_SAVE, 0);
				} else if(data == GUI_TOSA_CONFIG_LOAD) {
					sendMessage(getMessageQueue(), EVENT_TL6100_TOSA_CONFIG_LOAD, 0);
				} 
				A2_TTOSA_MEMORY[REG_WAVELENGTH_COMMAND] = 0x00;
				break;
			default :
				break;
		}
	}
}

/*
 * Name         : processTL6100
 * Description  : 
 * Return Value : void
 * Parameter    : EVENT_LIST, uint16
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 23 2019
 * History      : 
 */
#if 0	/* jhRoh */
void processTL6100(EVENT_LIST event, uint16 data)
#else
void processTL6100(EVENT_LIST event)
#endif
{
	static uint8 pair = 0;
	uint16 value = 0;
	uint8 channel = 0;
	uint16 msaMaxChannel = getMsaMaxChannel();
	
	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
	
	switch(event)
	{	
		case EVENT_TL6100_FRONT_PAIR :
			pair = A2_TTOSA_MEMORY[REG_TTOSA_FRONT_PAIR];
			setOutputMuxMax5112(pair - 1);
			break;
		case EVENT_TL6100_FRONT_ODD :
			if((pair >= FS_PAIR_MINIMUM) && (pair <= FS_PAIR_MAXIMUM)) {		/* 1 ~ 7 */
				if((pair % 2) != 0) {	/* Odd */
					value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_ODD]);
				} else {				/* Even */
					value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_EVEN]);
				}
				writeMax5112(DAC_FRONT_ODD_CODE, value);
			}
			break;
		case EVENT_TL6100_FRONT_EVEN :
			if((pair >= FS_PAIR_MINIMUM) && (pair <= FS_PAIR_MAXIMUM)) {		/* 1 ~ 7 */
				if((pair % 2) != 0) {	/* Odd */
					value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_ODD]);
				} else {				/* Even */
					value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_EVEN]);
				}
				writeMax5112(DAC_FRONT_EVEN_CODE, value);
			}
			break;
		case EVENT_TL6100_PHASE :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
			writeMax5112(DAC_PHASE_CODE, value);
			break;
		case EVENT_TL6100_REAR :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]);
			writeMax5112(DAC_REAR_CODE, value);
			break;
		case EVENT_TL6100_GAIN :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_GAIN]);
			writeMax5112(DAC_GAIN_CODE, value);
			break;
		case EVENT_TL6100_FRONT_SOA :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]);
			writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, value);
			break;
		case EVENT_TL6100_IMB_LEFT :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_IMB_LEFT]);
			writeMax5112(DAC_IMB_LEFT_CODE, value);
			break;
		case EVENT_TL6100_IMB_RIGHT :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_IMB_RIGHT]);
			writeMax5112(DAC_IMB_RIGHT_CODE, value);
			break;
		case EVENT_TL6100_BIAS_LEFT :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_BIAS_L]);
			setBias(SET_LEFT);
			break;
		case EVENT_TL6100_BIAS_RIGHT :
			value = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_BIAS_R]);
			setBias(SET_RIGHT);
			break;
		case EVENT_TL6100_TOSA_CONFIG_SAVE :
		 	channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
			//channel = A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL];			
			//if((channel == 0) || (channel > MAX_WAVELENGTH_CHANNEL)) {
			if((channel == 0) || (channel > msaMaxChannel)) {
			  //범위 밖 채널일 경우 break;
				break;
			}		
			channel = A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL];			
			memcpy(&A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET], &A2_VendorFn[DAC_TEC_SET_Add], 2);        
			memcpy(&A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET], &A2_VendorFn[DAC_AMP_SET_Add], 2);                    
			A2_TTOSA_MEMORY[REG_TTOSA_SAVE] = 0xAA;
			ttosaChannel = channel; 
			flashFlagByte2 |= A2_TTOSAMemoryWrFlag;
			break;
		case EVENT_TL6100_TOSA_CONFIG_LOAD :
			channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
			//channel = getTunableTosaChannel(channel);
			//if((channel == 0) || (channel > MAX_WAVELENGTH_CHANNEL) || (A2_UserData[AMCC_LINK_STATUS] & 0x1)) {
			if((channel == 0) || (channel > msaMaxChannel) || (A2_UserData[AMCC_LINK_STATUS] & 0x1)) {
				break;
			}		
			channel = A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL];
#ifdef USE_EXT_EEPROM             
			eepromReadFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_EEPROM + A2_TTOSA_MEMORY_SIZE *(channel - 1));
#else
            FLASH_Read(A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (channel - 1),A2_TTOSA_MEMORY_ADD, A2_TTOSA_MEMORY_SIZE); 
#endif
            memcpy(&A2_VendorFn[DAC_TEC_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET], 2);  // respectively channel for tec change 
			memcpy(&A2_VendorFn[DAC_AMP_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET], 2);  // respectively channel for tec change
			startLaser(false);	
			//writeLaserInformation(); 
#if 0	/* jhRoh */
			A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~(0x10);   // SFF-8690 Tx Tune flag clear
			A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~(0x20);   // SFF-8690 Wavlength Unlock flag clear
#else
			A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~(0x30);
#endif
			break;
		default :
			break;
	}
	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
}

/*
 * Name         : startLaser
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 25 2019
 * History      : 
 */

void startLaser(bool isFirstBit)
{
//	PO1_bit.PO16 = 1;
	uint8 pair = 0;
	uint16 dataOdd, dataEven;
    if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & AMCC_TUNE){    //AMCC
        writeMax5112(DAC_REAR_CODE, 0);
        writeMax5112(DAC_PHASE_CODE, 0);
        writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, 0);
        writeMax5112(DAC_GAIN_CODE, 0);	      
    } else if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & MSA_TUNE) {
        writeMax5112(DAC_REAR_CODE, 0);
        writeMax5112(DAC_PHASE_CODE, 0);
        writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, 0);
        writeMax5112(DAC_GAIN_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_GAIN]));	 
    }  
	writeMax5112(DAC_IMB_LEFT_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_IMB_LEFT]));       
	writeMax5112(DAC_IMB_RIGHT_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_IMB_RIGHT]));

	setBias(SET_LEFT);   
	setBias(SET_RIGHT);
       
	pair = A2_TTOSA_MEMORY[REG_TTOSA_FRONT_PAIR];
	setOutputMuxMax5112(pair - 1);
  
	dataOdd = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_ODD]);
	dataEven = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_EVEN]);
	if((pair >= FS_PAIR_MINIMUM) && (pair <= FS_PAIR_MAXIMUM)) {		
		if((pair % 2) != 0) {
			writeMax5112(DAC_FRONT_ODD_CODE, dataOdd);
			writeMax5112(DAC_FRONT_EVEN_CODE, dataEven); 
		} else {
			writeMax5112(DAC_FRONT_ODD_CODE, dataEven);
			writeMax5112(DAC_FRONT_EVEN_CODE, dataOdd);        
		}
	}
	
	if(isFirstBit == false) {
//		writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]));    
		if(A2_VendorFn[LDMODE_Add] & 0x01){
			writeMax5112(DAC_FRONT_SOA_SOURCE_CODE,lddRegMap[APCL_REG][SET]);
		} else {
			writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]));
		}
		writeMax5112(DAC_GAIN_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_GAIN]));  
		writeMax5112(DAC_REAR_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]));    // code added by pks 20.01.02   
		writeMax5112(DAC_PHASE_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]));
    }
  	         
}

void stopLaser(void)
{   
//	PO1_bit.PO16 = 0;
    writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, 0);
    writeMax5112(DAC_GAIN_CODE, 0);	 
  	writeMax5112(DAC_REAR_CODE, 0);
    writeMax5112(DAC_PHASE_CODE, 0);
 
}

void setBias(uint8 option)
{
      uint16 biasData=0,dutyData=0;
      float offset=0;
      if(option == SET_LEFT){
        biasData = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_BIAS_L]);      
      }else if(option == SET_RIGHT){
        biasData = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_BIAS_R]);   
      }
         
      offset = (0.05*biasData)+30;
      biasData = biasData + (uint16)offset;
      dutyData = (biasData*BIT_RESOLUTION)/(4*VREF);
     
      if(option == SET_LEFT){
          //controlDsDacDuty(DS_CH_5,dutyData);
            DS_DAC_Duty(DS_CH_5, dutyData);
      }else if(option == SET_RIGHT){
          //controlDsDacDuty(DS_CH_6,dutyData);
            DS_DAC_Duty(DS_CH_6, dutyData);
      }    
}
void controlWavelengthLocker(void)
{   
    uint16 phaseValue = 0,phaseHighLimit = 0, phaseLowLimit = 0, rearValue = 0;
    uint16 referenceLockerAdc = 0;
    uint16 lockerAdc = 0;
    sint16 lockerLutIndex = 0;
    uint8 channel = 0;
	uint8 tempIndex=0; 
	uint16 frequency = 0;
	uint8 phaseOffset; 
	uint8 rearOffset;
	uint8 wavelengthHys; 
	sint32 waveHysH, waveHysL;
	uint8 valueRatio;
	
	if(A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] < 33){
		rearOffset = A2_8690_LIMIT[REAR_OFFSET_VALUE1_Add];
		phaseOffset = A2_8690_LIMIT[PHASE_OFFSET_VALUE1_Add];
	} else if((A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] >= 33) && (A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL] < 66)){
		rearOffset = A2_8690_LIMIT[REAR_OFFSET_VALUE2_Add];
		phaseOffset = A2_8690_LIMIT[PHASE_OFFSET_VALUE2_Add];
	} else {
		rearOffset = A2_8690_LIMIT[REAR_OFFSET_VALUE3_Add];
		phaseOffset = A2_8690_LIMIT[PHASE_OFFSET_VALUE3_Add];
	}
	
	if((A2_DiagData[STATUS_CONTROL_ADD] & B7_H) || (A2_DiagData[STATUS_CONTROL_ADD] & B6_H)){  // Return Wavelength locker algorithm, while tx disable or soft tx disable is high in bit and gpio pin
		return;
	}
	 
    if((A2_VendorFn[WAVELENGTH_LOCKING_EN] & 0x1) && !(A2_UserData[AMCC_LINK_STATUS] & 0x1)){     // Check wavelength locking enable bit, Check whether ASP proecessing
//      if((A2_VendorFn[WAVELENGTH_LOCKING_EN] & 0x1) && !(A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] & TUNING) && !(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] & TUNING_ENABLE)){  // Check wavelength locking enable bit, Check whether ASP proecessing
          lockerAdc = Reg2ByteToWordLoad(&A2_VendorFn[ADC_TX_WAVE_ADD]);
          tempIndex = A2_VendorFn[TINDEX_Add];
          lockerLutIndex = IndexToTemp(tempIndex);
          referenceLockerAdc = tempToReferenceAdc(lockerLutIndex, &A2_TTOSA_MEMORY[REG_WAVELENGTH_CONSTATNT_FACTOR]);
		  wavelengthHys = A2_8690_LIMIT[WAVELENGTH_HYS_Add];
          //referenceLockerAdc = convert2BytesToWord(&A2_TTOSA_MEMORY[REG_TTOSA_LOCKER_CONDITION]);   
          //RegWordTo2ByteSave(referenceLockerAdc,&A2_TTOSA_MEMORY[0xEE-0x80]);
		  waveHysH = (sint32)referenceLockerAdc+wavelengthHys;
		  waveHysL = (sint32)referenceLockerAdc-wavelengthHys;
		  if(waveHysH > 65000){
		  	waveHysH = 65000;
		  }
		  if(waveHysL < 10){
		  	waveHysL = 10;
		  }
		  
		   if((lockerAdc < waveHysH) && (lockerAdc > waveHysL)) {
		   		A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~(0x20); 	// Tx_wave가 target range 안에 들어오면 wavelength Unlocked LOW	
		   } else {
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] |= 0x20;		// Tx_wave가target range 를 넘어갔을 때 wavelength Unlocked HIGH 
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] |= 0x20;			   
		   }
		  
          if((lockerAdc < referenceLockerAdc+phaseOffset ) && (lockerAdc > referenceLockerAdc-phaseOffset)){	//phase offset 만큼 target +-
                ;
          }else if(lockerAdc > referenceLockerAdc+phaseOffset){	  	
                A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;			// AMCC blocking
                phaseValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
                phaseHighLimit = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE_HIGH]);      // phase high limit
                phaseLowLimit = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE_LOW]);        // phase low limit
				rearValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]);
				
				channel = A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL];
				frequency = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_WAVELENGTH_GHZ]);
				//100Ghz이면 
				if(abs(referenceLockerAdc - lockerAdc) >= Reg2ByteToWordLoad(&A2_8690_LIMIT[REAR_PHASE_mV_HYS1_Add])){
					valueRatio = 10;
				} else if((abs(referenceLockerAdc - lockerAdc) < Reg2ByteToWordLoad(&A2_8690_LIMIT[REAR_PHASE_mV_HYS1_Add])) && 
						  (abs(referenceLockerAdc - lockerAdc) >= Reg2ByteToWordLoad(&A2_8690_LIMIT[REAR_PHASE_mV_HYS2_Add]))){
					valueRatio = 5;
				} else {
					valueRatio = 1;
				}				
				if( (channel % 2 == 0) ){		  
					phaseValue += phaseOffset * valueRatio;   
					
					if ((phaseValue > (phaseHighLimit - PHASE_LIMIT_OFFSET))) {
						phaseValue = phaseHighLimit - PHASE_LIMIT_OFFSET; 
					}else{
						rearValue += rearOffset * valueRatio;
					}					
					//writeMax5112(DAC_PHASE_CODE, phaseValue);					
                }else{
					phaseValue -= phaseOffset * valueRatio;
					
					if ((phaseValue < (phaseLowLimit + PHASE_LIMIT_OFFSET))){
						phaseValue = phaseLowLimit + PHASE_LIMIT_OFFSET;
					}else{
						rearValue -= rearOffset * valueRatio;
					}
					//writeMax5112(DAC_PHASE_CODE, phaseValue);
                }
				writeMax5112(DAC_PHASE_CODE, phaseValue);	
				writeMax5112(DAC_REAR_CODE, rearValue);	
				
                RegWordTo2ByteSave(phaseValue,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
				RegWordTo2ByteSave(rearValue,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]);
				A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;								// AMCC blocking
				
          }else if(lockerAdc < referenceLockerAdc-phaseOffset){			  
               A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;				// AMCC blocking
               phaseValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
               phaseHighLimit = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE_HIGH]);
               phaseLowLimit = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE_LOW]);
			   rearValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]);
			   
			   channel = A2_TTOSA_MEMORY[REG_WAVELENGTH_CHANNEL];               
			   frequency = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_WAVELENGTH_GHZ]);
			   //100Ghz이면 
			   if(abs(referenceLockerAdc - lockerAdc) >= Reg2ByteToWordLoad(&A2_8690_LIMIT[REAR_PHASE_mV_HYS1_Add])){
					valueRatio = 10;
			   } else if((abs(referenceLockerAdc - lockerAdc) < Reg2ByteToWordLoad(&A2_8690_LIMIT[REAR_PHASE_mV_HYS1_Add])) && 
						  (abs(referenceLockerAdc - lockerAdc) >= Reg2ByteToWordLoad(&A2_8690_LIMIT[REAR_PHASE_mV_HYS2_Add]))){
					valueRatio = 5;
			   } else {
					valueRatio = 1;
			   }				   
			   if( (channel % 2 == 0) ){
					phaseValue -= phaseOffset * valueRatio;
					
					if ((phaseValue < (phaseLowLimit + PHASE_LIMIT_OFFSET))){
						phaseValue = phaseLowLimit + PHASE_LIMIT_OFFSET;
					}else{
						rearValue -= rearOffset * valueRatio;
					}
                      //writeMax5112(DAC_PHASE_CODE, phaseValue);
               }else{
                      phaseValue += phaseOffset * valueRatio;
					 
                      if ((phaseValue > (phaseHighLimit - PHASE_LIMIT_OFFSET))) {
                              phaseValue = phaseHighLimit - PHASE_LIMIT_OFFSET;
                      }else{
							   rearValue += rearOffset * valueRatio;
                      }
                      //writeMax5112(DAC_PHASE_CODE, phaseValue);
               }
			   writeMax5112(DAC_PHASE_CODE, phaseValue);
			   writeMax5112(DAC_REAR_CODE, rearValue);	
			   
			   RegWordTo2ByteSave(phaseValue,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
			   RegWordTo2ByteSave(rearValue,&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_REAR]);
			   A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;					// AMCC blocking
          }
      }     
      
}

