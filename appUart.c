/*
 * FILE NAME            : appUart.c
 * DATE                 : 
 * AUTHOR               : 
 * HISTORY              : 
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <intrinsics.h>
#include <iods4835.h>
#include <stdlib.h>

#include "includes.h"
#include "main.h"
#include "board.h"
#include "timer.h"
#include "appGpio.h"
#include "appUart.h"
#include "appUartQueue.h"
#include "max5112.h"
#include "appWrapper.h"
#include "tl6100.h"
#include "sff8690.h"
#include "appSmartTune.h"


static uint8 uartRxData = 0x00;
static uint8 uartTxData = 0x00;
static uint8 uartRxDataCount = 0;
static uint8 uartTxDataCount = 0;
static UART_RX_STATE_E uartRxState = UART_RX_STATE_IDLE;
static UART_TX_STATE_E uartTxState = UART_TX_STATE_IDLE;
static UART_RX_CHECK_STATE_E rxCheckState = HEADER_1ST_PREAMBLE;
static UART_TX_AMCC_STATE_E rodState = UART_ROD_IDLE;
#ifdef ADDON_JMA_WIRELESS
static UART_TX_DIS_STATE_E txDisState = TX_DIS_IDLE;
#endif
static UART_QUEUE_TYPE_T uartTxQueue;
static uint8 rxDataBuff[32] = {0,};
static uint16 txSoaValue = 0;
static uint8 losRetry=_ON;


void uartReceiveData(void)
{        
      uint8 crcData=0;
      uint16 rxErrorCount=0,rxSuccessCount=0;
                 
	switch(uartRxState)
	{
		case UART_RX_STATE_START :       
			setTimer1Tbc(UART_DURATION);
			uartRxState = UART_RX_STATE_DATA;
			uartRxData = 0x00;
			uartRxDataCount = 0;		
			//LOS_TEST = 0;
		break;
		case UART_RX_STATE_DATA :
		  	//(UART_RX & 0x01) ? (LOS_TEST = 1) : (LOS_TEST = 0);
			uartRxData |= UART_RX << uartRxDataCount;  
			uartRxDataCount++;
			if(uartRxDataCount > 7) {            
				uartRxState = UART_RX_STATE_STOP;                                
			}
		break; 
		case UART_RX_STATE_STOP : 
			if(UART_RX && uartRxDataCount == 8)
            { 
			  	//LOS_TEST = 1;	           
				*(rxDataBuff+A2_UART_MEMORY[UART_RX_INDEX_COUNT]) = uartRxData;               
				if(A2_UART_MEMORY[UART_RX_INDEX_COUNT] == (rxDataBuff[3]+RX_CRC_INDEX) )
                {    // rxDataBuff[3] = datalength         //MSA로 통신시 안탐   
					crcData = makeCRC8code(&rxDataBuff[3],(rxDataBuff[3]+RX_CRC_LEGNTH));
					if(crcData != uartRxData)
                    {
						rxErrorCount = Reg2ByteToWordLoad(&A2_UART_MEMORY[UART_RX_ERROR_COUNT]);
						rxErrorCount++;
						RegWordTo2ByteSave(rxErrorCount, &A2_UART_MEMORY[UART_RX_ERROR_COUNT]);
						RegWordTo2ByteSave(rxErrorCount, &A2_UART_MEMORY[0xDE-0x80]);
					}
                    else
                    {
						rxSuccessCount = Reg2ByteToWordLoad(&A2_UART_MEMORY[UART_RX_SUCCESS_COUNT]);
						rxSuccessCount++;
						rxDataBuff[0] = PREAMBLE_CODE;
						rxDataBuff[1] = PREAMBLE_CODE;
						rxDataBuff[2] = STX_CODE;
						A2_UART_MEMORY[UART_RX_BUF_START_INDEX+rxDataBuff[3]+RX_ETX_INDEX] = ETX_CODE;             
						memcpy(&A2_UART_MEMORY[UART_RX_BUF_START_INDEX], &rxDataBuff[0] , (rxDataBuff[3]+RX_ETX_INDEX)); 
						RegWordTo2ByteSave(rxSuccessCount, &A2_UART_MEMORY[UART_RX_SUCCESS_COUNT]);
						RegWordTo2ByteSave(rxSuccessCount, &A2_UART_MEMORY[0xDC-0x80]);
                                                                                
						if(rxDataBuff[UART_CMD] == CMD_REQ_DDM)
                        {                    
							rodState = UART_ROD_DDM; 
						}
                        else if(rxDataBuff[UART_CMD] == CMD_DDM)
                        {
						  	A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] = 0x1;		// UART Tx/Rx success
						  	memcpy(&A2_FAR_END_A2_0x22[UART_ROD_DDM_TEMP],&A2_UART_MEMORY[UART_RX_BUF_DATA_0_IDX],0xA);
							rodState = UART_ROD_IDLE;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_AWS_REQ)
                        {
							A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = rxDataBuff[5]; 
                            if((rxDataBuff[6] >= 1) && (rxDataBuff[6] <= 98))
                            {    
								A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = rxDataBuff[6];
								rodState = UART_AWS_RESPONSE;
							}
                            else
                            {
							  	uint8 mode;
								uint16 channel;
							  	mode = A2_UserData[MODULE_ID_SET] & 0x0f;
								if(mode == AWS_RANDOM)
                                {
								  if(rodState != UART_AWS_REQUEST)
                                  {
										channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
										A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] = channel;
										A2_DiagData[STATUS_CONTROL_ADD] &= ~(0x40);
										RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);		// for immediate excute 
										rodState = UART_AWS_REQUEST;		// ASP retry method
								  }
								}
							}
						}
                        else if(rxDataBuff[UART_CMD] == CMD_AWS_CH_REQ)
                        {
							A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL] = rxDataBuff[5];
							rodState = UART_AWS_CH_SET_RESPONSE;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_AWS_CH_RES)
                        { 
							rodState = UART_AWS_COMM_SUCCESS;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_AWS_COMM_CHECK)
                        {
							rodState = UART_AWS_COMM_COMPLETE;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_COMM_CHECK)
                        {
							rodState = UART_COMM_TRY_RESPONSE;
						}else if(rxDataBuff[UART_CMD] == CMD_COMM_CHECK_RES){
							rodState = UART_AWS_COMM_COMPLETE;
						}else if(rxDataBuff[UART_CMD] == CMD_AWS_MASTER_REQ){
							A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = rxDataBuff[5];
							rodState = UART_AWS_SLAVE_SET;
						}else if(rxDataBuff[UART_CMD] == CMD_AWS_MASTER_SET){
							A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = rxDataBuff[5];
							rodState = UART_AWS_MASTER_SET;
						}else if(rxDataBuff[UART_CMD] == CMD_AWS_MASTER_RES){
							rodState = UART_AWS_COMM_COMPLETE;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_CH_REQ)
                        {
							A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = rxDataBuff[5];
							rodState = UART_ROD_CH_RES;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_MER)
                        {
							rodState = UART_ROD_MEM_READ_RECV;
						}
                        else if(rxDataBuff[UART_CMD] == CMD_MER_RES)
                        {
						  	RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);		// for immediate excute
							rodState = UART_ROD_MEM_READ_RES;
						}
					}                               
				}
                                   
				switch(rxCheckState)
				{
					case HEADER_1ST_PREAMBLE :

						if(uartRxData == PREAMBLE_CODE) {
							rxCheckState = HEADER_2ND_PREAMBLE;
						} else {
							rxCheckState = HEADER_1ST_PREAMBLE;
						}
					break;
					case HEADER_2ND_PREAMBLE :
						if(uartRxData == PREAMBLE_CODE) {       ////////////////////
							rxCheckState = HEADER_STX_CODE;
						} else {
							rxCheckState = HEADER_1ST_PREAMBLE;
						}
					break;
					case HEADER_STX_CODE :
						if(uartRxData == STX_CODE) {
							rxCheckState = DATA_CODE;
							A2_UART_MEMORY[UART_RX_INDEX_COUNT] = 3;
						} else {
							rxCheckState = HEADER_1ST_PREAMBLE;
						}
					break;
					case DATA_CODE :
                      
						if( A2_UART_MEMORY[UART_RX_INDEX_COUNT] >= (rxDataBuff[UART_DLEN]+RX_ETX_INDEX) ){
							A2_UART_MEMORY[UART_RX_INDEX_COUNT] = 0;
							rxDataBuff[UART_DLEN] = 0;
							rxCheckState = HEADER_1ST_PREAMBLE;
						}
                        else
                        {
							A2_UART_MEMORY[UART_RX_INDEX_COUNT] += 1;
							if(A2_UART_MEMORY[UART_RX_INDEX_COUNT] > 32){
								A2_UART_MEMORY[UART_RX_INDEX_COUNT] = 0;
								rxDataBuff[UART_DLEN] = 0;
								rxCheckState = HEADER_1ST_PREAMBLE; 
							}						
#if 1							
							if(rxDataBuff[UART_CMD] == CMD_AWS_REQ){
							  if(A2_UART_MEMORY[UART_RX_INDEX_COUNT] > 4)
								  rxDataBuff[UART_DLEN] = 2;			// ASP REQ command data length 2 fix
							}
#endif							
						}
					break;
					default :
					break;				
				}
				uartRxState = UART_RX_STATE_IDLE;
                
			} else {                                    
				uartRxState = UART_RX_STATE_IDLE;
			}
		break;
			
		case UART_RX_STATE_IDLE : 
		default :      
            EIF0_bit.EIF06 = OFF;
			stopTimer1();
			EIE0_bit.EIE06 = ON;
			uartRxState = UART_RX_STATE_IDLE; 
		break;
	}
}


#if 0
uint16 amccConstantCalculation(uint16 input)
{
	float p;
	float f_temp[3];
	uint8 i, buff[3];
	uint16 data;

	p = input;

	for(i=0; i<2; i++){   
		memcpy(&buff[0],&A2_UART_MEMORY[UART_AMCC_SLOPE_Add+(i*4)],sizeof(unsigned long));
		swap_uint32(&buff[0]);
		memcpy(&f_temp[i], &buff[0], sizeof(float));	
	}
  
	data = (f_temp[2]*p*p) + (f_temp[1]*p) + f_temp[0];
        
	return data;
}
#endif

void uartTransmitData(void)
{
        static uint8 offset=0;
//        uint8 channel=0;  
	if(A2_UART_MEMORY[UART_TX_PROCESS_FLAG] == 1){
	  	writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, txSoaValue); // stop condition
	  	stopTimer2();
	  	uartTxDataCount = 0;
		uartTxState = UART_TX_STATE_IDLE;
		return;
	}
	switch(uartTxState)
	{         
		case UART_TX_STATE_START :
			uartTxState = UART_TX_STATE_DATA;
			uartTxDataCount = 0;

			uartTxData = dequeueUartData(&uartTxQueue);                        
			txSoaValue = Reg2ByteToWordLoad(&A2_TTOSA_MEMORY[REG_TTOSA_CTRL_FRONT_SOA]);
			offset = A2_VendorFn[AMCC_MODULATION_OFFSET];
            /*            
			if(A2_UART_MEMORY[UART_AMCC_SLOPE_ENABLE_Add] == 0x1){
				channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
				if(channel > A2_UART_MEMORY[UART_AMCC_SLOPE_CHANNEL_Add])             
					offset = amccConstantCalculation(channel);
				}
			*/		
			writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, (txSoaValue-offset)); // start condition
            //TEST_PORT = ~0; // start condition       
			break;
		case UART_TX_STATE_DATA :
			
			((uartTxData >> uartTxDataCount) & 0x01) ? writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, txSoaValue) : writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, (txSoaValue-offset));
        
			uartTxDataCount++;
			if(uartTxDataCount > 7) {
				uartTxDataCount = 0;
				uartTxState = UART_TX_STATE_STOP;     
			}
			break;
		case UART_TX_STATE_STOP :
			//TEST_PORT = ~1;       
            writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, txSoaValue); // stop condition
			stopTimer2();
			uartTxState = UART_TX_STATE_IDLE;
			break;
		case UART_TX_STATE_IDLE :
		default :
			uartTxState = UART_TX_STATE_IDLE;
			break;
	}
           
}



void startUartTx()
{ 
  
	uint16 uartPacketPeriod=0,uartPacketCounter=0;
  
	if( (A2_VendorFn[TEST_BUF_2] == 0x55) && ((A2_UserData[MODULE_CONTROL_STATUS] & AMCC_ENABLE) || (A2_UserData[MODULE_CONTROL_STATUS] & AWS_ENABLE)) ){
	  	A2_UART_MEMORY[UART_ROD_STATE] = rodState;
		masterState();
	  	uartPacketPeriod = Reg2ByteToWordLoad(&A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);  
		uartPacketCounter = Reg2ByteToWordLoad(&A2_UART_MEMORY[UART_TX_PACKET_COUNTER]);
		
		if(uartPacketCounter > uartPacketPeriod){
			//rodState = UART_TEST;
#ifndef NORMAL_CUSTOMER
		  	A2_UserData[MODULE_ID_STATUS] = A2_UserData[MODULE_ID_SET] & 0x0f;
			
//			if((A2_DiagData[STATUS_CONTROL_ADD] & B7_H) || (A2_DiagData[STATUS_CONTROL_ADD] & B6_H)){  // tx disable, AMCC don't operate   
//				stopTimer2();
//				if(A2_DiagData[STATUS_CONTROL_ADD] & B6_H){  // tx disable, AMCC don't operate   
//					writeMax5112(DAC_GAIN_CODE, 0);
//					writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, 0);
//				}
//			}else{
                
				uartStateManager();
//			}  
				uartPacketCounter=0;
#else
		  	A2_UserData[MODULE_ID_STATUS] = A2_UserData[MODULE_ID_SET] & 0x0f;
			
			if((A2_DiagData[STATUS_CONTROL_ADD] & B7_H) || (A2_DiagData[STATUS_CONTROL_ADD] & B6_H)){  // tx disable, AMCC don't operate   
				stopTimer2();
				if(A2_DiagData[STATUS_CONTROL_ADD] & B6_H){  // tx disable, AMCC don't operate   
					writeMax5112(DAC_GAIN_CODE, 0);
					writeMax5112(DAC_FRONT_SOA_SOURCE_CODE, 0);
				}
			}else{  
				uartStateManager();
			}  
				uartPacketCounter=0;				
#endif
				
		}

		uartPacketCounter++; 
		RegWordTo2ByteSave(uartPacketCounter, &A2_UART_MEMORY[UART_TX_PACKET_COUNTER]);
	}else{
		rodState = UART_ROD_IDLE;
        if((A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] != MSA_TUNE) && (changeAspFlag == 0)) {
		    A2_UserData[AMCC_LINK_STATUS] &= ~(0x1);     
        }
	}
                    
	if(!isUartQueueEmpty(&uartTxQueue)){ 
		startTimer2();
		uartTxState = UART_TX_STATE_START;
	}

}

void startUartRx(void)
{
	//if(uartRxState == UART_RX_STATE_IDLE) {     
		setTimer1Tbc(UART_DURATION / 4);            //// 주기 4로 바꿔서 확인      
		startTimer1();
		uartRxState = UART_RX_STATE_START;
        
	//}
}





void initializeUart()
{
        initializeUartQueue(&uartTxQueue);
        A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] = 0x0;                        // AWS success vefity byte initial
        A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x0;                        // AWS flag write byte initial
        A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] = 0x0;                            // AMCC retry count initial
        A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] = 0x1;                        // AWS Tx local channel initial
        A2_UART_MEMORY[UART_AWS_SEND_COUNT] = 0x0;                              // AWS packet send count
        A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = 0xff;                           // AWS Rx local channel initial
        A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = 0xff;                          // AWS Rx remote channel for AWS bi-direction mode initial
        A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL] = 0xff;                        // AWS Rx response channel for AWS bi-direction mode initial
        A2_UART_MEMORY[UART_RX_INDEX_COUNT] = 0x0;
        RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_RX_ERROR_COUNT]); 
        RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_RX_SUCCESS_COUNT]);
        RegWordTo2ByteSave(30, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
        RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_COUNTER]);
		
#ifdef ADDON_JMA_WIRELESS
		A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x0;
		RegWordTo2ByteSave(300, &A2_UART_MEMORY[UART_TX_DIS_COUNTER]); 
		RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_DIS_COUNTER1]); 
		RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_DIS_COUNTER2]); 
#endif
		if((A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] != MSA_TUNE) && (changeAspFlag == 0)) {
            A2_UserData[AMCC_LINK_STATUS] &= ~(0x1);                                // AWS tuning status initial
        }
		A2_UART_MEMORY[UART_TX_PROCESS_FLAG] = 0;
}

void uartPacketParsing(uint8 dataLength, uint8 command,uint8* data)
{
  uint8 i=0,packetIndex=0,uartTxBuff[32];
  uint8 dummyDataNum = 0;
  uint8 crcidxTemp = 0;
 
  dummyDataNum = A2_VendorFn[AMCC_DUMMY_DATA];
  
  memset(&uartTxBuff[0],0xDB,dummyDataNum);
  packetIndex += dummyDataNum;
  
  uartTxBuff[packetIndex] = PREAMBLE_CODE;
  packetIndex++;
  uartTxBuff[packetIndex] = PREAMBLE_CODE;
  packetIndex++;
  uartTxBuff[packetIndex] = STX_CODE;
  packetIndex++;
  uartTxBuff[packetIndex] = dataLength;
  crcidxTemp = packetIndex;
  packetIndex++;
  uartTxBuff[packetIndex] = command;
  packetIndex++;

  memcpy(&uartTxBuff[packetIndex],&data[0],dataLength);
  packetIndex += dataLength;
  
  uartTxBuff[packetIndex] = makeCRC8code(&uartTxBuff[crcidxTemp],(packetIndex-(3+dummyDataNum))); // Except PREAMBLE,STX_CODE
  A2_DEBUGTAB[CRCINDEX] = makeCRC8code(&uartTxBuff[crcidxTemp],(packetIndex-(3+dummyDataNum)));
  packetIndex++;
  uartTxBuff[packetIndex] = ETX_CODE;
  packetIndex++;

  for(i=0;i<packetIndex;i++){
    enqueueUartData(&uartTxQueue, uartTxBuff[i]);
  }
}

uint8 makeCRC8code(uint8* data, uint16 length)
{
  uint8 i,crcResult=0;

  CRCOUT = 0x00;
  
  for(i=0;i<length;i++){
    CRCIN = data[i];
  }
  
  crcResult = CRCOUT;  
  
  return crcResult;
} 

void uartStateManager(void)
{
      uint8 data[22] = {0,};
      uint8 txSetChannel=0;
      uint8 datalength = 0;
      uint8 sendData = 0;
	  uint8 mode = 0;
	  uint16 msaMaxChannel = getMsaMaxChannel();
	  
      switch(rodState)
      {
          case UART_ROD_IDLE :        
			A2_UserData[AMCC_LINK_STATUS] &= ~(0x1);     // Exit ASP tuning 
            rodState = UART_ROD_IDLE;

            break;
			
		  case UART_AWS_REQUEST : 
            
			if( !(A2_UserData[MODULE_CONTROL_STATUS] & AWS_ENABLE)){
			  	rodState = UART_ROD_IDLE;
				A2_UserData[AMCC_LINK_STATUS] &= ~(0x1);     // Exit ASP tuning 
               
				break;
			}
			writeMax5112(DAC5_CONFIG_ADDRESS, 0x8800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0x8800);
			
			RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
			A2_UserData[AMCC_LINK_STATUS] |= 0x1;   // Start ASP tuning
           
			A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] = 0x0;
			mode = A2_UserData[MODULE_ID_SET] & 0x0f;
			
			//if(A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] > MAX_WAVELENGTH_CHANNEL){  
			if(A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] > msaMaxChannel){  
				A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] = 1;	
				A2_UART_MEMORY[UART_AWS_SEND_COUNT] += 1;

				if(A2_UART_MEMORY[UART_AWS_SEND_COUNT] > 9){
					A2_UART_MEMORY[UART_AWS_SEND_COUNT] = 0;
					if(mode == AWS_RANDOM){
					  changeTxChannel(A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL]);                                       
					  stopLaser();
                      if((rxDataBuff[6] >= 1) && (rxDataBuff[6] <= 98)){    
                          A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = rxDataBuff[6]; 
                          rodState = UART_AWS_RESPONSE;
                          break;
                      }     
					  RegWordTo2ByteSave(0x0000, &A2_VendorFn[DAC_AMP_SET_Add]);
					  A2_UserData[AMCC_LINK_STATUS] &= ~(0x1);     // Exit ASP tuning
					  A2_DiagData[STATUS_CONTROL_ADD] |= B6_H;
					  rodState = UART_ROD_IDLE;
					}			
				}else{ 
				  	stopLaser();							// UART_AWS_REQUEST -> UART_AWS_REQUEST_LASER_ON 20ms delay
                    if((rxDataBuff[6] >= 1) && (rxDataBuff[6] <= 98)){    
                          A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = rxDataBuff[6]; 
                          rodState = UART_AWS_RESPONSE;
                          break;
                      }     
					rodState = UART_AWS_REQUEST_LASER_ON;	
				}
			}else{
				stopLaser();
                if((rxDataBuff[6] >= 1) && (rxDataBuff[6] <= 98)){    
                          A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = rxDataBuff[6]; 
                          rodState = UART_AWS_RESPONSE;
                          break;
                      }     
				rodState = UART_AWS_REQUEST_LASER_ON;
			}
			break;

		  case UART_AWS_REQUEST_LASER_ON:
			
			RegWordTo2ByteSave(8, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);	// UART_AWS_REQUEST_LASER_ON -> UART_AWS_REQUEST_TX_REQUEST 100ms delay
			changeTxChannel(A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL]);  
            if((rxDataBuff[6] >= 1) && (rxDataBuff[6] <= 98)){    
                  A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = rxDataBuff[6]; 
                  rodState = UART_AWS_RESPONSE;
                  break;
              }     
            rodState = UART_AWS_REQUEST_TX_REQUEST;
			break;
		  case UART_AWS_REQUEST_TX_REQUEST:
			
			RegWordTo2ByteSave(37, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
			data[0] = A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL];
			if(A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] != 0xff){
				data[1] = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];   // Parse tx remote data 
				A2_UART_MEMORY[0x9C-0x80] = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];
			}else{
				data[1] = 0xff;
			}
			
			mode = A2_UserData[MODULE_ID_SET] & 0x0f;
			if(mode == AWS_RANDOM){
				uartPacketParsing(0x2,CMD_AWS_REQ,data);
				uartPacketParsing(0x2,CMD_AWS_REQ,data);
			}else if(mode == AWS_MASTER){
				uartPacketParsing(0x1,CMD_AWS_MASTER_REQ,data);
				uartPacketParsing(0x1,CMD_AWS_MASTER_REQ,data);
			}

			A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] += 1;		// Add channel index 	
			rodState = UART_AWS_REQUEST;
			break;
          case UART_AWS_RESPONSE :
			RegWordTo2ByteSave(30, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
			A2_UART_MEMORY[0x9C-0x80] = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];
            txSetChannel = A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL];
            RegWordTo2ByteSave((uint16)txSetChannel,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);  // Save channel info 
            A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x1;
            changeTxChannel(txSetChannel);
            rodState = UART_AWS_CH_SET;
            break;
        
          case UART_AWS_CH_SET :
			
            if(A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] > 10){
                A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] = 0;
                A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = 0xff;
                A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = 0xff;
                A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL] = 0xff;
                A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x0;
				A2_UART_MEMORY[UART_AWS_SEND_COUNT] = 0;
               	rodState = UART_AWS_REQUEST;       
            }else{
                data[0] = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];
                uartPacketParsing(0x1,CMD_AWS_CH_REQ,data);
                A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] += 1;
            }
            
            break;
          
          case UART_AWS_CH_SET_RESPONSE :
			 RegWordTo2ByteSave(30, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
             txSetChannel = A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL];            
             RegWordTo2ByteSave((uint16)txSetChannel,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);   // Save channel info 
             A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x1;
             changeTxChannel(txSetChannel);  
			 if(A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] > 10){
			    A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] = 0;
                A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = 0xff;
                A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = 0xff;
                A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL] = 0xff;
                A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x0;
				A2_UART_MEMORY[UART_AWS_SEND_COUNT] = 0;
				A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] -= 1;
				rodState = UART_AWS_REQUEST;
             }else{ 
			   data[0] = txSetChannel;
               uartPacketParsing(0x1,CMD_AWS_CH_RES,data);
			   A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] += 1;
			 }
             break;

          case UART_AWS_COMM_SUCCESS :
             data[0] = 0x55;
             uartPacketParsing(0x1,CMD_AWS_COMM_CHECK,data);
             rodState = UART_AWS_COMM_COMPLETE;           
             break;
             
          case UART_AWS_COMM_COMPLETE :
         
            if(A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] == 0x1){
              //flashFlagByte2 |= TTOSAChannelWrFlag;
              sendMessage(getMessageQueue(), EVENT_SFF_8690_CHANNEL, 0);
              A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x0;
            }
             A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] = 0x1;
             A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] = 0;
             A2_UART_MEMORY[UART_AWS_SEND_COUNT] = 0;
             A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = 0xff;
             A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = 0xff;
             A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL] = 0xff;
			 memcpy(&A2_VendorFn[DAC_AMP_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET], 2); // Copy modulation DAC
             A2_UserData[AMCC_LINK_STATUS] &= ~(0x1);     // Exit ASP tuning
             
#ifdef ADDON_JMA_WIRELESS
			 txDisState = TX_DIS_1ST_OBEY;
			 A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x01;
#endif	 
             rodState = UART_ROD_IDLE;
			 break;      
			 
           case UART_AWS_SLAVE_SET :
              RegWordTo2ByteSave(35, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);          
              txSetChannel = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] + A2_UART_MEMORY[AWS_CHANNEL_OFFSEST];
              RegWordTo2ByteSave((uint16)txSetChannel,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);  // Save channel info 
              A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x1;
              changeTxChannel(txSetChannel);
              rodState = UART_AWS_MASTER_CH_REQUEST;
            break;
            
          case UART_AWS_MASTER_CH_REQUEST :
            
			if(A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] > 10){
                  A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] = 0;
                  A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = 0xff;
                  A2_UART_MEMORY[UART_RX_REMOTE_CHANNEL] = 0xff;
                  A2_UART_MEMORY[UART_RX_RESPONSE_CHANNEL] = 0xff;
                  A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x0;                          
                  rodState = UART_ROD_IDLE;                   // Initialize slave mode 
            }else{
                data[0] = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];
                uartPacketParsing(0x1,CMD_AWS_MASTER_SET,data);
                A2_UART_MEMORY[UART_AMCC_RETRY_COUNT] += 0x1;
            }
            break;
            
           case UART_AWS_MASTER_SET:
            txSetChannel = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];
            RegWordTo2ByteSave((uint16)txSetChannel,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);  // Save channel info 
            A2_UART_MEMORY[UART_AWS_FLASH_WRITE_FLAG] = 0x1;
            changeTxChannel(txSetChannel);
            rodState = UART_AWS_MASTER_RESPONSE;
            break;
            
            case UART_AWS_MASTER_RESPONSE :
            data[0] = 0xAA;
            uartPacketParsing(0x1,CMD_AWS_MASTER_RES,data);
            rodState = UART_AWS_COMM_COMPLETE;  
            break;
            
          case UART_REQ_ROD_DDM :           //MASTER SEND
            data[0] = 0x1; 
            uartPacketParsing(0x1,CMD_REQ_DDM,data);
            rodState = UART_ROD_IDLE;
            break;
          
          case UART_ROD_DDM :            //SLAVE RECEIVE
			memcpy(&data[0],&A2_DiagData[DIAGNOSTICS_TEMP],0xA);
			uartPacketParsing(0xA,CMD_DDM,data); 
			A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] = 0x1;					// UART Tx/Rx success
            rodState = UART_ROD_IDLE;
            break;
#if 0
          case UART_ROD_CH_REQ :
            data[0] = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_CHANNEL];
            uartPacketParsing(0x1,CMD_CH_REQ,data); 
            rodState = UART_ROD_IDLE;
            break;
          case UART_ROD_CH_RES :
            txSetChannel = A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL];
            if(txSetChannel > MAX_WAVELENGTH_CHANNEL){
                  ;            
            }else{
              changeTxChannel(txSetChannel);
              A2_UART_MEMORY[UART_RX_LOCAL_CHANNEL] = 0xff;
			  RegWordTo2ByteSave((uint16)txSetChannel,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);  // Save channel info 
			  sendMessage(getMessageQueue(), EVENT_SFF_8690_CHANNEL, 0);
            }
            rodState = UART_ROD_IDLE;
            break;

          case UART_ROD_MEM_READ_REQ:
            
            data[0] = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_DEVICE_ADDRESS];
            data[1] = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_TABLE_ADDRESS];
            data[2] = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_START_ADDRESS];
            data[3] = A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_BYTE_NUMBER];
            
            uartPacketParsing(0x4,CMD_MER,data); 
            
            rodState = UART_ROD_IDLE;
            break;
          case UART_ROD_MEM_READ_RECV:
            
            if(A2_UART_MEMORY[UART_RX_BUF_DATA_0_IDX] == 0xA0){     // device address
                if(A2_UART_MEMORY[UART_RX_BUF_DATA_2_IDX] < 128){	// start address
                    memcpy(&data,&A0_SerialID[A2_UART_MEMORY[UART_RX_BUF_DATA_2_IDX]],A2_UART_MEMORY[UART_RX_BUF_DATA_3_IDX]);
                }
            }else if(A2_UART_MEMORY[UART_RX_BUF_DATA_0_IDX] == 0xA2){
                if(A2_UART_MEMORY[UART_RX_BUF_DATA_2_IDX] < 128){
				  	memcpy(&data,&A2_DiagData[A2_UART_MEMORY[UART_RX_BUF_DATA_2_IDX]],A2_UART_MEMORY[UART_RX_BUF_DATA_3_IDX]);
                }else{
                    if(A2_UART_MEMORY[UART_RX_BUF_DATA_1_IDX] == 0x2){
                     
					 memcpy(&data,&A2_TTOSA_PAGE_0x02[(A2_UART_MEMORY[UART_RX_BUF_DATA_2_IDX]-UART_ADDRESS_OFFSET)],A2_UART_MEMORY[UART_RX_BUF_DATA_3_IDX]);
                    }
                }
            }
               
            uartPacketParsing(A2_UART_MEMORY[UART_RX_BUF_DATA_3_IDX],CMD_MER_RES,data);            
            rodState = UART_ROD_IDLE;
            break;
          case UART_ROD_MEM_READ_RES:
            
            if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_DEVICE_ADDRESS] == 0xA0){
			  	memcpy(&A2_FAR_END_A0_0x20[A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_START_ADDRESS]], &A2_UART_MEMORY[UART_RX_BUF_DATA_0_IDX],A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_BYTE_NUMBER]);
            }else if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_DEVICE_ADDRESS] == 0xA2){
                if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_TABLE_ADDRESS] == 0x02){
					memcpy(&A2_FAR_END_02_0x24[(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_START_ADDRESS]-UART_ADDRESS_OFFSET)], &A2_UART_MEMORY[UART_RX_BUF_DATA_0_IDX],A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_REMOTE_BYTE_NUMBER]);
                }
            }
            
            rodState = UART_ROD_IDLE;
            break;
#endif
         case UART_TEST:
              RegWordTo2ByteSave(50, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
              datalength = A2_VendorFn[AMCC_DATALENGTH];
              sendData = A2_VendorFn[AMCC_DATA];
              memset(&data,sendData,datalength);
			  /*
			  for(i=0;i<datalength;i++){
                  data[i] = sendData;
              }*/
			  		  
             uartPacketParsing(datalength,0x55,data);

            rodState = UART_ROD_IDLE;
            break;
			
          default :
            break;
        
      }

}

void getAwsMode(void)
{
      uint16 channel;
      uint8  mode;
	  uint16 msaMaxChannel = getMsaMaxChannel();
	  
      if( (A2_UserData[MODULE_CONTROL_STATUS] & AWS_ENABLE) && (A2_UserData[MODULE_CONTROL_STATUS] != 0xff)){
		mode = A2_UserData[MODULE_ID_SET] & 0x0f;
		if(mode == AWS_SLAVE){
			rodState = UART_ROD_IDLE;
		}else if((mode == AWS_MASTER) || (mode == AWS_RANDOM)){
				channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
                 //if(channel > MAX_WAVELENGTH_CHANNEL){   
				if(channel > msaMaxChannel){   
                    channel = 0x1;
                 }else{
                    A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] = channel;
                 }            
                rodState = UART_AWS_REQUEST;   
		}	  
      }
}
uint16 disCounter1, disCounter2, disCounterMax=0;
void masterState(void)
{  
   uint16 channel=0; 
   uint16 losCounter=0;
   uint8 mode;
   uint16 rodPeriod=0;
   
#if 1
   mode = A2_UserData[MODULE_ID_SET] & 0x0f;		
   
   if( ((mode == AWS_MASTER) || (mode == AWS_RANDOM)) && (A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] & 0x1) ){ 
	 
#ifdef ADDON_JMA_WIRELESS
	 disCounterMax = Reg2ByteToWordLoad(&A2_UART_MEMORY[UART_TX_DIS_COUNTER]);
	 switch(txDisState){
	 	case TX_DIS_IDLE :
			txDisState = TX_DIS_IDLE;
			A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x0;
		break;
		
		case TX_DIS_1ST_OBEY :
		  	losRetry = _OFF;
		  	A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x02;
			writeMax5112(DAC5_CONFIG_ADDRESS, 0xC800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0xA800);	
			startLaser(false);
			txDisState = TX_DIS_1ST_OBEY_CNT;
		break;
		
		case TX_DIS_1ST_OBEY_CNT :
			disCounter1++;
			RegWordTo2ByteSave(disCounter1, &A2_UART_MEMORY[UART_TX_DIS_COUNTER1]); 
			if(disCounter1 >= disCounterMax){
				disCounter1 = 0;
				txDisState = TX_DIS_IGNORE;
			}
		break;
		
		case TX_DIS_IGNORE :
		  	A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x04;
			writeMax5112(DAC5_CONFIG_ADDRESS, 0x8800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0x8800);		
			startLaser(false);
			txDisState = TX_DIS_IGNORE_CNT;
		break;
	
		case TX_DIS_IGNORE_CNT :
			disCounter2++;
			RegWordTo2ByteSave(disCounter2, &A2_UART_MEMORY[UART_TX_DIS_COUNTER2]); 
			if(disCounter2 >= disCounterMax){
				disCounter2 = 0;
				txDisState = TX_DIS_2ND_OBEY;
			}		  
		break;		
		
	 	case TX_DIS_2ND_OBEY :
			A2_UART_MEMORY[UART_TX_DIS_FLAG] = 0x08;
			writeMax5112(DAC5_CONFIG_ADDRESS, 0xC800);
			writeMax5112(DAC6_CONFIG_ADDRESS, 0xA800);
			startLaser(false);
			losRetry = _ON;
			txDisState = TX_DIS_IDLE;
		break;
	 }
#endif
	 if(changeAspFlag == 0) {
         if((A2_DiagData[STATUS_CONTROL_ADD] & 0x2) && (losRetry == _ON)){										// LOS retry method
           losCounter = Reg2ByteToWordLoad(&A2_UART_MEMORY[UART_RX_LOS_COUNTER]);      
             losCounter += 1; 
            if(losCounter > 1000){			// 10 sec period
                losCounter = 0;
                channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
                A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] = channel;
                if( (A2_UserData[MODULE_CONTROL_STATUS] & AWS_ENABLE) && (A2_UserData[MODULE_CONTROL_STATUS] != 0xff)){ // ASP enable bit 
                    RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);		// for immediate excute
                    rodState = UART_AWS_REQUEST;
                }
            
             }else{
                    losCounter = 0;
             }
           }
     }
	 RegWordTo2ByteSave(losCounter, &A2_UART_MEMORY[UART_RX_LOS_COUNTER]);
   }
#endif
   
   	mode = A2_UserData[MODULE_ID_SET] & 0xf0;
    if((mode == AMCC_MASTER) && (A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] & 0x1)){
        
        if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] & 0x40){
             A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] &= ~(0x40);
			 RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);		// for immediate excute
             rodState = UART_ROD_CH_REQ;        
        }else if(A2_UserData[AMCC_LINK_STATUS] & 0x10){
            if( (A2_UserData[MODULE_CONTROL_STATUS] & AWS_ENABLE) && (A2_UserData[MODULE_CONTROL_STATUS] != 0xff)){ // ASP enable bit 
                   A2_UserData[AMCC_LINK_STATUS] &= ~(0x10);                 
                   channel = Reg2ByteToWordLoad(&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
                   A2_UART_MEMORY[UART_AWS_TX_LOCAL_CHANNEL] = channel;                
				   rodState = UART_AWS_REQUEST;
              
            }else{
			    A2_UserData[AMCC_LINK_STATUS] &= ~(0x10);
            }
			RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);		// for immediate excute
        }else if(A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] & 0x20){
              A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_FUNCTION_CONTROL] &= ~(0x20);
			  RegWordTo2ByteSave(0, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);	// for immediate excute
              rodState = UART_ROD_MEM_READ_REQ;   
        }else if(rodState == UART_ROD_IDLE){
			  if((A2_UserData[MODULE_CONTROL_STATUS] & AMCC_ENABLE)){
				  rodPeriod = Reg2ByteToWordLoad(&A2_DEBUGTAB[UART_ROD_PERIOD_Add]);
				  RegWordTo2ByteSave(rodPeriod, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);	// ROD period 10 sec                  
				  rodState = UART_REQ_ROD_DDM;
			  }else{
				 RegWordTo2ByteSave(50, &A2_UART_MEMORY[UART_TX_PACKET_PERIOD]);
			  }
        }
    }
}

void changeTxChannel(uint8 channel)
{   
  	uint16 msaMaxChannel = getMsaMaxChannel();
  	//if((channel == 0) || (channel > MAX_WAVELENGTH_CHANNEL)) {
	uint16 tosaChannel = 0;
  	if((channel == 0) || (channel > msaMaxChannel)) {
	   channel = 1;
	}
  
	    tosaChannel = getTunableTosaChannel(channel);

#ifdef USE_EXT_EEPROM      
    eepromReadFunction(A2_TTOSA_MEMORY,A2_TTOSA_CHANNEL_01_EEPROM + A2_TTOSA_MEMORY_SIZE * (tosaChannel - 1));
#else
	I2CCN_S_bit.I2CEN = 0;
    FLASH_Read(A2_TTOSA_CHANNEL_01_FLASH + A2_TTOSA_MEMORY_SIZE * (tosaChannel - 1),A2_TTOSA_MEMORY_ADD, A2_TTOSA_MEMORY_SIZE); 
	I2CCN_S_bit.I2CEN = 1;
#endif
    memcpy(&A2_VendorFn[DAC_TEC_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_TEMPERATURE_SET], 2);  // respectively channel for tec change
    //memcpy(&A2_VendorFn[DAC_AMP_SET_Add], &A2_TTOSA_MEMORY[REG_TTOSA_MODULATION_SET], 2); 
//	startLaser();
    RegWordTo2ByteSave((uint16)channel,&A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CHANNEL]);
	A2_UserData[TX_MC_VEN] = channel;	//Samsung req
    wavelengthMonitoring(channel);
    checkSumCalculateForA0Base();
}


