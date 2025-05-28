/*
 * FILE NAME            : appMessageHandler.c
 * DATE                 : May 17 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : May 17 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 *                                                                          
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <intrinsics.h>
#include <stdlib.h>
#include <iods4835.h>
#include "includes.h"
#include "main.h"
#include "appWrapper.h"
#include "appMessageHandler.h"
#include "appMessageQueue.h"
#include "appAdc.h"
#include "appGpio.h"
#include "appIDac.h"
#include "tl6100.h"
#include "sff8472.h"
#include "sff8690.h"
#include "appUart.h"
#include "appSmartTune.h"

/*
 * Name         : messageHandler
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
extern SMART_TUNE_TX_STATE_E smartTxState;
extern SMART_TUNE_RX_STATE_E smartRxState;
extern uint8 changeAspFlag;
extern bool manTransFlag;
bool ASPDisFlag;

void messageHandler(void)
{


//uint8 data;
#if 0	/* jhRoh */
	QUEUE_MESSAGE_T message = {EVENT_NONE, 0};
#else
	QUEUE_MESSAGE_T message = {EVENT_NONE};
#endif
	EVENT_LIST event = EVENT_NONE;
//	uint16 data = 0x0000;	/* jhRoh */

	message = getMessage(getMessageQueue());
	event = message.event;
//	data = message.uint16Data;	/* jhRoh */
	if(event == EVENT_SOFT_DISABLE){
      //	  if((A2_UART_MEMORY[0x98-0x80] & 0x04) || (A2_UserData[AMCC_LINK_STATUS] & 0x1)){  //UART_TX_DIS_FLAG || AMCC_LINK_STATUS
		softTxDisableControl();
	
	}else if( (event == EVENT_SFF_8690_CHANNEL) || (event == EVENT_SFF_8690_FREQUENCY) || (event == EVENT_SFP_8690_LOOPBACK)) {
	  processSFF8690(event);
	} else if((event >= EVENT_TL6100_FRONT_PAIR) && (event <= EVENT_TL6100_TOSA_CONFIG_LOAD)) {	  
	  processTL6100(event);
	}else {		
	  ;
	}
    if((A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & MSA_TUNE) && (changeAspFlag == 1)) {
        if((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] & SUPPORTED) && (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] & TUNING_ENABLE)) {
			ASPDisFlag = true;	
            smartTxData();
            smartRxData();
#ifdef ADDON_JMA_WIRELESS
//            txDisable();
#endif
        } else {	//stop ASP when during the tuning
			smartTxState = SMART_TUNE_TX_INIT;
			smartRxState = SMART_TUNE_RX_INIT;			
			if(ASPDisFlag== true) {
#ifdef RSSI_LOS_TYPE
			    A2_GN2146Sync[LOS_SOFT_ASSERT_CTRL_REG_0_Add] &= ~0x0A;		//CDR Rx_LOS output not Assert
			    I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, LOS_SOFT_ASSERT_CTRL_REG_0, &A2_GN2146Sync[LOS_SOFT_ASSERT_CTRL_REG_0_Add], 1);	
				configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_3,GPIO_HIGH_Z_INPUT,false);	//Rx_LOS Input mode		
#endif
				manTransFlag = false;
				A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] &= ~TUNING;
				A2_TTOSA_PAGE_0x02[TUNING_LATCHED_STATUS] &= ~TUNING;
				A2_UserData[SFF_8690_CURRENT_STATUS_VEN] &= ~TUNING;	//Samsung req
				A2_UserData[SFF_8690_LATCHED_STATUS_VEN] &= ~TUNING;
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~(0x10);
				
				writeMax5112(DAC5_CONFIG_ADDRESS, 0xC800); 
			  	writeMax5112(DAC6_CONFIG_ADDRESS, 0xB800);				
				stopTimer1();
				stopTimer2();
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
				
				softTxDisableControl();
									
				ASPDisFlag = false;
			}
		}
    }
}
