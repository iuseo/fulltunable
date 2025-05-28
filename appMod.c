/*
 * FILE NAME            : appMod.c
 * DATE                 : March 21 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 21 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <iods4835.h>
#include "main.h"
#include "includes.h"
#include "appTimer.h"
#include "appWrapper.h"
#include "appMod.h"
#include "max5112.h"
#include "appUart.h"

extern uint16 lddRegMap[DACNum][2];

/*
 * Name         : controlMod1Register
 * Description  : Scheduled every 50ms in process50msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
void controlMod1Register(void)
{
	uint8 data;

#ifdef SAMSUNG
	if(lddRegMap[AMPU_REG][SET] != lddRegMap[AMPU_REG][BUF]) {
#else
	if((lddRegMap[AMPU_REG][SET] != lddRegMap[AMPU_REG][BUF]) && !(A2_UserData[AMCC_LINK_STATUS] & 0x1)) {
#endif

	  	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
		
		data = lddRegMap[AMPU_REG][SET] & 0x7;
		I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_LD_MAIN_SWING_REG_0, &data, 1);
		data = (lddRegMap[AMPU_REG][SET] & 0x3f8) >> 3;
		I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_LD_MAIN_SWING_REG_1, &data, 1);

		lddRegMap[AMPU_REG][BUF] = lddRegMap[AMPU_REG][SET];
		if(A2_VendorFn[LDMODE_Add] & LUTMOD_ENABLE) {
			RegWordTo2ByteSave(lddRegMap[AMPU_REG][SET],&A2_VendorFn[DAC_AMP_SET_Add]);     		
		}    
		I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_CDR_BYPASS_CTRL_REG0, &data, 1);
		A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;

	}
}

