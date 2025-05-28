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
// twsiSlave.h
//-----------------------------------------------------------------------------
//
//  add Maxim file header
//
//-----------------------------------------------------------------------------

#ifndef TWSISLAVE_H
#define TWSISLAVE_H

//#include "memory_map.h"






//-----------------------------------------------------------------------------
// defines
//-----------------------------------------------------------------------------



#define SLAVE_ADDRESS_A0    0xA0
#define SLAVE_ADDRESS_A2    0xA2

#define SLAVE_ADDRESS_B0    0xB0
#define SLAVE_ADDRESS_B2    0xB2


extern unsigned char flashFlagByte1;
extern unsigned char flashFlagByte2;


//-----------------------------------------------------------------------------
// function prototypes
//-----------------------------------------------------------------------------

void i2cMemcpy(unsigned char* dest, unsigned char* source, char memory_SIZE);
void I2CS_Init(void);

void UpdateTxBuffer(void);
void UpdateTxBuffer1_2(void);
void UpdateAllTxBuffers(void);
void ReadI2CRxBuffer(unsigned int AddrRxCount,unsigned int AddrI2CBuffer );
void ProcessReceivedI2CData(void);

// extern unsigned char M37049Syncflag;

#endif // TWSISLAVE_H

//-----------------------------------------------------------------------------
// end of file
//-----------------------------------------------------------------------------


