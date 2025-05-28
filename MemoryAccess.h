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
// MemoryAccess.h
//-----------------------------------------------------------------------------

#ifndef MEMORYACCESS_H
#define MEMORYACCESS_H

//-----------------------------------------------------------------------------
// FW version defines
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// defines
//----------------------------------------------------------------------------- 

//#define ADDON_JMA_WIRELESS
//#define AXIOM
//#define NORMAL_CUSTOMER
#define SAMSUNG
#define RSSI_LOS_TYPE

//#define USE_EXT_EEPROM

#define CDR_GN2146 
#define FW_VERSION							76

#ifdef CDR_GN2146
	#define PRODUCT_ID 						'S'
	/*******    Memory locations for in SRAM  lightorn   ********/
	#define	RESERVED1							0x0000							/* location of debugging working space */
	#define	RX_COUNT_ADD						0x0018							/* location for Rx_Count */
	#define	MEM_ADD_ADD							0x001A							/* location for Mem_Address */
	#define	COMMON_MEM_SRAM						0x001C							/* location of scratch area for Flash write */
	#define	I2C_RX_BUF_ADD						0x009C							/* location of 256 byte I2C Rx Buffer */
	#define	A2_UserData_ADD						0x019C							/* location of A2 USER EEPROM memeory in SRAM */
	#define	A2_VendorFn_ADD						0x021C							/* location of A2 Vendor memeory in SRAM */
	#define	A2_GN2146Sync_ADD					0x029C							/* location of A2 GN2146 Sync. memeory in SRAM */
	#define	A2_APCLUTAB_ADD						0x031C							/* location of A2 APC LUT memeory in SRAM */
	#define	A2_MODLUTAB_ADD						0x039C							/* location of A2 MOD LUT memeory in SRAM */
	#define	A2_TECLUTAB_ADD						0x041C							/* location of A2 TEC LUT memeory in SRAM */
	#define	A2_APDLUTAB_ADD						0x049C							/* location of A2 APD LUT memeory in SRAM */
	#define	A2_Reserve0_ADD						0x051C							/* location of A2 reserved1 memeory in SRAM */
	#define	A2_Reserve1_ADD						0x059C							/* location of A2 reserved2 memeory in SRAM */
	#define	A2_Reserve2_ADD						0x05C0							/* location of A2 reserved3 memeory in SRAM */
	#define	A2_Reserve3_ADD						0x05E0							/* location of A2 reserved4 memeory in SRAM */
	#define	A0_SerialID_ADD						0x0600							/* location of A0 SerialID memeory in SRAM */
	#define	A0_Reserved_ADD						0x0680							/* location of A0 Reserved memeory in SRAM */
	#define	A2_DiagData_ADD						0x0700							/* location of A2 diagdata memeory in SRAM */
	#define	A2_UpperMEM_ADD						0x0780							/* location of A2 upper memeory in SRAM */
	#define A2_TTOSA_PAGE_0x02_ADD				0x0880							/* location of A2 Tunable TOSA Page 0x02 memory in SRAM */
	#define A2_DEBUGTAB_ADD                		0x0980                     		/* location of A2 DEBUG moemory in SRAM */
	#define A2_FAR_END_A0_0x20_ADD          	0x0A00                 			/* location of A2 FAR END A0 0x81 in SRAM */
	#define A2_FAR_END_A2_0x22_ADD         		0x0A80                  		/* location of A2 FAR END A2 0x91 in SRAM */
	#define A2_FAR_END_02_0x24_ADD        		0x0B00                    		/* location of A2 FAR END 0x02 Table 0xA1 in SRAM */
	#define A2_UART_MEMORY_ADD         			0x0B80                   		/* location of A2 UART memory in SRAM */
	#define	A2_TTOSA_MEMORY_ADD					0x0C00							/* location of A2 Tunable TOSA memory in SRAM */
	#define	A2_8690_LIMIT_ADD					0x0C80							/* location of A2 Tunable TOSA memory in SRAM */

	#define	A2_UserData_SIZE					128
	#define	A2_VendorFn_SIZE					128
	#define	A2_GN2146Sync_SIZE					128
	#define	A2_APCLUTAB_SIZE					128
	#define	A2_MODLUTAB_SIZE					128
	#define	A2_TECLUTAB_SIZE					128
	#define	A2_APDLUTAB_SIZE					128
	#define	A2_Reserve0_SIZE					128
	#define	A2_Reserve1_SIZE					36 
	#define	A2_Reserve2_SIZE					32 
	#define	A2_Reserve3_SIZE					32 
	#define	A0_SerialID_SIZE					128
	#define	A0_Reserved_SIZE					128
	#define	A2_DiagData_SIZE					128
	#define	A2_UpperMEM_SIZE					128
	#define	A2_TTOSA_PAGE_0x02_SIZE				128
	#define	A2_TTOSA_MEMORY_SIZE				128
	#define A2_DEBUGTAB_SIZE               		128
	#define	A2_8690_LIMIT_SIZE					128
	#define A2_FAR_END_A0_0x20_SIZE       		128
	#define A2_FAR_END_A2_0x22_SIZE         	128
	#define A2_FAR_END_02_0x24_SIZE       		128
	#define	A2_UART_MEMORY_SIZE					128
	/*******    Memory locations for A0, A2 and LUTs in Flash     ********/


	#define MAXQPassword_FLASH					0x0020
	// Set the 32-byte MAXQ flash password at code address 10h, need to check "Place constants in CODE" in Options/General/Target
        
//	#define	A2_UserData_FLASH	     		5500							/* location of A2 USER EEPROM memeory in SRAM */
//#ifdef USE_EXT_EEPROM                        
  	#define	A2_VendorFn_EEPROM					0x0080							/* location of A2 Vendor memeory in SRAM */
	#define	A2_GN2146Sync_EEPROM 				0x0100							/* location of A2 XN2139 Sync. memeory in SRAM */
	#define	A2_APCLUTAB_EEPROM					0x0180							/* location of A2 APC LUT memeory in SRAM */
	#define	A2_MODLUTAB_EEPROM					0x0200							/* location of A2 MOD LUT memeory in SRAM */
	#define	A2_TECLUTAB_EEPROM					0x0280							/* location of A2 TEC LUT memeory in SRAM */
	#define	A2_APDLUTAB_EEPROM					0x0300							/* location of A2 APD LUT memeory in SRAM */
	#define TTOSA_CHANNEL_NUMBER_EEPROM			0x0380        
	#define	A2_TTOSA_PAGE_0x02_EEPROM			0x0400 							/* location of A2 A2_TTOSA_PAGE_0x02_SIZE in SRAM */ 
	#define	A0_SerialID_EEPROM					0x0480							/* location of A0 SerialID memeory in SRAM */
	#define	A0_Reserved_EEPROM					0x0500							/* location of A0 Reserved memeory in SRAM */
	#define	A2_DiagData_EEPROM					0x0580							/* location of A2 diagdata memeory in SRAM */
	#define	A2_UpperMEM_EEPROM					0x0600							/* location of A2 upper memeory in SRAM */
	#define	A2_UserData_EEPROM					0x0680							/* location of A2 USER EEPROM memeory in SRAM */
	#define	A2_8690_LIMIT_EEPROM				0x0700							/* location of A2 USER EEPROM memeory in SRAM */

	#define	A2_TTOSA_CHANNEL_01_EEPROM			0x2000							/* location of A2 TTOSA_CH01 in SRAM */
	#define	A2_TTOSA_CHANNEL_98_EEPROM			0x5080							/* location of A2 TTOSA_CH98 in SRAM */

    #define A2_DEBUGTEAB_FLASH                	0xF000							/* location of A2 DEBUG moemory in SRAM */
//#else
//  #define   A2_DEBUGTEAB_FLASH                	0xC400                          /* location of A2 DEBUG moemory in SRAM */
  	#define	A2_VendorFn_FLASH					0xF200							/* location of A2 Vendor memeory in SRAM */
	#define	A2_GN2146Sync_FLASH					0xF280							/* location of A2 XN2139 Sync. memeory in SRAM */
	#define	A2_APCLUTAB_FLASH					0xF300							/* location of A2 APC LUT memeory in SRAM */
	#define	A2_MODLUTAB_FLASH					0xF380							/* location of A2 MOD LUT memeory in SRAM */
	#define	A2_TECLUTAB_FLASH					0xF400							/* location of A2 TEC LUT memeory in SRAM */

	#define	A2_APDLUTAB_FLASH					0xF480							/* location of A2 APD LUT memeory in SRAM */
	#define	A2_TTOSA_PAGE_0x02_FLASH			0xFA00							/* location of A2 A2_TTOSA_PAGE_0x02_SIZE in SRAM */ 
	#define	A0_SerialID_FLASH					0xF580							/* location of A0 SerialID memeory in SRAM */
	#define	A0_Reserved_FLASH					0xF600							/* location of A0 Reserved memeory in SRAM */
    #define A2_Reserved_FLASH                   0xF680
    #define	A2_DiagData_FLASH					0xF700							/* location of A2 diagdata memeory in SRAM */
	#define	A2_UpperMEM_FLASH					0xF780							/* location of A2 upper memeory in SRAM */
	#define	A2_UserData_FLASH					0xF800							/* location of A2 USER EEPROM memeory in SRAM */

	#define	A2_TTOSA_CHANNEL_01_FLASH			0xAF80							/* location of A2 TTOSA_CH01 in SRAM */
	#define	A2_TTOSA_CHANNEL_98_FLASH			0xE080							/* location of A2 TTOSA_CH98 in SRAM */
//	#define TTOSA_CHANNEL_NUMBER_FLASH			0xFA00							/* 128 * 4 = 512, TTOSA Channel Number */
    #define A2_DEBUGTEAB_FLASH                  0xF000
	#define A2_8690_LIMIT_0x40_FLASH			0xFC00
//#endif
	/* flashFlagByte1 */
	#define A2_UserDataWrFlag					0x01
	#define A2_VendorFnWrFlag					0x02
	#define A2_GN2146SyncWrFlag					0x04
	#define A2_APCLUTABWrFlag					0x08 	
	#define A2_MODLUTABWrFlag					0x10
	#define A2_TECLUTABWrFlag					0x20
	#define A2_APDLUTABWrFlag					0x40

	/* flashFlagByte2 */
	#define A0_SerialIDWrFlag					0x01
	#define A0_ReservedWrFlag					0x02
	#define A2_DiagDataWrFlag					0x04
	#define A2_DEBUGTABWrFlag					0x08
	#define A2_8690_LIMITWrFlag					0x10
	#define A2_TTOSAMemoryWrFlag				0x20
	#define TTOSAChannelWrFlag					0x40
    #define A2_TTOSA_PAGE_0x02WrFlag            0x80

	
	

	//#define TTOSA_CHANNEL_NUMBER_EEPROM			0x0000       
#endif
					  
#define ACCESS_LEVEL_NONE                     	0
#define ACCESS_LEVEL_RO                       	1
#define ACCESS_LEVEL_RW                       	2


//Password definition
#define PASSWORD1_B0                          	0xFF   
#define PASSWORD1_B1                          	0xFF   
#define PASSWORD1_B2                          	0xFF
#define PASSWORD1_B3                          	0xFF
                          
#define PASSWORD2_B0                          	0xFF
#define PASSWORD2_B1                          	0xFF
#define PASSWORD2_B2                          	0xFF
#define PASSWORD2_B3                          	0xFF

// LUT definition
#define LUT_1       1
#define LUT_2       2
#define LUT_3       3
#define LUT_4       4
#define LUT_5       5
#define LUT_6       6
#define LUT_7       7
#define LUT_8       8

#define _LOW	0
#define _HIGH	1

#define _OFF	0
#define _ON	1

#define MEMORY_WRITE_BUFFER_SIZE    8

//possible flags for WriteBufferStatus
typedef enum WRITE_BUFFER_STATUS_ENUM_TAG
{
   MEMORY_WRITE_BUFFER_IDLE,
   MEMORY_WRITE_BUFFER_WRITING,
   MEMORY_WRITE_BUFFER_WAITING_TO_PROCESS,
   MEMORY_WRITE_BUFFER_READING
} WriteBufferStatus_E;

//structure of parameters for communication from host over I2C
typedef struct WRITE_BUFFER_STRUCTURE_TAG
{
   unsigned char           devAddress;                                    //A0 or A2
   WriteBufferStatus_E     bufferStatus;                                  //flag showing status of transaction
   unsigned char           numOfEntries;                                  //num of bytes received           
   unsigned char           address;                                       //current register address
   unsigned char           writeBufferEntry[MEMORY_WRITE_BUFFER_SIZE];    //I2C data bytes
} WriteBuffer_S, *WriteBuffer_P;



//**** typedef
// typedef bit					BOOLEAN;
typedef unsigned char		INT8U; 
typedef signed   char		INT8S; 
typedef unsigned short		INT16U; 
typedef signed   short		INT16S; 
typedef unsigned long		INT32U;  
typedef signed   long		INT32S;     
typedef float				FP32;  


#endif // MEMORYACCESS_H

//-----------------------------------------------------------------------------
// end of file
//-----------------------------------------------------------------------------

