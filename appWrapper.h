#ifndef _APPWRAPPER_H_
#define _APPWRAPPER_H_

/* From timer.c */
void initializeTimer1(void);

/* From adc.c */
void adcConfig(uint16 index, uint16 priority, uint16 acquistion, uint16 average, uint16 bipSelect);
void adcChannelConfig(uint16 index, uint16 positive, uint16 negative);
uint16 adcConvert2mV(uint16 configNo);
sint16 adcConvert2mVBiPolar(uint16 configNo);
uint16 adcReadOut(uint16 configNo);

/* From dc_dc.c */
void piConfig(uint16 index, uint16 value);
uint16 calculateBuckTarget(uint16 target);
uint16 calculatePiBuckTarget(uint16 target);
void rampDcDcBuck(uint16 target);

/* From tec.c */
void configTec1(uint16 index, uint16 value);
void configTec2(uint16 index, uint16 value);
sint16 getTec2(uint16 index);

/* From flash.c */
uint16 convert2BytesToWord(uint8* address);
void convertWordTo2Bytes(uint16 data, uint8* address);
void convertWordTo4Bytes(float data, uint8* address);
void swapUint32(uint8* value);
void flashRead(uint16 destination, uint16 source, uint16 numberBytes);
void flashUpdate(uint16 source, uint16 destination, uint16 numberBytes);

/* From i2cm.c */
void initializeI2CMaster(uint8 master, uint16 clockRate, uint8 pin, uint8 useInterrupt);
uint8 i2cMWriteBytes(uint8 master, uint8 slaveAddress, uint8 registerAddress, const uint8 *data, uint8 numberBytes);
uint8 i2cMReadBytes(uint8 master, uint8 slaveAaddress, uint8 registerAddress, uint8 *data, uint8 numberBytes);

/* From i2cslave.c */
void initializeI2CSlave(void);

/* From DS_DAC.c */
void initializeDSDac(uint16 channel, uint16 config, uint16 fullScale);
void controlDsDacDuty(uint16 channel, uint16 duty);
void controlDsConfig(uint16 channel, uint16 regSel, uint16 data);
void dsDacOff(uint8 channel, uint8 onoff);
#endif
