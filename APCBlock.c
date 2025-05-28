//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <iods4835.h>
#include <intrinsics.h>
#include <string.h>
#include "includes.h"
// #include "APCBlock.h"


void APC_Init(unsigned char Channel, unsigned int Set_MAXBIAS, unsigned int Set_INITBIAS, unsigned int Set_ISTEP)
{  
    switch ( Channel )
    {
		case APCBlock1 : 
			APCIDX1 = APC_TARGET;
			APCDATA1 = 0x0000;	// at power on
			
			APCIDX1 = MAXBIAS;
			APCDATA1 = Set_MAXBIAS;	
			
			APCIDX1 = INIT_BIAS;
			APCDATA1 = Set_INITBIAS;				
			
			APCIDX1 = I_BIAS;
			APCDATA1 = Set_ISTEP;					
		break;
	}
}
void APC_Init_MAXBIAS(unsigned char Channel, unsigned int Set_MAXBIAS)
{
    switch ( Channel )
    {
		case APCBlock1 : 			
			APCIDX1 = MAXBIAS;
			APCDATA1 = Set_MAXBIAS;	
		break;
	}
}
void APC_Init_INITBIAS(unsigned char Channel, unsigned int Set_INITBIAS)
{
    switch ( Channel )
    {
		case APCBlock1 : 			
			APCIDX1 = INIT_BIAS;
			APCDATA1 = Set_INITBIAS;	
		break;
	}	
}
void APC_Init_ISTEP(unsigned char Channel, unsigned int Set_ISTEP)
{
    switch ( Channel )
    {
		case APCBlock1 : 						
			APCIDX1 = I_BIAS;
			APCDATA1 = Set_ISTEP;		
		break;
	}		
}
unsigned char Get_APCENABLE(unsigned char Channel)
{
	unsigned char redata;
    switch ( Channel )
    {
		case APCBlock1 : 
			redata = APCCN1_bit.APC_EN;
		break;
	}		
	return redata;
}

void Set_APCENABLE(unsigned char Channel, unsigned char EN)
{
    switch ( Channel )
    {
		case APCBlock1 : 
		if ( EN != APCDISABLE )
		{	// APC Enable
			APCCN1_bit.APC_EN = 1; 				//enable APC loop
			APCCN1_bit.APCCONV = 1;				//start apc conversion
			
			APCCN1_bit.APCIE = 1;				//start apc conversion			
			interruptMask();
		}
		else
		{	// APC Disable
			APCCN1_bit.APC_EN = 0; 				//disable APC loop
			APCCN1_bit.APCCONV = 0;				//start apc conversion			
			
			APCCN1_bit.APCIE = 0;				//start apc conversion					
		}
		break;
	}		
}
unsigned short Get_BIASREG(unsigned char Channel)
{
	unsigned short redata;
    switch ( Channel )
    {
		case APCBlock1 : 
			APCIDX1 = BIASREG;
		break;
	}		
	redata = APCDATA1;			
	return redata;
}

void Set_APC(unsigned char Channel, unsigned int APCdata)
{
    switch ( Channel )
    {
		case APCBlock1 : 
			APCIDX1 = APC_TARGET;
			APCDATA1 = (unsigned short)APCdata;
		break;
	}	
}
void Set_Bias(unsigned short Biasdata)
{	// only for IDACS
	IDCD1 = Biasdata;
}

void StatusMachine_APC(unsigned char Channel,  unsigned char *rawdata)
{
	unsigned short newbias;
	switch ( Channel )
	{
		case APCBlock1 : 
			if (APCCN1_bit.APCI)
			{				
				newbias = Get_BIASREG(Channel);
				Set_Bias(newbias);
				RegWordTo2ByteSave(newbias, rawdata);
				APCCN1_bit.APCI = 0; //clear flag
				APCCN1_bit.APCCONV = 1; //start next conversion
			}
		break;	
	}
}













