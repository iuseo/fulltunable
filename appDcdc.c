/*
 * FILE NAME            : appDcdc.c
 * DATE                 : April 17 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : April 17 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <intrinsics.h>
#include <iods4835.h>
#include "main.h"
#include "includes.h"
#include "appTimer.h"
#include "appWrapper.h"
#include "appDcdc.h"

/*
 * Name         : initializeBuck
 * Description  : Enables one of DC-DC buck and sets the target voltage
 * Return Value : void
 * Parameter    : uint16 channel : Valid value DC_DC_CH1,DC_DC_CH2 and DC_DC_CH3
 *				  uint16 target : 16-bit value wrt 6.55V full scale
 * Author       : 
 * Date         : April 17 2019
 * History      : 
 */
void initializeBuck(uint16 channel, uint16 target)
{
  	uint16 buckDelayTime = 0;
	uint16 piEnable = 0x8000;
	if(channel <= DC_DC_CH3 ) {
		DCDC_SEL = channel;				/* Select one of DCDC */
		if(DCCN != 0x032E) {			/* If DCDC is not enabled then execute the below code */
			NMOS_ON_ST = __swap_bytes(0x0000); 
            NMOS_ON = __swap_bytes(0x0000); 
            PMOS_ON_ST = __swap_bytes(0x6900);
            PMOS_ON = __swap_bytes(0x1800);
            IZ_DAC = __swap_bytes(0x0302);

            DCDC_OPT = __swap_bytes(0x3000);            
            piConfig(PI_INDEX_KP, __swap_bytes(0x6400) );
            piConfig(PI_INDEX_KI, __swap_bytes(0x1027) );
            piConfig(PI_INDEX_SHIFT, __swap_bytes(0x3700) );
            piConfig(PI_INDEX_OP_CLAMP, __swap_bytes(0xFF7F) );			
			if(piEnable & 0x8000) {
				piConfig(PI_INDEX_SETPOINT, calculatePiBuckTarget(target));            
				PICNT = piEnable;                                         
				DCCN = 0x032E;			/* Digital control enable */
			} else {
				target = calculateBuckTarget(target);	/* Convert the input value into Buck Target */
				if(target < START_BUCK) {
					VRDCDC = target;
				} else {
					VRDCDC = START_BUCK;              
				}
				DCCN = 0x032E;			/* DC-DC Enable */
				buckDelayTime = Reg2ByteToWordLoad(&A2_DEBUGTAB[BUCK_delay_time]);
				swDelay(buckDelayTime);			/* Delay to allow startup */

				if(target > START_BUCK) {
				/* Imnportant Note: */
				/* The DC-DC ramp should allow to startup DC for >200mA startup load for various DC_DC target output */
					rampDcDcBuck(target);                                       
				}
			}		
		}			
	}
  /*
  unsigned int dcdc;

	 dcdc = DCDC_SEL; 	
	TECCN1 = 0;

	if(channel <= DC_DC_CH3 ) {
		DCDC_SEL = channel;				
		if(DCCN != 0x032E) {			
			NMOS_ON_ST = __swap_bytes(0x0000); 
            NMOS_ON = __swap_bytes(0x0000); 
            PMOS_ON_ST = __swap_bytes(0x6900);
            PMOS_ON = __swap_bytes(0x1800);
            IZ_DAC = __swap_bytes(0x0302);

            DCDC_OPT = __swap_bytes(0x3000);            
            piConfig(PI_INDEX_KP, __swap_bytes(0x6400) );
            piConfig(PI_INDEX_KI, __swap_bytes(0x1027) );
            piConfig(PI_INDEX_SHIFT, __swap_bytes(0x3700) );
            piConfig(PI_INDEX_OP_CLAMP, __swap_bytes(0xFF7F) );			
			piConfig(PI_INDEX_SETPOINT, calculatePiBuckTarget(target));            
			PICNT = 0x8000;				
			DCCN = 0x032E;				
		}			
	}

	DCDC_SEL = dcdc;
*/
}

void Buck_Init1(unsigned int Channel, unsigned int Target)
{
	unsigned int PI_Enable = 0x8000;
        INT16U Buff;
        if ( Channel <= DC_DC_CH3 )
        {
		DCDC_SEL = Channel;                                                     //Select One of DCDC
		if( DCCN != 0x032E )                                                    //if DCDC is not enable then execute the below code
		{        
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0x9D-0x80]);
			NMOS_ON_ST = __swap_bytes(Buff); 
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0x9F-0x80]);
                        NMOS_ON = __swap_bytes(Buff); 
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xA1-0x80]);
                        PMOS_ON_ST = __swap_bytes(Buff);
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xA3-0x80]);
                        PMOS_ON = __swap_bytes(Buff);
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xA5-0x80]);
                        IZ_DAC = __swap_bytes(Buff);

                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xA7-0x80]);
                        DCDC_OPT = __swap_bytes(Buff);    
                        
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xA9-0x80]);
                        PI_Config( PI_INDEX_KP, __swap_bytes(Buff) );
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xAB-0x80]);
                        PI_Config( PI_INDEX_KI, __swap_bytes(Buff) );
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xAD-0x80]);
                        PI_Config( PI_INDEX_SHIFT, __swap_bytes(Buff) );
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xAF-0x80]);
                        PI_Config( PI_INDEX_OP_CLAMP, __swap_bytes(Buff) );			
                        if( PI_Enable & 0x8000)
                        {
                              PI_Config( PI_INDEX_SETPOINT, CalcPIBuckTarget(Target) );            
                              PICNT = PI_Enable;                                       
                              DCCN = 0x032E;                                                    //Digital control enable          
                        }
                        else
                        {
				Target = CalcBuckTarget(Target);                                //Convert the input value into Buck Target

				if(Target < START_BUCK)
				{
					VRDCDC = Target;
				}
				else
				{
					VRDCDC = START_BUCK;              
				}
				DCCN = 0x032E;                                                  //DC-DC Enable

				swDelay(2000);                                                 //Delay to allow startup

				if(Target > START_BUCK)
				{
				//Imnportant Note:
				//The DC-DC ramp should allow to startup DC for >200mA startup load for various DC_DC target output
					RampDCDCBuck(Target);                                       
				}
			}		
		}			
	}
}

void Buck_Init2(unsigned int Channel, unsigned int Target)
{
	unsigned int PI_Enable = 0x8000;
        INT16U Buff;
        if ( Channel <= DC_DC_CH3 )
        {
		DCDC_SEL = Channel;                                                     //Select One of DCDC
		if( DCCN != 0x032E )                                                    //if DCDC is not enable then execute the below code
		{        
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xBD-0x80]);
			NMOS_ON_ST = __swap_bytes(Buff); 
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xBF-0x80]);
                        NMOS_ON = __swap_bytes(Buff); 
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xC1-0x80]);
                        PMOS_ON_ST = __swap_bytes(Buff);
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xC3-0x80]);
                        PMOS_ON = __swap_bytes(Buff);
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xC5-0x80]);
                        IZ_DAC = __swap_bytes(Buff);

                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xC7-0x80]);
                        DCDC_OPT = __swap_bytes(Buff);    
                        
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xC9-0x80]);
                        PI_Config( PI_INDEX_KP, __swap_bytes(Buff) );
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xCB-0x80]);
                        PI_Config( PI_INDEX_KI, __swap_bytes(Buff) );
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xCD-0x80]);
                        PI_Config( PI_INDEX_SHIFT, __swap_bytes(Buff) );
                        Buff = Reg2ByteToWordLoad(&A2_UserData[0xCF-0x80]);
                        PI_Config( PI_INDEX_OP_CLAMP, __swap_bytes(Buff) );			
                        if( PI_Enable & 0x8000)
                        {
                              PI_Config( PI_INDEX_SETPOINT, CalcPIBuckTarget(Target) );            
                              PICNT = PI_Enable;                                       
                              DCCN = 0x032E;                                                    //Digital control enable          
                        }
                        else
                        {
				Target = CalcBuckTarget(Target);                                //Convert the input value into Buck Target

				if(Target < START_BUCK)
				{
					VRDCDC = Target;
				}
				else
				{
					VRDCDC = START_BUCK;              
				}
				DCCN = 0x032E;                                                  //DC-DC Enable

				swDelay(2000);                                                 //Delay to allow startup

				if(Target > START_BUCK)
				{
				//Imnportant Note:
				//The DC-DC ramp should allow to startup DC for >200mA startup load for various DC_DC target output
					RampDCDCBuck(Target);                                       
				}
			}		
		}			
	}
}

