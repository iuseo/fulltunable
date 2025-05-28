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
// includes
//-----------------------------------------------------------------------------
#include <intrinsics.h>
#include <iods4835.h>


#include "includes.h"
#include "main.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "sff8690.h"
#include "appGpio.h"
#include "appSmartTune.h"

//-----------------------------------------------------------------------------
// global variables defined in this module
//-----------------------------------------------------------------------------
   
static unsigned char ActiveSlave;
static unsigned char memoryStartBuffer;
//unsigned int Rx_Count;
//unsigned int Mem_Address;



//-----------------------------------------------------------------------------
// external global variables defined elsewhere
//-----------------------------------------------------------------------------
extern unsigned char PasswordLevel;

extern uint8 i2cSlaveReadFlagCount;

//extern unsigned char sfpStat[];

extern WriteBuffer_S twsiSlaveBuf;
WriteBuffer_S twsiSlaveBuf;
static void I2C_Slave_Stop(void);
__interrupt void I2CS_Interrupt(void);

extern void processTable0x02(uint8 startAddress, uint8 numberOfBytes);
extern void processTable0x30(uint8 startAddress, uint8 numberOfBytes);

//-----------------------------------------------------------------------------
// SmbusInit() function
//   - initialize the slave I2C port 
//-----------------------------------------------------------------------------
void i2cMemcpy(unsigned char* dest, unsigned char* source, char memory_SIZE){
  I2CCN_S_bit.I2CEN = 0;  
  memcpy((unsigned char*)dest, (unsigned char*)source, memory_SIZE);			
  I2CCN_S_bit.I2CEN = 1;  
}

void I2CS_Init(void)
{    
	I2CCN_S = 0x0000;                                                //Clear All I2C modules Settings
	I2CIE_S = 0x0000;
	I2CST_S = 0x0000;
	I2CCN_S = 0x0000;    
	
	MADDR = A2_DiagData_ADD;		                                 //Slave Address 1 (I2CSLA_S = A2), set location
	MADDR_bit.ROLLOVR = 1;                                           //256 bit rollover

	MADDR2 = A0_SerialID_ADD;                                        //Slave Address 2 (I2CSLA2_S = A0), set location
	MADDR2_bit.ROLLOVR = 1;                                          //256 bit rollover
  
	I2CTXFIE_bit.TXPG_EN = 1;                                        //Enable Tx Pages
	CUR_SLA_bit.MADDR_EN1 = 1;                                       //Enable Mem Address detection for Slave Address (0xA2) 
	CUR_SLA_bit.MADDR_EN2 = 1;                                       //Enable Mem Address detection for Slave Address (0xA0) 
	I2CIE2_S_bit.MADIE = 1;                                          //Enable Interrupt for Memory Address 
	I2CIE2_S_bit.SADIE = 1;                                          //Enable Start+Address clock interrupt  
	I2CIE2_S_bit.I2CSPIE = 1;                                        //Enable Stop Interrupt   
	I2CIE_S_bit.I2CAMIE = 1;                                         //Enable Interrupt for Address Cycle Interrupt 

	I2CTXFIE = 0x0082;                                               //Enable Tx Page and Threshold Interrupt        
	I2CRXFIE = 0x0082;                                               //Enable Rx FIFO and Threshold Interrupt    
	I2CTXFST = 0x0000;
	I2CRXFST = 0x0000;

	I2CSLA_S = SLAVE_ADDRESS_A2;                                     //Configure Slave Address 0xA2
	I2CSLA2_S = SLAVE_ADDRESS_A0;                                    //Configure Slave Address 0xA0   
	I2CCN_S_bit.ADDR2EN = 1;                                         //Enable Slave2 
	I2CCN_S_bit.I2CEN = 1;                                           //Enable I2C module

	Rx_Count = 0;                                                    //initialize parameters
	Mem_Address = 0;

	UpdateTxBuffer1_2();

	IMR_bit.IM2 = 1;                                                 //I2C Slave module interrupt enable
	
}

/*****************************************************************************
*
*  Name:              ProcessReceivedI2CData
*
*  Description:       This function checks 
*                     the status of the write buffer. If a write transaction
*                     from the host has placed data in the buffer then this
*                     function processes the data and forwards it to RAM, FLASH
*                     or the MAX395X
*****************************************************************************/

void ProcessReceivedI2CData(void)
{
	unsigned char i;          
	unsigned char *sram_addr_ptr;
	unsigned int num_bytes;
	unsigned int addr, flashwrite=0;
	if(twsiSlaveBuf.bufferStatus == MEMORY_WRITE_BUFFER_WAITING_TO_PROCESS) {	/* There is data to process */
		if(twsiSlaveBuf.devAddress != SLAVE_ADDRESS_A2) {	/* Slave address A0 */
			addr = twsiSlaveBuf.address;
			num_bytes = twsiSlaveBuf.numOfEntries;
			
			if(A2_DiagData[PROTECTION_ADD] & 0x01) {
				return;
			}
		
			if(addr < 128) {
				sram_addr_ptr = (unsigned char*)A0_SerialID_ADD;			/* set ram address */
				sram_addr_ptr += addr;								
				for(i = 0; i < num_bytes; i++)
				{
					*sram_addr_ptr = I2C_Rx_Buffer[i];
					*sram_addr_ptr++;
					addr++;
					if(addr >= 128) {
						sram_addr_ptr = (unsigned char*)A0_Reserved_ADD;
					}
				}
				flashFlagByte2 |= A0_SerialIDWrFlag;
			} else {
				// address > 128, reseved memeory
				sram_addr_ptr = (unsigned char*)A0_Reserved_ADD;										/* set ram address */
				sram_addr_ptr += (addr - 128);								
				for(i = 0; i < num_bytes; i++)
				{
					*sram_addr_ptr = I2C_Rx_Buffer[i];
					*sram_addr_ptr++;
					addr++;
					if(addr >= 256) {
						addr = 0;
						sram_addr_ptr = (unsigned char*)A0_SerialID_ADD;
					}
				}
				flashFlagByte2 |= A0_ReservedWrFlag;
			}				
		} else {
			/* Slave address A2 */
			if(twsiSlaveBuf.address < 0x80) {	
				addr = twsiSlaveBuf.address;
				num_bytes = twsiSlaveBuf.numOfEntries;			
				sram_addr_ptr = (unsigned char*)A2_DiagData_ADD;										/* set ram address */
				sram_addr_ptr += addr;			
				for(i = 0; i < num_bytes; i++)
				{	
					if(addr <= 95) {
						if(A2_DiagData[PROTECTION_ADD] & 0x01) {
							return;
						}
						*sram_addr_ptr = I2C_Rx_Buffer[i];				
						flashwrite = 1;						
					} else if((addr >= 110) && (addr <= 119)) {	
						*sram_addr_ptr = I2C_Rx_Buffer[i];	
						if(addr == 110){  
							sendMessage(getMessageQueue(), EVENT_SOFT_DISABLE, 0);                                           /* soft tx disable event */
						}
						flashwrite = 0;						
					} else if((addr >= 123) && (addr <= 126)) {
						*sram_addr_ptr = I2C_Rx_Buffer[i];				
						flashwrite = 0;						
					} else if(addr == 0x7F) {
						/* addr = 127, table select */
						flashwrite = 0;
						A2_DiagData[127] = I2C_Rx_Buffer[i];											/* set new table in sram */
						
                        if((A2_DiagData[127] == 0x00) || (A2_DiagData[127] == 0x01) ){
							i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_UserData_ADD, A2_UserData_SIZE);																																																						
						} else if(A2_DiagData[127] == 0x02){ 
//							if(!((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_DITHERING] & SUPPORTED) && (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_TX_DITHER] & TUNING_ENABLE))){
								i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_TTOSA_PAGE_0x02_ADD, A2_TTOSA_PAGE_0x02_SIZE);
//							}
						}else if(A2_DiagData[127] == 0x70 + 0x80){
						  	i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_DEBUGTAB_ADD, A2_DEBUGTAB_SIZE);
						}else if(A2_DiagData[127] == 0x20 + 0x80){
							i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_FAR_END_A0_0x20_ADD, A2_FAR_END_A0_0x20_SIZE);
						}else if(A2_DiagData[127] == 0x22 + 0x80){
							i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_FAR_END_A2_0x22_ADD, A2_FAR_END_A2_0x22_SIZE);
						}else if(A2_DiagData[127] == 0x24 + 0x80){
							i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_FAR_END_02_0x24_ADD, A2_FAR_END_02_0x24_SIZE);
						} else {	
							if(A2_DiagData[PROTECTION_ADD] & 0x01) {
								memset(A2_UpperMEM, 0xFF, 128);				
								return;
							}					
							switch(A2_DiagData[127])
							{
								case 0x10 + 0x80 : 
									i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_VendorFn_ADD, A2_VendorFn_SIZE);
									break;					
#ifdef CDR_GN2146						
								case 0x11 + 0x80 :
									i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_GN2146Sync_ADD, A2_GN2146Sync_SIZE);
									break;
#endif
								case 0x13 + 0x80 :
									i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_MODLUTAB_ADD, A2_MODLUTAB_SIZE);
									break;					
								case 0x14 + 0x80 :
									i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_TECLUTAB_ADD, A2_TECLUTAB_SIZE);
									break;	
								case 0x15 + 0x80 :
									i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_APDLUTAB_ADD, A2_APDLUTAB_SIZE);
									break;
								case 0x30 + 0x80 :
									i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_TTOSA_MEMORY_ADD, A2_TTOSA_MEMORY_SIZE);
									break;
								case 0x40 + 0x80 :
								  	i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_8690_LIMIT_ADD, A2_8690_LIMIT_SIZE);
									  break;
								case 0x80 :
								  	i2cMemcpy((unsigned char*)A2_UpperMEM_ADD, (unsigned char*)A2_UART_MEMORY_ADD, A2_UART_MEMORY_SIZE); 
									break;

								default : 
									break;						
							}
						}							
					}
				
					*sram_addr_ptr++;
					addr++;        
					if(addr >= 128) {
						// addr = 0;
						sram_addr_ptr = (unsigned char*)A2_UpperMEM_ADD;
					}

//					if(flashwrite) FLASH_Update(A2_DiagData_ADD, A2_DiagData_FLASH, A2_DiagData_SIZE);			
					if(flashwrite) {
						flashFlagByte2 |= A2_DiagDataWrFlag;
					}
				}	
			} else {
				addr = twsiSlaveBuf.address;
				num_bytes = twsiSlaveBuf.numOfEntries;
				sram_addr_ptr = (unsigned char*)A2_UpperMEM_ADD;                   //set ram address
				sram_addr_ptr += (addr - 128);			
				
				if((A2_DiagData[127] > 0x02) && (A2_DiagData[PROTECTION_ADD] & 0x01)) {
					return;			
				}
				
				if((A2_DiagData[127] == 0x02) && (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x80)) {	//smart tune 도중 채널 파장 변경 불가 
					if((addr == SFF_8690_A2_02_CHANNEL + 0x80) || (addr == SFF_8690_A2_02_CHANNEL_LSB + 0x80) || 
					   (addr == SFF_8690_A2_02_WAVELENGTH + 0x80) || (addr == SFF_8690_A2_02_WAVELENGTH + 1 + 0x80)) {
						return;
					}
				}
				
                if((A2_DiagData[127] == 0x02) && (A2_DiagData[PROTECTION_ADD]&0x01)) {
                    if(addr == (TUNING_LATCHED_STATUS + 0x80)) return;
                }
                
				for(i = 0; i < num_bytes; i++)
				{
					if((A2_DiagData[127] == 0x02) && (A2_DiagData[PROTECTION_ADD]&0x01)) {
						if((addr == (SFF_8690_CURRENT_STATUS_VEN + 0x80)) || (addr == (SFF_8690_LATCHED_STATUS_VEN + 0x80)) 
						   || (addr == (TX_MC_VEN + 0x80)) || (addr == (TX_YC_VEN + 0x80)) || (addr == (RX_MC_VEN + 0x80)) 
							   || (addr == (RX_YC_VEN + 0x80)) || (addr == (TX_MAX_CH_VEN + 0x80))) {
							;   
						} else {
							*sram_addr_ptr = I2C_Rx_Buffer[i];
							*sram_addr_ptr++;
							addr++;
							if(addr >=256) {
								addr = 0;
								sram_addr_ptr = (unsigned char*)A2_DiagData_ADD;
							}						   
						}
					} else {
						*sram_addr_ptr = I2C_Rx_Buffer[i];
						*sram_addr_ptr++;
						addr++;
						if(addr >=256) {
							addr = 0;
							sram_addr_ptr = (unsigned char*)A2_DiagData_ADD;
						}
					}
				}
				switch(A2_DiagData[127])			
				{
					case 0x00 :
					case 0x01 :			
#ifdef SAMSUNG						
						i2cMemcpy((unsigned char*)A2_UserData_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_UserData_SIZE - 8);

						flashFlagByte1 |= A2_UserDataWrFlag;
#else
						i2cMemcpy((unsigned char*)A2_UserData_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_UserData_SIZE);
						if(addr == (AMCC_LINK_STATUS_ADDRESS+1)){
								;
						}else{  
						  flashFlagByte1 |= A2_UserDataWrFlag;
						}		
#endif	
#ifdef ADDON_JMA_WIRELESS
                          if(addr == SFF_8690_A2_00_CHANNEL + 1){
                            sendMessage(getMessageQueue(), EVENT_SFF_8690_CHANNEL, 0);
                          }
#endif
						break;
						
					case 0x10 + 0x80 : 
						i2cMemcpy((unsigned char*)A2_VendorFn_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_VendorFn_SIZE);
						i2cSlaveWriteFlag = ON;	/* Not to update lggRegMap for a while */
   						flashFlagByte1 |= A2_VendorFnWrFlag;
						break;
#ifdef CDR_GN2146		
					case 0x11 + 0x80 :
						i2cMemcpy((unsigned char*)A2_GN2146Sync_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_GN2146Sync_SIZE);
						cdrSyncFlag = _ON;
						flashFlagByte1 |= A2_GN2146SyncWrFlag;
						break;
#endif		
					case 0x13 + 0x80 :
						i2cMemcpy((unsigned char*)A2_MODLUTAB_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_MODLUTAB_SIZE);
						flashFlagByte1 |= A2_MODLUTABWrFlag;
						break;					
					case 0x14 + 0x80 :
						i2cMemcpy((unsigned char*)A2_TECLUTAB_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_TECLUTAB_SIZE);
						tecTableFlag = _ON;
						flashFlagByte1 |= A2_TECLUTABWrFlag;
						break;
					case 0x15 + 0x80 :
						i2cMemcpy((unsigned char*)A2_APDLUTAB_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_APDLUTAB_SIZE);
						apdTableFlag = _ON;
						flashFlagByte1 |= A2_APDLUTABWrFlag;
						break;
					case 0x02 :
						i2cMemcpy((unsigned char*)A2_TTOSA_PAGE_0x02_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_TTOSA_PAGE_0x02_SIZE);             
						processTable0x02(twsiSlaveBuf.address, twsiSlaveBuf.numOfEntries);		
						break;
					case 0x30 + 0x80 :
						i2cMemcpy((unsigned char*)A2_TTOSA_MEMORY_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_TTOSA_MEMORY_SIZE);
						processTable0x30(twsiSlaveBuf.address, twsiSlaveBuf.numOfEntries);
						break;
					case 0x40 + 0x80 :
					  	i2cMemcpy((unsigned char*)A2_8690_LIMIT_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_8690_LIMIT_SIZE);
					  	//processTable0x02(0x8c, 1);
						flashFlagByte2 |= A2_8690_LIMITWrFlag;
                        break;
					case 0x70 + 0x80 :
						i2cMemcpy((unsigned char*)A2_DEBUGTAB_ADD, (unsigned char*)A2_UpperMEM_ADD, A2_DEBUGTAB_SIZE);
						flashFlagByte2 |= A2_DEBUGTABWrFlag;
						break;				
				}				
			}
		}
//		twsiSlaveBuf.numOfEntries = 0;												/* clear */
//		twsiSlaveBuf.bufferStatus = MEMORY_WRITE_BUFFER_IDLE;						/* set as idle */
//		I2CCN_S_bit.I2CACK = 0;    		
	}
}


//-----------------------------------------------------------------------------
// I2C Slave Stop function
//  Sets up received data to be processed
//-----------------------------------------------------------------------------
static void I2C_Slave_Stop(void)
{
	PO1_bit.PO16 ^= 1;
	if( (twsiSlaveBuf.devAddress == SLAVE_ADDRESS_A2) || (twsiSlaveBuf.devAddress == SLAVE_ADDRESS_A0) )  	//if slave address is correct
	{    
		ReadI2CRxBuffer(RX_COUNT_ADD,I2C_RX_BUF_ADD);                            	//Read from FIFO
		//this limits to 8 bytes being copied to writeBufferEntry
		if( (twsiSlaveBuf.numOfEntries < MEMORY_WRITE_BUFFER_SIZE) && (Rx_Count <= MEMORY_WRITE_BUFFER_SIZE) )
		{ 			 
			for(unsigned char i = 0; i < Rx_Count; i++)                           	//for each byte
			{				
				twsiSlaveBuf.writeBufferEntry[twsiSlaveBuf.numOfEntries] = I2C_Rx_Buffer[i];  //copy
				twsiSlaveBuf.numOfEntries++;                                        //increment count
			}
		}

		if(twsiSlaveBuf.numOfEntries)                                           	//Anything to be written
		{
			twsiSlaveBuf.bufferStatus = MEMORY_WRITE_BUFFER_WAITING_TO_PROCESS;   	//set flag for processing
			
			I2CCN_S_bit.I2CACK = 1;                                               	//NACK all trafic while processing			
			ProcessReceivedI2CData();		
			twsiSlaveBuf.numOfEntries = 0;                                              //clear
			twsiSlaveBuf.bufferStatus = MEMORY_WRITE_BUFFER_IDLE;                       //set as idle
			I2CCN_S_bit.I2CACK = 0;   			
			Rx_Count = 0;			
		}   
	}

}


//-----------------------------------------------------------------------------
// I2C Slave Isr function
//   - i2c slave interrupt service routine
//-----------------------------------------------------------------------------
extern unsigned short task3;
#pragma vector = 2
__interrupt void I2CS_Interrupt(void)
{
//	PO1_bit.PO16 ^= 1;
	MACSEL_bit.MACRSEL = 1;												//switch to user MAC
	// STO
	if(I2CST2_S_bit.I2CSPI)                                          	 //Stop Interrupt flag
	{   
		I2CST2_S_bit.I2CSPI = 0;                                        //Clear stop interrupt flag
		if(I2CCN_S_bit.I2CMODE == 0)                                    //if write mode
		{   // STA / A0 / 00 / STO			
           
			I2C_Slave_Stop();                                           //Read Buffer and execute  
			I2CRXFST_bit.THSH = 0;                                      //Clear threshold flag as data has been read
		} 
        
        task3 = 0;
//		restartTimer(PROCESS_100ms_TIMER_ID);	/* jhroh */
                   
		MPNTR = 0;                                                      //Dummy write, this will update current address update
		Mem_Address = MPNTR;                                            //Read Memory address including SRAM memory map
		Mem_Address &=0x00FF;  
				
		if(A2_DiagData[TABLE_SEL_Add] == 0x02) {   	  
		   if((Mem_Address - 1 >= SFF_8690_LATCHED_STATUS_ADDRESS) && (memoryStartBuffer <= SFF_8690_LATCHED_STATUS_ADDRESS) && (I2CCN_S_bit.I2CMODE != 0)) {
			  A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] = 0x00;  // SFF-8690 Latched status clear 
			  A2_UserData[SFF_8690_LATCHED_STATUS_VEN] &= ~0x80;	//Samsung req
		   }
		}	
		
		if((A2_DiagData[TABLE_SEL_Add] == 0x00) || (A2_DiagData[TABLE_SEL_Add] == 0x01)) {
			if((Mem_Address - 1 >= SFF_8690_LATCHED_STATUS_VEN + 0x80) && (memoryStartBuffer <= SFF_8690_LATCHED_STATUS_VEN + 0x80) && (I2CCN_S_bit.I2CMODE != 0)) {
				A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_LATCHED_STATUS] &= ~0x80;
				A2_UserData[SFF_8690_LATCHED_STATUS_VEN] &= ~0x80;
			}
		}				
	}
        
	// STA  or RSTA
	if(I2CST2_S_bit.SADI)												//Interrupt after Start and Address clock started
	{   			
		I2CST2_S_bit.SADI = 0;                                          //Clear Start and Address clock started flag   
		UpdateAllTxBuffers();                                      		//Assembly function call-Update all TXPAGE with current address and be ready for transmission if requested  
		Rx_Count = 0;                                                   //Set rx_count = 0    
                if(A2_DEBUGTAB[InrushDone_Add] == 0x01){
                  i2cSlaveReadFlag = ON;
                  i2cSlaveReadFlagCount = 0;
                }
	}
  
	// STA  / A0 / 
	// RSTA / A1 / 
	if(I2CST_S_bit.I2CAMI)                                            	//Interrupt after Address matching 
	{  
		I2CST_S_bit.I2CAMI = 0;                                         //Clear Address matched interrupt flag
	   
		switch(CUR_SLA & 0x000F)                                        //get slave address  
		{
		case 1: 
			twsiSlaveBuf.devAddress = SLAVE_ADDRESS_A2;
		break;
				
		case 2:
			twsiSlaveBuf.devAddress = SLAVE_ADDRESS_A0;
		break;
				
		case 4: 
			twsiSlaveBuf.devAddress = SLAVE_ADDRESS_B2;
		break;
				
		case 8: 
			twsiSlaveBuf.devAddress = SLAVE_ADDRESS_B0;
		break;
		
		default:
		break;
		}
		Rx_Count = 0;                                                   //Set rx_count = 0
	} 
	// STA / A0 / 00 /
	else if(I2CST2_S_bit.MADI)                                        	//memory address detected
	{   		     
		I2CST2_S_bit.MADI = 0;                                          //Clear Memory Address interrupt flag 		
		MPNTR = 0;                                                      //Dummy write, this will update current address update
		Mem_Address = MPNTR;                                            //Read Memory address including SRAM memory map
		Mem_Address &=0x00FF;                                           //Clear page info 
		twsiSlaveBuf.address = (unsigned char)Mem_Address;
        memoryStartBuffer = (unsigned char)Mem_Address;

		twsiSlaveBuf.numOfEntries = 0; 
		Rx_Count = 0;                                                   //Set rx_count = 0
		ActiveSlave = CUR_SLA & 0x000F;                                 //Findout Active Slave Address
		if(ActiveSlave == 1)
		{	// STA / A2 / 00, ready to master read to next
			CUR_SLA = 0xF1;
			UpdateTxBuffer();                                        //Update TX Page for A2 Slave Address  
		}
		else if(ActiveSlave == 2)
		{	// STA / A0 / 00, ready to master read to next
			CUR_SLA = 0xF2;          
			UpdateTxBuffer();                                        //Update TX Page for A0 Slave Address
		}           
		else if(ActiveSlave == 4)
		{
			CUR_SLA = 0xF4;          
			UpdateTxBuffer();                                        //Update TX Page for A0 Slave Address
		}           
		else if(ActiveSlave == 8)
		{
			CUR_SLA = 0xF8;                    
			UpdateTxBuffer();                                        //Update TX Page for A0 Slave Address
		}           
	}
	
	else if(I2CRXFST_bit.THSH)                                        	//receive FIFO has received 4 bytes
	{   // up to 4byte, master siquence write	
		I2CRXFST_bit.THSH = 0;                                          //Clear Threshold flag 
		ReadI2CRxBuffer(RX_COUNT_ADD,I2C_RX_BUF_ADD);                   //Read data from receive buffer    
	}  
  
	else if(I2CTXFST_bit.THSH)                                        //TX Page Threshold Interrupt, 4 bytes remaining
	{	// up to 4byte, master siquence read
		
		I2CTXFST_bit.THSH = 0;                                          //clear flag
		ActiveSlave = CUR_SLA & 0x000F;                                 //Findout Active Slave Address

		if(ActiveSlave == 1)
		{  			
			// CUR_SLA = 0xF1;
			CUR_SLA = 0x11;
			UpdateTxBuffer();                                        //Update TX Page for A2 Slave Address  
		}
		else if(ActiveSlave == 2)
		{
			CUR_SLA = 0xF2;          
			UpdateTxBuffer();                                        //Update TX Page for A0 Slave Address
		}           
		else if(ActiveSlave == 4)
		{
			CUR_SLA = 0xF4;          
			UpdateTxBuffer();                                        //Update TX Page for A0 Slave Address
		}           
		else if(ActiveSlave == 8)
		{
			CUR_SLA = 0xF8;                    
			UpdateTxBuffer();                                        //Update TX Page for A0 Slave Address
		}
		twsiSlaveBuf.address += 8;                                      //add 8 to the address
	}   
	
	MACSEL_bit.MACRSEL = 0;                                           //change back to normal MAC
}


//-----------------------------------------------------------------------------
// end of file
//-----------------------------------------------------------------------------



//End

