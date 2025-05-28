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
#ifndef I2CM_H
#define I2CM_H

// uncomment below to stop the register addr write on a read operation
#define ENABLE_AARDVARK_READ_TEST_MODE
#if defined(ENABLE_AARDVARK_READ_TEST_MODE)
#define ARRDVARK_DELAY() {for (i=0;i<1000000;i++) {}}
#endif

// this should be in main or system include file
#define CPU_CLOCK_HZ (16000000UL)

// I2C master select
#define I2CM_MASTER_1   (0)
#define I2CM_MASTER_2   (1)

// master clock divider settings (these may have to be tweaked for better freq match)
#define I2CM_CLOCK_DIV_1000KHZ ((((CPU_CLOCK_HZ / 1000000UL) / 2) - 1) * 256) | (((CPU_CLOCK_HZ / 1000000UL) / 2) - 1)
#define I2CM_CLOCK_DIV_500KHZ  ((((CPU_CLOCK_HZ / 500000UL) / 2) - 1) * 256) | (((CPU_CLOCK_HZ / 500000UL) / 2) - 1)
#define I2CM_CLOCK_DIV_400KHZ  ((((CPU_CLOCK_HZ / 400000UL) / 2) - 1) * 256) | (((CPU_CLOCK_HZ / 400000UL) / 2) - 1)
#define I2CM_CLOCK_DIV_100KHZ  ((((CPU_CLOCK_HZ / 100000UL) / 2) - 1) * 256) | (((CPU_CLOCK_HZ / 100000UL) / 2) - 1)

// master data MSDA1 pin select
#define I2CM_MSDA_PIN_DEFAULT (0x00)
#define I2CM_MSDA_PIN_GP10    (0x01)
#define I2CM_MSDA_PIN_GP12    (0x02)
#define I2CM_MSDA_PIN_GP13    (0x04)
#define I2CM_MSDA_PIN_GP14    (0x08)

// master write return codes
#define I2CM_WRITE_STATUS_OK    (0x00) // no errors
#define I2CM_WRITE_STATUS_ADDR  (0x01) // fail on slave address transfer
#define I2CM_WRITE_STATUS_MEM   (0x02) // fail on mem address transfer
#define I2CM_WRITE_STATUS_WDATA (0x03) // fail on data byte transfer in write cycle
#define I2CM_WRITE_STATUS_TMOUT (0xFF) // timeout

// timeout on master write operations (used in counter not timer--empirical worst-case 8 bytes at 100KHz)
#define I2CM_TX_TIMEOUT_COUNT   (0x1000)

#define EXTERNAL_EEPROM_ADDRESS		0x54
#define EXTERNAL_EEPROM_OFFSET		0x0007
#define EXTERNAL_EEPROM_PAGE_BYTE	0x0080

extern void I2CM_Init (unsigned char Master, unsigned int Clock_Rate, unsigned char Pin, unsigned char use_ints);
extern unsigned char I2CM_WriteNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned char reg_addr,
                               const unsigned char *data,
                               unsigned char num_bytes);
extern unsigned char I2CM_ReadNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned char reg_addr,
                               unsigned char *data,
                               unsigned char num_bytes);							   

extern unsigned char I2CM_16WriteNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned short reg_addr,
                               const unsigned char *data,
                               unsigned char num_bytes);

extern unsigned char I2CM_16ReadNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned short reg_addr,
                               unsigned char *data,
                               unsigned char num_bytes);								   
							   
extern unsigned char I2CM_32WriteNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned long reg_addr,
                               const unsigned char *data,
                               unsigned char num_bytes);

extern unsigned char I2CM_32ReadNBytes(unsigned char master, unsigned char slave_addr,
                               unsigned long reg_addr,
                               unsigned char *data,
                               unsigned char num_bytes);	



//extern void eepromReadFunction(unsigned char* copy_ram,unsigned short addr);
//extern void eepromWriteFunction(unsigned char* copy_ram,unsigned short addr);
#endif
