/***********************************************************************//**
 * @file		VTimer.c
 * @brief		Controller used to create Virtual Timer
 * @version	1.0
 * @date		25. Feb. 2011
 * @author	ChipFC Team
 *
 * 				Service used to create Virtual Timer
 * @todo		Change period for VTimer to 1ms instead of 10ms
 **********************************************************************/
#include "VTimer.h"

/**
 *	@defgroup VTimerController	Virtual Timer Controller
 *	@ingroup  Controller
 *	@{
 */

/** 		timerPeriod: Array contain the current value for each Virtual Timer
 *	@par	if timerPeriod[i] = 0 ==> Timer i is free and can be reused
 *  @par	if timerPeriod[i] = 1 ==> Timer i is fired and is waiting to be call VTimerOut to be reused
 */
volatile uint32_t timerPeriod[NUMBER_OF_VIRTUAL_TIMER];
static uint8_t ServiceIndex = 0;

///Init Virtual Timer Controller
void VTimerInitController(void)
{
	uint8_t i = 0;
	for(i = 0; i< NUMBER_OF_VIRTUAL_TIMER; i++)
	{
		timerPeriod[i] = 0;
	}
}
/********************************************************************//**
 * @brief		Check if an Virtual Timer is fired or not
 * @param[in]	TimerId: Id of VTimer that you want to check
 * @return
 *				- 1 : Fired
 *				- 0 : Not Fired
 *********************************************************************/
uint8_t VTimerIsFired(uint8_t TimerId)
{
	if (timerPeriod[TimerId] == 1)
		return 1;
	else return 0;
}

/********************************************************************//**
 * @brief		Check if an Virtual Timer is fired or not
 * @param[in]	timerId: Id of VTimer that you want to check
 * @return
 *				- 1 : Fired
 *				- 0 : Not Fired
 *********************************************************************/
uint8_t VTimerOut(uint8_t timerId)
{
	if (timerPeriod[timerId] == 1)
		return 1;
	else return 0;
}
/********************************************************************//**
 * @brief		Virtual Timer Service to update VTimer status
 * @param[in]	None
 * @warning		This Service has to be called from SysTick_Handler()
 * @return 		None
 *********************************************************************/
void VTimerService(void)
{	
	for(ServiceIndex = 0; ServiceIndex < NUMBER_OF_VIRTUAL_TIMER; ServiceIndex++)
	{
		if (timerPeriod[ServiceIndex] > 1 ){
			timerPeriod[ServiceIndex] --;
		}
	}
}
/********************************************************************//**
 * @brief		Setup a VTimer to fire after a period
 * @param[in]	timerId: ID of VTimer
 * @param[in]	period: VTimer will fire after this period(ms)
 * @return 			- 0 : FAIL
 *					- 1 : SUCCESS
 *********************************************************************/
uint8_t VTimerSet(uint8_t timerId, uint32_t period)
{
	//if (timerPeriod[timerId] >0) return 0;
	timerPeriod[timerId] = period+1;
	return 1;
}
/********************************************************************//**
 * @brief		Get ID of a Free Virtual Timer
 * @param[in]	None
 * @warning		You have only a NUMBER_OF_VIRTUAL_TIMER
 * @return 		ID of a Free Virtual Timer
 *********************************************************************/
uint8_t VTimerGetID()
{
	uint8_t i = 1;
	for(i = 1; i< NUMBER_OF_VIRTUAL_TIMER; i++)
	{
		if (timerPeriod[i] == 0)
		{
			timerPeriod[i] = 1;
			return i;
		}
	}
	return 0;
}
/********************************************************************//**
 * @brief		Release a VTimer that you don't use anymore
 * @param[in]	timerId: ID of VTimer that you want to release
 * @warning		You have only a NUMBER_OF_VIRTUAL_TIMER, so release VTimer immediatelly if you
 				don't you it anymore
 * @return 		None
 *********************************************************************/
void VTimerRelease(uint8_t timerId)
{
	timerPeriod[timerId] = 0;
}
/********************************************************************//**
 * @brief		Delay milisecond
 * @param[in]	period: time you want to delay (ms)
 * @warning		This function depends on System Tick to run, so don't call it
 *				from SysTick_Handler()
 *
 *				Period has to above 10ms because System Tick is 10ms
 * @return 		None
 *********************************************************************/
void DelayMs(uint32_t ms)
{
	timerPeriod[0] = 1 + ms;
	while (timerPeriod[0] != 1);
}

int32_t VTimerGetTimerPeriod(uint8_t timerId)
{
	return timerPeriod[timerId];
}

/**
 * @}
 */
