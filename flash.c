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


#include <intrinsics.h>
#include <iods4835.h>

#include "flash.h"
#include "includes.h"
#include "main.h"
#include "appGpio.h"
/*******************************************************************************
 * Function: ReadFromFlash
 *
 * This function reads data from flash and write into SRAM.
 *
 * Input:     None
 *
 * Output:    None
 *
 * Example:   To Recall (read) block of data from flash into sram...
 *                  ReadFromFlash();
 *********
 * Notes:
 *********
 * 1.
 * During block read from flash global interrupt is disabled.
 *
 * 2.
 * Last parmeter into read_block_from_flash() function is no of WORDS to be
 * read from flash.
 *
 ******************************************************************************/

void ReadFromFlash(unsigned int SourceAddress, unsigned int DesAddress, unsigned int length)
{
     //Read data from flash and store into sram
     //Assembly function call
     read_block_from_flash(SourceAddress, DesAddress, length>>1);
}


unsigned int Read_Word_from_Flash(unsigned int SourceAddress)
{
    unsigned int Word;
    Word = read_word_from_flash(SourceAddress);
    return Word;
}

/*******************************************************************************
 * Function: StoreConfigInFlash
 *
 * This function reads data from flash and write into SRAM.
 *
 * Input:     None
 *
 * Output:    None
 *
 * Example:   To Store block of data from sram into flash.
 *                  StoreConfigInFlash();
 *********
 * Notes:
 *********
 * 1.
 * During block read from flash global interrupt is disabled.
 *
 * 2.
 * Last parmeter into write_block_to_flash() function is no of WORDS to be
 * written into flash. This should be less than page size (256 Words)
 *
 * 3.
 * Flash erase timing          40msec (MAX.)
 * Flash per word write time   40usec (MAX.)
 * To meet the above timing on power dowm, Power Supply should have large value
 *
 * 4.
 * Flash Endurance is 20,000 (Min.) WRITE CYCLES.
 *
 ******************************************************************************/
void StoreConfigInFlash(unsigned int flash_address, unsigned int sram_address, unsigned int length)
{
     unsigned int uiFlashFail;

     //Disable Global Interrupt
     __disable_interrupt();

     //Flash page erase can take max. of 40msec and 
     //if DVDD level (say power supply start falling) is lower than 
     //Supply Monitor Level, don't write into flash.
  //   if((!SVMI))
     {
         //Flash Page Erase
         //Assembly Function Calls
         uiFlashFail = clear_block_in_flash(flash_address);

        //Write Block of data from SRAM to Flash
        //Assembly call
        if(!uiFlashFail)  //Write new Configuration in flash is no flash failure
        {
            uiFlashFail = write_block_to_flash(flash_address, sram_address, length>>1);
        }
     }     
     
     if(uiFlashFail != 0)
     {
        //not able to attempt write to flash
        uiFlashFail |= 0x80;   //TOSANJAY : System Error
     }

     //Enable Global Interrupt
     __enable_interrupt();
	 
}

void CopySramToFlash(unsigned int sram_add, unsigned int flash_add, unsigned int length)
{
     unsigned int fail = 0x000;
     unsigned char interrupt;
     
     interrupt = IC;                                                      //save current settings
     __disable_interrupt();                                               //Disable Global Interrupt

     
     if((!SVM_bit.SVMI))                                                  //do not attempt delete if VCC is below threshold 
     {                  
        fail = clear_block_in_flash(flash_add);                           //Flash Page Erase, assembly routine.  Can take up to 40ms to complete

        if(!fail)                                                         //if erase executed properly
        {
          write_block_to_flash(flash_add, sram_add, length>>1);           //assembly call to write block of data to flash    
        }
     }
     
     IC = interrupt;                                                      //re-store interrupts

     //Enable Global Interrupt
     __enable_interrupt();     
   
}

//-----------------------------------------------------------------------------
// FlashReadBlock
//   - read a block of words from flash starting at FlashAddr and storing to RAM 
//    starting at uiramAddr
//-----------------------------------------------------------------------------
void FlashReadBlock(unsigned int flash_add, unsigned int sram_add, unsigned int length)
{    
  read_block_from_flash(flash_add, sram_add, (length>>1) );         //Read data from flash and store into sram, Assembly function call 
}

//End

// void FLASH_Clear (unsigned int dest, unsigned int src, unsigned int numbytes);
// void FLASH_Write (unsigned int dest, unsigned int src, unsigned int numbytes);
// void FLASH_Read (unsigned int dest, unsigned int src, unsigned int numbytes);

// void FLASH_Copy (unsigned int dest, unsigned int src, unsigned int numbytes);

// -----------------------------------------------------------------------------
// Function Prototypes
// -----------------------------------------------------------------------------
// void FLASH_ByteWrite (unsigned int addr, unsigned int byte);
// void FLASH_ByteRead(unsigned int addr, unsigned int byte);
// void FLASH_PageErase (unsigned int addr);

//-----------------------------------------------------------------------------
// FLASH_ByteWrite
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of the byte to write to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   2) char byte - byte to write to flash.
//
// This routine writes <byte> to the linear flash address <addr>.
//
//-----------------------------------------------------------------------------
// void RegWordTo2ByteSave (unsigned int data, unsigned char* addr)
// {	
	// *(unsigned int * )( addr ) = __swap_bytes(data);
// }
void RegWordTo2ByteSave (unsigned short data, unsigned char* addr)
{	
	unsigned short d; 
	d =__swap_bytes(data);
	memcpy(addr, &d, sizeof(unsigned short));
}
// void RegWordTo4ByteSave (float data, unsigned char* addr)
// {	
	// *(float * )( addr ) = data;
	// swap_uint32(addr);
// }
void RegWordTo4ByteSave (float data, unsigned char* addr)
{	
	memcpy(addr, &data, sizeof(float));
	swap_uint32(addr);
}

unsigned short swap_uint16( unsigned int val ) 
{
    return (val << 8) | (val >> 8 );
}

//! Byte swap short
// int16_t swap_int16( int16_t val ) 
// {
    // return (val << 8) | ((val >> 8) & 0xFF);
// }

//! Byte swap unsigned int
void swap_uint32( unsigned char* val )
{
	unsigned char temp;
	temp = *(val+3);
	*(val+3) = *(val);
	*(val) = temp;
	
	temp = *(val+2);
	*(val+2) = *(val+1);
	*(val+1) = temp;
}

//! Byte swap int
// int32_t swap_int32( int32_t val )
// {
    // val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF ); 
    // return (val << 16) | ((val >> 16) & 0xFFFF);
// }

// unsigned short Reg2ByteToWordLoad (unsigned char* addr)
// {
	// unsigned short d;
	// d = (*(addr)<<8);
	// d |= *(++addr);
	// return  d;
// }
unsigned short Reg2ByteToWordLoad (unsigned char* addr)
{
	unsigned short d;
	memcpy(&d, addr, sizeof(unsigned short));
	return __swap_bytes(d);
}
// unsigned short Reg2ByteToWordLoad (unsigned char* addr)
// {
	// unsigned int d;
	// d = __swap_bytes( *(unsigned int * )( addr )) ;
	// return unsigned short(d);
// }


void FLASH_ByteWrite (unsigned int addr, unsigned int byte)
{
	unsigned char interrupt;

	interrupt = IC;                                                      //save current settings
	//__disable_interrupt();                                               //Disable Global Interrupt	
	if((!SVM_bit.SVMI))                                                  //do not attempt delete if VCC is below threshold 
	{ 	
		write_block_to_flash(addr, byte, 1);
	}
	IC = interrupt;                                                      //re-store interrupts

	//Enable Global Interrupt
	//__enable_interrupt();   	
	
}
//-----------------------------------------------------------------------------
// FLASH_ByteRead
//-----------------------------------------------------------------------------
//
// Return Value :
//      uint8_t - byte read from flash
// Parameters   :
//   1) FLADDR addr - address of the byte to read to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//
// This routine reads a <byte> from the linear flash address <addr>.
//
//-----------------------------------------------------------------------------
void FLASH_ByteRead(unsigned int addr, unsigned int byte)
{
        // ReadFromFlash(addr, byte, 2);         //Read data from flash and store into sram, Assembly function call 
        read_block_from_flash(addr, byte, 1);
   
}
//-----------------------------------------------------------------------------
// FLASH_PageErase
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of any byte in the page to erase
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//
// This routine erases the flash page containing the linear flash address
// <addr>.  Note that the page of flash page containing the Lock Byte cannot be
// erased if the Lock Byte is set.
//
//-----------------------------------------------------------------------------
void FLASH_PageErase(unsigned int addr)
{
	unsigned char interrupt;

	interrupt = IC;                                                      //save current settings
	//__disable_interrupt();                                               //Disable Global Interrupt
	if((!SVM_bit.SVMI))                                                  //do not attempt delete if VCC is below threshold 
	{ 
		clear_block_in_flash(addr);  
	}
	IC = interrupt;                                                      //re-store interrupts

	//Enable Global Interrupt
	//__enable_interrupt();   	
}

//-----------------------------------------------------------------------------
// FLASH_Write
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR dest - starting address of the byte(s) to write to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   2) char *src - pointer to source bytes
//   3) uint16_t numbytes - the number of bytes to write
//                              valid range is is range of integer
//
// This routine copies <numbytes> from <src> to the linear flash address
// <dest>.  The bytes must be erased to 0xFF before writing.
// <dest> + <numbytes> must be less than the maximum flash address.
//
//-----------------------------------------------------------------------------
void FLASH_Write (unsigned int dest, unsigned int src, unsigned int numbytes)
{
	unsigned char interrupt;
	
	interrupt = IC;                                                      //save current settings
	__disable_interrupt();                                               //Disable Global Interrupt
	
	if((!SVM_bit.SVMI))                                                  //do not attempt delete if VCC is below threshold 
	{                  
		write_block_to_flash(dest, src, numbytes>>1);           //assembly call to write block of data to flash    
	}	
	IC = interrupt;                                                      //re-store interrupts
	
	//Enable Global Interrupt
	__enable_interrupt();     
	
}
//-----------------------------------------------------------------------------
// FLASH_Read
//-----------------------------------------------------------------------------
//
// Return Value : None
//   1) char *dest - pointer to destination bytes
// Parameters   :
//   1) char *dest - pointer to destination bytes
//   2) FLADDR src - address of source bytes in flash
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   3) uint16_t numbytes - the number of bytes to read
//                              valid range is range of integer
//
// This routine copies <numbytes> from the linear flash address <src> to
// <dest>.
// <src> + <numbytes> must be less than the maximum flash address.
//
//-----------------------------------------------------------------------------
void FLASH_Read (unsigned int dest, unsigned int src, unsigned int numbytes)
{
  	unsigned char interrupt;
	interrupt = IC;                                                      //save current settings
	__disable_interrupt();                                               //Disable Global Interrupt
	
	read_block_from_flash(dest, src, numbytes>>1);	
	IC = interrupt;                                                      //re-store interrupts

	//Enable Global Interrupt
	
	__enable_interrupt();   
}
//-----------------------------------------------------------------------------
// FLASH_Clear
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR addr - address of the byte to write to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   2) uint16_t numbytes - the number of bytes to clear to 0xFF
//                    valid range is 0 to FLASH_PAGESIZE
//
// This routine erases <numbytes> starting from the flash addressed by
// <dest> by performing a read-modify-write operation using <FLASH_TEMP> as
// a temporary holding area.
// <addr> + <numbytes> must be less than the maximum flash address.
//
//-----------------------------------------------------------------------------



void FLASH_Clear (unsigned int dest, unsigned int src, unsigned int numbytes)
{
	unsigned int dest_1_page_start;           // First address in 1st page
									   // containing <dest>
	unsigned int dest_1_page_end;             // Last address in 1st page
									   // containing <dest>
	unsigned int dest_2_page_start;           // First address in 2nd page
									   // containing <dest>
	unsigned int dest_2_page_end;             // Last address in 2nd page
									   // containing <dest>
	unsigned int numbytes_remainder;        // When crossing page boundary,
									   // number of <src> bytes on 2nd page
	unsigned int FLASH_pagesize;            // Size of flash page to update

	unsigned int  wptr;                       // Write address
	unsigned int  rptr;                       // Read address

	unsigned int length;
	
	FLASH_pagesize = FLASH_PAGESIZBYTE;

	dest_1_page_start = dest & ~(FLASH_pagesize - 1);
	dest_1_page_end = dest_1_page_start + FLASH_pagesize - 1;
	dest_2_page_start = (dest + numbytes - 1)  & ~(FLASH_pagesize - 1);
	dest_2_page_end = dest_2_page_start + FLASH_pagesize - 1;

	if (dest_1_page_end == dest_2_page_end)
	{                                            			
		// 1. Erase Scratch page	
		FLASH_PageErase (FLASH_TEMP);
		// 2. Copy bytes from first byte of dest page to dest-1 to Scratch page

		wptr = FLASH_TEMP;
		rptr = dest_1_page_start;
		length = dest - dest_1_page_start;
		FLASH_Copy (wptr, rptr, length);

		// 3. Copy from (dest+numbytes) to dest_page_end to Scratch page
		wptr = FLASH_TEMP + dest - dest_1_page_start + numbytes;
		rptr = dest + numbytes;
		length = dest_1_page_end - dest - numbytes + 1;
		FLASH_Copy (wptr, rptr, length);		
		
		// 4. Erase destination page
		FLASH_PageErase (dest_1_page_start);
      
		// 5. Copy Scratch page to destination page
		wptr = dest_1_page_start;
		rptr = FLASH_TEMP;
		length = FLASH_pagesize;
		FLASH_Copy (wptr, rptr, length);				
	}	  
	else
	{	// value crosses page boundary
		// 1. Erase Scratch page
		FLASH_PageErase (FLASH_TEMP);

		// 2. Copy bytes from first byte of dest page to dest-1 to Scratch page

		wptr = FLASH_TEMP;
		rptr = dest_1_page_start;
		length = dest - dest_1_page_start;
		FLASH_Copy (wptr, rptr, length);

		// 3. Erase destination page 1
		FLASH_PageErase (dest_1_page_start);

		// 4. Copy Scratch page to destination page 1
		wptr = dest_1_page_start;
		rptr = FLASH_TEMP;
		length = FLASH_pagesize;
		FLASH_Copy (wptr, rptr, length);

		// now handle 2nd page

		// 5. Erase Scratch page
		FLASH_PageErase (FLASH_TEMP);

		// 6. Copy bytes from numbytes remaining to dest-2_page_end to Scratch page

		numbytes_remainder = numbytes - (dest_1_page_end - dest + 1);
		wptr = FLASH_TEMP + numbytes_remainder;
		rptr = dest_2_page_start + numbytes_remainder;
		length = FLASH_pagesize - numbytes_remainder;
		FLASH_Copy (wptr, rptr, length);

		// 7. Erase destination page 2
		FLASH_PageErase (dest_2_page_start);

		// 8. Copy Scratch page to destination page 2
		wptr = dest_2_page_start;
		rptr = FLASH_TEMP;
		length = FLASH_pagesize;
		FLASH_Copy (wptr, rptr, length);
		
	}
	
} 
//-----------------------------------------------------------------------------
// FLASH_Copy
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR dest - pointer to destination bytes in flash
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   2) FLADDR src - address of source bytes in flash
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   3) uint16_t numbytes - the number of bytes to copy
//                              valid range is range of integer
//
// This routine copies <numbytes> from <src> to the linear flash address
// <dest>.  The destination bytes must be erased to 0xFF before writing.
// <src>/<dest> + <numbytes> must be less than the maximum flash address.
//
//-----------------------------------------------------------------------------
void FLASH_Copy (unsigned int dest, unsigned int src, unsigned int numbytes)
{
	unsigned int i;
	for (i = 0; i < (numbytes/2); i++) 
	{		
		FLASH_ByteRead(src+(i*2), COMMON_MEM_SRAM);
		FLASH_ByteWrite(dest+(i*2), COMMON_MEM_SRAM);
	}
}


//-----------------------------------------------------------------------------
// FLASH_Update
//-----------------------------------------------------------------------------
//
// Return Value : None
// Parameters   :
//   1) FLADDR dest - starting address of the byte(s) to write to
//                    valid range is from 0x0000 to 0x1FFF for 8 kB devices
//                    valid range is from 0x0000 to 0x0FFF for 4 kB devices
//                    valid range is from 0x0000 to 0x07FF for 2 kB devices
//   2) char *src - pointer to source bytes
//   3) uint16_t numbytes - the number of bytes to update
//                              valid range is 0 to FLASH_PAGESIZE
//
// This routine replaces <numbytes> from <src> to the flash addressed by
// <dest>.  This function calls FLASH_Clear() to initialize all <dest> bytes
// to 0xff's prior to copying the bytes from <src> to <dest>.
// <dest> + <numbytes> must be less than the maximum flash address.
//
//-----------------------------------------------------------------------------
void FLASH_Update (unsigned int src, unsigned int dest, unsigned int numbytes)
{

	// 1. Erase <numbytes> starting from <dest>
	FLASH_Clear (dest, src, numbytes);
	// 2. Write <numbytes> from <src> to <dest>
	FLASH_Write(dest, src, numbytes);
}




