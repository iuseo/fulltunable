
#ifndef _SFF8472_H_
#define _SFF8472_H_

void convertAdcToDdm(void);
void checkAlarmWarning(uint8* diagData);
static uint16 convertVoltageToPower(uint8* address, float data);
static sint16 temperatureCalibration(sint16 temperature, uint8* address);
void setStatusControlBits(void);
void optimizeAlarmWarningFunction(sint16 data1, sint16 data2,uint8 addressOffset,uint8 value, uint8 index);
void optimizeUnsignedAlarmWarningFunction(uint16 data1, uint16 data2,uint8 addressOffset,uint8 value, uint8 index);
void controlRDTFunc(unsigned char status);
#endif
