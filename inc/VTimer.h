#ifndef __VTIMER__
#define __VTIMER__

#include "stm32f1xx_hal.h"

#define NUMBER_OF_VIRTUAL_TIMER 		10

void VTimerInitController(void);
uint8_t VTimerIsFired(uint8_t timerId);
uint8_t VTimerOut(uint8_t timerId);
void VTimerService(void);
uint8_t VTimerSet(uint8_t timerId, uint32_t period);
uint8_t VTimerGetID(void);
void VTimerRelease(uint8_t timerId);
void DelayMs(uint32_t ms);
int32_t VTimerGetTimerPeriod(uint8_t timerId);
/**
 * @}
 */

#endif

