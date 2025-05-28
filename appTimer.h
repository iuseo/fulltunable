#ifndef _APPTIMER_H_
#define _APPTIMER_H_

#define TIMER_1ms									1			/* 1 * 1ms = 1ms */
#define TIMER_5ms									(5 + 1)		/* 25 * 1ms = 5ms */
#define TIMER_9ms									(9 + 1)		/* 25 * 1ms = 9ms */
#define TIMER_25ms									(25 + 1)	/* 25 * 1ms = 5ms */
#define TIMER_50ms									(50 + 1)	/* 50 * 1ms = 50ms */
#define TIMER_100ms									(100 + 1)	/* 100 * 1ms = 100ms */

typedef void (*timerHandler)(void);

typedef struct
{
	bool start;
	bool busy;
	uint32 tick;
	uint32 period;
	timerHandler handler;
} TIMER_T;

typedef enum
{
	MESSAGE_HANDLER_TIMER_ID = 0U,
    PROCESS_9ms_TIMER_ID,
	PROCESS_25ms_TIMER_ID,
	PROCESS_50ms_TIMER_ID,
	PROCESS_100ms_TIMER_ID,
	MAX_TIMER_ID
} TIMER_ID;

void registerTimer(uint8 timerID, timerHandler handler);
void unregisterTimer(uint8 timerID);
void startTimer(uint8 timerID, uint32 period);
void restartTimer(uint8 timerID);
void stopTimer(uint8 timerID);
void clearTimer(void);
uint32 getTimerPeriod(uint8 timerID);
void tickTimer(void);
void processTimer(void);
void setTxUartTimerTick(void);
#endif
