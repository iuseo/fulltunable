#ifndef _APPDCDC_H_
#define _APPDCDC_H_

#define DCDC_16_V							16500		/* 1.65V */
#define DCDC_30_V							30000		/* 3.0V */

void initializeBuck(uint16 channel, uint16 target);
void Buck_Init1(unsigned int Channel, unsigned int Target);
void Buck_Init2(unsigned int Channel, unsigned int Target);
#endif
