#include "RealTimeClock.h"
#include "time.h"
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
void RealTime_Init(uint32_t k){
  
  uint32_t ss, s, h, d, m, epochtime, day, month, year, x, b, c, e;
	ss = k % 86400;
	h = ss / 3600;
	m = (ss % 3600) / 60;
	s = k % 60;
	
	epochtime  = k / 86400;
	x = (epochtime * 4 + 102032) / 146097 + 15;
	b = epochtime + 2442113 + x - (x / 4);
	c = (b * 20 - 2442) / 7305;
	d = b - 365 * c - c / 4;
	e = d * 1000 / 30601;
	day = d - e * 30 - e * 601 / 1000;
	
	if (e < 14)
	{
		year = c - 4716;
		month = e - 1;
	}
	else
	{
		year = c - 4715;
		month = e - 13;
	}

	sTime.Hours = h;
	sTime.Minutes = m;
	sTime.Seconds = s;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK){
		Error_Handler();
	}

  DateToUpdate.Date = day;
  DateToUpdate.Month = month;
  DateToUpdate.Year = year-2000;
  
  printf("------------------------------------------------------\n");
  printf("%dh:%dm:%ds   \n", sTime.Hours, sTime.Minutes, sTime.Seconds);
  printf("%d/%d/20%d \n", DateToUpdate.Date, DateToUpdate.Month, DateToUpdate.Year);

  
	if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK){
		Error_Handler();
	}
	uint16_t i;
  uint32_t Timestamp=0;	
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
	printf("TimeStamp after caculate %d \r\n",Timestamp);
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
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  
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


void RTC_CalendarShow()
{
  RTC_DateTypeDef sdatestructureget;
  RTC_TimeTypeDef stimestructureget;
  
  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display time Format : hh:mm:ss */
  printf("%02d:%02d:%02d \n",stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
  /* Display date Format : mm-dd-yy */
  printf("%02d-%02d-%02d \n",sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year); 
} 
void ConvertTimestampToDate(uint32_t k)
{

	uint32_t ss, s, h, d, m, epochtime, day, month, year, x, b, c, e;
	k = k + 25200;
	ss = k % 86400;//not enough 1day
	h = ss / 3600;//so gio hien tai
	m = (ss % 3600) / 60;// so phut hien tai
	s = k % 60;//so giay hien tai
	
	epochtime  = k / 86400;
	x = (epochtime * 4 + 102032) / 146097 + 15;
	b = epochtime + 2442113 + x - (x / 4);
	 c = (b * 20 - 2442) / 7305;
	 d = b - 365 * c - c / 4;
	 e = d * 1000 / 30601;
	day = d - e * 30 - e * 601 / 1000;
	
	if (e < 14)
	{
		year = c - 4716;
		month = e - 1;
		printf("%dh:%dm:%ds   ", h, m, s);
		printf("%d/%d/%d  GMT+7.00\r\n", day, month, year);
	}
	else
	{
		year = c - 4715;
		month = e - 13;
		printf("%dh:%dm:%ds   ", h,m,s);
		printf("%d/%d/%d  GMT+7.00\r\n", day,month,year);
	}
}
void SaveDataToBKR(uint32_t BackupRegister,uint16_t data)
{
  RTC_HandleTypeDef RtcHandle;
  RtcHandle.Instance = RTC;
  HAL_RTCEx_BKUPWrite(&RtcHandle,BackupRegister,data);
}
uint16_t GetDataFromBKR(uint32_t BackupRegister)
{
  RTC_HandleTypeDef RtcHandle;
  RtcHandle.Instance = RTC;
  return HAL_RTCEx_BKUPRead(&RtcHandle,BackupRegister);
}
void RealTime_Init1(void){

  time_t rawtime;
  struct tm * timeinfo;
  timeinfo = localtime (&rawtime);
  
	uint32_t Timestamp=0;	
	
	sTime.Hours = timeinfo->tm_hour;
	sTime.Minutes = timeinfo->tm_min;
	sTime.Seconds = timeinfo->tm_sec;

	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK){
		Error_Handler();
	}

	DateToUpdate.WeekDay = timeinfo->tm_wday;
	DateToUpdate.Month = timeinfo->tm_mon+1;
	DateToUpdate.Date = timeinfo->tm_mday;
	DateToUpdate.Year = (timeinfo->tm_year+1900)-2000;

	printf("%d:%d:%d     %d/%d/20%d \r\n",timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec,timeinfo->tm_mday,timeinfo->tm_mon,timeinfo->tm_year);
	
	if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BIN) != HAL_OK){
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


