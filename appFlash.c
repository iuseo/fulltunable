/*
 * FILE NAME            : appFlash.c
 * DATE                 : March 21 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 21 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <intrinsics.h>
#include <iods4835.h>
#include "main.h"
#include "board.h"
#include "MemoryAccess.h"
#include "appWrapper.h"
#include "appGpio.h"
#include "flash.h"
#include "i2cm.h"


extern uint8 flashFlagByte1;
extern uint8 flashFlagByte2;
extern uint8 ttosaChannel;
extern void saveTosaChannelNumber(void);

/*
 * Name         : flashRegRead
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 25 2019
 * History      : 
 */
#if 0
void flashRegRead(void)
{
	FLASH_Read(A2_UserData_FLASH, A2_UserData_ADD, A2_UserData_SIZE);
	FLASH_Read(A2_VendorFn_FLASH, A2_VendorFn_ADD, A2_VendorFn_SIZE);
	FLASH_Read(A2_GN2146Sync_FLASH, A2_GN2146Sync_ADD, A2_GN2146Sync_SIZE);
	FLASH_Read(A2_APCLUTAB_FLASH, A2_APCLUTAB_ADD, A2_APCLUTAB_SIZE);
	FLASH_Read(A2_MODLUTAB_FLASH, A2_MODLUTAB_ADD, A2_MODLUTAB_SIZE);
	FLASH_Read(A2_TECLUTAB_FLASH, A2_TECLUTAB_ADD, A2_TECLUTAB_SIZE);
	FLASH_Read(A2_APDLUTAB_FLASH, A2_APDLUTAB_ADD, A2_APDLUTAB_SIZE);
	FLASH_Read(A0_SerialID_FLASH, A0_SerialID_ADD, A0_SerialID_SIZE);
	FLASH_Read(A0_Reserved_FLASH, A0_Reserved_ADD, A0_Reserved_SIZE);
	FLASH_Read(A2_DiagData_FLASH, A2_DiagData_ADD, A2_DiagData_SIZE);
	FLASH_Read(A2_DEBUGTEAB_FLASH, A2_DEBUGTAB_ADD, A2_DEBUGTAB_SIZE);    
}
#endif
/*
 * Name         : flashAllRegWrite
 * Description  : Scheduled every 100ms in process100msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : March 27 2019
 * History      : 
 */
#if 0
void flashAllRegWrite(void)
{
	uint8 i = 0, j = 0x00;

	for(i = 0; i < 8; i++)
	{
		j = (0x01 << i);
		if(flashFlagByte1 & j) {
			
			flashFlagByte1 &= ~j;	/* flashFlagByte1 can be updated by I2C ISR during updating the flash memory */
			switch(j)
			{
				case A2_UserDataWrFlag :
					flashUpdate(A2_UserData_ADD, A2_UserData_FLASH, A2_UserData_SIZE);
					break;
				case A2_VendorFnWrFlag :
					flashUpdate(A2_VendorFn_ADD, A2_VendorFn_FLASH, A2_VendorFn_SIZE);
					break;
				case A2_GN2146SyncWrFlag :
					flashUpdate(A2_GN2146Sync_ADD, A2_GN2146Sync_FLASH, A2_GN2146Sync_SIZE);
					break;
				case A2_APCLUTABWrFlag :
					flashUpdate(A2_APCLUTAB_ADD, A2_APCLUTAB_FLASH, A2_APCLUTAB_SIZE);
					break;
				case A2_MODLUTABWrFlag :
					flashUpdate(A2_MODLUTAB_ADD, A2_MODLUTAB_FLASH, A2_MODLUTAB_SIZE);
					break;
				case A2_TECLUTABWrFlag :
					flashUpdate(A2_TECLUTAB_ADD, A2_TECLUTAB_FLASH, A2_TECLUTAB_SIZE);
					break;
				case A2_APDLUTABWrFlag :
					flashUpdate(A2_APDLUTAB_ADD, A2_APDLUTAB_FLASH, A2_APDLUTAB_SIZE);
					break;
				default :
					break;
			}
//			flashFlagByte1 &= ~j;
		}
	}

       
	for(i = 0; i < 7; i++)
	{
		j = (0x01 << i);
		if(flashFlagByte2 & j) {        /* flashFlagByte2 can be updated by I2C ISR during updating the flash memory */
			
			flashFlagByte2 &= ~j;	
			switch(j)
			{
				case A0_SerialIDWrFlag :
					flashUpdate(A0_SerialID_ADD, A0_SerialID_FLASH, A0_SerialID_SIZE);
					break;
				case A0_ReservedWrFlag :
					flashUpdate(A0_Reserved_ADD, A0_Reserved_FLASH, A0_Reserved_SIZE);
					break;
				case A2_DiagDataWrFlag :
					flashUpdate(A2_DiagData_ADD, A2_DiagData_FLASH, A2_DiagData_SIZE);
					break;
				case A2_DEBUGTABWrFlag :
					flashUpdate(A2_DEBUGTAB_ADD, A2_DEBUGTEAB_FLASH, A2_DEBUGTAB_SIZE);
					break;
				case A2_TTOSAMemoryWrFlag :
					flashUpdate(A2_TTOSA_MEMORY_ADD, A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (ttosaChannel - 1), A2_TTOSA_MEMORY_SIZE);
					break;
				case TTOSAChannelWrFlag :
					saveTosaChannelNumber();                           
					break;
				default :
					break;
			}
//			flashFlagByte2 &= ~j;
		}
	}
        
}

#endif
