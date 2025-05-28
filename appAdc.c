/*
 * FILE NAME            : appAdc.c
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
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appWrapper.h"
#include "appAdc.h"
#include "appGpio.h"
/*
 * Name         : initializeAdc
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
void initializeAdc(void)
{
  
	
        /* Thermistor Temperature , ADC01 with GP01 */
//	adcConfig(CONFIG_THERMISTOR_TEMPERATURE, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);    
//	adcChannelConfig(CONFIG_THERMISTOR_TEMPERATURE, ADC01_GP01_TOTEMP, GND_REF);
    
	/* Thermistor Temperature , ADC01 with GP01 */
	//adcConfig(CONFIG_THERMISTOR_TEMPERATURE, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);      
	ADC_Config(CONFIG_THERMISTOR_TEMPERATURE, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);      
	ADC_Channel_Config(CONFIG_THERMISTOR_TEMPERATURE, ADC16_GP20_TO_TEMP, GND_REF);
    
	/* Tx Wave */
	//adcConfig(CONFIG_TX_WAVE, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);         
	ADC_Config(CONFIG_TX_WAVE, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);         
	ADC_Channel_Config(CONFIG_TX_WAVE, ADC01_GP01_TXWAVE, GND_REF);
  
	/* ET MON */
	ADC_Config(CONFIG_ET_MON, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);         
	ADC_Channel_Config(CONFIG_ET_MON, ADC12_GP14_ET_MON, GND_REF);
	
	/* APD1(GP22), ADC02 with GP02 */  
	ADC_Config(CONFIG_EXT_BOOSTER_APD1_FB, PRIORITY_1, ACQ_EXT_1_0us, ADC_AVG_008SMP, ADC_UNI_POLAR);    // adc sampling average change
	ADC_Channel_Config(CONFIG_EXT_BOOSTER_APD1_FB, ADC02_GP02_APD1_FB, GND_REF);
    
	/* RSSI, ADC05 with GP05 */
	//adcConfig(CONFIG_RSSI, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);     
	ADC_Config(CONFIG_RSSI, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);     
	ADC_Channel_Config(CONFIG_RSSI, ADC05_GP05_RSSI, GND_REF);
        
	/* DCDC3, VCC1.6V_FB, ADC07 with GP07 */
	ADC_Config(CONFG_DCDC3_LX3_FB, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_004SMP, ADC_UNI_POLAR);    
	ADC_Channel_Config(CONFG_DCDC3_LX3_FB, ADC07_GP07_RSSI, GND_REF);
        
	/* VTEC, TEC+ Voltage, ADC10 with GP12 */
	ADC_Config(CONFIG_VTEC, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_BI_POLAR);    
	ADC_Channel_Config(CONFIG_VTEC, ADC10_GP12_VTEC, ADC08_LX2);

	/* Tx Power */
	//adcConfig(CONFIG_TX_POWER, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);    
	ADC_Config(CONFIG_TX_POWER, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);    
	ADC_Channel_Config(CONFIG_TX_POWER, ADC15_GP17_TX_POWER, GND_REF);
        

        
	/* APD3(GP21), ADC17 with GP21 */  
	ADC_Config(CONFIG_VEE_DCDC_APD2_FB, PRIORITY_1, ACQ_EXT_1_0us, ADC_AVG_001SMP, ADC_BI_POLAR);    
	ADC_Channel_Config(CONFIG_VEE_DCDC_APD2_FB, GND_REF, ADC17_GP21_APD3_FB);

	/* TEC1 */
	//adcConfig(CONFIG_TEC1_THERMISTOR, PRIORITY_2, ACQ_EXT_1_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);    
	ADC_Config(CONFIG_TEC1_THERMISTOR, PRIORITY_1, ACQ_EXT_1_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);    
	ADC_Channel_Config(CONFIG_TEC1_THERMISTOR, ADC16_GP20_TO_TEMP, GND_REF);  
        
	/* VAPD , ADC23 with GP27 */
	//adcConfig(CONFIG_VAPD_MON, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);    
	ADC_Config(CONFIG_VAPD_MON, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);    
	ADC_Channel_Config(CONFIG_VAPD_MON, ADC23_GP27_VAPD_MON, GND_REF);
        
        /* VCC */
	//adcConfig(CONFIG_VCC, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_128SMP, ADC_UNI_POLAR);         
	ADC_Config(CONFIG_VCC, PRIORITY_2, ACQ_EXT_2_0us, ADC_AVG_016SMP, ADC_UNI_POLAR);         
	ADC_Channel_Config(CONFIG_VCC, ADC29_VCC_DIV, GND_REF);
        
	TEMPCN_bit.ETEMP = 0;                            //External Temperature Sensor pin is shared with inverter FB so it is not being used in EVKIT
	TEMPCN_bit.ITEMP = 1;
	TEMPCN_bit.TEMP_AVG = 5;			/* Number of samples averaged : Invalid */  
	TEMPCN_bit.MAVG_EN = 1;

//	ADST_bit.ADDAIE = 0;				/* ADC Data Available Interrupt Enable */
//	ADST_bit.ITEMPIE = 0;				/* Internal Temperature Interrupt Enable */

	ADCN = ADCNSET;                                                            //ADC Enable    
}

/*
 * Name         : readAdcChannels
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
void readAdcChannels(void)
{
	uint16 adcValue = 0;
	uint16 mV = 0;
	sint16 tecV = 0;
	sint16 temperature = 0;
	
	if(ADST_bit.ADDAI) {
		/* Measured Laser Temperature */
		adcValue = ADC_ReadOut(CONFIG_THERMISTOR_TEMPERATURE);
		RegWordTo2ByteSave(adcValue, &A2_VendorFn[ADC_LD_THERMISTOR_RAW_ADD]);   		
          
		/* Internally Measured Supply Voltage in Transceiver */
		mV = ADC_ConverttomV(CONFIG_VCC);		/* VCC_DIV = VCC / 4 */
		RegWordTo2ByteSave(mV, &A2_VendorFn[ADC_VCC_ADD]);
		/* Measured Tx Power */
		mV = ADC_ConverttomV(CONFIG_TX_POWER);
		RegWordTo2ByteSave(mV, &A2_VendorFn[ADC_TX_PWR_ADD]);        	  
		/* Measured Tx Wave */
		mV = ADC_ConverttomV(CONFIG_TX_WAVE);
		RegWordTo2ByteSave(mV, &A2_VendorFn[ADC_TX_WAVE_ADD]);         
		/* Measured Rx Input Power */
		mV = ADC_ConverttomV(CONFIG_RSSI);		/* RSSI */
		RegWordTo2ByteSave(mV, &A2_VendorFn[ADC_RX_PWR_ADD]);  
          
  
		mV = ADC_ConverttomV(CONFIG_THERMISTOR_TEMPERATURE);
		RegWordTo2ByteSave(mV, &A2_VendorFn[ADC_LD_THERMISTOR_ADD]);
          
		/* APD_FB */
		mV = ADC_ConverttomV(CONFIG_EXT_BOOSTER_APD1_FB);
		RegWordTo2ByteSave(mV, &A2_VendorFn[ADC_APD_FB_ADD]);
    
		/* VTEC with ITEC_MON */
		tecV = ADC_ConverttomV_BI(CONFIG_VTEC);
		RegWordTo2ByteSave((uint16)tecV, &A2_VendorFn[ADC_TEC_VOLTAGE_ADD]);
  
		/* VAPD MON */
		adcValue = ADC_ReadOut(CONFIG_VAPD_MON);
		RegWordTo2ByteSave(adcValue, &A2_VendorFn[ADC_VAPD_MON_ADD]);
         
		/* ET MON */
		adcValue = ADC_ReadOut(CONFIG_ET_MON);
		RegWordTo2ByteSave(adcValue, &A2_VendorFn[ADC_ET_MON_ADD]);
		
		/* RSSI quick trip */
		QT_Config(QTCONFG_ADC02_RSSI,Reg2ByteToWordLoad(&A2_DEBUGTAB[RSSI_TH_L])/0.037, Reg2ByteToWordLoad(&A2_DEBUGTAB[RSSI_TH_H])/0.037); 
		ADC_Config(QTCONFG_ADC02_RSSI, PRIORITY_1, ACQ_EXT_2_0us, ADC_AVG_001SMP, 0); 
		ADC_Channel_Config(QTCONFG_ADC02_RSSI, ADC05_GP05_RSSI, GND_REF);			
		

		ADST_bit.ADDAI = 0;
	}
        
	if(ADST_bit.ITEMPI) {
		temperature = ITEMP_DATA;
		RegWordTo2ByteSave((uint16)temperature, &A2_VendorFn[ADC_TEMP_ADD]);
		ADST_bit.ITEMPI = 0;
	}
}

/*
 * Name         : readInternalTemperature
 * Description  : Reads the internal temperature of the chip
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
void readInternalTemperature(void)
{
  /*
	sint16 temperature = 0;
        if(ADST_bit.ITEMPI) {
          temperature = ITEMP_DATA;
          RegWordTo2ByteSave((uint16)temperature, &A2_VendorFn[ADC_TEMP_ADD]);
          ADST_bit.ITEMPI = 0;
      }
  */
}

/*
 * Name         : convertVoltageToAdcRaw
 * Description  : 
 * Return Value : uint16
 * Parameter    : uint16
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
uint16 convertVoltageToAdcRaw(uint16 voltage)
{
	float result, data;
	
	data = voltage;
	result = data / 0.037;
        
	return (uint16)result;
}

