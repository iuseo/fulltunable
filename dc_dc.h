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

#ifndef _DC_DC_H_
#define _DC_DC_H_

////////////////////////////////////////////////////////////////////////////////
//PMIC Configuration here
////////////////////////////////////////////////////////////////////////////////
#define DC_DC_CH1            0                                                  //DC-DC Chaanel 1
#define DC_DC_CH2            1                                                  //DC-DC Chaanel 2
#define DC_DC_CH3            2                                                  //DC-DC Chaanel 3
#define DC_DC_CH4            3                                                  //DC-DC Chaanel 4


#define PI_INDEX_SETPOINT    0
#define PI_INDEX_KP          1
#define PI_INDEX_KI          2
#define PI_INDEX_OP_CLAMP    3
#define PI_INDEX_SHIFT       4

#define DC_DC_BUCK_FACTOR    9005
#define BUCK_RIPPLE          40
#define BUCK_STEP            1
#define BUCK_RAMP_STEP       10
#define START_BUCK           100
void RampDCDCBuck(unsigned int Target);

void BuckCloseLoop(unsigned int Channel, unsigned int Target, unsigned int ADCReadOut);


////////////////////////////////////////////////////////////////////////////////
//APD Configuration here
////////////////////////////////////////////////////////////////////////////////
#define EXT_BOOST_CH1        0                                                  //DC-DC Chaanel 1
#define EXT_BOOST_CH2        1                                                  //DC-DC Chaanel 2
#define EXT_BOOST_CH3        2                                                  //DC-DC Chaanel 3
#define EXT_BOOST_CH4        3                                                  //DC-DC Chaanel 4

// #define EXT_BOOST_MAX        5500                                               //EVKIT FW limit 55.0V
#define EXT_BOOST_MAX        2200                                               //SFP28 ER/ZR FW limit 21.3V

#define IDX_ERRP_START_CLAMP                    0
#define IDX_DUTY_CLAMP                          1
#define IDX_TARGET                              2
#define IDX_INTEGAL_GAIN                        3
#define IDX_START_DUTY_CLAMP                    4
#define IDX_CLAMP_INTEGAL                       5
#define IDX_ERRN_CLAMP                          6
#define IDX_ERRP_CLAMP                          7
#define IDX_PROP_GAIN                           8
#define IDX_RESERVED	                        10
#define IDX_FEEDBACK_ERR	                11
#define IDX_CLAMP_STATUS  	                12
#define IDX_DUTY_CYCLE                          13




#define MAX_VRDCDC                          0x03FF    //10-bit (VRDCDC DAC is 10-bit)
#define  MAX_BOOST_VALUE                    0x0FFF    //12-bit (Vtarget is 12-bit)

unsigned int CalcBuckTarget(unsigned int Target);
unsigned int CalcBoostTarget(unsigned int Target);
unsigned int CalcExternalBoostTarget(unsigned int Target);
void ShutdownDC_DC(void);
void PI_Config(unsigned int Index, unsigned int Value);
unsigned int CalcPIBuckTarget(unsigned int Target);
unsigned int CalcPIBoostTarget(unsigned int Target);

#endif //_DC_DC_H_

