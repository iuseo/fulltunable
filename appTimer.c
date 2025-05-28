/*
 * FILE NAME            : appTimer.c
 * DATE                 : March 21 2019
 * AUTHOR               : Joonghee Roh, jhroh@lightron.co.kr
 * HISTORY              : March 21 2019 created
 * FUNCTIONS            : N/A
 * DESCRIPTION          : 
 * REMARKS              : 
 * 
 * COPYRIGHT(C) LIGHTRON 2019
 */

#include <intrinsics.h>
#include <iods4835.h>

#include "main.h"
#include "board.h"
#include "appTimer.h"


static TIMER_T appTimer[MAX_TIMER_ID] = {{false, false, 0, 0, NULL_PTR}, };

/*
 * Name         : registerTimer
 * Description  : 
 * Return Value : void
 * Parameter    : uint8, timerHandler
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
void registerTimer(uint8 timerID, timerHandler handler)
{
	if(appTimer[timerID].handler == NULL_PTR) {
		appTimer[timerID].tick = 0;
		appTimer[timerID].handler = handler;
	} else {
		;
	}
}

/*
 * Name         : unregisterTimer
 * Description  : 
 * Return Value : void
 * Parameter    : uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
void unregisterTimer(uint8 timerID)
{
	if(appTimer[timerID].handler != NULL_PTR) {
		appTimer[timerID].handler = NULL_PTR;
	}
}

/*
 * Name         : startTimer
 * Description  : 
 * Return Value : void
 * Parameter    : uint8, uint32
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
void startTimer(uint8 timerID, uint32 period)
{
	if(appTimer[timerID].handler != NULL_PTR) {
		appTimer[timerID].start = true;
		appTimer[timerID].tick = 0;
		appTimer[timerID].period = period;
	} else {
		;
	}
}

/*
 * Name         : restartTimer
 * Description  : 
 * Return Value : void
 * Parameter    : uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 22 2019
 * History      : 
 */
void restartTimer(uint8 timerID)
{
	if(appTimer[timerID].handler != NULL_PTR) {
		appTimer[timerID].start = true;
		appTimer[timerID].tick = 0;
	} else {
		;
	}
}


/*
 * Name         : stopTimer
 * Description  : 
 * Return Value : void
 * Parameter    : uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
void stopTimer(uint8 timerID)
{
	if(appTimer[timerID].handler != NULL_PTR) {
		appTimer[timerID].start = false;
		appTimer[timerID].tick = 0;
		appTimer[timerID].period = 0;
	} else {
		;
	}
}

/*
 * Name         : clearTimer
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
void clearTimer(void)
{
	uint8 i = 0;

	for(i = 0; i < MAX_TIMER_ID; i++)
	{
		appTimer[i].start = false;
		appTimer[i].busy = false;
		appTimer[i].tick = 0;
		appTimer[i].period = 0;
		appTimer[i].handler = NULL_PTR;
	}
}

/*
 * Name         : getTimerPeriod
 * Description  : 
 * Return Value : uint32
 * Parameter    : uint8
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
uint32 getTimerPeriod(uint8 timerID)
{
	if(appTimer[timerID].handler != NULL_PTR) {
		return appTimer[timerID].period;
	} else {
		return 0;
	}
}

/*
 * Name         : tickTimer
 * Description  : Called every 5ms in the timer1 ISR
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */
void tickTimer(void)
{
	uint8 i = 0;

	for(i = 0; i < MAX_TIMER_ID; i++)
	{
		if((appTimer[i].start == false)
			|| (appTimer[i].handler == NULL_PTR)) {
			continue;
		}
		if(appTimer[i].busy == false) {
			appTimer[i].tick++;
		}
	}
}

/*
 * Name         : processTimer
 * Description  : 
 * Return Value : void
 * Parameter    : void
 * Author       : Joonghee Roh, jhroh@lightron.co.kr
 * Date         : March 21 2019
 * History      : 
 */

void processTimer(void)
{
	uint8 i = 0;

	for(i = 0; i < MAX_TIMER_ID; i++)
	{
		if((appTimer[i].start == false)
			|| (appTimer[i].handler == NULL_PTR)) {
			continue;
		} else {
			;
		}
		if(appTimer[i].tick >= appTimer[i].period) {
			appTimer[i].busy = true;
			appTimer[i].tick = 0;
			appTimer[i].handler();
			appTimer[i].busy = false;
		} else {
			;
		}
	}
}

void setTxUartTimerTick(void)
{
  appTimer[PROCESS_9ms_TIMER_ID].tick = 7;
}

