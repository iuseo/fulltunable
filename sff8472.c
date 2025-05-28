/*
 * FILE NAME            : sff8472.c
 * DATE                 : May 14 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : May 14 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <iods4835.h>
#include "math.h"
#include "main.h"
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appWrapper.h"
#include "appAdc.h"
#include "appTec.h"
#include "appGpio.h"
#include "sff8472.h"
#include "tl6100.h"
#include "sff8690.h"
#include "appSmartTune.h"

extern uint16 tempScale,tempOffset;
static uint8 tecHighAlarm, tecLowAlarm, tecHighWarning, tecLowWarning = 0;
sint16 DdmTemp = 0;
/*
 * Name         : convertAdcToDdm
 * Description  : Scheduled every 25ms in process25msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
void convertAdcToDdm(void)
{
	uint16 u16Data1, u16Data2;	
	signed int s16Data1, s16Data2; 	
//	uint8 u8Data;
	float p;

	/* Not to update A2_DiagData for 100ms when the host tries to read A2_DiagData */
	      
	/* Internally Measured Module Temperature, MSA-8472 */
	s16Data1 = (sint16)Reg2ByteToWordLoad(&A2_VendorFn[ADC_TEMP_ADD]);
	s16Data2 = temperatureCalibration((sint16)s16Data1, &A2_VendorFn[INT_TEMP_CAL_0_ADD]);
    DdmTemp = Reg2ByteToWordLoad(&A2_DiagData[DIAGNOSTICS_TEMP]);
	if(i2cSlaveReadFlag == ON) {
		return;
	}
	RegWordTo2ByteSave((sint16)s16Data2, &A2_DiagData[DIAGNOSTICS_TEMP]);

	/* Internally Measured Supply Voltage in Transceiver, MSA-8472 */		
	u16Data1 = Reg2ByteToWordLoad(&A2_VendorFn[ADC_VCC_ADD]);
	p = u16Data1;
	u16Data2 = p * VCC_DIV_SCALE_RATIO * SUPPLY_VOLTAGE_CONVERSION_FACTOR;
	s16Data2 = Reg2ByteToWordLoad(&A2_VendorFn[INT_VCC_OFFEST_ADD]);
	u16Data2 += s16Data2;
	if(i2cSlaveReadFlag == ON) {
		return;
	}
	RegWordTo2ByteSave(u16Data2, &A2_DiagData[DIAGNOSTICS_VCC]);

	/* Internally Measured Tx Bias Current, MSA-8472 */
        
        
	u16Data1 = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_MON_SOA]);
	u16Data2 = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_MON_REAR]);
	u16Data1 += u16Data2;
	u16Data2 = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_PHASE]);
	u16Data1 += u16Data2;
	u16Data2 = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_MON_FSN]);
	u16Data1 += u16Data2;
        
	if(i2cSlaveReadFlag == ON) {
		return;
	}
	if((A2_DiagData[STATUS_CONTROL_ADD] & B6_H) || (A2_DiagData[STATUS_CONTROL_ADD] & B7_H)){
		RegWordTo2ByteSave(0, &A2_DiagData[DIAGNOSTICS_TX_BIAS]);             
	}else{
		RegWordTo2ByteSave(u16Data1, &A2_DiagData[DIAGNOSTICS_TX_BIAS]);
	}

	/* Measured Tx Output Power, MSA-8472 */
	u16Data1 = Reg2ByteToWordLoad(&A2_VendorFn[ADC_TX_PWR_ADD]);
	//u16Data2 = convertVoltageToPower(&A2_VendorFn[INT_TX_PWR_CAL_0_ADD], (float)u16Data1);
	u16Data2 = convertVoltageToPower(&A2_TTOSA_MEMORY[REG_TTOSA_POWER_CAL], (float)u16Data1);		// T-TOSA channel calibration
	if(i2cSlaveReadFlag == ON) {
		return;
	}
	RegWordTo2ByteSave(u16Data2, &A2_DiagData[DIAGNOSTICS_TX_POWER]);

	/* Measured Rx Input Power, MSA-8472 */
	u16Data1 = Reg2ByteToWordLoad(&A2_VendorFn[ADC_RX_PWR_ADD]);	
	u16Data2 = convertVoltageToPower(&A2_VendorFn[INT_RX_PWR_CAL_0_ADD], (float)u16Data1);
        if(i2cSlaveReadFlag == ON) {
	   return;
	}
	RegWordTo2ByteSave(u16Data2, &A2_DiagData[DIAGNOSTICS_RX_POWER]);

	/* Measured Laser Temperature, MSA-8472 */
	u16Data1 = Reg2ByteToWordLoad(&A2_VendorFn[ADC_LD_THERMISTOR_ADD]);
	p = calculateThermistorResistance(u16Data1);
	s16Data2 = calculateThermistorTemperature(p);
    if(i2cSlaveReadFlag == ON) {
	   return;
	}
	RegWordTo2ByteSave((sint16)s16Data2, &A2_DiagData[DIAGNOSTICS_TEC_TEMP]);

	/* Measured TEC Current */

	u16Data1 = Reg2ByteToWordLoad(&A2_VendorFn[ADC_TEC_VOLTAGE_ADD]);
	p = (sint16)u16Data1;
	p = p / 4.0;
    s16Data2 = p / 0.1;
	
	if(i2cSlaveReadFlag == ON) {
	   return;
	}
	RegWordTo2ByteSave((sint16)s16Data2, &A2_DiagData[DIAGNOSTICS_TEC_CURR]);
}

/*
 * Name         : checkAlarmWarning
 * Description  : Scheduled every 25ms in process25msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 27 2019
 * History      : 
 */
void checkAlarmWarning(uint8* diagData)
{
	uint8 i;
    uint16 checkSum;
	uint16 u16Monitor, u16Threshold;
	sint16 s16Monitor, s16Threshold;
#if 1
	i = 0;
	s16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_TEMP + (i * 2)));		/* read monitoring */
	/* Temperature High Alarm */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_ALARM + (i * 8)));
	
    optimizeAlarmWarningFunction(s16Monitor, s16Threshold,ALARM_FLAGS_ADD, 0x80, i);
       
	/* Temperature Low Alarm */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_ALARM + (i * 8)));
        
	optimizeAlarmWarningFunction(s16Threshold,s16Monitor , ALARM_FLAGS_ADD, 0x40, i);
        /*
	if(s16Monitor < s16Threshold) {
		*(diagData + ALARM_FLAGS_ADD) |= (0x40 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD) &= ~(0x40 >> (i * 2));
	}*/	
	/* Temperature High Warning */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_WARNING + (i * 8)));
	optimizeAlarmWarningFunction(s16Monitor, s16Threshold, WARNING_FLAGS_ADD, 0x80, i);
        /*
	if(s16Monitor > s16Threshold) {
		*(diagData + WARNING_FLAGS_ADD) |= (0x80 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD) &= ~(0x80 >> (i * 2));
	}*/
	/* Temperature Low Warning */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_WARNING + (i * 8)));
	optimizeAlarmWarningFunction(s16Threshold,s16Monitor, WARNING_FLAGS_ADD, 0x40, i);
	/*
        if(s16Monitor < s16Threshold) {
		*(diagData + WARNING_FLAGS_ADD) |= (0x40 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD) &= ~(0x40 >> (i * 2));
	}*/
	
	for(i = 1; i < 4; i++)
	{
		u16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_TEMP + (i * 2)));		/* read monitoring */
		/* High Alarm for Vcc, Tx Bias & Tx Power */
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_ALARM + (i * 8)));
		optimizeUnsignedAlarmWarningFunction(u16Monitor,u16Threshold,ALARM_FLAGS_ADD,0x80,i);
                /*
		if(u16Monitor > u16Threshold) {
			*(diagData + ALARM_FLAGS_ADD) |= (0x80 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD) &= ~(0x80 >> (i * 2));
		}*/
		/* Low Alarm for Vcc, Tx Bias & Tx Power */		
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_ALARM + (i * 8)));
		optimizeUnsignedAlarmWarningFunction(u16Threshold,u16Monitor,ALARM_FLAGS_ADD,0x40,i);
                /*
		if(u16Monitor < u16Threshold) {
			*(diagData + ALARM_FLAGS_ADD) |= (0x40 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD) &= ~(0x40 >> (i * 2));
		}*/
		/* High Warning for Vcc, Tx Bias & Tx Power */
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_WARNING + (i * 8)));
		optimizeUnsignedAlarmWarningFunction(u16Monitor,u16Threshold,WARNING_FLAGS_ADD,0x80,i);
                /*
		if(u16Monitor > u16Threshold) {
			*(diagData + WARNING_FLAGS_ADD) |= (0x80 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD) &= ~(0x80 >> (i * 2));
		}*/
		/* Low Warning for Vcc, Tx Bias & Tx Power */
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_WARNING + (i * 8)));
		optimizeUnsignedAlarmWarningFunction(u16Threshold,u16Monitor,WARNING_FLAGS_ADD,0x40,i);
                /*
		if(u16Monitor < u16Threshold) {
			*(diagData + WARNING_FLAGS_ADD) |= (0x40 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD) &= ~(0x40 >> (i * 2));
		}*/
	}

	i = 0;
	u16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_RX_POWER + (i * 2)));		/* read monitoring */
	/* Rx Power High Alarm */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_ALARM));
	optimizeUnsignedAlarmWarningFunction(u16Monitor,u16Threshold,(ALARM_FLAGS_ADD+1),0x80,i);
        /*
	if(u16Monitor > u16Threshold) {
		*(diagData + ALARM_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
	}*/
	/* Rx Power Low Alarm */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_ALARM));
	optimizeUnsignedAlarmWarningFunction(u16Threshold,u16Monitor,(ALARM_FLAGS_ADD+1),0x40,i);
        /*
	if(u16Monitor < u16Threshold) {
		*(diagData + ALARM_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
	}*/
	/* Rx Power High Warning */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_WARNING));
	optimizeUnsignedAlarmWarningFunction(u16Monitor,u16Threshold,(WARNING_FLAGS_ADD+1),0x80,i);
        /*
	if(u16Monitor > u16Threshold) {
		*(diagData + WARNING_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
	}*/
	/* Rx Power Low Warning */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_WARNING));
	optimizeUnsignedAlarmWarningFunction(u16Threshold,u16Monitor,(WARNING_FLAGS_ADD+1),0x40,i);
        /*
	if(u16Monitor < u16Threshold) {
		*(diagData + WARNING_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
	}*/
	
//	u16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_TEC_TEMP + (i * 2)));		/* read monitoring */
//	/* LD Temp High Alarm */
//	u16Threshold = Reg2ByteToWordLoad(diagData + (LASER_TEMP_HIGH_ALARM));
//	optimizeUnsignedAlarmWarningFunction(u16Monitor,u16Threshold,(ALARM_FLAGS_ADD+1),0x20,i);
//
//	/* LD Temp Low Alarm */
//	u16Threshold = Reg2ByteToWordLoad(diagData + (LASER_TEMP_LOW_ALARM));
//	optimizeUnsignedAlarmWarningFunction(u16Threshold,u16Monitor,(ALARM_FLAGS_ADD+1),0x10,i);
//
//	/* LD Temp High Warning */
//	u16Threshold = Reg2ByteToWordLoad(diagData + (LASER_TEMP_HIGH_WARNING));
//	optimizeUnsignedAlarmWarningFunction(u16Monitor,u16Threshold,(WARNING_FLAGS_ADD+1),0x20,i);
//
//	/* LD Temp Low Warning */
//	u16Threshold = Reg2ByteToWordLoad(diagData + (LASER_TEMP_LOW_WARNING));
//	optimizeUnsignedAlarmWarningFunction(u16Threshold,u16Monitor,(WARNING_FLAGS_ADD+1),0x10,i);
	
    if(((A2_TTOSA_PAGE_0x02[TUNING_CURRENT_STATUS] & TUNING) == 0 || (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] & TUNING_ENABLE == 0))){
        u16Monitor = Reg2ByteToWordLoad(&A2_TECLUTAB[TEC_LUT_Add]);
        u16Monitor *= 0.037;    //convert to mV
        u16Monitor = calculateThermistorResistance(u16Monitor);
        s16Monitor = calculateThermistorTemperature(u16Monitor);
        
        RegWordTo2ByteSave(s16Monitor + 0x0200,&A2_DiagData[LASER_TEMP_HIGH_ALARM]); 
        RegWordTo2ByteSave(s16Monitor - 0x0200,&A2_DiagData[LASER_TEMP_LOW_ALARM]); 
        RegWordTo2ByteSave(s16Monitor + 0x0100,&A2_DiagData[LASER_TEMP_HIGH_WARNING]); 
        RegWordTo2ByteSave(s16Monitor - 0x0100,&A2_DiagData[LASER_TEMP_LOW_WARNING]);
        
        for(i = 0; i <95; i++)
        {
            checkSum += A2_DiagData[i];
        }
        A2_DiagData[CHECK_SUM] = (checkSum & 0x00FF);
    }
    for(i = 1; i < 3; i++)
	{
		s16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_RX_POWER + (i * 2)));	/* read monitoring */
		/* High Alarm for Optional Laser Temperature & Optical TEC Current */
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_ALARM + (i * 8)));
		optimizeAlarmWarningFunction(s16Monitor,s16Threshold,ALARM_FLAGS_ADD + 1,0x80,i);
        if(s16Threshold == Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_ALARM + (1 * 8)))){
          if(s16Monitor < s16Threshold){
              tecHighAlarm = 0;
          }else if(s16Monitor > s16Threshold){
              tecHighAlarm = 1;
          }
        }        
                /*
                if(s16Monitor > s16Threshold) {
			*(diagData + ALARM_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
		}*/
		/* Low Alarm for Optional Laser Temperature & Optical TEC Current */
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_ALARM + (i * 8)));
		optimizeAlarmWarningFunction(s16Threshold,s16Monitor,ALARM_FLAGS_ADD + 1,0x40,i);
        if(s16Threshold == Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_ALARM + (1 * 8)))){
            if(s16Monitor > s16Threshold){
              tecLowAlarm = 0;
            }else if(s16Monitor < s16Threshold){
              tecLowAlarm = 1;
            }
        }        
		/*
                if(s16Monitor < s16Threshold) {
			*(diagData + ALARM_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
		}*/
		/* High Warning for Optional Laser Temperature & Optical TEC Current */		
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_WARNING + (i * 8)));
		optimizeAlarmWarningFunction(s16Monitor,s16Threshold,WARNING_FLAGS_ADD + 1,0x80,i);
        if(s16Threshold == Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_WARNING + (1 * 8)))){
          if(s16Monitor < s16Threshold){
              tecHighWarning = 0;
          }else if(s16Monitor > s16Threshold){
              tecHighWarning = 1;
          }
        }                 
                /*
		if(s16Monitor > s16Threshold) {
			*(diagData + WARNING_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
		}*/
		/* Low Warning for Optional Laser Temperature & Optical TEC Current */
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_WARNING + (i * 8)));
		optimizeAlarmWarningFunction(s16Threshold,s16Monitor,WARNING_FLAGS_ADD + 1,0x40,i);
        if(s16Threshold == Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_WARNING + (1 * 8)))){
          if(s16Monitor > s16Threshold){
              tecLowWarning = 0;
          }else if(s16Monitor < s16Threshold){
              tecLowWarning = 1;
          }
        }        
        /*
                if(s16Monitor < s16Threshold) {
			*(diagData + WARNING_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
		}*/
        if((tecHighAlarm == 1) || (tecLowAlarm == 1) || (tecHighWarning == 1) || (tecLowWarning == 1)){        //HA, LA, HW, LW, 4개 중 하나라도 ON 일 때 tec_fault ON
            A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] |= 0x40;    
        } else if((tecHighAlarm == 0) && (tecLowAlarm == 0) && (tecHighWarning == 0) && (tecLowWarning == 0)){ //HA, LA, HW, LW 전부 OFF일 때 tec_fault OFF
            A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~0x40; 
        }        
	}    
	*(diagData + ALARM_FLAGS_ADD + 1) &= ~0x03;
	*(diagData + WARNING_FLAGS_ADD + 1) &= ~0x03;
#endif
        
#if 0
	i = 0;
	s16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_TEMP + (i * 2)));		/* read monitoring */
	/* Temperature High Alarm */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_ALARM + (i * 8)));
	if(s16Monitor > s16Threshold) {
		*(diagData + ALARM_FLAGS_ADD) |= (0x80 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD) &= ~(0x80 >> (i * 2));
	}
        

	/* Temperature Low Alarm */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_ALARM + (i * 8)));
	if(s16Monitor < s16Threshold) {
		*(diagData + ALARM_FLAGS_ADD) |= (0x40 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD) &= ~(0x40 >> (i * 2));
	}	
	/* Temperature High Warning */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_WARNING + (i * 8)));
	if(s16Monitor > s16Threshold) {
		*(diagData + WARNING_FLAGS_ADD) |= (0x80 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD) &= ~(0x80 >> (i * 2));
	}
	/* Temperature Low Warning */
	s16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_WARNING + (i * 8)));
	if(s16Monitor < s16Threshold) {
		*(diagData + WARNING_FLAGS_ADD) |= (0x40 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD) &= ~(0x40 >> (i * 2));
	}
	
	for(i = 1; i < 4; i++)
	{
		u16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_TEMP + (i * 2)));		/* read monitoring */
		/* High Alarm for Vcc, Tx Bias & Tx Power */
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_ALARM + (i * 8)));
		if(u16Monitor > u16Threshold) {
			*(diagData + ALARM_FLAGS_ADD) |= (0x80 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD) &= ~(0x80 >> (i * 2));
		}
		/* Low Alarm for Vcc, Tx Bias & Tx Power */		
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_ALARM + (i * 8)));
		if(u16Monitor < u16Threshold) {
			*(diagData + ALARM_FLAGS_ADD) |= (0x40 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD) &= ~(0x40 >> (i * 2));
		}
		/* High Warning for Vcc, Tx Bias & Tx Power */
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_HIGH_WARNING + (i * 8)));
		if(u16Monitor > u16Threshold) {
			*(diagData + WARNING_FLAGS_ADD) |= (0x80 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD) &= ~(0x80 >> (i * 2));
		}
		/* Low Warning for Vcc, Tx Bias & Tx Power */
		u16Threshold = Reg2ByteToWordLoad(diagData + (TEMP_LOW_WARNING + (i * 8)));
		if(u16Monitor < u16Threshold) {
			*(diagData + WARNING_FLAGS_ADD) |= (0x40 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD) &= ~(0x40 >> (i * 2));
		}
	}

	i = 0;
	u16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_RX_POWER + (i * 2)));		/* read monitoring */
	/* Rx Power High Alarm */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_ALARM));
	if(u16Monitor > u16Threshold) {
		*(diagData + ALARM_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
	}
	/* Rx Power Low Alarm */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_ALARM));
	if(u16Monitor < u16Threshold) {
		*(diagData + ALARM_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
	} else {
		*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
	}
	/* Rx Power High Warning */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_WARNING));
	if(u16Monitor > u16Threshold) {
		*(diagData + WARNING_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
	}
	/* Rx Power Low Warning */
	u16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_WARNING));
	if(u16Monitor < u16Threshold) {
		*(diagData + WARNING_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
	} else {
		*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
	}
	
	for(i = 1; i < 3; i++)
	{
		s16Monitor = Reg2ByteToWordLoad(diagData + (DIAGNOSTICS_RX_POWER + (i * 2)));	/* read monitoring */
		/* High Alarm for Optional Laser Temperature & Optical TEC Current */
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_ALARM + (i * 8)));
		if(s16Monitor > s16Threshold) {
			*(diagData + ALARM_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
		}
		/* Low Alarm for Optional Laser Temperature & Optical TEC Current */
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_ALARM + (i * 8)));
		if(s16Monitor < s16Threshold) {
			*(diagData + ALARM_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
		} else {
			*(diagData + ALARM_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
		}
		/* High Warning for Optional Laser Temperature & Optical TEC Current */		
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_HIGH_WARNING + (i * 8)));
		if(s16Monitor > s16Threshold) {
			*(diagData + WARNING_FLAGS_ADD + 1) |= (0x80 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x80 >> (i * 2));
		}
		/* Low Warning for Optional Laser Temperature & Optical TEC Current */
		s16Threshold = Reg2ByteToWordLoad(diagData + (RX_POWER_LOW_WARNING + (i * 8)));
		if(s16Monitor < s16Threshold) {
			*(diagData + WARNING_FLAGS_ADD + 1) |= (0x40 >> (i * 2));
		} else {
			*(diagData + WARNING_FLAGS_ADD + 1) &= ~(0x40 >> (i * 2));
		}
	}

	*(diagData + ALARM_FLAGS_ADD + 1) &= ~0x03;
	*(diagData + WARNING_FLAGS_ADD + 1) &= ~0x03;
#endif
        
}

void optimizeAlarmWarningFunction(sint16 data1, sint16 data2,uint8 addressOffset,uint8 value, uint8 index)
{
      if(data1 > data2) {
		A2_DiagData[addressOffset] |= (value >> (index * 2));         
      } else {
		A2_DiagData[addressOffset] &= ~(value >> (index * 2));
      }   
}

void optimizeUnsignedAlarmWarningFunction(uint16 data1, uint16 data2,uint8 addressOffset,uint8 value, uint8 index)
{
      if(data1 > data2) {
		A2_DiagData[addressOffset] |= (value >> (index * 2));         
      } else {
		A2_DiagData[addressOffset] &= ~(value >> (index * 2));
      }   
}

/*
 * Name         : convertVoltageToPower
 * Description  : 
 * Return Value : uint16
 * Parameter    : uint8*, float
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
static uint16 convertVoltageToPower(uint8* address, float data)
{
	uint8 i, u8Buffer[4];
	FP32 floatBuffer[5], floatData;

	for(i = 0; i < 5; i++)
	{
		memcpy(&u8Buffer[0], (address + (i * 4)), sizeof(uint32));	
		swap_uint32(&u8Buffer[0]);
		memcpy(&floatBuffer[i], &u8Buffer[0], sizeof(float));		
	}

	floatData = 0;
	for (i = 0; i < 5; i++) {
		//floatData += (floatBuffer[i] * calculatePower(data, i));
		floatData += (floatBuffer[i] * pow(data, i));
	}

//	floatData =floatBuffer[0]+floatBuffer[1]*data;

	if((data <= 1.0) || (floatData <= 1.0)) {
		floatData = (uint16)1;
	} else if(floatData >= 0xFFFF) {
		floatData = (uint16) 0xFFFF;
	}

	return (uint16) floatData;
}

float calculatePower(float data, uint8 power)
{
	float result;
	uint8 i;

	result = 1;
	for(i = 0; i < power; i++)
	{
		result *= data;
	}

	return result;
}

/*
 * Name         : temperatureCalibration
 * Description  : Converts the internal temperature of the chip to the temperature of the module
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
static sint16 temperatureCalibration(sint16 temperature, uint8* address)
{
	uint8 i, buffer[4];
	sint32 s32Data1, s32Data2;
	float floatBuffer[3], floatData;
	
	s32Data1 = temperature;
	s32Data1 *= tempScale;
	s32Data1 >>= 12;
	s32Data1 -= tempOffset;
	s32Data1 <<= 1;

//	temperature = (sint16) value;
	floatData = (float)s32Data1;
		
	for(i = 0; i < 3; i++) 
	{
		memcpy(&buffer[0], (address + (i * 4)), sizeof(uint32));	
		swap_uint32(&buffer[0]);
		memcpy(&floatBuffer[i], &buffer[0], sizeof(float));			
	}	
	s32Data2 = (floatBuffer[2] * floatData * floatData) + (floatBuffer[1] * floatData) + floatBuffer[0];
	
	return s32Data2; // 1unit 1/255 deg
}

/*
 * Name         : setStatusControlBits
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : May 22 2019
 * History      : 
 */
void setStatusControlBits(void)
{  
	if(TX_FAULT_HOST) {
		A2_DiagData[STATUS_CONTROL_ADD] &= B2_L;
	} else {
        A2_DiagData[STATUS_CONTROL_ADD] |= B2_H;
	}
	if(TX_DIS_HOST) {
		A2_DiagData[STATUS_CONTROL_ADD] |= B7_H;
	} else {
		A2_DiagData[STATUS_CONTROL_ADD] &= B7_L;
	}
	if(CDR_LOS) {
		A2_DiagData[STATUS_CONTROL_ADD] |= B1_H;
	} else {
		A2_DiagData[STATUS_CONTROL_ADD] &= B1_L;
	}
	if(RS0_HOST) {
		A2_DiagData[STATUS_CONTROL_ADD] |= 0x10;
	} else {
		A2_DiagData[STATUS_CONTROL_ADD] &= ~0x10;
	}
    if(RS1_HOST) {
        A2_DiagData[STATUS_CONTROL_ADD] |= 0x20;
    } else {
        A2_DiagData[STATUS_CONTROL_ADD] &= ~0x20;
    }        
}

/*
 * Name         : controlRDTFunc
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : Jan 24 2022
 * History      : 
 */
uint8 valBuff=0, valBuffPre=1;
void controlRDTFunc(unsigned char status){
	//unsigned char modeBuff=0;
	
	if(status == _ON){
		A2_GN2146Sync[RX_OFFSET_CORRECTION_CTRL_REG_0_Add] |= 0x07;
		
		valBuff = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_RDT_CONTROL];
		//valBuffPre = A2_DEBUGTAB[RDT_BUFF_Add];
		if(valBuff != valBuffPre){
			A2_DEBUGTAB[RDT_BUFF_Add] = valBuff;
			flashFlagByte2 |= A2_DEBUGTABWrFlag;
			valBuffPre = valBuff;
			if((valBuff >= 0x80) && (valBuff <= 0xFE)){ 	//negative value
				valBuff -= 0x80;
			}else if((valBuff >= 0x01) && (valBuff <= 0x7F)){ 				//positive or zero value
				valBuff += 0x80;
			}else if(valBuff == 0xFF){
				valBuff -= 0x81;
			}else if(valBuff == 0x00){
			  	valBuff = 0x80;
			}
			A2_GN2146Sync[RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0_Add] = valBuff;
			I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0, &A2_GN2146Sync[RX_OFFSET_CORRECTION_OVERWRITE_VAL_REG_0_Add], 1);	// setting slice level			
		}	
	}else{
		A2_GN2146Sync[RX_OFFSET_CORRECTION_CTRL_REG_0_Add] = 0x02;
	}
	

}
