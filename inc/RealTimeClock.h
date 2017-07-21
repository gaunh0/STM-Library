#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include "stm32f1xx_hal.h"


void MX_RTC_Init(void);
void RealTime_Init(uint8_t year,uint8_t month,uint8_t date,uint8_t weekday,uint8_t hour,uint8_t minute,uint8_t second);
uint8_t CheckLeapYear(uint16_t _year);
uint32_t GetAndConvertToTimestamp(void);
void RTC_TmeStampShow(void);
void RTC_CalendarShow(void);


#endif

