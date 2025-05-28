/*******************************************************************************
* Copyright (C) 2017 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/

//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include <intrinsics.h>
#include <iods4835.h>
#include "adc.h"

////////////////////////////////////////////////////////////////////////////////
//ADC Configuration
//Priority, ADC Acquistion, Average and BIP_Polar Select
////////////////////////////////////////////////////////////////////////////////
void ADC_Config(unsigned int Index, unsigned int Priority, unsigned int Acquistion, unsigned int Avg, unsigned int BIP_Sel)
{
    unsigned int Config;
    
    Config = Priority;                                                          //Priorty
    Config |= Acquistion;                                                       //Acquistion Extension
    Config |= Avg;                                                              //Averaging
    Config |= BIP_Sel;                                                              //Averaging    

    ADCN_bit.REGSEL = CONFIG_REG;                                               //Point to Config Register
    ADCN_bit.INDEX = Index;                                                     //Index   
    
    ADDATA = Config;                                                            //Write ADC Config Register
}

////////////////////////////////////////////////////////////////////////////////
//ADC Channels configuration
////////////////////////////////////////////////////////////////////////////////
void ADC_Channel_Config(unsigned int Index, unsigned int Positive, unsigned int Negative)
{
    unsigned int Config = Negative;                                            //Negative Mux[4:0]

    Positive = Positive << 8;                                                   //Positive Mux [12:8]
    Config |= Positive;                                                         //Channel Config
 
    ADCN_bit.REGSEL = CHANNEL_CONFIG;                                           //Point to Channel Config
    ADCN_bit.INDEX = Index;                                                     //Index
    ADDATA = Config;                                                            //Write Channel Config Register
}

void ADC_StatusInterupt(unsigned int Pirority)
{
	switch(Pirority)
	{
		case PRIORITY_1 :
			ADST_bit.PR1IE = 1;
		break;
		case PRIORITY_2 :
			ADST_bit.PR2IE = 1;
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//ADC Readout (Interrupt Protected)
////////////////////////////////////////////////////////////////////////////////
unsigned int ADC_ReadOut(unsigned int Config_No)
{
    unsigned int ReadOut;
    
    __disable_interrupt();                                                      //Disable Global Interrupt     
   
    //Note: To allow INDEX and REG_REL from interrupt, below code is interrupt protected  
    ADCN_bit.INDEX = Config_No;                                                 //Point to corresponding DATA Buffer
    ADCN_bit.REGSEL = DATA_BUFFG;                                               //Point to DATA Buffer
  
    ReadOut = ADDATA;                                                           //Read the data from ADDATA
    
    __enable_interrupt();                                                       //Enable Global Interrupt     
    
    return ReadOut;  
}

////////////////////////////////////////////////////////////////////////////////
//ADC Readout (Interrupt Protected)
////////////////////////////////////////////////////////////////////////////////
unsigned int ADC_ConverttomV(unsigned int Config_No)
{
    unsigned int ReadOut;	
	float p;
	ReadOut = ADC_ReadOut(Config_No);
	// ReadOut = Config_No;
	
        // p = ( (double)ReadOut / 65535.0 )* 2400.0; 
        // p = ( (double)13000 / 65535.0 )* 2400.0; 
   
	p = ReadOut * 0.037;
      
    return (unsigned int) p;  
}
signed int ADC_ConverttomV_BI(unsigned int Config_No)
{
    signed int ReadOut;	
	float p;
	ReadOut = (signed int)ADC_ReadOut(Config_No);
	// ReadOut = Config_No;
	
    // p = ( (double)ReadOut / 65535.0 )* 2400.0; 
    // p = ( (double)13000 / 65535.0 )* 2400.0; 
//	p = ReadOut * 0.037;
	/* Full Scale for Bipolar Mode : +/- VREF/2 for RANGE with 0, +/- VREF for RANGE with 1 */
	/* So if the RANGE bit is set as 1, the result should be doubled again */
	p = (ADCN_bit.RANGE == 0) ? (ReadOut * 0.037) : (ReadOut * 0.037 * 2);

    return (unsigned int) p;  
}

////////////////////////////////////////////////////////////////////////////////
//ADC Readout check for ADC channels which are ground reference
////////////////////////////////////////////////////////////////////////////////
unsigned int ADC_ReadOutCheck(unsigned int Value)
{
    if ( Value > 0x7FFF)
    {
       Value = 0;
    }
    return Value;  
}

////////////////////////////////////////////////////////////////////////////////
//QuickTrip Configuration
//================
//Important Note:
//================
// The DS4835 ADC Controller supports 8 quick trip and these quicktrip should be
// defined between configuration 0 to 8.
////////////////////////////////////////////////////////////////////////////////


void ADC_LQInterupt(unsigned int Index)
{
	unsigned int config=LTI;
	config |= (1<<(Index+8));
	LTI = config;
}
void ADC_HQInterupt(unsigned int Index)
{
	unsigned int config=HTI;
	config |= (1<<(Index+8));
	HTI = config;
}


unsigned int QT_Config(unsigned int Index, unsigned int LowThreshold, unsigned int HighThreshold)
{
   unsigned int Error = 0;
   if ( Index > 8)
   {
       Error = 1; 
   }
   else
   {
      //LOW Threshold
      ADCN_bit.REGSEL = LOW_THRESHOLD;                                          //Point to Low Threshold
      ADCN_bit.INDEX = Index;                                                   //Index
      ADDATA = LowThreshold;                                                    //Write Low Threshold
      
      //High Threshold
      ADCN_bit.REGSEL = HIGH_THRESHOLD;                                         //Point to High Threshold     
      ADCN_bit.INDEX = Index;                                                   //Index
      ADDATA = HighThreshold;                                                   //Write Hiigh Threshold
   } 
   return Error;
}

////////////////////////////////////////////////////////////////////////////////
//End
////////////////////////////////////////////////////////////////////////////////
