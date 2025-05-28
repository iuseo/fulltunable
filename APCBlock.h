//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
//#include <intrinsics.h>
//#include <iods4835.h>


#ifndef __APCBlock_H_
#define __APCBlock_H_

void APC_Init(unsigned char Channel, unsigned int Set_MAXBIAS, unsigned int Set_INITBIAS, unsigned int Set_ISTEP);

void APC_Init_MAXBIAS(unsigned char Channel, unsigned int Set_MAXBIAS);
void APC_Init_INITBIAS(unsigned char Channel, unsigned int Set_INITBIAS);
void APC_Init_ISTEP(unsigned char Channel, unsigned int Set_ISTEP);

unsigned char Get_APCENABLE(unsigned char Channel);
void Set_APCENABLE(unsigned char Channel, unsigned char EN);
#define APCDISABLE 	0
#define APCENABLE 	1

unsigned short Get_BIASREG(unsigned char Channel);

void Set_APC(unsigned char Channel, unsigned int APCdata);
void Set_Bias(unsigned short Biasdata);
void Start_APC_Conv(unsigned short Biasdata);

void StatusMachine_APC(unsigned char Channel, unsigned char *rawdata);

extern void interruptMask(void);

#define APCBlock1			1
#define APCBlock2			2
#define APCBlock3			3
#define APCBlock4			4


#define APC_TARGET			0
#define MAXBIAS				1
#define INIT_BIAS			2
#define I_BIAS				3
#define BIASREG				4


#endif












