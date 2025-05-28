/*
 * FILE NAME            : appIDac.c
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
#include <stdlib.h>
#include "main.h"
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appWrapper.h"
#include "appIDac.h"
#include "appGpio.h"

extern uint16 lddRegMap[DACNum][2];

/*
 * Name         : initializeIDac
 * Description  : IDAC1 with GP13, IDAC2 with GP14, IDAC3 with GP15, IDAC4 with GP16
 * Return Value : void
 * Parameter    : uint8, uint16
 * Author       : 
 * Date         : March 25 2019
 * History      : 
 */
void initializeIDac(uint8 channel, uint16 data)
{ 
	switch(channel)
	{	
		case IDAC00 :
			/* Full Scale Current(mA) */
			IDCN1_bit.RANGE_SEL = 0;	/* [3:2] : 00 for 25mA, 01 for 50mA, 10 for 100mA, 11 for 200mA */               
			IDCN1_bit.EN_2MA = 1;		/* Disable the 2mA current range of IDAC */
			IDCN1_bit.IDAC_EN = 1;		/* Enable the current DAC function */
			IDCD1 = data & 0x0FFF;		/* [11:0] */
			break;
		case IDAC01 :
			IDCN2_bit.RANGE_SEL = 0;	/* [3:2] : 00 for 25mA, 01 for 50mA, 10 for 100mA, 11 for 200mA */               
			IDCN2_bit.EN_2MA = 1;		/* Disable the 2mA current range of IDAC */
			IDCN2_bit.IDAC_EN = 1;		/* Enable the current DAC function */
			IDCD2 = data & 0x0FFF;		/* [11:0] */
			break;
		case IDAC02 :
			IDCN3_bit.RANGE_SEL = 0;	/* [3:2] : 00 for 25mA, 01 for 50mA, 10 for 100mA, 11 for 200mA */               
			IDCN3_bit.EN_2MA = 1;		/* Disable the 2mA current range of IDAC */
			IDCN3_bit.IDAC_EN = 1;		/* Enable the current DAC function */
			IDCD3 = data & 0x0FFF;		/* [11:0] */	
			break;
		case IDAC03 :
			/* Full Scale Current(mA) */
			IDCN4_bit.RANGE_SEL = 0;	/* [3:2] : 00 for 25mA, 01 for 50mA, 10 for 100mA, 11 for 200mA */
			IDCN4_bit.EN_2MA = 1;		/* Disable the 2mA current range of IDAC */
			IDCN4_bit.IDAC_EN = 1;		/* Enable the current DAC function */
			IDCD4 = data & 0x0FFF;		/* [11:0] */
			break;
		default :
			break;
	}
}

/*
 * Name         : iDacOnOff
 * Description  : Enable or disable IDAC1(GP13) & IDAC4(GP16)
 * Return Value : void
 * Parameter    : uint8, uint8
 * Author       : 
 * Date         : March 28 2019
 * History      : 
 */
void iDacOnOff(uint8 channel, uint8 onOff)
{
	switch(channel)
	{
		case IDAC00 :
			if(onOff == ON) {
				IDCN1 |= 0x0001;
			} else {
				IDCN1 &= ~0x0001;
			}
			break;
                case IDAC01 :
			if(onOff == ON) {
				IDCN2 |= 0x0001;
			} else {
				IDCN2 &= ~0x0001;
			}
			break;        
		case IDAC03 :
			if(onOff == ON) {
				IDCN4 |= 0x0001;
			} else {
				IDCN4 &= ~0x0001;
			}
			break;
		default :
			break;
	}
}

void controlAgcRegister(void)
{
   // uint16 agcSlope=0,agcOffset=0,rssi=0,preRssi=0;
    //float p=0;
    //uint16 agcData=0;
    //sint32 slopeBuffer=0,offsetBuffer=0,rssiBuffer=0,preRssiBuffer=0,rssiAbs=0;
   /* 
    if( (A2_VendorFn[CTRLPIN_Add] & 0x1) == 0x1){

          rssi = convert2BytesToWord(&A2_VendorFn[ADC_RX_PWR_ADD]);
          preRssi = convert2BytesToWord(&A2_VendorFn[ADC_RX_PWR_TEMP_ADD]);
          rssiBuffer = rssi;
          preRssiBuffer = preRssi;
          rssiAbs = abs(rssiBuffer-preRssiBuffer);
          
          if( (rssi > 500) && (IDCD2 == 1000) ){
                 if(rssi>900){
                   if(IDCD2 == 0){
                        ;
                   }else{
                        agcData = 0;
                        IDCD2 = agcData & 0x0fff;
                   }
                 }
          }else{
                if(rssiAbs > 15){
                  //EIE0_bit.EIE06 = OFF;
                  //EIF0_bit.EIF06 = OFF;
                  agcSlope = convert2BytesToWord(&A2_VendorFn[DAC_AGC_SLOPE]);
                  agcOffset = convert2BytesToWord(&A2_VendorFn[DAC_AGC_OFFSET]);
                  
                  slopeBuffer = agcSlope;
                  offsetBuffer = agcOffset;
                  
                  if( (rssi > 500) || (rssi <20) ){
                    
                    if(rssi > 900){
                        agcData = 0;
                    }else{
                        agcData = 1000;
                    }
                  }else{
                    p = (((float)-slopeBuffer)/100)*((float)rssi)+(((float)offsetBuffer)/10);
                    agcData = p;
                  }
                  
                  if(agcData > 3500){
                    agcData = 3500;
                  }
                  
                  IDCD2 = agcData & 0x0fff;
                  
                  convertWordTo2Bytes(agcData,&A2_VendorFn[DAC_RESERVED_2_Add]);
                  convertWordTo2Bytes(rssi,&A2_VendorFn[ADC_RX_PWR_TEMP_ADD]);
                  timerDelay(100);
                  //EIF0_bit.EIF06 = OFF;
                  //EIE0_bit.EIE06 = ON;     
                }
          }
    }else if((A2_VendorFn[CTRLPIN_Add] & 0x1) == 0x0) {
    */  
    /*
       rssi = convert2BytesToWord(&A2_VendorFn[ADC_RX_PWR_ADD]);
          if( rssi <20 ){
              //lddRegMap[AGCC_REG][SET] = 1000;
          }
      */
    /*
       if(lddRegMap[AGCC_REG][SET] != lddRegMap[AGCC_REG][BUF]) {     
          //EIE0_bit.EIE06 = OFF;
          //EIF0_bit.EIF06 = OFF;    
          
          if(lddRegMap[AGCC_REG][SET] > 4000){
              lddRegMap[AGCC_REG][SET] = 4000;
          }
  
          IDCD2 = lddRegMap[AGCC_REG][SET] & 0x0fff;    
          RegWordTo2ByteSave(lddRegMap[AGCC_REG][SET],&A2_VendorFn[DAC_AGC_SET_Add]);
          lddRegMap[AGCC_REG][BUF] = lddRegMap[AGCC_REG][SET];     
          //timerDelay(200);
          //EIF0_bit.EIF06 = OFF;
          //EIE0_bit.EIE06 = ON;    
      }
    //}
    */  
}
