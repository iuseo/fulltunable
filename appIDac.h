#ifndef _APPIDAC_H_
#define _APPIDAC_H_

void initializeIDac(uint8 channel, uint16 data);
void iDacOnOff(uint8 channel, uint8 onOff);
void controlAgcRegister(void);

#endif
