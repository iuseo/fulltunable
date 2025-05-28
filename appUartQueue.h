#ifndef _APPUARTQUEUE_H_
#define _APPUARTQUEUE_H_

#define UART_QUEUE_SIZE_MAX		64

typedef struct
{
	uint8 uartData[UART_QUEUE_SIZE_MAX];
	uint8 front;
	uint8 rear;
} UART_QUEUE_TYPE_T;

void initializeUartQueue(UART_QUEUE_TYPE_T *queue);
bool isUartQueueFull(UART_QUEUE_TYPE_T *queue);
bool isUartQueueEmpty(UART_QUEUE_TYPE_T *queue);
bool enqueueUartData(UART_QUEUE_TYPE_T *queue, uint8 data);
uint8 dequeueUartData(UART_QUEUE_TYPE_T *queue);
#endif
