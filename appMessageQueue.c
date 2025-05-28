/*
 * FILE NAME            : appMessageQueue.c
 * DATE                 : March 22 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 22 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 *                                                                          
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include "main.h"
#include "appMessageQueue.h"

static QUEUE_TYPE_T messageQueue;

/*
 * Name         : initializeQueue
 * Description  : 
 * Return Value : void
 * Parameter    : QUEUE_TYPE_T
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
void initializeQueue(QUEUE_TYPE_T *queue)
{
	queue->front = 0;
	queue->rear = 0;
}

/*
 * Name         : isQueueFull
 * Description  : 
 * Return Value : bool
 * Parameter    : QUEUE_TYPE_T
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
static bool isQueueFull(QUEUE_TYPE_T *queue)
{
	if(queue->front == ((queue->rear + 1) % QUEUE_SIZE_MAX)) {
		return true;
	} else {
		return false;
	}
}

/*
 * Name         : isQueueEmpty
 * Description  : 
 * Return Value : bool
 * Parameter    : QUEUE_TYPE_T
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
static bool isQueueEmpty(QUEUE_TYPE_T *queue)
{
	if(queue->front == queue->rear) {
		return true;
	} else {
		return false;
	}
}

/*
 * Name         : sendMessage
 * Description  : 
 * Return Value : void
 * Parameter    : QUEUE_TYPE_T, EVENT_LIST, uint16
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
void sendMessage(QUEUE_TYPE_T *queue, EVENT_LIST event, uint16 data)
{
#if 0	/* jhRoh */
	QUEUE_MESSAGE_T message = {EVENT_NONE, 0};
#else
	QUEUE_MESSAGE_T message = {EVENT_NONE};
#endif

	if(messageReadyFlag == OFF) {
		return;
	}

	message.event = event;
//	message.uint16Data = data;	/* jhRoh */

	putMessage(queue, message);
}

/*
 * Name         : putMessage
 * Description  : 
 * Return Value : void
 * Parameter    : QUEUE_TYPE_T, QUEUE_MESSAGE_T
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
static void putMessage(QUEUE_TYPE_T *queue, QUEUE_MESSAGE_T message)
{
	if(isQueueFull(queue) == false) {
		queue->rear = (queue->rear + 1) % QUEUE_SIZE_MAX;
		queue->messageArray[queue->rear] = message;
	} else {
		;
	}
}

/*
 * Name         : getMessage
 * Description  : 
 * Return Value : QUEUE_MESSAGE_T
 * Parameter    : QUEUE_TYPE_T
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
QUEUE_MESSAGE_T getMessage(QUEUE_TYPE_T *queue)
{
#if 0	/* jhRoh */
	QUEUE_MESSAGE_T message = {EVENT_NONE, 0};
#else
	QUEUE_MESSAGE_T message = {EVENT_NONE};
#endif

	if(isQueueEmpty(queue) == false) {
		queue->front = (queue->front + 1) % QUEUE_SIZE_MAX;
		message = queue->messageArray[queue->front];
	} else {
		;
	}

	return message;
}

/*
 * Name         : getMessageQueue
 * Description  : 
 * Return Value : QUEUE_TYPE_T
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
QUEUE_TYPE_T *getMessageQueue(void)
{
	return &messageQueue;
}

/*
 * Name         : waitForEvent
 * Description  : 
 * Return Value : bool
 * Parameter    : EVENT_LIST
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : May 20 2019
 * History      : 
 */
bool waitForEvent(EVENT_LIST targetEvent)
{
#if 0	/* jhRoh */
	QUEUE_MESSAGE_T message = {EVENT_NONE, 0};
#else
	QUEUE_MESSAGE_T message = {EVENT_NONE};
#endif
	EVENT_LIST event = EVENT_NONE;
	uint16 count = 0;
	bool result = false;

	while(event != targetEvent)
	{
		swDelay(1);
		count++;
		message = getMessage(getMessageQueue());
		event = message.event;
		if(event == targetEvent) {
			result = true;
			break;
		}
		if(count > 1000) {
			result = false;
			break;
		}
	}

	return result;
}

