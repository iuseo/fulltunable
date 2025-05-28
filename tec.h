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

#ifndef _TEC_H_
#define _TEC_H_

#define  TEC_CH1          0
#define  TEC_CH2          1

// #define  TEC_CONTROL_REG          0xE2AB
// #define  TEC_CONTROL_REG          0xC2AB	// adc conversion normal
#define  TEC_CONTROL_REG          0xC22B	// TEC Disable
// #define  TEC_CONTROL_REG          0xC26B	// TEC Disable, firmware mode
// #define  TEC_CONTROL_REG          0xC26B	// TEC Disable
// #define  TEC_CONTROL_REG          0xE2EB	// tec firmware mode
#define  INDEX_TEC_PWMDUTY       0
#define  INDEX_TEC_SETPOINT      1
#define  INDEX_TEC_Kp            2
#define  INDEX_TEC_Ki            3
#define  INDEX_TEC_Kd            4
#define  INDEX_TEC_SHIFT         5
#define  INDEX_TEC_POS_ERR_CP    6
#define  INDEX_TEC_NEG_ERR_CP    7
#define  INDEX_TEC_POS_OP_CP     8
#define  INDEX_TEC_NEG_OP_CP     9
#define  INDEX_TEC_CURR_CONFIG   10
#define  INDEX_TEC_CURR_MEAS     11
#define  INDEX_TEC_OFFSET_TRIG   12
#define  INDEX_TEC_DUTY_OFFSET   13



void ConfigTec1(unsigned int Index, unsigned int Value);
void ConfigTec2(unsigned int Index, unsigned int Value);
signed int GetTec1(unsigned int Index );
signed int GetTec2(unsigned int Index );


#endif //_TEC_H_

