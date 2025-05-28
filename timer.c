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

#include <iods4835.h>

#include "main.h"
#include "timer.h"


void Timer1_Init(void)
{
#if 0
     //Timer 1 Configuration
     //General Timer Compare Value for 50msec
     // TBC1 = 50000;    
     TBC1 = 25000;    
     //Prescaler Selection -->Timer Clock (fastest Clock)
     TBCN1_bit.TBPS  = 2;
     //General Timer1 Interrupt Enable
     TBCN1_bit.TBIE = 0;
  
     //General Timer1 Mode --> Compare mode
     TBCN1_bit.MODE = 1;
  
     //General Timer1 Run Control --> Timer Start
     TBCN1_bit.TBR = 1;
#else	/* Modified by jhRoh, March 20 2019 */
        IMR_bit.IM0 = 1;		/* interrupt mask system register */
	TBC1 = UART_DURATION;			/* 830us for 1200bps */
	TBCN1_bit.TBPS  = 2;	/* timer prescaler select : Timer Clock / 16 */
	TBCN1_bit.TBIE = 1;		/* timer interrupt enable */
	TBCN1_bit.MODE = 1;		/* timer mode select : compare mode */
	TBCN1_bit.TBR = 0;		/* timer run control : start */
#endif
}

void Timer2_Init(void)
{
#if 0
     //Timer 2 Configuration
     //Genera2 Timer Compare Value for 50msec
     TBC2 = 50000;    
     //Prescaler Selection -->Timer Clock (fastest Clock)
     TBCN2_bit.TBPS  = 2;
     //General Timer2 Interrupt Enable
     TBCN2_bit.TBIE = 0;
  
     //Genera2 Timer2 Mode --> Compare mode
     TBCN2_bit.MODE = 1;
  
     //General Timer1 Run Control --> Timer Start
     TBCN2_bit.TBR = 1;
#endif
     	IMR_bit.IM0 = 1;		/* interrupt mask system register */
	TBC2 = UART_DURATION;			/* 830us for 1200bps */
	TBCN2_bit.TBPS  = 2;	/* timer prescaler select : Timer Clock / 16 */
	TBCN2_bit.TBIE = 1;		/* timer interrupt enable */
	TBCN2_bit.MODE = 1;		/* timer mode select : compare mode */
	TBCN2_bit.TBR = 0;		/* timer run control : start */
}

void startTimer1(void)
{
    TBCN1_bit.TBR = 0;
	TBCN1_bit.TBR = 1;		/* timer run control : start */
}

void stopTimer1(void)
{
	TBCN1_bit.TBR = 0;		/* timer run control : stop */  
}


void startTimer2(void)
{   
    TBCN2_bit.TBR = 0;
	TBCN2_bit.TBR = 1;		/* timer run control : start */
}

void stopTimer2(void)
{        
	TBCN2_bit.TBR = 0;		/* timer run control : stop */
}

void setTimer1Tbc(unsigned short tbc1)
{
	TBC1 = tbc1;
}

void setTimer2Tbc(unsigned short tbc2)
{
	TBC2 = tbc2;
}

void Timer3_Init(void)
{
#if 0
     //Timer 3 Configuration
     //Genera3 Timer Compare Value for 50msec
     // TBC3 = (100000/4);    
     TBC3 = 5000;    
     //Prescaler Selection -->Timer Clock (fastest Clock)
     // TBCN3_bit.TBPS  = 3;
     TBCN3_bit.TBPS  = 2;
     //General Timer3 Interrupt Enable
     TBCN3_bit.TBIE = 0;
  
     //General Timer3 Mode --> Compare mode
     TBCN3_bit.MODE = 1;
  
     //General Timer3 Run Control --> Timer Start
     TBCN3_bit.TBR = 1;
#endif
     	IMR_bit.IM3 = 1;		/* interrupt mask system register */
	TBC3 = 1000;	                /* 1ms */
	TBCN3_bit.TBPS  = 2;	/* timer prescaler select : Timer Clock / 16 */
	TBCN3_bit.TBIE = 1;		/* timer interrupt enable */
	TBCN3_bit.MODE = 1;		/* timer mode select : compare mode */
	TBCN3_bit.TBR = 1;		/* timer run control : start */
}
//End

void stopTimer3(void)
{        
	TBCN3_bit.TBR = 0;		/* timer run control : stop */
}
void startTimer3(void)
{   
    TBCN3_bit.TBR = 0;
	TBCN3_bit.TBR = 1;		/* timer run control : start */
}
