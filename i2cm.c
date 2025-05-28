/*******************************************************************************
* Copyright (C) 2017 Maxim Integrated Products, Inc., All rights Reserved.
* 
* Maxim Integrated Company Confidential
*
* * This software is protected by copyright laws of the United States and
* of foreign countries. This material may also be protected by patent laws
* and technology transfer regulations of the United States and of foreign
* countries. This software is furnished under a license agreement and/or a
* nondisclosure agreement and may only be used or reproduced in accordance
* with the terms of those agreements. Dissemination of this information to
* any party or parties not specified in the license agreement and/or
* nondisclosure agreement is expressly prohibited.
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
#include <stdio.h>
#include <string.h>
#include "i2cm.h"
#include "main.h"
#include "max5112.h"


unsigned char tx_rx_complete;
unsigned char interrupt_count = 0;

//extern uint8 gn2146DisFlag;

static void I2CM_Reset(unsigned char master);

/*******************************************************************************
 * Function: I2CM_init
 *
 * Initialize I2C master interface
 *
 * Input:   unsigned char Master - I2C master select from i2cm.h
 *          unsigned int Clock_Rate - clock rate code from master clock
 *                                    divider settings in i2cm.h
 *          unsigned char Pin       - master data MSDA pin select from ic2m.h
 *                                    ONLY VALID for master 1
 *
 * Output:    None
 *
 * Example:   I2CM_init(TBDTBD);
 *
 * Remarks:   No validation performed on any parameters
 *
 ******************************************************************************/ 
void I2CM_Init (unsigned char master, unsigned int clock_rate,
                unsigned char pin, unsigned char use_ints)
{
	unsigned short buf;
       
	if (master == 0)                      // master 1
	{
        // set up GPIOs
        PD1_bit.PD11 = 1;                 // set clk gpio as output
        switch (pin) 
        {         
            case I2CM_MSDA_PIN_DEFAULT:
            case I2CM_MSDA_PIN_GP10:
              PD1_bit.PD10 = 0;
              IEN1_bit.IEN10 = 1;
              break;
#if 0			  
            case I2CM_MSDA_PIN_GP12:
              PD1_bit.PD12 = 1;
              IEN1_bit.IEN12 = 0;
              break;
            case I2CM_MSDA_PIN_GP13:
              PD1_bit.PD13 = 1;
              IEN1_bit.IEN13 = 0;
              break;             
            case I2CM_MSDA_PIN_GP14:
              PD1_bit.PD14 = 0; 
              IEN1_bit.IEN14 = 1;
              break;             
#endif			  
            default: ;
        }
        CNT2W1 = 0x8000;                     // enable
        CNT2W1_bit.TWI = 0;               // clear any status
        if (use_ints)
        {
            CNT2W1_bit.TWI2WIE = 1;        // enable ints if req.
        }
        CCK2W1 = clock_rate;              // set clock rate
        MIS2 = pin;                       // map to primary or alt MSDA pins
        CNT2W1_bit.TW2WEN = 1;            // enable master I2C   
		
		buf = MTSPEC1;
		buf &= ~0xFC00;
		MTSPEC1 = buf|0xC000;

    }
#if 0
    else                                  // master 2
    {
        CNT2W2 = 0x8000;
        CNT2W2_bit.TWI = 0;               // clear any status
        if (use_ints)
        {
            CNT2W2_bit.TWI2WIE = 1;        // enable ints if req.
        }
        PD0_bit.PD03 = 1;                 // set clk gpio as output
        PD0_bit.PD02 = 1;                 // set data gpio as output
        CCK2W2 = clock_rate;              // set clock rate
        CNT2W2_bit.TW2WEN = 1;            // enable master I2C 
    }
#endif
}


/*******************************************************************************
 * Function: I2CM_Disable
 *
 * Initialize I2C master interface
 *
 * Input:   unsigned char Master - I2C master select from i2cm.h
 *          unsigned char Pin       - master data MSDA pin select from ic2m.h
 *                                    ONLY VALID for master 1
 *
 * Output:    None
 *
 * Example:   
 *
 * Remarks:   No validation performed on any parameters
 *
 ******************************************************************************/ 
void I2CM_Disable (unsigned char master, unsigned char pin)
{
	if (master == 0)                      // master 1
	{
		CNT2W1 = 0x00;                    // disable and clear all status
		PD1_bit.PD11 = 0;                 // set clk gpio to default
		switch (pin) 
		{         
		case I2CM_MSDA_PIN_DEFAULT:
		case I2CM_MSDA_PIN_GP10:
			PD1_bit.PD10 = 0; 
		break;
		case I2CM_MSDA_PIN_GP12:
			PD1_bit.PD12 = 0; 
		break;
		case I2CM_MSDA_PIN_GP13:
			PD1_bit.PD13 = 0; 
		break;             
		case I2CM_MSDA_PIN_GP14:
			PD1_bit.PD14 = 0; 
		break;             
		default: ;
		}       
	}
	else                                  // master 2
	{     
		CNT2W2 = 0x00;                    // disable and clear all status
		PD0_bit.PD03 = 0;                 // set clk gpio to default
		PD0_bit.PD02 = 0;                 // set data gpio to default
	}  
}


/*******************************************************************************
 * Function: I2CM_WriteNBytes
 *
 * Write 1..8 bytes to I2C slave at specified location
 *
 * Input:     unsigned char master - I2C master select from i2cm.h
 *            unsigned char slave_addr - slave address, 7 bits, not shifted left
 *            unsigned char reg_addr - register address for write
 *            const unsigned char *data_ptr - pointer to data array to write
 *            unsigned char num_bytes - number of bytes to write, max 8
 *
 * Output:    return_code - 0x00: no errors
*                           0x01: fail on slave address transfer
*                           0x02: fail on mem address transfer
*                           0x03: fail on data byte transfer in write cycle
*                           0xFF: timeout
 *
 * Example:   return = I2CM_WriteNBytes(TBDTBD);
 *
 * Remarks:   No validation performed on any parameters
 *
 ******************************************************************************/
unsigned char I2CM_WriteNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned char reg_addr,
                               const unsigned char *data,
                               unsigned char num_bytes)

{
//  if((master == I2CM_MASTER_1) && (slave_addr == 0x56)) {  //GN2146_DEVICE_ADDRESS
//    gn2146DisFlag = ON;
//  }
	unsigned char i;
	unsigned char return_code = I2CM_WRITE_STATUS_OK;
	unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;
	
	if (num_bytes > 8)                        // clamp bytes written to max 8
	{
		num_bytes = 8; 
	}
	if (num_bytes == 0)                       // clamp bytes written min 1
	{
		num_bytes = 1; 
	}
  
	tx_rx_complete = 0;
	if (master == 0)                          // master 1
	{
		CNT2W2_bit.XFR_FAIL = 0;            // clear all status
		CNT2W2_bit.FAIL_STATUS = 0; 		
		
		CNT2W1_bit.ADSEL2W = 1;             // point DAD2W to MEM_ADDR
		DAD2W1 = reg_addr;                    // set slave reg addr	
		CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
		
		CNT2W1_bit.WMA = 1;                 // set up for dummy index reg write
		CNT2W1_bit.MADR_2W = 0;             // start from first buffer
		CNT2W1_bit.NB_2W = num_bytes - 1;     // number of bytes to write
		for (i = 0; i < num_bytes; i++)
		{
			// DAD2W1 = data[i];                 // copy data into tx FIFO        
			DAD2W1 = *(data);                 // copy data into tx FIFO       
			data++;
		}   			
		CNT2W1_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		
		SLA2W1 = (slave_addr << 1);           // set slave addr/initiate TX
     
      // while (((CNT2W1_bit.TWI2WIE == 0) && (CNT2W1_bit.TWI == 0)) ||
             // ((CNT2W1_bit.TWI2WIE == 1) && (tx_rx_complete == 0))) // wait for complete
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
			break;
			}    
		}
      
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1); 
		}
                     
		CNT2W1 &= 0x8800;                     // clear all status 
       
	}
#if 0
	else                                      // master 2
	{
		CNT2W2_bit.ADSEL2W = 1;               // point DAD2W to MEM_ADDR
		DAD2W2 = reg_addr;                    // set slave reg addr
		CNT2W2_bit.ADSEL2W = 0;               // point DAD2W to data buffers

		CNT2W2_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W2_bit.MADR_2W = 0;               // use first buffer      
		CNT2W2_bit.NB_2W = num_bytes - 1;     // number of bytes to write		
		for (i = 0; i < num_bytes; i++)
		{
			// DAD2W2 = data[i];                 // copy data into tx FIFO        
			DAD2W2 = *(data);                 // copy data into tx FIFO       
			data++;			
		}     
		CNT2W2_bit.TWI = 0;                   // clear complete flag
		CNT2W2_bit.EWS = 1;                   // set to xmit stop bit
		SLA2W2 = (slave_addr << 1);           // set slave addr/initiate TX  TBD need to define bitfield for this
 
	 // while (((CNT2W2_bit.TWI2WIE == 0) && (CNT2W2_bit.TWI == 0)) ||
             // ((CNT2W2_bit.TWI2WIE == 1) && (tx_rx_complete == 0))) // wait for completeut
		while(!(CNT2W2_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}

		if (CNT2W2_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W2_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_2); 
		}
                     
		CNT2W2 &= 0x8800;                     // clear all status 
	}
#endif
//	gn2146DisFlag = OFF;
	return (return_code);
}


unsigned char I2CM_16WriteNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned short reg_addr,
                               const unsigned char *data,
                               unsigned char num_bytes)
{
//  if((master == I2CM_MASTER_1) && (slave_addr == 0x56)) {  //GN2146_DEVICE_ADDRESS
//    gn2146DisFlag = ON;
//  }
	unsigned char i, reg_addr_8bit[2];
	unsigned char return_code = I2CM_WRITE_STATUS_OK;
	unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;
	
	
	reg_addr_8bit[0] = ((reg_addr >> 8) & 0xFF); // High Address
	reg_addr_8bit[1] =  (reg_addr & 0xFF);		// Low Address
	
	if (num_bytes > 8)                        // clamp bytes written to max 8
	{
		num_bytes = 8; 
	}
	if (num_bytes == 0)                       // clamp bytes written min 1
	{
		num_bytes = 1; 
	}
  
	tx_rx_complete = 0;
	if (master == 0)                          // master 1
	{
		CNT2W1_bit.ADSEL2W = 1;               // point DAD2W to MEM_ADDR
		// DAD2W1 = reg_addr;                    // set slave reg addr
		DAD2W1 = reg_addr_8bit[0];             // set slave reg addr
		CNT2W1_bit.ADSEL2W = 0;               // point DAD2W to data buffers
		
		CNT2W1_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W1_bit.MADR_2W = 0;               // use first buffer 		
		// CNT2W1_bit.NB_2W = num_bytes - 1;     // number of bytes to write
		CNT2W1_bit.NB_2W = num_bytes;     // number of bytes to write
		DAD2W1 = reg_addr_8bit[1];                    // set slave reg addr
		for (i = 0; i < num_bytes; i++)
		{
			// DAD2W1 = data[i];                 // copy data into tx FIFO        
			DAD2W1 = *(data);                 // copy data into tx FIFO       
			data++;
		}   			
		CNT2W1_bit.EWS = 1;                   // set to xmit stop bit
		CNT2W1_bit.TWI = 0;                   // clear complete flag
		SLA2W1 = (slave_addr << 1);           // set slave addr/initiate TX
      // while (((CNT2W1_bit.TWI2WIE == 0) && (CNT2W1_bit.TWI == 0)) ||
             // ((CNT2W1_bit.TWI2WIE == 1) && (tx_rx_complete == 0))) // wait for complete
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{	
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}  
		}
      
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1); 
		}
                     
	CNT2W1 &= 0x8800;                     // clear all status 

	}
#if 0
	else                                      // master 2
	{
		CNT2W2_bit.ADSEL2W = 1;               // point DAD2W to MEM_ADDR
		DAD2W2 = reg_addr;                    // set slave reg addr
		CNT2W2_bit.ADSEL2W = 0;               // point DAD2W to data buffers

		CNT2W2_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W2_bit.MADR_2W = 0;               // use first buffer      
		CNT2W2_bit.NB_2W = num_bytes - 1;     // number of bytes to write		
		for (i = 0; i < num_bytes; i++)
		{
			// DAD2W2 = data[i];                 // copy data into tx FIFO        
			DAD2W2 = *(data);                 // copy data into tx FIFO       
			data++;			
		}     
		CNT2W2_bit.TWI = 0;                   // clear complete flag
		CNT2W2_bit.EWS = 1;                   // set to xmit stop bit
		SLA2W2 = (slave_addr << 1);           // set slave addr/initiate TX  TBD need to define bitfield for this
 
	 // while (((CNT2W2_bit.TWI2WIE == 0) && (CNT2W2_bit.TWI == 0)) ||
             // ((CNT2W2_bit.TWI2WIE == 1) && (tx_rx_complete == 0))) // wait for completeut
		while(!(CNT2W2_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}

		if (CNT2W2_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W2_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_2); 
		}
                     
		CNT2W2 &= 0x8800;                     // clear all status 
	}
#endif
//    gn2146DisFlag = OFF;
	return (return_code);
}

unsigned char I2CM_32WriteNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned long reg_addr,
                               const unsigned char *data,
                               unsigned char num_bytes)
{
//  if((master == I2CM_MASTER_1) && (slave_addr == 0x56)) {  //GN2146_DEVICE_ADDRESS
//    gn2146DisFlag = ON;
//  }
	unsigned char i, reg_addr_8bit[3];
	unsigned char return_code = I2CM_WRITE_STATUS_OK;
	unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;
	
	reg_addr_8bit[0] = ((reg_addr >> 16) & 0xFF); // Hihg High Address
	reg_addr_8bit[1] =  ((reg_addr >> 8) & 0xFF); // High Address
	reg_addr_8bit[2] =  (reg_addr & 0xFF);		// Low Address
	
	if (num_bytes > 6)                        // clamp bytes written to max 8
	{
		num_bytes = 6; 
	}
	if (num_bytes == 0)                       // clamp bytes written min 1
	{
		num_bytes = 1; 
	}
	tx_rx_complete = 0;
	if (master == 0)                          // master 1
	{
		CNT2W1_bit.ADSEL2W = 1;               // point DAD2W to MEM_ADDR
		// DAD2W1 = reg_addr;                    // set slave reg addr
		DAD2W1 = reg_addr_8bit[0];                    // set slave reg addr
		CNT2W1_bit.ADSEL2W = 0;               // point DAD2W to data buffers
		
		CNT2W1_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W1_bit.MADR_2W = 0;               // use first buffer 		
		CNT2W1_bit.NB_2W = num_bytes+1;     // number of bytes to write
		// CNT2W1_bit.NB_2W = num_bytes;     // number of bytes to write
		DAD2W1 = reg_addr_8bit[1];                    // set slave reg addr
		DAD2W1 = reg_addr_8bit[2];                    // set slave reg addr
		for (i = 0; i < num_bytes; i++)
		{
			// DAD2W1 = data[i];                 // copy data into tx FIFO        
			DAD2W1 = *(data);                 // copy data into tx FIFO       
			data++;
		}   			
		CNT2W1_bit.EWS = 1;                   // set to xmit stop bit
		CNT2W1_bit.TWI = 0;                   // clear complete flag
		SLA2W1 = (slave_addr << 1);           // set slave addr/initiate TX
      // while (((CNT2W1_bit.TWI2WIE == 0) && (CNT2W1_bit.TWI == 0)) ||
             // ((CNT2W1_bit.TWI2WIE == 1) && (tx_rx_complete == 0))) // wait for complete
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
			break;
			}  
		}
      
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1); 
		}
                     
	CNT2W1 &= 0x8800;                     // clear all status 
       
	}
	else                                      // master 2
	{
		CNT2W2_bit.ADSEL2W = 1;               // point DAD2W to MEM_ADDR
		DAD2W2 = reg_addr;                    // set slave reg addr
		CNT2W2_bit.ADSEL2W = 0;               // point DAD2W to data buffers

		CNT2W2_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W2_bit.MADR_2W = 0;               // use first buffer      
		CNT2W2_bit.NB_2W = num_bytes - 1;     // number of bytes to write		
		for (i = 0; i < num_bytes; i++)
		{
			// DAD2W2 = data[i];                 // copy data into tx FIFO        
			DAD2W2 = *(data);                 // copy data into tx FIFO       
			data++;			
		}     
		CNT2W2_bit.TWI = 0;                   // clear complete flag
		CNT2W2_bit.EWS = 1;                   // set to xmit stop bit
		SLA2W2 = (slave_addr << 1);           // set slave addr/initiate TX  TBD need to define bitfield for this
 
	 // while (((CNT2W2_bit.TWI2WIE == 0) && (CNT2W2_bit.TWI == 0)) ||
             // ((CNT2W2_bit.TWI2WIE == 1) && (tx_rx_complete == 0))) // wait for completeut
		while(!(CNT2W2_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}

		if (CNT2W2_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W2_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_2); 
		}
                     
		CNT2W2 &= 0x8800;                     // clear all status 
	}
//    gn2146DisFlag = OFF;
	return (return_code);
}

/*******************************************************************************
 * Function: I2CM_ReadNBytes
 *
 * Read 1..8 bytes to I2C slave from specified location
 *
 * Input:     unsigned char master - I2C master select from i2cm.h
 *            unsigned char slave_addr - slave address, 7 bits, not shifted left
 *            unsigned char reg_addr - register address for read
 *            unsigned char *data_ptr - pointer to data array for read data
 *            unsigned char num_bytes - number of bytes to read, max 8
 *
 * Output:    return_code - 0x00: no errors
*                           0x01: fail on slave address transfer
*                           0x02: fail on mem address transfer
*                           0x03: fail on data byte transfer in write cycle
*                           0xFF: timeout
 *
 * Example:   return = I2CM_ReadNBytes(TBDTBD);
 *
 * Remarks:   No validation performed on any parameters
 *
 ******************************************************************************/
unsigned char I2CM_ReadNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned char reg_addr,
                               unsigned char *data,
                               unsigned char num_bytes)
{
  
//  if((master == I2CM_MASTER_1) && (slave_addr == 0x56)) {  //GN2146_DEVICE_ADDRESS
//    gn2146DisFlag = ON;
//  }
	unsigned char i;
	unsigned char return_code = I2CM_WRITE_STATUS_OK;
	unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;
 	
 
	if (num_bytes > 8)                      // clamp bytes written to max 8
	{
		num_bytes = 8; 
	}
	if (num_bytes == 0)                     // clamp bytes written min 1
	{
		num_bytes = 1; 
	}
   
	tx_rx_complete = 0;
    
	if (master == 0)                        // master 1
	{    
		CNT2W2_bit.XFR_FAIL = 0;            // clear all status
		CNT2W2_bit.FAIL_STATUS = 0; 

		CNT2W1_bit.ADSEL2W = 1;             // point DAD2W to MEM_ADDR
		CNT2W1_bit.WMA = 1;                 // set up for dummy index reg write
		CNT2W1_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		CNT2W1_bit.NB_2W = num_bytes - 1;   // number of bytes to read

		DAD2W1 = reg_addr;                  // set slave reg index

		CNT2W1_bit.MADR_2W = 0;             // start from first buffer
		SLA2W1 = (slave_addr << 1) | 0x01;  // set slave address/initiates rx 
		
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}		
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to data buffers
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
        CNT2W1_bit.MADR_2W = 0;             // start from first buffer

        
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		
		if (!return_code)                      
		{
			for (i = 0; i < num_bytes; i++)
			{
				// data[i] = DAD2W1;             // copy data to caller buff  
				*(data) = DAD2W1;             // copy data to caller buff  
				data++;
			}
		}
		else                                  // err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1);        
		}                      
        CNT2W1_bit.XFR_FAIL = 0;              // clear all status
        CNT2W1_bit.FAIL_STATUS = 0;
    }
    else
    {
		CNT2W2_bit.XFR_FAIL = 0;              // clear all status
		CNT2W2_bit.FAIL_STATUS = 0;
		
		CNT2W2_bit.ADSEL2W = 1;             // point DAD2W to MEM_ADDR
		CNT2W2_bit.WMA = 1;                 // set up for dummy index reg write
		CNT2W2_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W2_bit.TWI = 0;                 // clear complete flag
		CNT2W2_bit.NB_2W = num_bytes - 1;   // number of bytes to read
		
		DAD2W2 = reg_addr;                  // set slave reg index
		
		CNT2W2_bit.MADR_2W = 0;             // start from first buffer
		SLA2W2 = (slave_addr << 1) | 0x01;  // set slave address/initiates rx      
      
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}

        CNT2W2_bit.ADSEL2W = 0;             // point DAD2W to data buffers
		CNT2W2_bit.TWI = 0;                 // clear complete flag
		
        CNT2W2_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
        CNT2W2_bit.MADR_2W = 0;             // start from first buffer

        
		if (CNT2W2_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W2_bit.FAIL_STATUS;          
		}
		
		if (!return_code)                      
		{
			for (i = 0; i < num_bytes; i++)
			{
				// data[i] = DAD2W2;               // copy data to caller buff  
				*(data) = DAD2W2;             // copy data to caller buff  
				data++;
			}
		}
		else                                  // err clean up master and slave
		{
		I2CM_Reset(I2CM_MASTER_2);        
		}                      
		CNT2W2_bit.XFR_FAIL = 0;              // clear all status
		CNT2W2_bit.FAIL_STATUS = 0;    
	}
//    gn2146DisFlag = OFF;
	return (return_code);
}

void Delay_10us(unsigned int DelayCounter)
{   
	unsigned int j, iCounter = 0;
	for(iCounter = 0; iCounter < DelayCounter; iCounter++)
	{
		//Reset Watchdog Timer
		WDCN_bit.RWT = 1; 
		for(j=0;j<10;j++)
		{
			asm("nop");
			asm("nop");
			asm("nop");         
			asm("nop");         
		}
	}  
}
#if 0
void eepromReadFunction(unsigned char* copy_ram,unsigned short addr)
{
  	unsigned char i=0,j=0;
	int value = 0;
	
	for(i=0;i<18;i++){
		max5112StopCondition();
		for(j=0;j<5;j++){
			value = I2CM_16ReadNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS,addr+(EXTERNAL_EEPROM_OFFSET*i),&copy_ram[(EXTERNAL_EEPROM_OFFSET*i)], 3);			
			if(value == 0)
				break;		
		}
		for(j=0;j<5;j++){
			value = I2CM_16ReadNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS,addr+(EXTERNAL_EEPROM_OFFSET*i+3),&copy_ram[(EXTERNAL_EEPROM_OFFSET*i+3)], 4);
			if(value == 0)
				break;
		}
//		I2CM_16ReadNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS,addr+(EXTERNAL_EEPROM_OFFSET*i),&copy_ram[(EXTERNAL_EEPROM_OFFSET*i)], 7);

#if 0
		if(i%3 == 0){
			max5112StopCondition();
		}
#endif		
	}
	max5112StopCondition();
	I2CM_16ReadNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS, addr+(EXTERNAL_EEPROM_OFFSET*18),&copy_ram[(EXTERNAL_EEPROM_OFFSET*18)], 2);
 
}


void eepromWriteFunction(unsigned char* copy_ram, unsigned short addr)
{
	unsigned char i=0;
	for(i=0;i<18;i++){
//		max5112StopCondition();
//		I2CM_16WriteNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS,addr+(EXTERNAL_EEPROM_OFFSET*i),&copy_ram[(EXTERNAL_EEPROM_OFFSET*i)], 3);
//		I2CM_16WriteNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS,addr+(EXTERNAL_EEPROM_OFFSET*i),&copy_ram[(EXTERNAL_EEPROM_OFFSET*i)], 4);
		I2CM_16WriteNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS,addr+(EXTERNAL_EEPROM_OFFSET*i),&copy_ram[(EXTERNAL_EEPROM_OFFSET*i)], 7);
		timerDelay(10);
	
		if(i%5 == 0){
			max5112StopCondition();
		}
	
	}
//	max5112StopCondition();
	I2CM_16WriteNBytes(I2CM_MASTER_1, EXTERNAL_EEPROM_ADDRESS, addr+(EXTERNAL_EEPROM_OFFSET*18),&copy_ram[(EXTERNAL_EEPROM_OFFSET*18)], 2);
	timerDelay(10);
	
}
#endif

extern unsigned char I2CM_16ReadNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned short reg_addr,
                               unsigned char *data,
                               unsigned char num_bytes)
{
//  if((master == I2CM_MASTER_1) && (slave_addr == 0x56)) { //GN2146_DEVICE_ADDRESS
//    gn2146DisFlag = ON;
//  }
	unsigned char i, reg_addr_8bit[2];
	unsigned char return_code = I2CM_WRITE_STATUS_OK;
	unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;

	reg_addr_8bit[0] = ((reg_addr >> 8) & 0xFF); // High Address
	reg_addr_8bit[1] =  (reg_addr & 0xFF);		// Low Address

	if (num_bytes > 8)                      // clamp bytes written to max 8
	{
		num_bytes = 8; 
	}
	if (num_bytes == 0)                     // clamp bytes written min 1
	{
		num_bytes = 1; 
	}
   
	tx_rx_complete = 0;
    
	if (master == 0)                        // master 1
	{
	  	CNT2W1_bit.ADSEL2W = 0;               // point DAD2W to data buffers
		DAD2W1 = reg_addr_8bit[0];                  // set slave reg index
		DAD2W1 = reg_addr_8bit[1];                  // set slave reg index
		CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
		
		// CNT2W1_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W1_bit.WMA = 0;                   // set up for dummy index reg write
		CNT2W1_bit.TWI = 0;                   // clear complete flag
		CNT2W1_bit.EWS = 1;                   // set to xmit stop bit		
//		CNT2W1_bit.EWS = 0;                   // set to xmit stop bit		
		CNT2W1_bit.MADR_2W = 0;               // use first buffer 		
		CNT2W1_bit.NB_2W = 1;     			  // number of bytes to write
		SLA2W1 = (slave_addr << 1);           // set slave addr/initiate TX

		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{	
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}       
		}		
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{	
			I2CM_Reset(I2CM_MASTER_1); 
			// return;
		}
		CNT2W1_bit.TWI = 0;                 // clear complete flag
                    
		Delay_10us(2);
		
		// CNT2W1 &= 0x8800;                     // clear all status .
		
		CNT2W2_bit.XFR_FAIL = 0;            // clear all status
		CNT2W2_bit.FAIL_STATUS = 0; 
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
		CNT2W1_bit.WMA = 0;                   // set up for dummy index reg write		
		CNT2W1_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		
        CNT2W1_bit.MADR_2W = 0;             // start from first buffer
		CNT2W1_bit.NB_2W = num_bytes - 1;   // number of bytes to read
		SLA2W1 = (slave_addr << 1) | 0x01;  // set slave address/initiates rx 	
		
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}		
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to data buffers
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
        CNT2W1_bit.MADR_2W = 0;             // start from first buffer

        
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		
		if (!return_code)                      
		{
			for (i = 0; i < num_bytes; i++)
			{
				// data[i] = DAD2W1;             // copy data to caller buff  
				*(data) = DAD2W1;             // copy data to caller buff  
				data++;
			}
		}
		else                                  // err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1);         
		}                      
        CNT2W1_bit.XFR_FAIL = 0;              // clear all status
        CNT2W1_bit.FAIL_STATUS = 0;  
		CNT2W1 &= 0x8800;                     // clear all status .		
    }
#if 0 
	else{

		CNT2W2_bit.XFR_FAIL = 0;              // clear all status
		CNT2W2_bit.FAIL_STATUS = 0;
		
		CNT2W2_bit.ADSEL2W = 1;             // point DAD2W to MEM_ADDR
		CNT2W2_bit.WMA = 1;                 // set up for dummy index reg write
		CNT2W2_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W2_bit.TWI = 0;                 // clear complete flag
		CNT2W2_bit.NB_2W = num_bytes - 1;   // number of bytes to read
		
		DAD2W2 = reg_addr;                  // set slave reg index
		
		CNT2W2_bit.MADR_2W = 0;             // start from first buffer
		SLA2W2 = (slave_addr << 1) | 0x01;  // set slave address/initiates rx      
      
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}

        CNT2W2_bit.ADSEL2W = 0;             // point DAD2W to data buffers
		CNT2W2_bit.TWI = 0;                 // clear complete flag
		
        CNT2W2_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
        CNT2W2_bit.MADR_2W = 0;             // start from first buffer

        
		if (CNT2W2_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W2_bit.FAIL_STATUS;          
		}
		
		if (!return_code)                      
		{
			for (i = 0; i < num_bytes; i++)
			{
				// data[i] = DAD2W2;               // copy data to caller buff  
				*(data) = DAD2W2;             // copy data to caller buff  
				data++;
			}
		}
		else                                  // err clean up master and slave
		{
		I2CM_Reset(I2CM_MASTER_2);        
		}                      
		CNT2W2_bit.XFR_FAIL = 0;              // clear all status
		CNT2W2_bit.FAIL_STATUS = 0;    

	}
#endif
//    gn2146DisFlag = OFF;
	return (return_code);
}

extern unsigned char I2CM_32ReadNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned long reg_addr,
                               unsigned char *data,
                               unsigned char num_bytes)
{
  
//  if((master == I2CM_MASTER_1) && (slave_addr == 0x56)) { //GN2146_DEVICE_ADDRESS
//    gn2146DisFlag = ON;
//  }
	unsigned char i, reg_addr_8bit[3];
	unsigned char return_code = I2CM_WRITE_STATUS_OK;
	unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;

	reg_addr_8bit[0] = ((reg_addr >> 16) & 0xFF); // Hihg High Address
	reg_addr_8bit[1] =  ((reg_addr >> 8) & 0xFF); // High Address
	reg_addr_8bit[2] =  (reg_addr & 0xFF);		// Low Address
		
	if (num_bytes > 8)                      // clamp bytes written to max 8
	{
		num_bytes = 8; 
	}
	if (num_bytes == 0)                     // clamp bytes written min 1
	{
		num_bytes = 1; 
	}
   
	tx_rx_complete = 0;
    
	if (master == 0)                        // master 1
	{    
		CNT2W1_bit.ADSEL2W = 0;               // point DAD2W to data buffers
		DAD2W1 = reg_addr_8bit[0];                  // set slave reg index
		DAD2W1 = reg_addr_8bit[1];                  // set slave reg index
		DAD2W1 = reg_addr_8bit[2];                  // set slave reg index
		CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
		
		// CNT2W1_bit.WMA = 1;                   // set up for dummy index reg write
		CNT2W1_bit.WMA = 0;                   // set up for dummy index reg write
		CNT2W1_bit.TWI = 0;                   // clear complete flag
		CNT2W1_bit.EWS = 1;                   // set to xmit stop bit		
		// CNT2W1_bit.EWS = 0;                   // set to xmit stop bit		
		CNT2W1_bit.MADR_2W = 0;               // use first buffer 		
		CNT2W1_bit.NB_2W = 2;     // number of bytes to write
		SLA2W1 = (slave_addr << 1);           // set slave addr/initiate TX
		
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}       
		}		
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		if (return_code)                      // if err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1); 
			// return;
		}
		CNT2W1_bit.TWI = 0;                 // clear complete flag
                    
		Delay_10us(2);
		
		// CNT2W1 &= 0x8800;                     // clear all status .
		
		CNT2W2_bit.XFR_FAIL = 0;            // clear all status
		CNT2W2_bit.FAIL_STATUS = 0; 
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
		CNT2W1_bit.WMA = 0;                   // set up for dummy index reg write		
		CNT2W1_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		
        CNT2W1_bit.MADR_2W = 0;             // start from first buffer
		CNT2W1_bit.NB_2W = num_bytes - 1;   // number of bytes to read
		SLA2W1 = (slave_addr << 1) | 0x01;  // set slave address/initiates rx 		
		
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}		
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to data buffers
		CNT2W1_bit.TWI = 0;                 // clear complete flag
		
        CNT2W1_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
        CNT2W1_bit.MADR_2W = 0;             // start from first buffer

        
		if (CNT2W1_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W1_bit.FAIL_STATUS;          
		}
		
		if (!return_code)                      
		{
			for (i = 0; i < num_bytes; i++)
			{
				// data[i] = DAD2W1;             // copy data to caller buff  
				*(data) = DAD2W1;             // copy data to caller buff  
				data++;
			}
		}
		else                                  // err clean up master and slave
		{
			I2CM_Reset(I2CM_MASTER_1);         
		}                      
        CNT2W1_bit.XFR_FAIL = 0;              // clear all status
        CNT2W1_bit.FAIL_STATUS = 0;  
		CNT2W1 &= 0x8800;                     // clear all status .		
    }
    else
    {
		CNT2W2_bit.XFR_FAIL = 0;              // clear all status
		CNT2W2_bit.FAIL_STATUS = 0;
		
		CNT2W2_bit.ADSEL2W = 1;             // point DAD2W to MEM_ADDR
		CNT2W2_bit.WMA = 1;                 // set up for dummy index reg write
		CNT2W2_bit.EWS = 1;                 // set to xmit stop bit
		CNT2W2_bit.TWI = 0;                 // clear complete flag
		CNT2W2_bit.NB_2W = num_bytes - 1;   // number of bytes to read
		
		DAD2W2 = reg_addr;                  // set slave reg index
		
		CNT2W2_bit.MADR_2W = 0;             // start from first buffer
		SLA2W2 = (slave_addr << 1) | 0x01;  // set slave address/initiates rx      
      
		while(!(CNT2W1_bit.TWI)) 			 
		{
			if (--timeout_cnt == 0)
			{
				return_code = I2CM_WRITE_STATUS_TMOUT;
				break;
			}        
		}

        CNT2W2_bit.ADSEL2W = 0;             // point DAD2W to data buffers
		CNT2W2_bit.TWI = 0;                 // clear complete flag
		
        CNT2W2_bit.ADSEL2W = 0;             // point DAD2W to MEM_ADDR
        CNT2W2_bit.MADR_2W = 0;             // start from first buffer

        
		if (CNT2W2_bit.FAIL_STATUS)           // report any HW error
		{
			return_code = CNT2W2_bit.FAIL_STATUS;          
		}
		
		if (!return_code)                      
		{
			for (i = 0; i < num_bytes; i++)
			{
				// data[i] = DAD2W2;               // copy data to caller buff  
				*(data) = DAD2W2;             // copy data to caller buff  
				data++;
			}
		}
		else                                  // err clean up master and slave
		{
		I2CM_Reset(I2CM_MASTER_2);        
		}                      
		CNT2W2_bit.XFR_FAIL = 0;              // clear all status
		CNT2W2_bit.FAIL_STATUS = 0;    
	}
//    gn2146DisFlag = OFF;
	return (return_code);
}




/*******************************************************************************
 * Function: I2CM_Reset
 *
 * Reset a master I2C interface
 *
 * Input:     unsigned char master - I2C master select from i2cm.h
 *
 * Output:    none
 *
 * Example:   I2CM_Reset(master);
 *
 * Remarks:   No validation performed on any parameters
 *
 ******************************************************************************/
static void I2CM_Reset(unsigned char master)
{
    unsigned int timeout_cnt = I2CM_TX_TIMEOUT_COUNT;

    if (master == 0)                        // master 1
    {
        CNT2W1_bit.TW2WEN = 0;              // disable/reset master I2C
        asm("nop");
        asm("nop");
      
        //clock at least 9 scl pulses to clear slave device if it is locked up
        //do this by reading 1 byte of data from slave address FFh
        CNT2W1_bit.NB_2W = 0;               // Read No of Bytes
        CNT2W1_bit.EWS = 1;                 // Issue a stop at end of transaction.
        CNT2W1_bit.MADR_2W = 0;             // Set Index Buffer to Zero
        SLA2W1 = 0xFF;                      // Start 2-Wire communication Read 
        
        while (CNT2W1_bit.TWI == 0)         // wait for complete or timeout
        {
          if (--timeout_cnt == 0)
            {
                break;
            }        
        }
        CNT2W1 &= 0x8000;                   // clear all control bits exc. enable TBD need bitfield defs
		CNT2W1_bit.TW2WEN = 1;				/* Enable the I2C Master */
    }
    else                                    // master 2
    {
        CNT2W2_bit.TW2WEN = 0;              // disable/reset master I2C
        asm("nop");
        asm("nop");
      
        //clock at least 9 scl pulses to clear slave device if it is locked up
        //do this by reading 1 byte of data from slave address FFh
        CNT2W2_bit.NB_2W = 0;               // Read No of Bytes
        CNT2W2_bit.EWS = 1;                 // Issue a stop at end of transaction.
        CNT2W2_bit.MADR_2W = 0;             // Set Index Buffer to Zero
        SLA2W2 = 0xFF;                      // Start 2-Wire communication Read 
        
        while (CNT2W2_bit.TWI == 0)         // wait for complete or timeout
        {
          if (--timeout_cnt == 0)
            {
                break;
            }        
        }
        CNT2W2 &= 0x8000;                   // clear all control bits exc. enable TBD need bitfield defs
    } 
}



//#pragma vector = 5          
//
//__interrupt void module_5_ISR()
//{
//    tx_rx_complete = 1;                 // completion flag
//    interrupt_count++;    
//    CNT2W1_bit.TWI = 0;                    // clear status for both
//    CNT2W2_bit.TWI = 0;
//}





