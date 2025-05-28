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

#ifndef _FLASH_H_
#define  _FLASH_H_

//assembly.asm functions
unsigned int write_block_to_flash(unsigned int flash_add, unsigned int ram_add, unsigned int bytes);
unsigned int clear_block_in_flash(unsigned int flash_add);
void read_block_from_flash(unsigned int flash_add, unsigned int ram_add, unsigned int words);


unsigned int read_word_from_flash(unsigned int SourceAddress);
unsigned int Read_Word_from_Flash(unsigned int SourceAddress);
void Device_Reset(void);

//Function prototpes here
void ReadFromFlash(unsigned int SourceAddress, unsigned int DesAddress, unsigned int length);
void StoreConfigInFlash(unsigned int SourceAddress, unsigned int DesAddress, unsigned int length);

void CopySramToFlash(unsigned int sram_add, unsigned int flash_add, unsigned int length);
void CopyLutToFlash(unsigned int table);
void FlashReadBlock(unsigned int flash_add, unsigned int sram_add, unsigned int length);
unsigned int Info_Read(unsigned int address);


void FLASH_Clear (unsigned int dest, unsigned int src, unsigned int numbytes);
void FLASH_Write (unsigned int dest, unsigned int src, unsigned int numbytes);
void FLASH_Read (unsigned int dest, unsigned int src, unsigned int numbytes);

void FLASH_Copy (unsigned int dest, unsigned int src, unsigned int numbytes);

//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
unsigned short swap_uint16( unsigned int val ) ;
void swap_uint32( unsigned char* val );

// void RegWordTo2ByteSave (unsigned int data, unsigned char* addr);
void RegWordTo2ByteSave (unsigned short data, unsigned char* addr);
void RegWordTo4ByteSave (float data, unsigned char* addr);
// unsigned short Reg2ByteToWordLoad (unsigned char* addr);
unsigned short Reg2ByteToWordLoad (unsigned char* addr);

void FLASH_ByteWrite (unsigned int addr, unsigned int byte);
void FLASH_ByteRead(unsigned int addr, unsigned int byte);
void FLASH_PageErase (unsigned int addr);

void FLASH_Update (unsigned int src, unsigned int dest, unsigned int numbytes);

#define FLASH_PAGESIZBYTE 512
#ifndef FLASH_TEMP
//#define FLASH_TEMP        0x7C00           // For 32 kB Flash devices
#define FLASH_TEMP        0xFE00           // For 32 kB Flash devices
#endif


#endif    // _FLASH_H_
//End
