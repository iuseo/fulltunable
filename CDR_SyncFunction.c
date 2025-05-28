//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <iods4835.h>
#include <intrinsics.h>
#include <string.h>
#include "main.h"
#include "includes.h"

#ifdef CDR_GN2146
#include "GN2146_Add.h"
#include "GN2146_Sync_Add.h"
#include "appGpio.h"
#include "appUart.h"
/*
 * Name         : resetGN2146
 * Description  : 
 * Return Value : void
 * Parameter    : uint8*, uint8*
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 26 2019
 * History      : 
 */

void resetGN2146(uint8* vendorFn)
{
	uint8 i = 0;
	uint8 data = 0x00;
                  
	timerDelay(50);
       
	GN2146_RESET = _OFF;
	timerDelay(1);

	GN2146_RESET = _ON;
	timerDelay(1);
        
	*(vendorFn + DeviceRST_Add) = 0x00;

	for(i = 0; i < 128; i++)
	{
		I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_BB_STEP_REG_0, &data, 1);
		
		if(data != 0x46) {
			swDelay(1);
		} else {
			*(vendorFn + DeviceRST_Add) |= 0x01;
			break;
		}		
	}	
             
	data = 0x02;
	I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, TX_LD_SWING_SEL_REG_0, &data, 1);	/* TX LD MAIN SWING OVERRIDE EN */
        
}

/*
 * Name         : initializeMemoryGN2146
 * Description  : 
 * Return Value : void
 * Parameter    : uint8*
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 26 2019
 * History      : 
 */
void initializeMemoryGN2146(uint8* syncMemory)
{
	uint8 i = 0;
	
	if(*(syncMemory + GN2146FAC_Add) != 0x0A) {
		for(i = 0; i < GN2146SyncCnt; i++)
		{
			*(syncMemory + i) = GN2146SyncDefault[i];
		}
        
		*(syncMemory + TX_LD_SWING_SEL_REG_0_Add) = 0x02;
		*(syncMemory + GN2146FAC_Add) = 0x0A;	
#ifdef USE_EXT_EEPROM
        eepromWriteFunction(A2_GN2146Sync,A2_GN2146Sync_EEPROM);
#else
        FLASH_Update(A2_GN2146Sync_ADD, A2_GN2146Sync_FLASH, A2_GN2146Sync_SIZE);	
#endif
	}
   
	cdrSyncFlag = _ON;
}

/*
 * Name         : syncGN2146
 * Description  : 
 * Return Value : void
 * Parameter    : uint8*
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 26 2019
 * History      : 
 */
void syncGN2146(unsigned char* syncMemory)
{
	uint8 i = 0, j = 0;
	uint8 temp, RDOnlyVal, WROnlyVal;

        
	cdrSyncFlag |= (A2_VendorFn[SYNC_FLAG_Add] & 0x01);
	if(cdrSyncFlag != _OFF) {
	A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 1;
	for(i = 0; i < GN2146SyncCnt; i++)
	{	  
	  	
		I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, GN2146Sync[i], &temp, 1);					
                        
		if(GN2146Sync[i] != GN2146ReadOnly[j]) {
			RDOnlyVal = GN2146ReadOnlybit[i] & temp;
			WROnlyVal = *(syncMemory + i) & ~GN2146ReadOnlybit[i];
			*(syncMemory + i) = RDOnlyVal | WROnlyVal;
			if(*(syncMemory + i) != temp) {
				I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, GN2146Sync[i], (syncMemory + i), 1);                           
				}
			}else {
				*(syncMemory + i) = temp;
				if(j < GN2146ROCnt) {
						j++;
				}
			}
		}            
		A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
	}
	cdrSyncFlag = _OFF;
	A2_VendorFn[SYNC_FLAG_Add] &= ~0x01;
}

/*
 * Name         : manualGN2146
 * Description  : 
 * Return Value : void
 * Parameter    : uint8*
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : April 26 2019
 * History      : 
 */
void manualGN2146(uint8* vendorFn)
{
	uint16 manualAddress;
        
	manualAddress = Reg2ByteToWordLoad((vendorFn + MANADDRESS_Add));
	if(*(vendorFn + MANCTRL_Add) & 0x01) {
		if(*(vendorFn+MANI2C_Add) & 0x80) {		/* Read */
	  		I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, manualAddress, vendorFn + MANREAD_Add, 1);						
			//I2CM_16ReadNBytes(I2CM_MASTER_1, 0x54, manualAddress, vendorFn + MANREAD_Add, 1);						
		  	//eepromReadFunction(A2_FAR_END_02_0x24,0x2800);
                       
		} else {          		                /* Write */
		 	I2CM_16WriteNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, manualAddress, vendorFn + MANWRITE_Add, 1);						
		  	//I2CM_16WriteNBytes(I2CM_MASTER_1, 0x54, manualAddress, vendorFn + MANWRITE_Add, 1);						
		  	//eepromWriteFunction(A2_MODLUTAB,0x2800);
			
		}
	}
	*(vendorFn + MANCTRL_Add) = 0x00; 	
}

#endif

