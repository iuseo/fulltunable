#ifndef _APPGPIO_H_
#define _APPGPIO_H_

#define EDGE_POSITIVE			0
#define EDGE_NEGATIVE			1

#define TX_FAULT_HOST		PD0_bit.PD00  
#define TX_DIS_HOST			PI1_bit.PI15
#define RS0_HOST			PI6_bit.PI63
#define RS1_HOST			PI1_bit.PI16
#define CDR_LOS				PI0_bit.PI03
#define Rx_LOS_OUT			PO0_bit.PO03

#define GN2146_RESET        PO1_bit.PO13        
#define TEST_PORT           PD0_bit.PD00  
#define UART_RX				PI0_bit.PI06



typedef enum
{
	GPIO_HIGH_Z_INPUT = 0,
	GPIO_WEAK_PULL_UP_INPUT,
	GPIO_PUSH_PULL_OUTPUT,
	GPIO_OPEN_DRAIN_OUTPUT
} GPIO_TYPE_E;

typedef enum
{
	GPIO_PORT_0 = 0,
	GPIO_PORT_1,
	GPIO_PORT_2
} GPIO_PORT_E;

typedef enum
{
	GPIO_PORT_NUMBER_0 = 0,
	GPIO_PORT_NUMBER_1,
	GPIO_PORT_NUMBER_2,
	GPIO_PORT_NUMBER_3,
	GPIO_PORT_NUMBER_4,
	GPIO_PORT_NUMBER_5,
	GPIO_PORT_NUMBER_6,
	GPIO_PORT_NUMBER_7,
	GPIO_PORT_NUMBER_MAX
} GPIO_PORT_NUMBER_E;

void initializeGpio(void);
void initializeExternalInterrupt(void);
void pollingGpio(void);
void configureGpio(GPIO_PORT_E port, GPIO_PORT_NUMBER_E portNumber, GPIO_TYPE_E type, bool interruptEnable);
void controlGpioPushPull(GPIO_PORT_E port, GPIO_PORT_NUMBER_E portNumber, uint8 onOff);
void controlGpioOpenDrain(GPIO_PORT_E port, GPIO_PORT_NUMBER_E portNumber, uint8 onOff);
#endif
