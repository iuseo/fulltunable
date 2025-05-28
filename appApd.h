#ifndef _APPAPD_H_
#define _APPAPD_H_

void initializeApdBoost(void);
void initializeVeeDcDc(uint16 target);
void controlApd1Register(void);
//void apdOnOff(uint8 onOff);
uint16 convertApdRawToVoltage(uint16 adcRaw);

#endif
