/*******************************************************************************
* Copyright (C) 2017 Maxim Integrated Products, Inc., All rights Reserved.
* * This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <intrinsics.h>
#include <iods4835.h>

#include "main.h"
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appWrapper.h"
#include "appGpio.h"
#include "appAdc.h"
#include "appIDac.h"
#include "appDcdc.h"
#include "tl6100.h"
#include "appUart.h"
#include "appTimer.h"
#include "appSmartTune.h"
#include "sff8690.h"

uint16 delaycount = 0;
uint16 timer1sCounter = 0;
uint16 timer1s = 0;
uint16 timer1ms = 0;
uint16 disCount1ms = 0;
uint8 RSSIReceiveFlag = 0;
uint8 RSSI_LOS;
void GPIO27_ExtenalInterrupt_Init(void)
{
	// GPIO GP27 interrupt 
	EIE2_bit.EIE27 = 1;	
	if ( PI2_bit.PI27 )
	{	// TxDisable status, negative edge
		EIES2_bit.EIES27 = 1;		
	}
	else
	{	// Txenable status, pasitive edge
		EIES2_bit.EIES27 = 0;		
	}
}
//test
void GPIO15_ExtenalInterrupt_Init(void)
{
	// GPIO GP15 Tx_Dis Host interrupt 
	EIE1_bit.EIE15 = 1;	
    EIES1_bit.EIES15 = 0;
}
//
void interruptMask(void)
{
	IMR_bit.IM3 = 1;
	IMR_bit.IM0 = 1;	
	IMR_bit.IM4 = 1;
}

void Eye_Safety(void)
{
	if (PI2_bit.PI27!=_ON) 
	{
//		TX_FAULT_HOST = _ON;
//		iDac1OnOff(_OFF);
	}
}
//test
void Fault_clear(void)
{
	TX_FAULT_HOST = FAULT_OFF;
}
//

/*
 * Name         : module0Isr
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 28 2019
 * History      : 
 */
extern uint16 task0, task1, task2,task3,manTransTimer;
#pragma vector = 0x00         
__interrupt void module0Isr()
{
      
   if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & MSA_TUNE) {
           if(TBCN1_bit.TBIF) {
                TBCN1_bit.TBIF = 0;   
                manReceiveData();   
                interruptMask();
            }        
#ifndef RSSI_LOS_TYPE
            if(EIE0_bit.EIE03 == ON) {
                 if(EIF0_bit.EIF03) {					 
                      EIF0_bit.EIF03 = 0;
                      EIE0_bit.EIE03 = OFF;
                      startSmartTuneRx();
                      interruptMask();
                 } 
            }
#endif
              /*SMART TX*/
         if(TBCN2_bit.TBIF) {
            TBCN2_bit.TBIF = 0;
            //LINK_STATE
//            if(!(A2_UserData[AMCC_LINK_STATUS] & 0x1)) {
//                TBCN3_bit.TBR = 0;		
//                setTxUartTimerTick();
//            }    
//            manTransmitData();
//            TBCN3_bit.TBR = 1;		
			manTransTimer++;
            interruptMask();
			
        } 
    }
    //test
    if(EIE1_bit.EIE15 == ON) {
          EIF1_bit.EIF15 = 0;
          Fault_clear();
          interruptMask();
    }
    //
}

/*
 * Name         : module3Isr
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 28 2019
 * History      : 
 */
extern uint8 test;
extern TIMING_VARIABLE_T timing;

#pragma vector = 0x03          
__interrupt void module3Isr()
{
//	PO1_bit.PO16 ^= 1;
  	if(TBCN3_bit.TBIF) {
		TBCN3_bit.TBIF = 0;
		task0++;
		task1++;
		task2++;
		task3++;
		
//        tickTimer();
        delaycount++;
        interruptMask();
        timer1ms++;       
        timer1sCounter++;
        disCount1ms++;
        if(timer1sCounter >= 1000) {
            timer1sCounter = 0;
            timer1s++;  
        }   
	}
}

/*
 * Name         : module4Isr
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 28 2019
 * History      : 
 */
#pragma vector = 0x04
__interrupt void module4Isr()
{ 
	
#ifdef RSSI_LOS_TYPE
//	if((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] & SUPPORTED) && (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] & TUNING_ENABLE)) {
		if (HTI_bit.HTI7) {	// polling time : 75us
			if(RSSIReceiveFlag) {
				startSmartTuneRx();
				RSSIReceiveFlag = 0;
			}
			LTI_bit.LTIE7 = 1;
			HTI_bit.HTIE7 = 0;			
			HTI_bit.HTI7 = 0;
			LTI_bit.LTI_7 = 0;
			RSSI_LOS = 0;
			if(A2_DEBUGTAB[RX_LOS_OUT_EN] == 1) {	//Rx_LOS Debugging
				Rx_LOS_OUT = RSSI_LOS;
			}				
			interruptMask(); 
		} 
		else if(LTI_bit.LTI_7){
			LTI_bit.LTIE7 = 0;
			HTI_bit.HTIE7 = 1;
			LTI_bit.LTI_7 = 0;
			HTI_bit.HTI7 = 0;
			RSSI_LOS = 1;
			if(A2_DEBUGTAB[RX_LOS_OUT_EN] == 1) {	//Rx_LOS Debugging
				Rx_LOS_OUT = RSSI_LOS;
			}				
			interruptMask(); 
		}
//	}
	
#endif
}

/*
 * Name         : initializeSystem
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
void initializeSystem(void)
{
  	unsigned int dcdc;
	
	initializeGpio();	
	initializeAdc();     
	INRSH_bit.INRUSH_END = 1;
	dcdc = DCDC_SEL;  	
	TECCN2 = 0;
	initializeBuck(DC_DC_CH3, DCDC_16_V);

	DCDC_SEL = dcdc;
	//initializeTimer1(); 
	Timer1_Init();
	Timer2_Init();
	Timer3_Init();
           
	//initializeDSDac(DS_CH_5, 0x8040, 0xffff);        /* GP25 for DSDAC5 : SET_BIAS_L */
	DS_DAC_Init(DS_CH_5,0x8040,0xffff);
	//initializeDSDac(DS_CH_6, 0x8040, 0xffff);        /* GP26 for DSDAC6 : SET_BIAS_R */
	DS_DAC_Init(DS_CH_6,0x8040,0xffff);
	//controlDsDacDuty(DS_CH_5,0);
	DS_DAC_Duty(DS_CH_5, 0);
	//controlDsDacDuty(DS_CH_6,0);
	DS_DAC_Duty(DS_CH_6, 0);
	//initializeIDac(IDAC01, 0x0000);		    /* GP14 for IDAC2 : AGC_CTRL */	
	//initializeI2CMaster(I2CM_MASTER_1, I2CM_CLOCK_DIV_400KHZ, I2CM_MSDA_PIN_DEFAULT, 0); 
	I2CM_Init(I2CM_MASTER_1, I2CM_CLOCK_DIV_400KHZ, I2CM_MSDA_PIN_DEFAULT, 0);
//	I2CM_Init(I2CM_MASTER_1, I2CM_CLOCK_DIV_100KHZ, I2CM_MSDA_PIN_DEFAULT, 0);
        
	//initializeI2CSlave();
	I2CS_Init();
	__enable_interrupt();							/* Enable Global Interrupt */
}

