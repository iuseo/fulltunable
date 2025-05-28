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
// includes
//-----------------------------------------------------------------------------
#include <intrinsics.h>
#include <iods4835.h>

#include "dc_dc.h"


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void PI_Config(unsigned int Index, unsigned int Value)
{
   if( Index <= PI_INDEX_SHIFT )
   {
       PICNT &= ~0x07;                                                          //Clear previous Index
       PICNT |= Index;
       PIDATA = Value;
   }
}

////////////////////////////////////////////////////////////////////////////////
// Convert Input voltage which is converted wrt 6.55V full scale
// VRDCDC = (Vout * 0.4 - 0.150)/0.00126, Here Vout is Buck Target voltage
// 
// Input : Target => 16-bit value ( Input Voltage / 2^16 * 6.55V)
//
////////////////////////////////////////////////////////////////////////////////
unsigned int CalcBuckTarget(unsigned int Target)
{
    unsigned long int CalculateTarget;
 
    //Fixed point maths
    //Convert 6.55V full scale, 16-bit input
    //Volt = Input/2^16*6.5535               .........(1)
    
    //VRDCDC = (Vout * 0.4 - 0.150)/0.00126  .........(2) from DS4835 UG
     
    //From (1) and (2) above
    //VRDCDC = ( (Input*0.4*6.5535)/(2^16*0.00126) - 0.150/0.00126
    
    //Simplify Equation is 
    //VRDCDC = Input*2080.476/2^16 - 119.047
  
    //DS4835 MAC can be used to perform below equation for time critical application
    CalculateTarget = Target;
    CalculateTarget = CalculateTarget * 2080;
    CalculateTarget = CalculateTarget >> 16;

    Target = (unsigned int)CalculateTarget;
    
    if (Target > 119)
    {
        Target = Target - 119;
    }
    else
    {
       Target = 0;
    }
    
    if( Target > MAX_VRDCDC )
    {
       Target = MAX_VRDCDC;
    }
    
    return Target;
}

////////////////////////////////////////////////////////////////////////////////
// Convert Input voltage which is converted wrt 6.55V full scale
// VRDCDC = (FB * 0.25 - 0.150)/0.00126, Here Vout is Boost Target voltage
// 
// Input : Target => 16-bit value ( Input Voltage / 2^16 * 6.55V)
//
////////////////////////////////////////////////////////////////////////////////

unsigned int CalcBoostTarget(unsigned int Target)
{
    unsigned long int CalculateTarget;
 
    //Fixed point maths
    //Convert 6.55V full scale, 16-bit input
    //Volt = Input/2^16*6.5535               .........(1)
    
    //VRDCDC = (FB - 0.150)/0.00126          .........(2) from DS4835 UG

    //VRDCDC = (FB - 0.083)/0.00126          .........(2) from (Design)
    
    //FB = VBOOST / 4  = VBOOST * 0.25       .........(3)
    
    //From (1) (2) and (3) above
    //VRDCDC = ( (Input*0.25*6.5535)/(2^16*0.00126) - 0.150/0.00126

    //VRDCDC = ( (VBOOST*0.25*6.5535)/(2^16*0.00126) - 0.150/0.00126
    //VRDCDC = Input*1300.298/2^16 - 119.047
      
    //DS4835 MAC can be used to perform below equation for time critical application

    CalculateTarget = Target;
    CalculateTarget = CalculateTarget * 1300;
    CalculateTarget = CalculateTarget >> 16;
    
    Target = (unsigned int)CalculateTarget;
    
    if (Target > 119)
    {
        Target = Target - 119;
    }
    else
    {
       Target = 0;
    }
    
    if( Target > MAX_VRDCDC )
    {
       Target = MAX_VRDCDC;
    }

    return Target;
}

////////////////////////////////////////////////////////////////////////////////
// Convert Input voltage having  LSB 10mV
// Vtarget[11:0] = (Output * FBR )/VREF * 2^16, Here Output is External Boost Target voltage
// EVKIT hardware has FBR = 10K/(10K+510K) =  0.019231
//
// Input : Target => 16-bit value, 10mV LSB
// 
////////////////////////////////////////////////////////////////////////////////
unsigned int CalcExternalBoostTarget(unsigned int Target)
{
    unsigned int CalculateTarget,Value;
    
    //Input voltage LSB is 10mV. Ex 65.00 V will have input 6500
    
    //EVKIT hardware has 50V components 
    //Limit Max Target output
    
    if ( Target > EXT_BOOST_MAX )
    {
         Target = EXT_BOOST_MAX;
    }
    //Vtarget[11:0] = ( (Output/100) * 0.019231 * 2^12)/VREF (2.4) )
    //Vtarget[11:0] =  Output * 0.000160256 * 2^12
    //0.000160256 can be converted in fixed point math using 5/2^15 = 0.000153
    //To compensate error 51/1024 multiple is added
    
    //Vtarget[11:0] =  Output * 5/2^15 * 2^12 + Error 
    //Vtarget[11:0] =  Output * 5/8 +  ( ( ( Output * 5 / 8) * 51 ) >> 10 )  
    
    //Fixed point calculation formula    
    //( (Input * 5 )>> 4 ) + ( ( ( (Input * 5) >> 4 ) * 51) >> 10)   
    
    CalculateTarget = ( Target * 5 ) >> 4;
    Value = CalculateTarget;
      
    Value = ( Value * 51 ) >> 10;
    CalculateTarget = CalculateTarget + Value;

    if ( CalculateTarget > MAX_BOOST_VALUE )
    {
         CalculateTarget =  MAX_BOOST_VALUE;
    }

    CalculateTarget = CalculateTarget << 4;
    
    return CalculateTarget;    
}

////////////////////////////////////////////////////////////////////////////////
// Slowing Ramp the DC_DC Buck output to target.
////////////////////////////////////////////////////////////////////////////////
void RampDCDCBuck(unsigned int Target)
{
    unsigned int delay, Step;

    //Slowing change DC-DC buck to target
    for(Step = VRDCDC; Step < Target; Step = Step + BUCK_RAMP_STEP)
    {
        VRDCDC = Step;                                                          //Start with initial value
        
        for(delay = 0; delay < 1000; delay++)                                    //Delay
        {
             //Reset Watchdog Timer
//             WDCN_bit.RWT = 1; 
        }
    }
    //Set the final output
    VRDCDC = Target;  
}


////////////////////////////////////////////////////////////////////////////////
// Convert Input voltage which is converted wrt 6.55V full scale
// VRDCDC = (Vout * 0.4 - 0.150)/0.00126, Here Vout is Buck Target voltage
// 
// Input : Target => 16-bit value ( Input Voltage / 2^16 * 6.55V)
//
////////////////////////////////////////////////////////////////////////////////
unsigned int CalcPIBuckTarget(unsigned int Target)
{
    unsigned long int CalculateTarget;
 
    //Fixed point maths
    //Convert 6.55V full scale, 16-bit input
    //Volt = Input/2^16*6.5535               .........(1)
    
    //Reference is 2.4V
    
    // So 6.55/(3*2.4) = 11179/4096
    //Buck has 1/3 internal divider
  
  
    //DS4835 MAC can be used to perform below equation for time critical application
    CalculateTarget = Target;
    CalculateTarget = CalculateTarget * 14905;
    CalculateTarget = CalculateTarget >> 14;

    Target = (unsigned int)CalculateTarget;
    
    return Target;
}

////////////////////////////////////////////////////////////////////////////////
// Convert Input voltage which is converted wrt 6.55V full scale
// VRDCDC = (Vout * 0.4 - 0.150)/0.00126, Here Vout is Buck Target voltage
// 
// Input : Target => 16-bit value ( Input Voltage / 2^16 * 6.55V)
//
////////////////////////////////////////////////////////////////////////////////
unsigned int CalcPIBoostTarget(unsigned int Target)
{
    unsigned long int CalculateTarget;
 
    //Fixed point maths
    //Convert 6.55V full scale, 16-bit input
    //Volt = Input/2^16*6.5535               .........(1)
    
    //Reference is 2.4V
    
    // So 6.55/2.4 = 11179/(4*4096)
    //Boost has 1/4 divider
  
    //DS4835 MAC can be used to perform below equation for time critical application
   // Target = Target >>2;                                                        //1/4 resisitor divider (3K+1K)in the EVKIT hardware
    CalculateTarget = Target;
    CalculateTarget = CalculateTarget * 2796;
    CalculateTarget = CalculateTarget >> 12;                                    

    Target = (unsigned int)CalculateTarget;
        
    return Target;
}


////////////////////////////////////////////////////////////////////////////////
//End
