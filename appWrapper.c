/*
 * FILE NAME            : appWrapper.c
 * DATE                 : March 28 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 28 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */
#include <iods4835.h>
#include <intrinsics.h>
#include "main.h"
#include "timer.h"
#include "adc.h"
#include "dc_dc.h"
#include "tec.h"
#include "flash.h"
#include "i2cm.h"
#include "i2cslave.h"
#include "MemoryAccess.h"
#include "CDR_SyncFunction.h"
#include "ds_dac.h"
#include "appGpio.h"

extern uint8 interruptForceDisable;
/* From timer.c */
void initializeTimer1(void)
{
	Timer1_Init();
        Timer2_Init();
        Timer3_Init();
}

/* From adc.c */
void adcConfig(uint16 index, uint16 priority, uint16 acquistion, uint16 average, uint16 bipSelect)
{
	ADC_Config(index, priority, acquistion, average, bipSelect);
}
void adcChannelConfig(uint16 index, uint16 positive, uint16 negative)
{
	ADC_Channel_Config(index, positive, negative);
}
uint16 adcConvert2mV(uint16 configNo)
{
	return ADC_ConverttomV(configNo);
}
sint16 adcConvert2mVBiPolar(uint16 configNo)
{
	return ADC_ConverttomV_BI(configNo);
}
uint16 adcReadOut(uint16 configNo)
{
	return ADC_ReadOut(configNo);
}

/* From dc_dc.c */
void piConfig(uint16 index, uint16 value)
{
	PI_Config(index, value);
}
uint16 calculateBuckTarget(uint16 target)
{
	return CalcBuckTarget(target);
}
uint16 calculatePiBuckTarget(uint16 target)
{
	return CalcPIBuckTarget(target);
}
void rampDcDcBuck(uint16 target)
{
	RampDCDCBuck(target);
}

/* From tec.c */
void configTec1(uint16 index, uint16 value)
{
	ConfigTec1(index, value);
}
void configTec2(uint16 index, uint16 value)
{
	ConfigTec2(index, value);
}
sint16 getTec2(uint16 index)
{
	return GetTec2(index);
}

/* From flash.c */
uint16 convert2BytesToWord(uint8* address)
{
	return Reg2ByteToWordLoad(address);
}
void convertWordTo2Bytes(uint16 data, uint8* address)
{
	RegWordTo2ByteSave(data, address);
}
void convertWordTo4Bytes(float data, uint8* address)
{
	RegWordTo4ByteSave(data, address);
}
void swapUint32(uint8* value)
{
	swap_uint32(value);
}
void flashRead(uint16 destination, uint16 source, uint16 numberBytes)
{
	FLASH_Read(destination, source, numberBytes);
}
void flashUpdate(uint16 source, uint16 destination, uint16 numberBytes)
{ 
	unsigned char interrupt;

	interrupt = IC;                                                      //save current settings
	__disable_interrupt();             
  
    FLASH_Update(source, destination, numberBytes);
   
    IC = interrupt;                                                      //re-store interrupts
	
	__enable_interrupt();   	

}

/* From i2cm.c */
void initializeI2CMaster(uint8 master, uint16 clockRate, uint8 pin, uint8 useInterrupt)
{
	I2CM_Init(master, clockRate, pin, useInterrupt);
}
uint8 i2cMWriteBytes(uint8 master, uint8 slaveAddress, uint8 registerAddress, const uint8 *data, uint8 numberBytes)
{
	return I2CM_WriteNBytes(master, slaveAddress, registerAddress, data, numberBytes);
}
uint8 i2cMReadBytes(uint8 master, uint8 slaveAaddress, uint8 registerAddress, uint8 *data, uint8 numberBytes)
{
	return I2CM_ReadNBytes(master, slaveAaddress, registerAddress, data, numberBytes);
}

/* From i2cslave.c */
void initializeI2CSlave(void)
{
	I2CS_Init();
}

/* From DS_DAC.c */
void initializeDSDac(uint16 channel, uint16 config, uint16 fullScale)
{
    DS_DAC_Init(channel,config,fullScale);
}

void controlDsDacDuty(uint16 channel, uint16 duty)
{
    DS_DAC_Duty(channel, duty);
}
void controlDsConfig(uint16 channel, uint16 regSel, uint16 data)
{
    DS_DAC_Config(channel,regSel,data);
}

void dsDacOff(uint8 channel, uint8 onoff)
{
    DS_DAC_OnOff(channel,onoff);
}
