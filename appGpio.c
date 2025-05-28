/*
 * FILE NAME            : appGpio.c
 * DATE                 : April 16 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : April 16 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <iods4835.h>
#include "main.h"
#include "board.h"
#include "includes.h"
#include "appTimer.h"
#include "appMessageQueue.h"
#include "appWrapper.h"
#include "appGpio.h"
#include "appUart.h"
#include "appSmartTune.h"
#include "sff8690.h"
#include "tl6100.h"
//#include "max5112.h"
/*
 * Name         : initializeGpio
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 16 2019
 * History      : 
 */
void  initializeGpio(void)
{
/********************************************************************************/
/*	GPIO : Port 0																*/
/********************************************************************************/
/*	Port0.0 : TX_FAULT					Output, OpenDrain						*/
/*	Port0.1 : TO_TEMP					ADC1									*/
/*	Port0.2 : APD_FB					APD DC-DC Feedback, ADC2				*/
/*	Port0.3 : RX_LOS					Input, High Impedance					*/
/*	Port0.4 : N/A																*/
/*	Port0.5 : RSSI					        ADC5								*/
/*	Port0.6 : AMCC_MCU					Input, High Impedance					*/	
/*	Port0.7 : BUCK_1.6V_FB					ADC7              		        	*/
/********************************************************************************/
	
        /* TX_FAULT */
        //configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_0,GPIO_OPEN_DRAIN_OUTPUT,false);
        PO0 &= ~0x01;
        PD0 |= 0x01;
        PD0 &= ~0x01;
       
        /* CDR LOS */
//       configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_3,GPIO_HIGH_Z_INPUT,false);      
#if 1	
		IEN0 |= 0x08;
        PD0 &= ~0x08;
        PO0 &= ~0x08;	
//         configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_3,GPIO_PUSH_PULL_OUTPUT,false); //testcode
#endif

#if 0	// LOS pin UART Rx debugging port enable
		PD0 |= 0x08;
        PO0 |= 0x08;
        PO0_bit.PO03 = 1;
#endif	
		
		
        /* AMCC_MCU */
       //configureGpio(GPIO_PORT_0,GPIO_PORT_NUMBER_6,GPIO_HIGH_Z_INPUT,false); 
        IEN0 |= 0x40;
        PD0 &= ~0x40;
        PO0 &= ~0x40; 
       
/********************************************************************************/
/*	GPIO : Port 1																*/
/********************************************************************************/
/*	Port1.0 : MSDA																*/
/*	Port1.1 : MSCL																*/
/*	Port1.2 : VTEC_MON			        ADC10									*/
/*	Port1.3 : RSTB_2146					Output, Push Pull						*/
/*	Port1.4 : ET_MON					ADC12									*/
/*	Port1.5 : TX_DISABLE			    Input, High Impedance					*/
/*	Port1.6 : Rate Select 1				Input, High Impedance					*/
/*	Port1.7 : MZ_PWR_ADC				ADC15									*/
/********************************************************************************/
	
        /* RSTB_2146 */
        //configureGpio(GPIO_PORT_1,GPIO_PORT_NUMBER_3,GPIO_PUSH_PULL_OUTPUT,false);
        PD1 |= 0x08;
        PO1 |= 0x08;
        //controlGpioPushPull(GPIO_PORT_1,GPIO_PORT_NUMBER_3, _OFF);
        PO1_bit.PO13 = 0;
        
        /* AMCC_ENABLE */	
        //configureGpio(GPIO_PORT_1,GPIO_PORT_NUMBER_4,GPIO_PUSH_PULL_OUTPUT,false);
        //PD1 |= 0x10;
        //PO1 |= 0x10;
        //controlGpioPushPull(GPIO_PORT_1,GPIO_PORT_NUMBER_4, _OFF);
        //PO1_bit.PO14 = 0;
        
        /* TX_DISABLE */
        //configureGpio(GPIO_PORT_1,GPIO_PORT_NUMBER_5,GPIO_HIGH_Z_INPUT,false);
        IEN1 |= 0x20;
        PD1 &= ~0x20;
        PO1 &= ~0x20;
        
        /* Rate Select 1 */
        //configureGpio(GPIO_PORT_1,GPIO_PORT_NUMBER_6,GPIO_HIGH_Z_INPUT,false);    
		IEN1 |= 0x40;
        PD1 &= ~0x40;
        PO1 &= ~0x40;
		/* Rate Select 1 TEST PIN */
//		configureGpio(GPIO_PORT_1,GPIO_PORT_NUMBER_6,GPIO_PUSH_PULL_OUTPUT,false);   //testcode
//      PO1_bit.PO16

/********************************************************************************/
/*	GPIO : Port 2																*/
/********************************************************************************/
/*	Port2.0 : WAVE_DIFF_POW_ADC			ADC16									*/
/*	Port2.1 : VEE_FB					VEE Feedback							*/
/*	Port2.2 : APD_PWM					APD1									*/
/*	Port2.3 : VEE_PWM					APD2									*/
/*	Port2.4 : AMCC_EN					Output, Push Pull						*/
/*	Port2.5 : SET_BIAS_L				DS_DAC5									*/
/*	Port2.6 : SET_BIAS_R				DS_DAC6                            		*/
/*	Port2.7 : VAPD_MON                	ADC23									*/
/********************************************************************************/
	
        /* Rate Select 0 */
        //configureGpio(GPIO_PORT_2,GPIO_PORT_NUMBER_4,GPIO_HIGH_Z_INPUT,false);
        //IEN2 |= 0x10;
        //PD2 &= ~0x10;
        //PO2 &= ~0x10;     
		/* AMCC_ENABLE */
		PD2 |= 0x10;
        PO2 |= 0x10;
        PO2_bit.PO24 = 0;
		
/********************************************************************************/
/*	LXn	*/
/********************************************************************************/
/*	LX1 : TEC+							TEC2+									*/
/*	LX2 : TEC-							TEC2-									*/
/*	LX3 : VCC_DIG						1.6V									*/
/*	LX4 : Rate Select 1					Input, High Impedance					*/
/********************************************************************************/
		/* Rate Select 0 */
		IEN6 |= 0x08;	
		/* Rate Select 0 TEST PIN */
		configureGpio(GPIO_PORT_2,GPIO_PORT_NUMBER_4,GPIO_PUSH_PULL_OUTPUT,false);
}

/*
 * Name         : initializeExternalInterrupt
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 29 2019
 * History      : 
 */
void initializeExternalInterrupt(void)
{
	if(TX_DIS_HOST) {
		EIES2_bit.EIES24 = EDGE_NEGATIVE;	/* Negative Edge */	
	} else {
		EIES2_bit.EIES24 = EDGE_POSITIVE;	/* Positive Edge */
	}
	if(RS0_HOST) {
		EIES2_bit.EIES26 = EDGE_NEGATIVE;	/* Negative Edge */	
	} else {
		EIES2_bit.EIES26 = EDGE_POSITIVE;	/* Positive Edge */
	}
    if(RS1_HOST) {
        EIES2_bit.EIES27 = EDGE_NEGATIVE;	/* Negative Edge */	
    } else {
        EIES2_bit.EIES27 = EDGE_POSITIVE;	/* Positive Edge */
    }
	if(CDR_LOS) {
		EIES0_bit.EIES07 = EDGE_NEGATIVE;	/* Negative Edge */	
	} else {
		EIES0_bit.EIES07 = EDGE_POSITIVE;	/* Positive Edge */
	}
}

/*
 * Name         : pollingGpio
 * Description  : Scheduled every 25ms in process25msTask()
 * Return Value : void
 * Parameter    : void
 * Author       : 
 * Date         : April 16 2019
 * History      : 
 */

extern bool DATA_RDY_SOFT;
void pollingGpio(void)
{

	I2CM_16ReadNBytes(I2CM_MASTER_1, GN2146_DEVICE_ADDRESS, LOSL_STATUS_REG_0, &A2_GN2146Sync[LOSL_STATUS_REG_0_Add], 1);

	   //gpioVerifyFunction(TX_FAULT_HOST,B2_H);
//  		if((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x05) || (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x70)) {
//		  if(A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x04) {			
//			 if((A2_DiagData[EXT_STATUS_CONTROL_ADD] & 0x08) == 0x08 || RS1_HOST == _ON) {
//			   TX_FAULT_HOST = _ON;
//			 } else {
//			   TX_FAULT_HOST = _OFF;
//			 }
//		  } else {
//			TX_FAULT_HOST = _ON;
//		  }		  
//		} else {
//		  TX_FAULT_HOST = _OFF;
//		}
        if((TX_DIS_HOST) || (A2_DiagData[STATUS_CONTROL_ADD] & B6_H)) {
            TX_FAULT_HOST = FAULT_OFF;    
        } else {
            if(((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x04) == 0x04) || (A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x01) || (A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x10)
               || ((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x20) == 0x20) || ((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x40) == 0x40)) {
              if((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x10) && ((A2_DEBUGTAB[FAULT_MASK] & 0x01) == 0)) {
                TX_FAULT_HOST = FAULT_ON;
              } else if(((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x20) == 0x20) && ((A2_DEBUGTAB[FAULT_MASK] & 0x01) == 0)) {
                TX_FAULT_HOST = FAULT_ON;
              } else if(((A2_TTOSA_PAGE_0x02[SFF_8690_A2_02_CURRENT_STATUS] & 0x40) == 0x40) && ((A2_DEBUGTAB[FAULT_MASK] & 0x01) == 0)) {
                TX_FAULT_HOST = FAULT_ON;
              } 
			  if((A2_DiagData[EXT_STATUS_CONTROL_ADD] & 0x08) == 0x08 || RS1_HOST == _ON) {
				  if(((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x04) == 0x04) && ((A2_DEBUGTAB[FAULT_MASK] & 0x02) == 0)) {
					 TX_FAULT_HOST = FAULT_ON;
				  } 
				  if((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x01) && ((A2_DEBUGTAB[FAULT_MASK] & 0x04) == 0)) {
				  	TX_FAULT_HOST = FAULT_ON;
				  }
			  } else {
			  	  if((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x01) && ((A2_DEBUGTAB[FAULT_MASK] & 0x04) == 0)) {
				  	TX_FAULT_HOST = FAULT_ON;	    		
				  }
			  }	  
//              if(((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x04) == 0x04) && ((A2_DiagData[EXT_STATUS_CONTROL_ADD] & 0x08) == 0x08 || RS1_HOST == _ON) && ((A2_DEBUGTAB[FAULT_MASK] & 0x02) == 0)) {  
//                TX_FAULT_HOST = FAULT_ON;
//              } 
//              if((A2_GN2146Sync[LOSL_STATUS_REG_0_Add] & 0x01) && ((A2_DEBUGTAB[FAULT_MASK] & 0x04) == 0)) {
//                TX_FAULT_HOST = FAULT_ON; 
//              }
            } else {
                TX_FAULT_HOST = FAULT_OFF;                
            }        
        }
     
        
        
        if(TX_FAULT_HOST) { //This fault is open drain output so, 1 : OFF, 0 : ON
			A2_DiagData[STATUS_CONTROL_ADD] &= B2_L;
		} else {
            A2_DiagData[STATUS_CONTROL_ADD] |= B2_H;
		}
			
		//gpioVerifyFunction(CDR_LOS,B1_H);
	
	
		if(CDR_LOS) {
			A2_DiagData[STATUS_CONTROL_ADD] |= B1_H;
		} else {
			uint8 amccMode=0;
			A2_DiagData[STATUS_CONTROL_ADD] &= B1_L;
//			amccMode = A2_UserData[MODULE_ID_SET] & 0x03;
//			if( (amccMode == AMCC_ENABLE) && (A2_UART_MEMORY[UART_ROD_STATE] == UART_ROD_IDLE)){
//				A2_UART_MEMORY[UART_AWS_SUCCESS_VARIABLE] = 0x1;
//			}
		}
		  
		if(TX_DIS_HOST) {
				A2_DiagData[STATUS_CONTROL_ADD] |= B7_H;
				TX_FAULT_HOST = FAULT_OFF;
		} else {
				A2_DiagData[STATUS_CONTROL_ADD] &= B7_L;
		}
		   
		if(RS0_HOST == _HIGH) {
			A2_DiagData[STATUS_CONTROL_ADD] |= B4_H;
		} else {
			A2_DiagData[STATUS_CONTROL_ADD] &= ~B4_H;
		}
	
//        if(A2_DEBUGTAB[SMART_TUNE_SELF_TUNING] & AMCC_TUNE) {
		if(RS1_HOST == _HIGH){
			A2_DiagData[STATUS_CONTROL_ADD] |= B5_H;
		} else {
			A2_DiagData[STATUS_CONTROL_ADD] &= ~B5_H;
		}
//        }
        
        if (DATA_RDY_SOFT != _OFF)
            A2_DiagData[STATUS_CONTROL_ADD] |= B0_H;		// 0000 | 000X
        else
            A2_DiagData[STATUS_CONTROL_ADD] &= ~B0_H;		// 110 | Bit 0
    // Data ready

}

/*
 * Name         : configureGpio
 * Description  : 
 * Return Value : void
 * Parameter    : GPIO_PORT_E, GPIO_PORT_NUMBER_E, GPIO_TYPE_E, bool
 * Author       : 
 * Date         : July 04 2019
 * History      : 
 */

void configureGpio(GPIO_PORT_E port, GPIO_PORT_NUMBER_E portNumber, GPIO_TYPE_E type, bool interruptEnable)
{
	uint8 shifedData = 0x00;

	if(portNumber >= GPIO_PORT_NUMBER_MAX) {
		return;
	}

	shifedData = (uint8)(0x01 << portNumber);

	switch(type)
	{
		case GPIO_HIGH_Z_INPUT :
		case GPIO_WEAK_PULL_UP_INPUT :
			if(port == GPIO_PORT_0) {
				IEN0 |= (uint8)shifedData;
				PD0 &= (uint8)~shifedData;
			} else if(port == GPIO_PORT_1) {
				IEN1 |= (uint8)shifedData;
				PD1 &= (uint8)~shifedData;
			} else if(port == GPIO_PORT_2) {
				IEN2 |= (uint8)shifedData;
				PD2 &= (uint8)~shifedData;
			}
			if(type == GPIO_HIGH_Z_INPUT) {
				if(port == GPIO_PORT_0) {
					PO0 &= (uint8)~shifedData;
				} else if(port == GPIO_PORT_1) {
					PO1 &= (uint8)~shifedData;
				} else if(port == GPIO_PORT_2) {
					PO2 &= (uint8)~shifedData;
				}
			} else if(type == GPIO_WEAK_PULL_UP_INPUT) {
				if(port == GPIO_PORT_0) {
					PO0 |= (uint8)shifedData;
				} else if(port == GPIO_PORT_1) {
					PO1 |= (uint8)shifedData;
				} else if(port == GPIO_PORT_2) {
					PO2 |= (uint8)shifedData;
				}
			}
			if(interruptEnable == true) {
				if(port == GPIO_PORT_0) {
					EIE0 |= (uint8)shifedData;
				} else if(port == GPIO_PORT_1) {
					EIE1 |= (uint8)shifedData;
				} else if(port == GPIO_PORT_2) {
					EIE2 |= (uint8)shifedData;
				}
			} else {
				if(port == GPIO_PORT_0) {
					EIE0 &= (uint8)~shifedData;
				} else if(port == GPIO_PORT_1) {
					EIE1 &= (uint8)~shifedData;
				} else if(port == GPIO_PORT_2) {
					EIE2 &= (uint8)~shifedData;
				}
			}
			break;
		case GPIO_PUSH_PULL_OUTPUT :
			if(port == GPIO_PORT_0) {
				PD0 |= (uint8)shifedData;
			} else if(port == GPIO_PORT_1) {
				PD1 |= (uint8)shifedData;
			} else if(port == GPIO_PORT_2) {
				PD2 |= (uint8)shifedData;
			}
			break;
		case GPIO_OPEN_DRAIN_OUTPUT :
			if(port == GPIO_PORT_0) {
				PO0 &= (uint8)~shifedData;
			} else if(port == GPIO_PORT_1) {
				PO1 &= (uint8)~shifedData;
			} else if(port == GPIO_PORT_2) {
				PO2 &= (uint8)~shifedData;
			}
			break;
		default :
			break;
	}
}


/*
 * Name         : controlGpioPushPull
 * Description  : 
 * Return Value : void
 * Parameter    : GPIO_PORT_E, GPIO_PORT_NUMBER_E, GPIO_TYPE_E, uint8
 * Author       : 
 * Date         : July 04 2019
 * History      : 
 */

void controlGpioPushPull(GPIO_PORT_E port, GPIO_PORT_NUMBER_E portNumber, uint8 onOff)
{
	uint8 shifedData = 0x00;

	if(portNumber >= GPIO_PORT_NUMBER_MAX) {
		return;
	}

	shifedData = (uint8)(0x01 << portNumber);

	if(port == GPIO_PORT_0) {
		(onOff == ON) ? (PO0 |= (uint8)shifedData) : (PO0 &= (uint8)~shifedData);
	} else if(port == GPIO_PORT_1) {
		(onOff == ON) ? (PO1 |= (uint8)shifedData) : (PO1 &= (uint8)~shifedData);
	} else if(port == GPIO_PORT_2) {
		(onOff == ON) ? (PO2 |= (uint8)shifedData) : (PO2 &= (uint8)~shifedData);
	}
}

/*
 * Name         : controlGpioOpenDrain
 * Description  : 
 * Return Value : void
 * Parameter    : GPIO_PORT_E, GPIO_PORT_NUMBER_E, GPIO_TYPE_E, bool
 * Author       : 
 * Date         : July 04 2019
 * History      : 
 */
void controlGpioOpenDrain(GPIO_PORT_E port, GPIO_PORT_NUMBER_E portNumber, uint8 onOff)
{
	uint8 shifedData = 0x00;

	if(portNumber >= GPIO_PORT_NUMBER_MAX) {
		return;
	}

	shifedData = (uint8)(0x01 << portNumber);

	if(port == GPIO_PORT_0) {
		(onOff == ON) ? (PD0 &= (uint8)~shifedData) : (PD0 |= (uint8)shifedData);
	} else if(port == GPIO_PORT_1) {
		(onOff == ON) ? (PD1 &= (uint8)~shifedData) : (PD1 |= (uint8)shifedData);
	} else if(port == GPIO_PORT_2) {
		(onOff == ON) ? (PD2 &= (uint8)~shifedData) : (PD2 |= (uint8)shifedData);
	}
}
