 

#ifndef _ADC_H_
#define _ADC_H_

#define ADCNSET 0xA900
#define ADCNEN  0x8000

////////////////////////////////////////////////////////////////////////////////
//ADC channel PRIORITY in the sequence
////////////////////////////////////////////////////////////////////////////////
#define PRIORITY_1                 1
#define PRIORITY_2                 2
#define PRIORITY_3                 3

////////////////////////////////////////////////////////////////////////////////
//ADC Acquistion Extension
////////////////////////////////////////////////////////////////////////////////
#define NO_ACQ_EXT                 0<<2
#define ACQ_EXT_0_5us              1<<2
#define ACQ_EXT_1_0us              2<<2
#define ACQ_EXT_2_0us              3<<2

////////////////////////////////////////////////////////////////////////////////
//ADC Averaging
////////////////////////////////////////////////////////////////////////////////
#define ADC_AVG_001SMP             0<<4
#define ADC_AVG_002SMP             1<<4
#define ADC_AVG_004SMP             2<<4
#define ADC_AVG_008SMP             3<<4
#define ADC_AVG_016SMP             4<<4
#define ADC_AVG_032SMP             5<<4
#define ADC_AVG_064SMP             6<<4
#define ADC_AVG_128SMP             7<<4

////////////////////////////////////////////////////////////////////////////////
//Bipoler
////////////////////////////////////////////////////////////////////////////////
#define ADC_BI_POLAR                1<<7
#define ADC_UNI_POLAR               0<<7

#define DATA_BUFFG                  0
#define CONFIG_REG                  1
#define CHANNEL_CONFIG              2
#define LOW_THRESHOLD               3
#define HIGH_THRESHOLD              4

void ADC_Config(unsigned int Index, unsigned int Priority, unsigned int Acquistion, unsigned int Avg, unsigned int BIP_Sel);
void ADC_Channel_Config(unsigned int Index, unsigned int Positive, unsigned int Negative);
void ADC_StatusInterupt(unsigned int Pirority);
void ADC_LQInterupt(unsigned int Index);
void ADC_HQInterupt(unsigned int Index);

unsigned int ADC_ReadOut(unsigned int Config_No);

unsigned int ADC_ConverttomV(unsigned int Config_No);
signed int ADC_ConverttomV_BI(unsigned int Config_No);
unsigned int QT_Config(unsigned int Index, unsigned int LowThreshold, unsigned int HighThreshold);
unsigned int ADC_ReadOutCheck(unsigned int Value);

#endif //_ADC_H_


