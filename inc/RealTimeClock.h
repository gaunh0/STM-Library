#ifndef REALTIMECLOCK_H
#define REALTIMECLOCK_H

#include "stm32f1xx_hal.h"


void MX_RTC_Init(void);
void RealTime_Init(uint32_t k);
uint8_t CheckLeapYear(uint16_t _year);
uint32_t GetAndConvertToTimestamp(void);

void RTC_CalendarShow(void);
void ConvertTimestampToDate(uint32_t k);
void SaveDataToBKR(uint32_t BackupRegister,uint16_t data);
uint16_t GetDataFromBKR(uint32_t BackupRegister);
void RealTime_Init1(void);

#endif

