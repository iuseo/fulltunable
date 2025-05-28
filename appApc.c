/*
 * FILE NAME            : appApc.c
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
#include "appApc.h"

extern uint16 lddRegMap[DACNum][2];

/*
 * Name         : controlApc1Register
 * Description  : Scheduled every 50ms in process50msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
void controlApc1Register(void)
{
//	uint8 data;

	if(lddRegMap[APCL_REG][SET] != lddRegMap[APCL_REG][BUF]) {
#if 0
		data = (uint8)lddRegMap[APCL_REG][SET];
		/* Sets the APC final target reference threshold */
		i2cMWriteBytes(I2CM_MASTER_1, GN1444SDevADD, APC_REG6, &data, 1);
		if(A2_VendorFn[LDMODE_Add] & LUTAPC_ENABLE) {
			convertWordTo2Bytes(lddRegMap[APCL_REG][SET], &A2_VendorFn[DAC_APC_SET_Add]);
		}
		lddRegMap[APCL_REG][BUF] = lddRegMap[APCL_REG][SET];
#else
		/* ToDo */
#endif
	}
}

