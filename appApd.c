/*
 * FILE NAME            : appApd.c
 * DATE                 : March 21 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 21 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <intrinsics.h>
#include <iods4835.h>
#include "main.h"
#include "includes.h"
#include "appTimer.h"
#include "appWrapper.h"
#include "appAdc.h"
#include "appApd.h"

extern uint16 lddRegMap[DACNum][2];

/*
 * Name         : initializeApdBoost
 * Description  : APD1 with GP22
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 17 2019
 * History      : 
 */
void initializeApdBoost(void)
{
	/* Configure APD */
	APDIDX1 = IDX_ERRP_START_CLAMP;			/* Error Start Clamp */
//	APDDAT1 = __swap_bytes(0x3F00);			/* 128 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_ErrorClamp_Add]);
	APDIDX1 = IDX_DUTY_CLAMP;                                       
//	APDDAT1 = __swap_bytes(0x00EF);			/* 0xEF00 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_DutyClamp_Add]);
	APDIDX1 = IDX_INTEGAL_GAIN;				/* Integral gain */
//	APDDAT1 = __swap_bytes(0x0801);			/* 0x0108 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_InternalGain_Add]);
	APDIDX1 = IDX_START_DUTY_CLAMP;			/* Duty Start Clamp */
//	APDDAT1 = __swap_bytes(0x0A00);			/* 0x000A */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_DutyStartClamp_Add]);
	APDIDX1 = IDX_CLAMP_INTEGAL;			/* Integral Clamp */
//	APDDAT1 = __swap_bytes(0x00FF);			/* 0xFF00 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_IntergratorClamp_Add]);
	APDIDX1 = IDX_ERRN_CLAMP;				/* Error Negative Clamp */
//	APDDAT1 = __swap_bytes(0x0800);			/* 0x0080, 128 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_NegativeErrorClamp_Add]);
	APDIDX1 = IDX_ERRP_CLAMP;				/* Error Positive Clamp */
//	APDDAT1 = __swap_bytes(0x7F00);			/* 127 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_PositiveErrorClamp_Add]);
	APDIDX1 = IDX_PROP_GAIN;				/* Proportional Gain */
//	APDDAT1 = __swap_bytes(0x8001);			/* 0x0180, 200 */
	APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_ProportionalGain_Add]);
	APDIDX1 = IDX_TARGET;					/* Target Voltage */
	APDDAT1 = 0x0000;            
	APDCN1 = 0x2040;						/* 0x0844, Enable APD with DS mode */
}

/*
 * Name         : initializeVeeDcDc
 * Description  : APD3 with GP17
 * Return Value : void
 * Parameter    : uint16
 * Author       : 
 * Date         : April 17 2019
 * History      : 
 */
void initializeVeeDcDc(uint16 target)
{  
	APDIDX2 = IDX_ERRP_START_CLAMP;			/* Error Start Clamp */
	APDDAT2 = __swap_bytes(0x3F00);			/* 128 */
	APDIDX2 = IDX_DUTY_CLAMP;
	APDDAT2 = __swap_bytes(0x00EF);			/* 0xEF00 */
	APDIDX2 = IDX_INTEGAL_GAIN;				/* Integral gain */
	APDDAT2 = __swap_bytes(0x2000);			/* 0x0108 */
	APDIDX2 = IDX_START_DUTY_CLAMP;			/* Duty Start Clamp */
	APDDAT2 = __swap_bytes(0x3F00);			/* 0x000A */
	APDIDX2 = IDX_CLAMP_INTEGAL;			/* Integral Clamp */
	APDDAT2 = __swap_bytes(0x00EF);			/* 0xFF00 */
	APDIDX2 = IDX_ERRN_CLAMP;				/* Error Negative Clamp */
	APDDAT2 = __swap_bytes(0x7F00);			/* 0x0080, 128 */
	APDIDX2 = IDX_ERRP_CLAMP;				/* Error Positive Clamp */
	APDDAT2 = __swap_bytes(0x7F00);			/* 127 */
	APDIDX2 = IDX_PROP_GAIN;				/* Proportional Gain */
	APDDAT2 = __swap_bytes(0x8001);			/* 0x0180, 200 */
	APDIDX2 = IDX_TARGET;					/* Target Voltage */
	APDDAT2 = target;
	APDCN2 = 0xA044;						/* 0x0844, Enable APD with DS mode *//* Configure APD */

}


/*
 * Name         : controlApd1Register
 * Description  : Scheduled every 50ms in process50msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 28 2019
 * History      : 
 */
void controlApd1Register(void)
{
	uint16 rawAdc;
	uint16 data;
	
	data = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_CN_Add]);
    if(A2_DEBUGTAB[SHUTDOWN_FLAG] == _ON) {
    } else {
	    APDCN1 = data;
    }
	RegWordTo2ByteSave(data, &A2_APDLUTAB[APD1_CN_Add]);
	
	/* APD */
	if(lddRegMap[APDS_REG][SET] != lddRegMap[APDS_REG][BUF]) {	/* APD Setting */
             
		if(lddRegMap[APDS_REG][SET] > 4000) {
			lddRegMap[APDS_REG][SET] = 4000;
		}

		rawAdc = convertApdRawToVoltage(lddRegMap[APDS_REG][SET]);
               
		APDIDX1 = IDX_TARGET;		/* Target Voltage */
		APDDAT1 = rawAdc;  	
		if(A2_VendorFn[LDMODE_Add] & LUTAPD_ENABLE) {
			RegWordTo2ByteSave(lddRegMap[APDS_REG][SET], &A2_VendorFn[DAC_APD_SET_Add]); 			
		}
		lddRegMap[APDS_REG][BUF] = lddRegMap[APDS_REG][SET];
	}
#if 1
	if(apdTableFlag == ON) {
		apdTableFlag = OFF;
		APDIDX1 = IDX_ERRP_START_CLAMP;			/* Error Start Clamp */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_ErrorClamp_Add]);
		APDIDX1 = IDX_DUTY_CLAMP;                                       
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_DutyClamp_Add]);
		APDIDX1 = IDX_INTEGAL_GAIN;				/* Integral gain */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_InternalGain_Add]);
		APDIDX1 = IDX_START_DUTY_CLAMP;			/* Duty Start Clamp */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_DutyStartClamp_Add]);
		APDIDX1 = IDX_CLAMP_INTEGAL;			/* Integral Clamp */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_IntergratorClamp_Add]);
		APDIDX1 = IDX_ERRN_CLAMP;				/* Error Negative Clamp */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_NegativeErrorClamp_Add]);
		APDIDX1 = IDX_ERRP_CLAMP;				/* Error Positive Clamp */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_PositiveErrorClamp_Add]);
		APDIDX1 = IDX_PROP_GAIN;				/* Proportional Gain */
		APDDAT1 = Reg2ByteToWordLoad(&A2_APDLUTAB[APD1_ProportionalGain_Add]);
		APDIDX1 = IDX_FEEDBACK_ERR;
		RegWordTo2ByteSave(APDDAT1,&A2_GN2146Sync[APD1_FeedbackErr_Add]);
//                RegWordTo2ByteSave(APDDAT1,&A2_APDLUTAB[0xFE-0x80]);
		//RegWordTo2ByteSave(APDDAT1,&A2_APDLUTAB[APD1_DutyCycle_Add]);
		APDIDX1 = IDX_CLAMP_STATUS;
		RegWordTo2ByteSave(APDDAT1,&A2_GN2146Sync[APD1_ClampStatus_Add]);
		APDIDX1 = IDX_DUTY_CYCLE;
		RegWordTo2ByteSave(APDDAT1,&A2_GN2146Sync[APD1_DutyCycle_Add]);
	}
#endif
}

/*
 * Name         : apdOnOff
 * Description  : 
 * Return Value : void
 * Parameter    : uint8
 * Author       : 
 * Date         : March 27 2019
 * History      : 
 */
/*
void apdOnOff(uint8 onOff)
{
	if(onOff == ON) {
		APDCN1 = 0x0044;
		APDCN2 = 0xA044;
	} else {
		APDCN1 = 0x0040;
		APDCN2 = 0xA040;
	}
}
*/
/*
 * Name         : convertApdRawToVoltage
 * Description  : 
 * Return Value : uint16
 * Parameter    : uint16
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
uint16 convertApdRawToVoltage(uint16 adcRaw)
{
	float result, data, fbr;
         
	fbr = 20.0/(510.0+20.0);
	adcRaw += 60;   // offset
	data = adcRaw*10.0;
     
	result = (data*fbr*65535) / 2400;
//  data = adcRaw;
//  result = data * 10.5; // APD step = mV convert

	return (uint16)result;
}

