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

#ifndef _TIMER_H_
#define _TIMER_H_

#define ms_1        1
#define ms_5        2
#define ms_10       3
#define ms_20       4
#define ms_50       5
#define ms_100      6

#define UART_DURATION		(840)		/* 830us for 1200bps */

#define MAN_DURATION		((32000) / A2_DEBUGTAB[DURATION_CONTROL_BIT])   /* 16ms for 62.5baud */
#define START_MAN_DURATION  ((1000) / A2_DEBUGTAB[DURATION_CONTROL_BIT])   /*start Duration - delete delay*/


void Timer1_Init(void); 
void Timer2_Init(void);
void Timer3_Init(void);

void startTimer1(void);
void stopTimer1(void);
void startTimer2(void);
void stopTimer2(void);
void startTimer3(void);
void stopTimer3(void);
void setTimer1Tbc(unsigned short tbc1);
void setTimer2Tbc(unsigned short tbc2);

#endif //_TIMER_H_
