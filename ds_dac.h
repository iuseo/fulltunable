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

#ifndef _DSDAC_H_
#define _DSDAC_H_

#define IDAC00					0
#define IDAC01					1
#define IDAC02					2
#define IDAC03					3

#define DS_CH_5                   5
#define DS_CH_6                   6


#define DS_CONFIG             0x00
#define DS_DUTY               0x01
#define DS_FULL_SCALE_CNFG    0x02

void DS_DAC_Init(unsigned int Channel, unsigned int Config, unsigned int FullScale);
void DS_DAC_Duty(unsigned int Channel, unsigned int Duty);
void DS_DAC_Read(unsigned int Channel);
void DS_DAC_Config(unsigned int Channel, unsigned int RegSel, unsigned int Data);
void DS_DAC_OnOff(unsigned char channel, unsigned char onOff);

#endif //_DSDAC_H_

