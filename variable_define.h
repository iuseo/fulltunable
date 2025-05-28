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


#ifndef __VARIABLEDEFINE_H_  
#define __VARIABLEDEFINE_H_


/*******    Create memory for all A2 lower registers in the defined locaitons     ********/

#pragma location = RX_COUNT_ADD
__no_init unsigned int Rx_Count;
#pragma required = Rx_Count

#pragma location = MEM_ADD_ADD
__no_init unsigned int Mem_Address;
#pragma required = Mem_Address

#pragma location = COMMON_MEM_SRAM 
__no_init unsigned char COMMON_MEMORY[128];
#pragma required = COMMON_MEMORY 

//I2C Receive buffer
#pragma location = I2C_RX_BUF_ADD
__no_init unsigned char I2C_Rx_Buffer[256];
#pragma required = I2C_Rx_Buffer


#pragma location = A2_UserData_ADD
__no_init unsigned char A2_UserData[A2_UserData_SIZE];					/* table 0x00 */
#pragma required = A2_UserData

#pragma location = A2_VendorFn_ADD
__no_init unsigned char A2_VendorFn[A2_VendorFn_SIZE];					/* table 0x10 */
#pragma required = A2_VendorFn

#ifdef CDR_GN2146
	#pragma location = A2_GN2146Sync_ADD
	__no_init unsigned char A2_GN2146Sync[A2_GN2146Sync_SIZE];			/* table 0x11 */
	#pragma required = A2_GN2146Sync
#endif
        
#pragma location = A2_APCLUTAB_ADD
__no_init unsigned char A2_APCLUTAB[A2_APCLUTAB_SIZE];					/* not using table */
#pragma required = A2_APCLUTAB
               
#pragma location = A2_MODLUTAB_ADD
__no_init unsigned char A2_MODLUTAB[A2_MODLUTAB_SIZE];					/* table 0x13 */
#pragma required = A2_MODLUTAB

#pragma location = A2_TECLUTAB_ADD
__no_init unsigned char A2_TECLUTAB[A2_TECLUTAB_SIZE];					/* table 0x14 */
#pragma required = A2_TECLUTAB

#pragma location = A2_APDLUTAB_ADD
__no_init unsigned char A2_APDLUTAB[A2_APDLUTAB_SIZE];					/* table 0x15 */
#pragma required = A2_APDLUTAB

#pragma location = A0_SerialID_ADD
__no_init unsigned char A0_SerialID[A0_SerialID_SIZE];					/* A0 */
#pragma required = A0_SerialID

#pragma location = A0_Reserved_ADD
__no_init unsigned char A0_Reserved[A0_Reserved_SIZE];					/* A0 reserved */
#pragma required = A0_Reserved	

#pragma location = A2_DiagData_ADD
__no_init unsigned char A2_DiagData[A2_DiagData_SIZE];					/* A2 lower */
#pragma required = A2_DiagData

#pragma location = A2_UpperMEM_ADD
__no_init unsigned char A2_UpperMEM[A2_UpperMEM_SIZE];					/* A2 upper */
#pragma required = A2_UpperMEM

#pragma location = A2_TTOSA_PAGE_0x02_ADD
__no_init unsigned char A2_TTOSA_PAGE_0x02[A2_TTOSA_PAGE_0x02_SIZE];	                /* table 0x02 */
#pragma required = A2_TTOSA_PAGE_0x02

#pragma location = A2_TTOSA_MEMORY_ADD
__no_init unsigned char A2_TTOSA_MEMORY[A2_TTOSA_MEMORY_SIZE];			        /* table 0x30 */
#pragma required = A2_TTOSA_MEMORY

#pragma location = A2_8690_LIMIT_ADD
__no_init unsigned char A2_8690_LIMIT[A2_8690_LIMIT_SIZE];			        /* table 0x40 */
#pragma required = A2_8690_LIMIT

#pragma location = A2_DEBUGTAB_ADD
__no_init unsigned char A2_DEBUGTAB[A2_DEBUGTAB_SIZE];			                /* table 0x70 */
#pragma required = A2_DEBUGTAB

#pragma location = A2_UART_MEMORY_ADD
__no_init unsigned char A2_UART_MEMORY[A2_UART_MEMORY_SIZE];			        /* table 0x80 */
#pragma required = A2_UART_MEMORY

#pragma location = A2_FAR_END_A0_0x20_ADD
__no_init unsigned char A2_FAR_END_A0_0x20[A2_FAR_END_A0_0x20_SIZE];			/* table 0x81 */
#pragma required = A2_FAR_END_A0_0x20

#pragma location = A2_FAR_END_A2_0x22_ADD
__no_init unsigned char A2_FAR_END_A2_0x22[A2_FAR_END_A2_0x22_SIZE];			/* table 0x91 */
#pragma required = A2_FAR_END_A2_0x22

#pragma location = A2_FAR_END_02_0x24_ADD
__no_init unsigned char A2_FAR_END_02_0x24[A2_FAR_END_02_0x24_SIZE];			/* table 0xA1 */
#pragma required = A2_FAR_END_02_0x24


#define DACNum				5
#define	APCL_REG			0	// APC SET
#define	AMPU_REG			1	// Mod SET
#define	WAVE_REG			2	// TEC SET
#define	APDS_REG			3	// APD SET 
#define	AGCC_REG			4	// AGC SET 


#define LUTAPD_ENABLE                   0x08
#define LUTTEC_ENABLE                   0x04
#define LUTMOD_ENABLE                   0x02
#define LUTAPC_ENABLE                   0x01


#define	_OFF				0
#define	_ON				1

#define FAULT_OFF       1
#define FAULT_ON        0


#define LUT_HIGH_Add		( LUT_LOW_Add + 72 )
#define LUT_LOW_Add			0x80 - 128
#define TEMP_INDEX			0x80
#define INDEXON				1
#define INDEXOFF			0
#define	SET				0
#define	BUF				1
#define	MANUAL				0x00

#define	SETPW				0
#define	CLRPW				1

//Bit set define	
#define	B7_H				0x80
#define	B7_L				0x7F
#define	B6_H				0x40 
#define	B6_L				0xBF 
#define	B5_H				0x20 
#define	B5_L				0xDF
#define	B4_H				0x10
#define	B4_L				0xEF
#define	B3_H				0x08
#define	B3_L				0xF7
#define	B2_H				0x04
#define	B2_L				0xFB	
#define	B1_H				0x02
#define	B1_L				0xFD
#define	B0_H				0x01
#define	B0_L				0xFE

#endif // __SFF_8431_H
