/*
 * FILE NAME            : appWrapper.c
 * DATE                 : March 28 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 28 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */
#include <intrinsics.h>
#include <iods4835.h>
#include <stdlib.h>
#include "includes.h"
#include "main.h"
#include "board.h"
#include "timer.h"
#include "appGpio.h"
#include "appUart.h"
#include "appUartQueue.h"
#include "appGpio.h"
#include "max5112.h"
#include "appWrapper.h"
#include "tl6100.h"
#include "sff8690.h"
#include "appSmartTune.h"

#ifdef ADDON_JMA_WIRELESS
SMART_TX_DIS_STATE_E smartDisState = SMART_DIS_IDLE;
#endif
extern SFF_8690_T sff8690Info;
SMART_TUNE_TX_STATE_E smartTxState = SMART_TUNE_TX_MISSION;
SMART_TUNE_RX_STATE_E smartRxState = SMART_TUNE_RX_MISSION;
static MAN_RX_STATE_E manRxState = MAN_RX_STATE_IDLE;
static MAN_TX_STATE_E manTxState = MAN_TX_STATE_IDLE;

extern uint16 timer1s;
extern uint16 timer1ms;
extern uint8 RSSI_LOS;
extern uint8 RSSIReceiveFlag;
extern bool manTransFlag;
//uint8 gn2146DisFlag = 0;

TX_RX_CHANNEL_T smartChannel = {0, 0, 0, 0};
TIMING_VARIABLE_T timing = {START,HOLD_START, false,0, 0, 0, 0, 0, false, false};
MAN_DATA_T manchester = {0x0000, 0x0000, 0x00, 0x00, 0, 0, false};
MSA_TIME msaMode = {0x000F,270,400000,400000,60000};
uint8 txDisFlag = 0;
uint8 restartFlag = 0;

void setManTxData(uint8 dataMc, uint8 dataYc, bool holdFlag)
{
#if 0
    if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x00) {
        msaMode.holdLaserBit = 0x0000;
        timing.bitNum = 0;
    } else if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x01) {
        msaMode.holdLaserBit = 0x0007;
        timing.bitNum = 3;
    } else if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x02) {
        msaMode.holdLaserBit = 0x000F;
        timing.bitNum = 4;
    } else if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x03) {
        msaMode.holdLaserBit = 0x001F;
        timing.bitNum = 5;
    } else {
        ;
    }
#endif
	
    
    if(holdFlag == false) {
        dataMc |= 0x80;  
        dataYc <<= 1;    
        manchester.manTxData = (dataMc << 8);
        manchester.manTxData += dataYc;  
        timing.bitFlag = false;      
	}
//    } else if(holdFlag == true) {
//        manchester.manTxData = msaMode.holdLaserBit;  
//        timing.bitFlag = true;      
//    }    
    manchester.manTxDataCount = 0;
    stopTimer2();
    manTxState = MAN_TX_STATE_START; 
	setTimer2Tbc(START_MAN_DURATION);  
	startTimer2();     
}

void startSmartTuneRx(void)
{ 
		setTimer1Tbc(MAN_DURATION / 4);      
		startTimer1();
		manRxState = MAN_RX_STATE_START;
}

void manReceiveData(void)
{ 
	static uint8 rx1stData = 0x00;
	static uint8 rx2ndData = 0x00;
	
	switch(manRxState)
	{
		case MAN_RX_STATE_START :
			if(A2_DEBUGTAB[DURATION_CONTROL_BIT] == 4){		//BPS 8ms
		  		setTimer1Tbc((MAN_DURATION / 2) - 500);
			} else {
				setTimer1Tbc((MAN_DURATION / 2));
			}
			manRxState = MAN_RX_STATE_DATA;
			manchester.manRxData = 0x0000;
			manchester.manRxDataCount = 0;		
#ifdef RSSI_LOS_TYPE
			rx1stData = RSSI_LOS << manchester.manRxDataCount;							/* start condition, 1 */
#else
			rx1stData = CDR_LOS << manchester.manRxDataCount;							/* start condition, 1 */
#endif
//			PO1_bit.PO16 = CDR_LOS; //test
//			PO1_bit.PO16 = RSSI_LOS;
//			if(rx1stData != 1) {	
			if(rx1stData != 0) {
				manchester.manRxData = 0xFFFF;											/* invalid data */
				manRxState = MAN_RX_STATE_STOP;
				break;
			}		
			manchester.manRxData |= rx1stData << 15;
			break;
			
		case MAN_RX_STATE_DATA :

			setTimer1Tbc(MAN_DURATION / 2);
		  	if((manchester.manRxDataCount % 2) == 0) {	
#ifdef	RSSI_LOS_TYPE
				rx1stData = RSSI_LOS;										/* 1st half-bit */			
#else
				rx1stData = CDR_LOS;										/* 1st half-bit */
#endif
//				PO1_bit.PO16 = CDR_LOS; //test
//				PO1_bit.PO16 = RSSI_LOS; //test
			} else {	
#ifdef	RSSI_LOS_TYPE
				rx2ndData = RSSI_LOS;				
#else
			  	rx2ndData = CDR_LOS;	
#endif
//				PO1_bit.PO16 = CDR_LOS; //test
//				PO1_bit.PO16 = RSSI_LOS; //test
				/* 2nd half-bit */
				if(rx1stData == rx2ndData) {
					manchester.manRxData = 0xFFFF;										/* invalid data */
					manRxState = MAN_RX_STATE_STOP;
					break;
				} else {
					manchester.manRxData |= (rx1stData << (15 - ((manchester.manRxDataCount / 2) + 1))) ;
				}
				if(manchester.manRxDataCount > 27) {								/* stop condition, 29 */
//					if((rx1stData == 0) || (rx2ndData == 1)) {
					if((rx1stData == 1) || (rx2ndData == 0)) {
						manchester.manRxData = 0xFFFF;										/* invalid data */
					}
//					manchester.manRxData = (manchester.manRxData >> 8);
					manchester.manMcData = (manchester.manRxData >> 8);
					manchester.manMcData &= ~0x80;
					
					manchester.manYcData = (uint8)(((uint8)manchester.manRxData & 0xFF) >> 1);	//need to add type casting
					manchester.manYcData &= ~0x80;
					
					manRxState = MAN_RX_STATE_STOP;    
				} else {
				  	;
				}
			}
			manchester.manRxDataCount++;    
        break;
			
		case MAN_RX_STATE_STOP :
			manchester.manRxData = 0x0000;
			manchester.manRxDataCount = 0;   
			manRxState = MAN_RX_STATE_IDLE;
			break;
			
		case MAN_RX_STATE_IDLE : 
		default :
#ifdef RSSI_LOS_TYPE
			stopTimer1();
			RSSIReceiveFlag = 1;
#else
		  	EIF0_bit.EIF03 = OFF;
			stopTimer1();
			EIE0_bit.EIE03 = ON;
#endif
			break;
	}
}

void manTransmitData(void)
{
  if(timing.bitFlag == false) {
    timing.bitCount = (15 - manchester.manTxDataCount);
    timing.bitNum = 16;     
  } else if(timing.bitFlag  == true) { 
    timing.bitCount = manchester.manTxDataCount; 
  }   
  	switch(manTxState)
	{   
		case MAN_TX_STATE_START :  
			manTransFlag = true;
//            manchester.isLaserOn = true; 
		 	manchester.isLaserOn = false; 
			stopTimer2();
            setTimer2Tbc(START_MAN_DURATION);
            startTimer2();
            if(((manchester.manTxData >> timing.bitCount) & 0x01) == 0) {
                manTxState = MAN_TX_STATE_DATA_LOW_LOW;       
            } else {
               manTxState = MAN_TX_STATE_DATA_HIGH_HIGH;
            }
			break;

		case MAN_TX_STATE_DATA_LOW_LOW :
			manTxState = MAN_TX_STATE_DATA_LOW_HIGH;
			stopTimer2();
			setTimer2Tbc(MAN_DURATION / 2);
			startTimer2(); 
//			stopLaser();
//            gn2146TxSoftDisable(ON);
			gn2146TxSoftDisable(OFF);
			if(manchester.isLaserOn == false) {	
//			  	PO1_bit.PO16 = 1;
				startLaser(false);
//            manchester.isLaserOn = false;
			  manchester.isLaserOn = true;
			}
			break;
            
		case MAN_TX_STATE_DATA_HIGH_HIGH :
			manTxState = MAN_TX_STATE_DATA_HIGH_LOW;
			stopTimer2();
			setTimer2Tbc(MAN_DURATION / 2);
			startTimer2();
//            if(manchester.isLaserOn == false) {
			if(manchester.isLaserOn == true) {
//		        startLaser();
//              gn2146TxSoftDisable(OFF);
			  gn2146TxSoftDisable(ON);
//			  PO1_bit.PO16 = 0;
			  stopLaser();		  
//                manchester.isLaserOn = true;
			  manchester.isLaserOn = false;
            }
			break;
			
		case MAN_TX_STATE_DATA_HIGH_LOW : 
		case MAN_TX_STATE_DATA_LOW_HIGH :
			stopTimer2();
			setTimer2Tbc(MAN_DURATION / 2);
			startTimer2();
			if(manTxState == MAN_TX_STATE_DATA_HIGH_LOW) {
				if(manchester.isLaserOn == false) {
//                stopLaser();
//                gn2146TxSoftDisable(ON);
				  gn2146TxSoftDisable(OFF);
//				  PO1_bit.PO16 = 1;
				  startLaser(false);
//                manchester.isLaserOn = false;
				  manchester.isLaserOn = true;
				}
			} else {
//              if(manchester.isLaserOn == false) {
				if(manchester.isLaserOn == true) {
//			  	startLaser();
//                gn2146TxSoftDisable(OFF);
				 gn2146TxSoftDisable(ON);	
//				 PO1_bit.PO16 = 0;
				 stopLaser();
//                manchester.isLaserOn = true;
				 manchester.isLaserOn = false;
              }
			}
            manchester.manTxDataCount++;           
              if(timing.bitFlag == false) {
                timing.bitCount = (15 - manchester.manTxDataCount);
              } else if(timing.bitFlag == true) {
                timing.bitCount = (timing.bitNum - manchester.manTxDataCount);
              }            
            if(manchester.manTxDataCount < timing.bitNum) {
                if(((manchester.manTxData >> timing.bitCount) & 0x01) == 0) {
                    manTxState = MAN_TX_STATE_DATA_LOW_LOW;
                } else {
                    manTxState = MAN_TX_STATE_DATA_HIGH_HIGH;                  
                }
            } else {  
                manTxState = MAN_TX_STATE_STOP;
            }
			break;

		case MAN_TX_STATE_STOP :  
//          if(manchester.isLaserOn == false) {
			if(manchester.isLaserOn == true) {
//     	    startLaser();
//            gn2146TxSoftDisable(OFF);
			gn2146TxSoftDisable(ON);
//			PO1_bit.PO16 = 0;
			stopLaser();
//            manchester.isLaserOn = true;
			manchester.isLaserOn = false;
          }
            stopTimer2();
            setTimer2Tbc(MAN_DURATION);
		    startTimer2();
            timing.sendFlag = STOP;
//            timing.holdFlag = HOLD_STOP;
		    manTxState = MAN_TX_STATE_IDLE;
            manchester.manTxDataCount = 0;
            manTransFlag = false;
			break;
			
		case MAN_TX_STATE_TX_HOLD :
			break;			

		case MAN_TX_STATE_IDLE :
		default :

		  	stopTimer2();  
			break;
	} 
}


void smartTxData(void)
{  
    A2_DEBUGTAB[TX_STATE] = smartTxState;	
    switch(smartTxState)
    {
        case SMART_TUNE_TX_INIT :
          A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;                  //CDR Control
          I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
          A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] &= ~0x04;
          I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);           
          A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] |= TUNING;     //7th Bit
          A2_TTOSA_PAGE_0x02[TUNING_LATCHED_STATUS] |= TUNING;     //7th Bit        
		  A2_UserData[SFF_8690_CURRENT_STATUS_VEN] |= TUNING;		//Samsung req
		  A2_UserData[SFF_8690_LATCHED_STATUS_VEN] |= TUNING;	
		  A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] |= (0x10);
#ifndef SAMSUNG
          A2_UserData[MODULE_CONTROL_STATUS] &= ~AMCC_CONTROL;    //AMCC CONTROL OFF
#endif
          smartChannel.TX_MC = 1;                                 //first init 
          smartChannel.TX_YC = 0;     
          smartTxState = SMART_TUNE_TX_LASER;
          timing.smartTxTimeOut = 0;
		  
#ifdef RSSI_LOS_TYPE
		  A2_GN2146Sync[LOS_SOFT_ASSERT_CTRL_REG_0_Add] |= 0x0A;		//CDR Rx_LOS output Assert
		  I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, LOS_SOFT_ASSERT_CTRL_REG_0, &A2_GN2146Sync[LOS_SOFT_ASSERT_CTRL_REG_0_Add], 1);
		  if(A2_DEBUGTAB[RX_LOS_OUT_EN] == 1) {
		  	configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_3,GPIO_PUSH_PULL_OUTPUT,false); 	//Rx_LOS Output mode
		  } else {
		  	configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_3,GPIO_HIGH_Z_INPUT,false);
		  }
#endif
          writeMax5112(DAC5_CONFIG_ADDRESS, 0x8800);  
          writeMax5112(DAC6_CONFIG_ADDRESS, 0x8800);    
          break;
          
        case SMART_TUNE_TX_LASER :          //Channel switch time 128ms 
          msaMode.switchTime = (Reg2ByteToWordLoad(&A2_DEBUGTAB[CHANNEL_SWITCH_TIME]));
		  A2_UserData[TX_YC_VEN] = smartChannel.TX_YC; 		//Samsung req	
          if(timing.smartTxTimeOut == 0)  {
//              PO1_bit.PO16 = 0;
			  stopLaser();
              manchester.isLaserOn = false;
			  changeTxChannel(smartChannel.TX_MC); 
              A2_UART_MEMORY[SMART_TUNE_TX_MC_CHANNEL] = smartChannel.TX_MC;
              A2_UART_MEMORY[SMART_TUNE_TX_YC_CHANNEL] = smartChannel.TX_YC; 
			  
			  gn2146TxSoftDisable(ON);	
              timer1ms = 0;         
          }      
          if(msaMode.switchTime > SWITCH_TIME_MAX) {
            msaMode.switchTime = SWITCH_TIME_MAX;
          } else if(msaMode.switchTime < SWITCH_TIME_MIN) {
            msaMode.switchTime = SWITCH_TIME_MIN;
          }        
          if(timer1ms >= msaMode.switchTime) { 
//			  startLaser(false);		  
              timing.smartTxTimeOut = 0;
              smartTxState = SMART_TUNE_TX_CHECK_YC;  
          }           	  
          timing.smartTxTimeOut = 1;         
          break;
          
        case SMART_TUNE_TX_CHECK_YC : 
		  smartChannel.TX_YC = smartChannel.RX_MC;           // Parse rx remote data 	
          timing.sendFlag = START;
          smartTxState = SMART_TUNE_TX_SEND_MSG;         
          break;
          
        case SMART_TUNE_TX_SEND_MSG :  
          if(timing.sendFlag == START) {
            setManTxData(smartChannel.TX_MC, smartChannel.TX_YC, false);      //send message 
//			  setManTxData(0x55, 0x35, false);
            timing.sendFlag = SETING;           
          } else if(timing.sendFlag == STOP) {             
            timing.sendFlag = START;
            if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] != 0x00) {
                timing.holdFlag = HOLD_START;
            } else {
                timing.holdFlag = HOLD_STOP;
            }
            smartTxState = SMART_TUNE_TX_HOLD_LSR;   
          }
          break;
          
        case SMART_TUNE_TX_HOLD_LSR :                 //Division rxYc         
          /* hold time 128ms을 flag를 통해 신호가 들어오면 제어 */  
           /* high - 4bit send */
          if(timing.holdFlag == HOLD_START) {			
			 timer1ms = 0;
			 gn2146TxSoftDisable(OFF);
//			 PO1_bit.PO16 = 1;
			 startLaser(false); 	
             timing.holdFlag = HOLD_SETING;  
			 if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x00) {
			 	timing.holdBit = 0;
			 } else if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x01) {
			 	timing.holdBit = 3;
			 } else if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x02) {
			 	timing.holdBit = 4;
			 } else if(A2_DEBUGTAB[OPTIONAL_HOLD_LASER_TIME] == 0x03) {
			 	timing.holdBit = 5;
			 } else {
			 	;
			 }
			 
		  } else if(timing.holdFlag == HOLD_SETING){ 


				if(timer1ms >= (timing.holdBit * 32)) {
					gn2146TxSoftDisable(ON);
//					PO1_bit.PO16 = 0;
					stopLaser(); 	
					timing.holdFlag = HOLD_STOP;
				}			  
			
//            setManTxData(0, 0, true);             //hold laser 4bit        
          } else if(timing.holdFlag == HOLD_STOP) { 
              timing.holdFlag = HOLD_START;         
              if(smartChannel.RX_YC == 0) {    
               smartTxState = SMART_TUNE_TX_NEXT_CHAN;
              } else if((smartChannel.TX_MC == smartChannel.RX_YC) && (smartChannel.TX_YC == smartChannel.RX_MC)) {   
				smartTxState = SMART_TUNE_TX_MISSION;  
				gn2146TxSoftDisable(OFF);
				writeMax5112(DAC5_CONFIG_ADDRESS, 0xC800); 
			  	writeMax5112(DAC6_CONFIG_ADDRESS, 0xB800); 						
//				PO1_bit.PO16 = 1;
				startLaser(false);  
                timing.changeAspFlagTx = true;
                stopTimer2();
                setTimer2Tbc(UART_DURATION);
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~(0x10);
#ifdef RSSI_LOS_TYPE				
			    A2_GN2146Sync[LOS_SOFT_ASSERT_CTRL_REG_0_Add] &= ~0x0A;		//CDR Rx_LOS output not Assert
			    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, LOS_SOFT_ASSERT_CTRL_REG_0, &A2_GN2146Sync[LOS_SOFT_ASSERT_CTRL_REG_0_Add], 1);	
				configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_3,GPIO_HIGH_Z_INPUT,false);	//Rx_LOS Input mode
#endif 
              } else if(smartChannel.RX_YC != 0) {
                smartTxState = SMART_TUNE_TX_SELFTUNE_CHAN;                
              } else {
                ;  /* nothing to do */
              } 
          } 
          break;
          
        case SMART_TUNE_TX_NEXT_CHAN :
          smartChannel.TX_MC = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL_LSB];
          smartChannel.TX_MC++;
          if(smartChannel.TX_MC > TX_MC_MAX){ 
            smartChannel.TX_MC = 1;
          }
#ifdef ADDON_JMA_WIRELESS
          RegWordTo2ByteSave((uint16)smartChannel.TX_MC,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);  // Save channel info
          RegWordTo2ByteSave((uint16)smartChannel.TX_MC,&A2_UserData[SFF_8690_A2_00_CHANNEL]);  // Save channel info
#else
          RegWordTo2ByteSave((uint16)smartChannel.TX_MC,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);  // Save channel info  
#endif          
          timing.smartTxTimeOut = 0;
          smartTxState = SMART_TUNE_TX_LASER;
          break;
          
        case SMART_TUNE_TX_SELFTUNE_CHAN :      
          smartChannel.TX_MC = smartChannel.RX_YC;
          timing.smartTxTimeOut = 0;
          smartTxState = SMART_TUNE_TX_LASER;  
          break;
          
        case SMART_TUNE_TX_MISSION : 
		
#if 1	
          if((A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] & TUNING) == 0) {   
//            startLaser(false);           
            if(smartChannel.RX_YC == 0) {   

                A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] |= TUNING;
                A2_TTOSA_PAGE_0x02[TUNING_LATCHED_STATUS] |= TUNING;
			    A2_UserData[SFF_8690_CURRENT_STATUS_VEN] |= TUNING;		//Samsung req
			    A2_UserData[SFF_8690_LATCHED_STATUS_VEN] |= TUNING;					
                manchester.manMcData = 0;
                manchester.manYcData = 0;                
//                EIF0_bit.EIF06 = 0;
//                EIE0_bit.EIE06 = OFF;
//                EIF0_bit.EIF03 = 0;                
//                EIE0_bit.EIE03 = ON;            
                smartTxState = SMART_TUNE_TX_INIT;   
                break;
            } 
            A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0; 
          } else {         
            if((timing.changeAspFlagTx == true) && (timing.changeAspFlagRx == true)) {
               
             if((A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & AMCC_TUNE) == 0) {
               
            /* ROD Disable (if you wanna start the ROD, you have to erase this block)*/
//                A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] |= AMCC_TUNE;       //use AMCC
//                A2_UserData[MODULE_CONTROL_STATUS] |= AMCC_CONTROL;     //AMCC CONTROL ON 
//                EIES0_bit.EIES06 = EDGE_NEGATIVE; // AMCC interrupt   
//                EIE0_bit.EIE06 = ON;              // AMCC interrupt   
                   
//                EIF0_bit.EIF03 = 0;
//                EIE0_bit.EIE03 = OFF;
//                EIES0_bit.EIES03 = EDGE_POSITIVE; 
//				EIES0_bit.EIES03 = EDGE_NEGATIVE;
                A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] &= ~TUNING;
      			
				
                if((A2_DiagData[EXT_STATUS_CONTROL_ADD] & 0x08) || (RS1_HOST == _ON)) {
                    A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
                    A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] &= ~0x04;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);    
                } else {
                    A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] |= 0x01;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
                    A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] |= 0x04;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);
                }   
             } else {  
                ;
             }               
            }
          }
#endif
          break;   
          
        default :    
          break;
    }      
}

void smartRxData(void)
{
  A2_DEBUGTAB[RX_STATE] = smartRxState;	
  switch(smartRxState)
  {
    case SMART_TUNE_RX_INIT :         
      A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;
      I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
      A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] &= ~0x04;
      I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);       
      A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1; 
 
      smartChannel.RX_MC = 0;
      smartChannel.RX_YC = 0;
      msaMode.timeOutStarted = false;
      smartRxState = SMART_TUNE_RX_DEC_00;
      break;
      
    case SMART_TUNE_RX_DEC_00 :
      smartReceive(manchester.manMcData, manchester.manYcData);        //receive data  
//		smartChannel.RX_MC = 0x10;
//		smartChannel.RX_YC = 0x06;
	  
	  A2_UserData[RX_MC_VEN] = smartChannel.RX_MC;	//samsung req	  
      if((smartChannel.RX_MC != 0) && (smartChannel.RX_YC == 0)) {     //Divison state
        smartRxState = SMART_TUNE_RX_DEC_X0; 
      } else if((smartChannel.RX_MC != 0) && (smartChannel.RX_YC != 0)) { 
          smartRxState = SMART_TUNE_RX_DEC_XY;
      }      
       
      break;
       
    case SMART_TUNE_RX_DEC_X0 :                         //rxMc만 들어 왔을 때 
       
      A2_UART_MEMORY[SMART_TUNE_RX_MC_CHANNEL] = smartChannel.RX_MC; 
	  
      if(smartChannel.RX_YC == 0) {
        if(msaMode.timeOutStarted == false) {
          msaMode.timeOutStarted = true;
          timer1s = 0;
        }
		
//        msaMode.t1TimeOut = SET_TIME_OUT_1_2;
        msaMode.t1TimeOut = Reg2ByteToWordLoad(&A2_DEBUGTAB[T1_TIME_OUT]);
        smartRxState = SMART_TUNE_RX_DEC_00; 
        if(msaMode.t1TimeOut > 420){
            msaMode.t1TimeOut = 420;
            RegWordTo2ByteSave(420, &A2_DEBUGTAB[T1_TIME_OUT]);
        } else if(msaMode.t1TimeOut < 380) {
            msaMode.t1TimeOut = 380;
            RegWordTo2ByteSave(380, &A2_DEBUGTAB[T1_TIME_OUT]);
        }
        
         
        if(timer1s >= msaMode.t1TimeOut) {                    //wait T1 timeout 400s.  
//		if(timer1s >= 30) { 
			manchester.manMcData = 0;
			manchester.manYcData = 0;	
			smartChannel.RX_MC = 0;
			smartChannel.RX_YC = 0;			
            smartRxState = SMART_TUNE_RX_INIT;          
        }        
      } else { 
        smartRxState = SMART_TUNE_RX_DEC_XY;    
		
      }        
      break;
      
    case SMART_TUNE_RX_DEC_XY :                          //rxMc, rxYc 둘 다 들어왔을 때     
      A2_UART_MEMORY[SMART_TUNE_RX_MC_CHANNEL] = smartChannel.RX_MC;
      A2_UART_MEMORY[SMART_TUNE_RX_YC_CHANNEL] = smartChannel.RX_YC;   
	  A2_UserData[RX_YC_VEN] = smartChannel.RX_YC;	//samsung req
#ifdef RSSI_LOS_TYPE
	 if((RSSI_LOS) && (smartChannel.RX_MC != 0) && (smartChannel.RX_YC != 0)) {
#else
     if((CDR_LOS) && (smartChannel.RX_MC != 0) && (smartChannel.RX_YC != 0)) { 
#endif
//     if((smartChannel.RX_MC != 0) && (smartChannel.RX_YC != 0)) { 
//        msaMode.t2TimeOut = SET_TIME_OUT_1_2;
        msaMode.t2TimeOut = Reg2ByteToWordLoad(&A2_DEBUGTAB[T2_TIME_OUT]);
        if(msaMode.t2TimeOut > 420) {
            msaMode.t2TimeOut = 420;
            RegWordTo2ByteSave(420, &A2_DEBUGTAB[T2_TIME_OUT]);
        } else if(msaMode.t2TimeOut < 380) {
            msaMode.t2TimeOut = 380;
            RegWordTo2ByteSave(380, &A2_DEBUGTAB[T2_TIME_OUT]);        
        }
        
        if(msaMode.timeOutStarted == false) {
            msaMode.timeOutStarted = true;
            timer1s = 0;	

        }  
        if(timer1s >= msaMode.t2TimeOut) {
//		if(timer1s >= 30) {
//			PO1_bit.PO16 = 0; //test
			manchester.manMcData = 0;
			manchester.manYcData = 0;	
			smartChannel.RX_MC = 0;
			smartChannel.RX_YC = 0;
            smartRxState = SMART_TUNE_RX_INIT; 
        }
      } else { 
        timing.changeAspFlagRx = true;
        smartRxState = SMART_TUNE_RX_MISSION;

      } 
      break;
      
    case SMART_TUNE_RX_MISSION : 
#if 1
      if((A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] & TUNING) == 0) {
		
		if(manchester.manYcData == 0) {
			
			smartRxState = SMART_TUNE_RX_INIT; 
		}
#ifdef RSSI_LOS_TYPE
		if(RSSI_LOS) { 
#else
        if(CDR_LOS) {  
#endif
            if(msaMode.timeOutStarted == false) {
                msaMode.timeOutStarted = true;
                timer1s = 0;                            
            }              
//            msaMode.t3TimeOut = SET_TIME_OUT_3;
            msaMode.t3TimeOut = A2_DEBUGTAB[T3_TIME_OUT];
            if(msaMode.t3TimeOut > 180) {
                msaMode.t3TimeOut = 180;
                A2_DEBUGTAB[T3_TIME_OUT] = 0xB4;    //180
            } else if(msaMode.t3TimeOut < 1) {
                msaMode.t3TimeOut = 1;
                A2_DEBUGTAB[T3_TIME_OUT] = 0x01;    //1                 
            }
            
			RegWordTo2ByteSave(timer1s,&A2_DEBUGTAB[TIME_OUT_SEC]);
            if(timer1s >= msaMode.t3TimeOut) {                   //wait timeout 60s 
                A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] &= ~AMCC_TUNE;
#ifndef SAMSUNG
                A2_UserData[MODULE_CONTROL_STATUS] &= ~AMCC_CONTROL;
#endif
//                EIF0_bit.EIF06 = 0;
//                EIE0_bit.EIE06 = OFF;
//                EIES0_bit.EIES03 = EDGE_POSITIVE; 
//				EIES0_bit.EIES03 = EDGE_NEGATIVE; 
//                EIE0_bit.EIE03 = ON;
                manchester.manMcData = 0;
                manchester.manYcData = 0;
                smartRxState = SMART_TUNE_RX_INIT;             
            }            
        } else {
           msaMode.timeOutStarted = false; 
        }

      } else { 
        if((timing.changeAspFlagTx == true) && (timing.changeAspFlagRx == true)) { 		
            if((A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & AMCC_TUNE) == 0) {
              /* ROD Disable (If you wanna start the ROD, you have to erase block)*/
//                A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] |= AMCC_TUNE;       //use AMCC
//                A2_UserData[MODULE_CONTROL_STATUS] |= AMCC_CONTROL;     //AMCC CONTROL ON 
//                EIES0_bit.EIES06 = EDGE_NEGATIVE; // AMCC interrupt   
//                EIF0_bit.EIF06 = 0;
//                EIE0_bit.EIE06 = ON;              // AMCC interrupt      
			
//                EIF0_bit.EIF03 = 0;
//                EIE0_bit.EIE03 = OFF;
//                stopTimer1();
				
                txDisFlag = 1;
                A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] &= ~TUNING;
                timing.changeAspFlagRx = false; 
                timing.changeAspFlagTx = false;
     
			
 				
                if((A2_DiagData[EXT_STATUS_CONTROL_ADD] & 0x08) || (RS1_HOST == _ON)) {
                    A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
                    A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] &= ~0x04;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);    
                } else {
                    A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] |= 0x01;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
                    A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] |= 0x04;
                    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);
                }

            } else {
                ;     
            } 
        }

     }
#endif	
      break; 
      
    default :   
      break;
  }
    
}

#ifdef ADDON_JMA_WIRELESS
void txDisable(void)
{
  uint16 disCount1 = 0;
  uint16 disCount2 = 0;
  uint16 disCountMax = 3000;
  switch(smartDisState){
	 	case SMART_DIS_IDLE :
			smartDisState = SMART_DIS_IDLE;
			A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x0;
		break;
		
		case SMART_DIS_1ST_OBEY :
          if(txDisFlag == 1){
			writeMax5112(DAC5_CONFIG_ADDRESS, 0xC800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0xA800);	
			startLaser(false);
			smartDisState = SMART_DIS_1ST_OBEY_CNT;
          }
		break;
		
		case SMART_DIS_1ST_OBEY_CNT :
			disCount1++;
			if(disCount1 >= disCountMax){
				disCount1 = 0;
				smartDisState = SMART_DIS_IGNORE;
			}
		break;
		
		case SMART_DIS_IGNORE :
		    A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] &= ~0x01;                  //CDR Control
            I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
            A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] &= ~0x04;
            I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);
			writeMax5112(DAC5_CONFIG_ADDRESS, 0x8800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0x8800);		
			startLaser(false);
			smartDisState = SMART_DIS_IGNORE_CNT;
		break;
	    
		case SMART_DIS_IGNORE_CNT :
			disCount2++;
			if(disCount2 >= disCountMax){
				disCount2 = 0;
				smartDisState = SMART_DIS_2ND_OBEY;
			}		  
		break;		
		
	 	case SMART_DIS_2ND_OBEY :
            A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add] |= 0x01;
            I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &A2_GN2146Sync[TX_CDR_BYPASS_CTRL_REG_0_Add], 1);
            A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add] |= 0x04;
            I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_DRV_PD_CTRL_REG_0, &A2_GN2146Sync[TX_DRV_PD_CTRL_REG_0_Add], 1);          
			writeMax5112(DAC5_CONFIG_ADDRESS, 0xC800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0xA800);
			startLaser(false);
			smartDisState = SMART_DIS_IDLE;
		break;
	 }
}
#endif
void smartReceive(uint8 myChannel, uint8 yourChannel)
{
    smartChannel.RX_MC = myChannel;
    smartChannel.RX_YC = yourChannel;    
}

//gn2146TxSoftDisable
//extern uint16 disCount1ms;
void gn2146TxSoftDisable(uint8 onOff)
{ 
  uint8 disValue = 0;
  uint8 count = 0;
	max5112StopCondition();
    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TXDSBL_SOFT_REG_0, &onOff, 1); 
    I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TXDSBL_SOFT_REG_0, &disValue, 1);  
    while(disValue != onOff)
    {
        I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TXDSBL_SOFT_REG_0, &onOff, 1); 
        I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TXDSBL_SOFT_REG_0, &disValue, 1);
        if(count++ > 10) {
            break;
        }         
    }
}
