/*
 * FILE NAME            : appSoa.c
 * DATE                 : OCT 26 2024
 * AUTHOR               : iuseo, iuseo@lightron.co.kr
 * HISTORY              : OCT 26 2014 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2024
 */
#include "main.h"
#include "appSoa.h"
#include "appMessageQueue.h"
#include "appSmartTune.h"
#include "includes.h"
#include "appTimer.h"
#include "appWrapper.h"
#include "appMod.h"
#include "max5112.h"
#include "sff8690.h"
#include "tl6100.h"

extern uint16 lddRegMap[DACNum][2];

void controlSoaRegister(void)
{
	
	if((!(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] & SUPPORTED)) || (!(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] & TUNING_ENABLE))){	//When not in smart tune
		if((!(A2_DiagData[STATUS_CONTROL_ADD] & B7_H)) || (!(A2_DiagData[STATUS_CONTROL_ADD] & B6_H))){	//When not in Tx_dis state
			if(A2_VendorFn[LDMODE_Add] & 0x01){
				writeMax5112(DAC_FRONT_SOA_SOURCE_CODE,lddRegMap[APCL_REG][SET]);
			} else {
				writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]));
			}	
		}
	}
}


