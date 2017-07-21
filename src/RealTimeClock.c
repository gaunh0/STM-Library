#include "RealTimeClock.h"

RTC_TimeTypeDef sTime;
RTC_DateTypeDef DateToUpdate;
RTC_HandleTypeDef hrtc;


  /* Buffers used for displaying Time and Date */


uint8_t day_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

void MX_RTC_Init(void){
	/**Initialize RTC Only */
	hrtc.Instance = RTC;
	hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
	hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
	if (HAL_RTC_Init(&hrtc) != HAL_OK){
		Error_Handler();
  	}
} 
void RealTime_Init(uint8_t year,uint8_t month,uint8_t date,uint8_t weekday,uint8_t hour,uint8_t minute,uint8_t second){
	uint32_t Timestamp=0;	
	
	sTime.Hours = hour;
	sTime.Minutes = minute;
	sTime.Seconds = second;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK){
		Error_Handler();
	}

	DateToUpdate.WeekDay = weekday;
	DateToUpdate.Month = month;
	DateToUpdate.Date = date;
	DateToUpdate.Year = year;

	if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK){
		Error_Handler();
	}
	uint16_t i;
	for (i=1970;i<(2000+DateToUpdate.Year);i++){
		if (CheckLeapYear(i)){
			Timestamp += 366;
		}
		else Timestamp += 365;
	}

	if (CheckLeapYear(DateToUpdate.Year)){
		for ( i = 1;i<DateToUpdate.Month;i++){
			if (i==2) {  // January
				Timestamp += 29;
			}
			else Timestamp += day_in_month[i-1];
		}
	}
	else {
		for (i = 1;i<DateToUpdate.Month;i++){
			Timestamp += day_in_month[i-1];
		}
	}
	Timestamp = Timestamp + DateToUpdate.Date -1;  // -1 vi tinh tu ngay 1/1/1970, tru di 1 ngay
	Timestamp = Timestamp*24;
	Timestamp = Timestamp*3600;		
	Timestamp = Timestamp + sTime.Hours*3600 + sTime.Minutes*60 + sTime.Seconds;
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,Timestamp);
}

uint8_t CheckLeapYear(uint16_t _year){
	if (((_year%4 == 0) && (_year%100 != 0))|| (_year%400 == 0)){
		return 1;
	}
	else return 0;
} 


uint32_t GetAndConvertToTimestamp(){
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, FORMAT_BCD);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, FORMAT_BCD);
  
	uint32_t Timestamp = 0;
	uint16_t i;
	for (i=1970;i<(2000+sdatestructureget.Year);i++){
		if (CheckLeapYear(i)){
			Timestamp += 366;
		}
		else Timestamp += 365;
	}
	
	if (CheckLeapYear(sdatestructureget.Year)){
		for ( i = 1;i<sdatestructureget.Month;i++){
			if (i==2) {  // January
				Timestamp += 29;
			}
			else Timestamp += day_in_month[i-1];
		}
	}
	else {
		for (i = 1;i<sdatestructureget.Month;i++){
			Timestamp += day_in_month[i-1];
		}
	}
	Timestamp = Timestamp + sdatestructureget.Date -1;  // -1 vi tinh tu ngay 1/1/1970, tru di 1 ngay
	Timestamp = Timestamp*24;
	Timestamp = Timestamp*3600;		
	Timestamp = Timestamp + stimestructureget.Hours*3600 + stimestructureget.Minutes*60 + stimestructureget.Seconds;
	return Timestamp;
}
void RTC_TmeStampShow(void)
{
	printf("Timestamp= %d\r\n",HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1));

}
void RTC_CalendarShow()
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, FORMAT_BCD);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, FORMAT_BCD);
  /* Display time Format : hh:mm:ss */
  printf("%02d:%02d:%02d \n",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  printf("%02d-%02d-%02d \n",sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year); 
	
} 

