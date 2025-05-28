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

#ifndef _BOARD_H_
#define   _BOARD_H_

////////////////////////////////////////////////////////////////////////////////
//ADC Channels Configuration here
////////////////////////////////////////////////////////////////////////////////
//#define CONFG_DC_DCFB1      0                                                   //Config 0
//#define CONFG_DC_DCFB2      1                                                   //Config 1
//#define CONFG_DC_DCFB3      2                                                   //Config 2
// #define CONFG_MON1          3                                                   //Config 3
// #define CONFG_MON2          4                                                   //Config 4
#define CONFG_MON3          5                                                   //Config 5
#define CONFG_MON4          6                                                   //Config 6
#define CONFG_MON5          7                                                   //Config 7
#define CONFG_MON6          8                                                   //Config 8
#define CONFG_TEC1          9                                                   //Config 9
#define CONFG_TEC2         10                                                   //Config 10
#define CONFG_EXT_BOOST1   12                                                   //Config 12 (APD-1, Fixed by ADC Controller)
#define CONFG_APD3         14                                                   //Config 14 (APD-3, Fixed by ADC Controller)
#define CONFG_APD4         15                                                   //Config 15 (APD-4, Fixed by ADC Controller, Used as BUCK4 in EVKIT)

#define CONFG_VTHERM1      16                                                   //Config 16 (TEC-1, Fixed by ADC Controller)

#define CONFG_VTHERM2      17                                                   //Config 17 (TEC-2, Fixed by ADC Controller)
#define CONFG_DC_DCFB1     18                                                   //Config 18 (DC-DC1, Fixed by ADC Controller)
#define CONFG_DC_DCFB2     19                                                   //Config 19 (DC-DC2, Fixed by ADC Controller)
#define CONFG_DC_DCFB3     20                                                   //Config 20 (DC-DC3, Fixed by ADC Controller)

#define CONFG_VCC          22                                                   //Config 22 for VCC (DS4835 has  Config 23 issue so VCC is also configured at Config 22)
#define CONFG_VCC_DAC      23                                                   //Config 23 VCC for DS-DAC and TEC, Fixed by ADC Controller (Does not work properly)
#define VCC_DIV            29
#define VCC_REF            31
#define GND_REF            30

#define TXP_GP00            0                                                   //Config 2 is for (ADC Channel 0)
#define RXP_GP01            1                                                   //Config 3 is for (ADC Channel 1)
#define DC_DC_FB1           4                                                   //Config 0 is for DC_DC_FB1 (ADC Channel 4)
#define GP03                3                                                   //Config 5 is for (ADC Channel 3)
#define GP05                5                                                   //Config 6 is for (ADC Channel 5)
#define GP06                6                                                   //Config 7 is for (ADC Channel 6)
#define DC_DC_FB3           7                                                   //Config 2 is for DC_DC_FB3 (ADC Channel 7)
#define LX2                 8                                                   //Config 2 is for LX2 (ADC Channel 8)
#define LX4                 9                                                   //Config 2 is for LX4 (ADC Channel 9)
#define DC_DC_FB2           10                                                  //Config 1 is for DC_DC_FB2 (ADC Channel 10)
#define GP21_ADC17          17                                                  //DC_DC_INV_FB (ADC Channel 17)

#define VTERHM_GP23         19                                                  //Config 1 is for DC_DC_FB2 (ADC Channel 10)
#define DC_DC_FB2           10                                                  //Config 1 is for DC_DC_FB2 (ADC Channel 10)
#define APDFB               18                                                  //Config 12 is for (ADC Channel 18, APDFB)
#define GP24                20                                                  //Config 4 is for (ADC Channel 20)
#define GP25_ADC21          21                                                  //Config 4 is for (ADC Channel 21)
#define TEC1_REF            28                                                  //Internal Registor divider for TEC1
#define TEC2_REF            29                                                  //Internal Registor divider for TEC2

////////////////////////////////////////////////////////////////////////////////
#define CONFG_ADC07_VTEC	1		// ADC config        

#define CONFG_ADC02_LX2FB	19		// ADC config                                              //Config 19, RMOD for EA driver
#define ADC02_LX2FB	        10		// ADC Channel

#define CONFG_ADC20_RSSI	6		// ADC config, 20->LX3 FB                                                //Config 04, RSSI
#define ADC20_RSSI	        12		// ADC Channel                                                 //Config 20

/* quick trip RSSI */
#define QTCONFG_ADC02_RSSI	7		//ADC config
 


// #define CONFG_ADC29_VCC 	22		// ADC config                                                   //Config 22 for VCC (DS4835 has  Config 23 issue so VCC is also configured at Config 22)
#define CONFG_ADC29_VCC 	23		// ADC config                                                   //Config 22 for VCC (DS4835 has  Config 23 issue so VCC is also configured at Config 22)
#define VCC_ADC29_DIV  		29		// ADC channel

#define CONFG_ADC00_LDTH	3		// ADC Config                                                   //Config 16, TEC Temp feedback
#define ADC00_LDTH	        0               // ADC Channel                                             //Config 10

#define CONFG_ADC02_APDFB	12		// ADC config                                                   //Config 12, External APD PWM feedback
#define ADC02_APDFB	        2               // ADC Channel                                              //Config 02

// #define CONFG_ADC08_LX2		9		// ADC config                                                   //Config 12, External APD PWM feedback
#define CONFG_ADC08_LX4		17		// ADC config                                                   //Config 12, External APD PWM feedback
#define ADC08_LX04      	9               // ADC Channel  

#define CONFG_ADC23_ITEC	7		// ADC config                                                   //Config 12, External APD PWM feedback
#define GP23_ADC19_ITEC         19		// ADC channel                                                   //Config 05
#define GP61_ADC08_VTEC         8		// ADC channel                                                   //Config 05
#define GP27_ADC23_ITEC         23		// ADC channel                                                   //Config 05
#define ADC07_VTEC              7               // ADC channel 

////////////////////////////////////////////////////////////////////////////////

// #define DCDC3_16vcc_FB       0x4000
#define DCDC3_18vcc_FB		0x4600
#define DCDC3_10vcc_FB		0x2800

////////////////////////////////////////////////////////////////////////////////

void GPIO27_ExtenalInterrupt_Init(void);
void GPIO15_ExtenalInterrupt_Init(void);
void interruptMask(void);
void Eye_Safety(void);
void Fault_clear(void);
void initializeSystem(void);

extern unsigned short TXPH_ADBuff, TXPH_Compatre_ADBuff;   
#endif   // _BOARD_H_

//End
