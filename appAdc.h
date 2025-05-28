#ifndef _APPADC_H_
#define _APPADC_H_

#define REFERENCE_VOLTAGE_VREF						2400	/* 2.4V */

/* Thermistor Temperature */
#define CONFIG_THERMISTOR_TEMPERATURE				0
#define ADC16_GP20_TO_TEMP							16

/* External Booster with APD1(GP22) & ADC02(GP02) */  
#define CONFIG_EXT_BOOSTER_APD1_FB					12					
#define ADC02_GP02_APD1_FB						  	2					

/* RSSI */
#define CONFIG_RSSI									1
#define ADC05_GP05_RSSI								5

/* DCDC3 FB */
#define CONFG_DCDC3_LX3_FB							20					
#define ADC07_GP07_RSSI								7

/* VTEC : TEC+ */
#define CONFIG_VTEC									2
#define ADC10_GP12_VTEC								10

/* Tx Power with GP00 */
#define CONFIG_TX_POWER								3
#define ADC15_GP17_TX_POWER							15

/* Tx Wave with GP15 */
#define CONFIG_TX_WAVE								4
#define ADC01_GP01_TXWAVE							1

/* VAPD */
#define CONFIG_VAPD_MON								5
#define ADC23_GP27_VAPD_MON							23

/* ET_MON_*/
#define CONFIG_ET_MON								6
#define ADC12_GP14_ET_MON							12

/* VEE DCDC with APD3(GP17) & ADC17(GP21) */  
#define CONFIG_VEE_DCDC_APD2_FB						13					
#define ADC17_GP21_APD3_FB							17					

#define CONFIG_TEC1_THERMISTOR                   	16

#define CONFG_DCDC1_LX1_FB							18					/* DCDC1 FB */
#define ADC04_VCCDAC_DCDC1_FB						4					/* ADC04 with VCCDAC */
/* VCC_DIG(1.6V) with DCDC2(LX2) & ADC10(GP12) */
#define CONFG_DCDC2_LX2_FB							19					/* DCDC2 FB */
#define ADC10_GP12_DCDC2_FB							10					/* ADC10 with GP12 */
/* ADC Internal Offset */
#define CONFIG_ADC_OFFSET							22
#define CONFIG_VCC									23
#define ADC29_VCC_DIV								29

#define ADC08_LX2									8
#define ADC09_LX4									9

#define VCC_DIV_SCALE_RATIO							4
#define SUPPLY_VOLTAGE_CONVERSION_FACTOR			10		/* 100uV unit */
#define TX_BIAS_CURRENT_CONVERSION_FACTOR			0.002	/* 2uA unit */
#define TX_BIAS_CURRENT_MINIMUM_LIMIT				0
#define TX_BIAS_CURRENT_MAXIMUM_LIMIT				131		/* mA */

void initializeAdc(void);
void readAdcChannels(void);
void readInternalTemperature(void);
uint16 convertVoltageToAdcRaw(uint16 voltage);

#endif
