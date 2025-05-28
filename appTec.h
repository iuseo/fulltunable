#ifndef _APPTEC_H_
#define _APPTEC_H_

#define THERMISTOR_DIVIDER_RESISTANCE			10000		/* 10k Ohm */
#define THERMISTOR_CONSTANT_1					(1.094616 * pow(10, -3))
#define THERMISTOR_CONSTANT_2					(2.39948 * pow(10, -4))
#define THERMISTOR_CONSTANT_3					(6.31947 * pow(10, -8))

#define KELVIN_FORMULA							273.15

void initializeTecChannel(uint16 channel);
void inrushForTec(void);
void controlTecRegister(void);
float calculateThermistorResistance(uint16 voltage);
uint16 calculateThermistorTemperature(float resistance);

#endif
