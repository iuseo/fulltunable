/*
 * FILE NAME            : appUartQueue.c
 * DATE                 : December 02 2019
 * AUTHOR               : DongJun Jeong, djjeong@lightron.co.kr
 * HISTORY              : December 02 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 *                                                                          
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include "main.h"
#include "appUartQueue.h"

/*
 * Name         : initializeUartQueue
 * Description  : 
 * Return Value : void
 * Parameter    : QUEUE_TYPE_T
 * Author       : Dongjun jeong, djjeong@lightron.co.kr
 * Date         : December 02 2019
 * History      : 
 */
void initializeUartQueue(UART_QUEUE_TYPE_T *queue)
{
	queue->front = 0;
	queue->rear = 0;
}

/*
 * Name         : isUartQueueFull
 * Description  : 
 * Return Value : bool
 * Parameter    : UART_QUEUE_TYPE_T
 * Author       : Dongjun jeong, djjeong@lightron.co.kr
 * Date         : December 02 2019
 * History      : 
 */
bool isUartQueueFull(UART_QUEUE_TYPE_T *queue)
{
	if(queue->front == ((queue->rear + 1) % UART_QUEUE_SIZE_MAX)) {
		return true;
	} else {
		return false;
	}
}

/*
 * Name         : isUartQueueEmpty
 * Description  : 
 * Return Value : bool
 * Parameter    : UART_QUEUE_TYPE_T
 * Author       : Dongjun jeong, djjeong@lightron.co.kr
 * Date         : December 02 2019
 * History      : 
 */
bool isUartQueueEmpty(UART_QUEUE_TYPE_T *queue)
{
	if(queue->front == queue->rear) {
		return true;
	} else {
		return false;
	}
}

/*
 * Name         : putMessage
 * Description  : 
 * Return Value : void
 * Parameter    : UART_QUEUE_TYPE_T, uint8
 * Author       : Dongjun jeong, djjeong@lightron.co.kr
 * Date         : December 02 2019
 * History      : 
 */
bool enqueueUartData(UART_QUEUE_TYPE_T *queue, uint8 data)
{
	if(isUartQueueFull(queue) == false) {
		queue->rear = (queue->rear + 1) % UART_QUEUE_SIZE_MAX;
		queue->uartData[queue->rear] = data;
                return true;
	} else {
		;
	}
        return false;
}

/*
 * Name         : dequeueUartData
 * Description  : 
 * Return Value : uint8
 * Parameter    : QUEUE_TYPE_T
 * Author       : Dongjun jeong, djjeong@lightron.co.kr
 * Date         : December 02 2019
 * History      : 
 */
uint8 dequeueUartData(UART_QUEUE_TYPE_T *queue)
{
	uint8 data = 0;

	if(isUartQueueEmpty(queue) == false) {
		queue->front = (queue->front + 1) % UART_QUEUE_SIZE_MAX;
		data = queue->uartData[queue->front];
	} else {
		;
	}
	return data;
}

