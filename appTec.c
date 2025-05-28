/*
 * FILE NAME            : appTec.c
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
#include "math.h"
#include "main.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appWrapper.h"
#include "appTec.h"
#include "appAdc.h"
#include "sff8472.h"
#include "appGpio.h"
#include "sff8690.h"
#include "tl6100.h"

extern uint16 lddRegMap[DACNum][2];

/*
 * Name         : initializeTecChannel
 * Description  : Enables one of TEC channel
 *				  target : 16-bit value wrt 6.55V full scale
 * Return Value : void
 * Parameter    : uint16
 * Author       : 
 * Date         : March 25 2019
 * History      : 
 */
void initializeTecChannel(uint16 channel)
{
        TECCN1 = 0x00;
#if 0 /* ksPark*/
	if(channel == TEC_CH1) {
		configTec1(INDEX_TEC_Kp, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKP_Add]));
		configTec1(INDEX_TEC_Ki, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKI_Add]));
		configTec1(INDEX_TEC_Kd, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKD_Add]));
		configTec1(INDEX_TEC_SHIFT, Reg2ByteToWordLoad(&A2_TECLUTAB[TECSHIFT_Add]));
		configTec1(INDEX_TEC_POS_ERR_CP, Reg2ByteToWordLoad(&A2_TECLUTAB[TECPosERRClamp_Add]));
		configTec1(INDEX_TEC_NEG_ERR_CP, Reg2ByteToWordLoad(&A2_TECLUTAB[TECNegERRClamp_Add]));
		configTec1(INDEX_TEC_POS_OP_CP, 0x0000);
		configTec1(INDEX_TEC_NEG_OP_CP, 0xFFFF);
		configTec1(INDEX_TEC_CURR_CONFIG, 0x0398);
		configTec1(INDEX_TEC_OFFSET_TRIG, 0x0000);
		configTec1(INDEX_TEC_DUTY_OFFSET, 0x0000);
                TECCN1 = 0xC22B;
	} else {
		configTec2(INDEX_TEC_Kp, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKP_Add]));
		configTec2(INDEX_TEC_Ki, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKI_Add]));
		configTec2(INDEX_TEC_Kd, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKD_Add]));
		configTec2(INDEX_TEC_SHIFT, Reg2ByteToWordLoad(&A2_TECLUTAB[TECSHIFT_Add]));
		configTec2(INDEX_TEC_POS_ERR_CP, Reg2ByteToWordLoad(&A2_TECLUTAB[TECPosERRClamp_Add]));
		configTec2(INDEX_TEC_NEG_ERR_CP, Reg2ByteToWordLoad(&A2_TECLUTAB[TECNegERRClamp_Add]));
		configTec2(INDEX_TEC_POS_OP_CP, 0x0000);
		configTec2(INDEX_TEC_NEG_OP_CP, 0xFFFF);
		configTec2(INDEX_TEC_CURR_CONFIG, 0x0398);
		configTec2(INDEX_TEC_OFFSET_TRIG, 0x0000);
		configTec2(INDEX_TEC_DUTY_OFFSET, 0x0000);
                TECCN2 = 0xC22B;
		/* TEC should be enabled just before ADC enable */
	}
#else
		ConfigTec1(INDEX_TEC_Kp, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKP_Add]));
		ConfigTec1(INDEX_TEC_Ki, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKI_Add]));
		ConfigTec1(INDEX_TEC_Kd, Reg2ByteToWordLoad(&A2_TECLUTAB[TECKD_Add]));
		ConfigTec1(INDEX_TEC_SHIFT, Reg2ByteToWordLoad(&A2_TECLUTAB[TECSHIFT_Add]));
		ConfigTec1(INDEX_TEC_POS_ERR_CP, Reg2ByteToWordLoad(&A2_TECLUTAB[TECPosERRClamp_Add]));
		ConfigTec1(INDEX_TEC_NEG_ERR_CP, Reg2ByteToWordLoad(&A2_TECLUTAB[TECNegERRClamp_Add]));
		ConfigTec1(INDEX_TEC_POS_OP_CP, 0x0000);
		ConfigTec1(INDEX_TEC_NEG_OP_CP, 0xFFFF);
		ConfigTec1(INDEX_TEC_CURR_CONFIG, 0x0398);
		ConfigTec1(INDEX_TEC_OFFSET_TRIG, 0x0000);
		ConfigTec1(INDEX_TEC_DUTY_OFFSET, 0x0000);
        TECCN1 = 0xC22B;
#endif
	
		NORMV = 0x4C90;
} 

/*
 * Name         : inrushForTec
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 25 2019
 * History      : 
 */
void inrushForTec(void)
{
	uint8 i, calStep, inrushStep, inrushCount;
	uint16 rawAdc,voltageData,tecEnable;
	sint16 data, startLimit, step;
	float currentTemperature, targetTemperature, deltaTemperature, p, tolerance;

	readAdcChannels();
	//readInternalTemperature();

	convertAdcToDdm();
	data = (sint16)Reg2ByteToWordLoad(&A2_DiagData[DIAGNOSTICS_TEMP]);
	currentTemperature = ((float)data) / 256;

	setTemperatureIndex(data);
	controlLddRegMap();
	
	tecEnable= Reg2ByteToWordLoad(&A2_TECLUTAB[TECCN_Add]);
	if( (A2_VendorFn[FACTORYDEF_Add] != 0x0A) || (tecEnable == 0xC22B) ) {		
		A2_VendorFn[FACTORYDEF_Add] = 0x0A;
		controlTecRegister();
		return;
	}	

	//rawAdc = convertVoltageToAdcRaw(lddRegMap[WAVE_REG][SET]);			
	rawAdc = lddRegMap[WAVE_REG][SET];
	ConfigTec1(INDEX_TEC_SETPOINT, rawAdc);
	RegWordTo2ByteSave(lddRegMap[WAVE_REG][SET], &A2_VendorFn[DAC_TEC_SET_Add]); 
	lddRegMap[WAVE_REG][BUF] = lddRegMap[WAVE_REG][SET];
	TECCN1_bit.TEC_EN = 1;
	
	//data = voltageToLdTemperature(lddRegMap[WAVE_REG][SET]);
	voltageData = lddRegMap[WAVE_REG][SET] * 0.037;
	p = calculateThermistorResistance(voltageData);
	//p = calculateThermistorResistance(lddRegMap[WAVE_REG][SET]);
	data = calculateThermistorTemperature(p);
	targetTemperature = ((float)data) / 256;
	deltaTemperature = currentTemperature - targetTemperature;
		
     /*
	if(deltaTemperature > 24.0) {		
		// tec target 55 deg, module temp 80 deg, cooling, 500mA 
		startLimit = 0x2000;
	} else if(deltaTemperature < -50.0) {
		// tec target 55, module temp 5, heating, 500mA 
		startLimit = 0x2000;
	} else {
		// tec target 45, module temp -5~70, heating/cooling, 250mA 
		startLimit = 0x2000;
	}
     */
	startLimit = 0x2000;
	data = (sint16)Reg2ByteToWordLoad(&A2_TECLUTAB[TECPositiveClamp_Add]);
	calStep = A2_DEBUGTAB[CALCULATE_STEP_Add];
	inrushCount = A2_DEBUGTAB[REACH_STEP_Add];
	
    step = (data - startLimit) / 7;
//	step = (data - startLimit) / calStep;
	if(step < 1) {
		step = 1;
	}
		
	tolerance = ((float)A2_DEBUGTAB[TEC_DELTA_TEMP_Add]) / 10.0;
	A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] |= 0x20; // Wavelength unlocked assert
	A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] |= 0x20; // Latched Wavelength unlocked assert

	inrushStep = 0;
	for(i = 0; i < calStep; i++)
	{		
          
		if(i<8){
			data = startLimit + (step * i);
			ConfigTec1(INDEX_TEC_POS_OP_CP, data);		
			data = 0xFFFF - (startLimit + (step * i));
			ConfigTec1(INDEX_TEC_NEG_OP_CP, data);	
		}
              
		timerDelay(500);
                        
		readAdcChannels();
		//readInternalTemperature();
		convertAdcToDdm();	
                        
		data = (sint16)Reg2ByteToWordLoad(&A2_DiagData[DIAGNOSTICS_TEC_TEMP]);
		currentTemperature = ((float)data) / 256.0;
		
		voltageData = lddRegMap[WAVE_REG][SET] * 0.037;
		p = calculateThermistorResistance(voltageData);
		//p = calculateThermistorResistance(lddRegMap[WAVE_REG][SET]);
		data = calculateThermistorTemperature(p);
		targetTemperature = ((float)data) / 256.0;
		deltaTemperature = fabs(currentTemperature - targetTemperature);

		if(deltaTemperature < tolerance) {
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] |= 0x08; 
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] &= ~0x60;
				controlTecRegister();
				inrushStep++;
				if(inrushStep >= inrushCount){
					break;
				}
		} else {
			inrushStep = 0;
		}
                        
	}	
     
	DEV_NUM |= 0x80;    // boot mode i2c address disable 
}

/*
 * Name         : controlTecRegister
 * Description  : Scheduled every 50ms in process50msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 01 2019
 * History      : 
 */
//static void controlTecRegister(void)
void controlTecRegister(void)
{
	uint16 rawAdc, data;
	data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECCN_Add]);
	TECCN1 = data;
	RegWordTo2ByteSave(data, &A2_TECLUTAB[TECCN_Add]);
       
	
	if(lddRegMap[WAVE_REG][SET] != lddRegMap[WAVE_REG][BUF]) {		/* TEC Setting */
             
		if(lddRegMap[WAVE_REG][SET] < 360){
			lddRegMap[WAVE_REG][SET] = 360;
		}
		rawAdc = lddRegMap[WAVE_REG][SET];
		//rawAdc = convertVoltageToAdcRaw(lddRegMap[WAVE_REG][SET]);	
		ConfigTec1(INDEX_TEC_SETPOINT, rawAdc);
		if(A2_VendorFn[LDMODE_Add] & LUTTEC_ENABLE) {
   			RegWordTo2ByteSave(lddRegMap[WAVE_REG][SET],&A2_VendorFn[DAC_TEC_SET_Add]);
			RegWordTo2ByteSave(lddRegMap[WAVE_REG][SET],&A2_TECLUTAB[TEC_LUT_Add]);
		}
		lddRegMap[WAVE_REG][BUF] = lddRegMap[WAVE_REG][SET];	                
	}      
#if 1        
	if(tecTableFlag != OFF) {
		tecTableFlag = OFF;
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECTUDY_Add]);
		ConfigTec1(INDEX_TEC_PWMDUTY, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECKP_Add]);	
		ConfigTec1(INDEX_TEC_Kp, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECKI_Add]);
		ConfigTec1(INDEX_TEC_Ki, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECKD_Add]);
		ConfigTec1(INDEX_TEC_Kd, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECSHIFT_Add]);
		ConfigTec1(INDEX_TEC_SHIFT, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECPosERRClamp_Add]);
		ConfigTec1(INDEX_TEC_POS_ERR_CP, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECNegERRClamp_Add]);
		ConfigTec1(INDEX_TEC_NEG_ERR_CP, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECPositiveClamp_Add]);
		ConfigTec1(INDEX_TEC_POS_OP_CP, data);
		data = Reg2ByteToWordLoad(&A2_TECLUTAB[TECNegativeClamp_Add]);
		ConfigTec1(INDEX_TEC_NEG_OP_CP, data);
		ConfigTec1(INDEX_TEC_DUTY_OFFSET, 0x00);	
	}
#endif   
}

/*
 * Name         : calculateThermistorResistance
 * Description  : Converts the thermistor voltage to the thermistor resistance
 * Return Value : float(ohm)
 * Parameter    : uint16(mV)
 * Author       : 
 * Date         : May 15 2019
 * History      : 
 */
float calculateThermistorResistance(uint16 voltage)
{
	float resistance = 0;

	resistance = (float)(THERMISTOR_DIVIDER_RESISTANCE * (float)voltage) / (float)(REFERENCE_VOLTAGE_VREF - voltage);

	return resistance;
}

/*
 * Name         : calculateThermistorTemperature
 * Description  : Converts the thermistor resistance to the thermistor temperature
 * Return Value : uint16(Celsius)
 * Parameter    : float(ohm)
 * Author       : 
 * Date         : May 15 2019
 * History      : 
 */
uint16 calculateThermistorTemperature(float resistance)
{
	float data = 0;
	sint16 result = 0;

	data = (float)(THERMISTOR_CONSTANT_1 + THERMISTOR_CONSTANT_2 * (float)log(resistance) + THERMISTOR_CONSTANT_3 * (float)pow(log(resistance), 3));
	data = (float)((float)(1 / data) - KELVIN_FORMULA);

	result = data * 255;

	return result;
}
