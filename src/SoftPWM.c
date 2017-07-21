#include "SoftPWM.h"
#include "MBI5026.h"

extern TIM_HandleTypeDef htim2;
void TimerInitController(){	
	RCC_ClkInitTypeDef    clkconfig;
	uint32_t              uwTimclock = 0;
	uint32_t              uwPrescalerValue = 0;
	uint32_t              pFLatency;

	/* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz */
	
	HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	 __HAL_RCC_TIM2_CLK_ENABLE();

	 /* Get clock configuration */
	HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);

	/* Compute TIM1 clock */
	uwTimclock = HAL_RCC_GetPCLK2Freq();

	/* Compute the prescaler value to have TIM2 counter clock equal to 1MHz */
	uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000) - 1);

	/* Initialize TIM2 */
	htim2.Instance = TIM2;

	/* Initialize TIMx peripheral as follow:
	+ Period = [(TIM1CLK/1000) - 1]. to have a (1/1000) s time base.
	+ Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
	+ ClockDivision = 0
	+ Counter direction = Up
	*/
	htim2.Init.Period = (100000 / 1000) - 1;
	htim2.Init.Prescaler = uwPrescalerValue;
	htim2.Init.ClockDivision = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	if(HAL_TIM_Base_Init(&htim2) == HAL_OK)
	{
	/* Start the TIM time Base generation in interrupt mode */
		HAL_TIM_Base_Start_IT(&htim2);
	}
  
}


#define SOFTPWM_CHANNEL	16
#define MAX_PWM			100

uint8_t LedFadeValue;
uint8_t LedFadeDirection;
uint8_t LedIntervalTimer = 0;
uint8_t softcount = 0;
uint8_t compare[SOFTPWM_CHANNEL];
uint8_t compbuff[SOFTPWM_CHANNEL];
uint8_t SoftPWMChannelIndex;

uint8_t FadeValue[16];

void LedDimmer(uint8_t led,uint8_t _value){	
	if (_value>=100) _value = 100;
	compbuff[led] = _value;
}

void LedFadeValueInit(){	
	for (uint8_t i=0;i<16;i++){
		//FadeValue[i] = 96 - (i*6);
		FadeValue[i] = 100;
	}
}
void LedFadeValueChange(){
	for (uint8_t i=0;i<16;i++){
		FadeValue[i] = FadeValue[i] - 6;
		if (FadeValue[i] == 0){
			FadeValue[i] = 96;
		}		
	}
}
void LedFade(uint8_t led){
	LedDimmer(led,LedFadeValue);
}

void SoftPWMInitController(){
	for(uint8_t i=0 ;i<SOFTPWM_CHANNEL ; i++){
		compare[i] =  0;           // set default PWM values
		compbuff[i] = 0;          // set default PWM values
	}	
}

uint16_t IndexTemp1 = 0;
void SoftPWMService(){	
	softcount++;
	softcount = softcount % 101;

	if(softcount == 0){
		for (SoftPWMChannelIndex=0;SoftPWMChannelIndex<SOFTPWM_CHANNEL;SoftPWMChannelIndex++){
			compare[SoftPWMChannelIndex] = compbuff[SoftPWMChannelIndex];
			if (compbuff[SoftPWMChannelIndex] != 0){	
				IndexTemp1 |= (1<<SoftPWMChannelIndex);				
			}
			else {
				IndexTemp1 &= (~(1<<SoftPWMChannelIndex));
			}
		}		
	}
	for (SoftPWMChannelIndex=0;SoftPWMChannelIndex<SOFTPWM_CHANNEL;SoftPWMChannelIndex++){
		if(compare[SoftPWMChannelIndex] == softcount && compare[SoftPWMChannelIndex] !=MAX_PWM){
			IndexTemp1 &= (~(1<<SoftPWMChannelIndex));
		}
	}
	MBI5026_SendData(IndexTemp1);
}


uint8_t LedFadeIndex = 0;
uint8_t LedFadeValueIndex = 0;
uint16_t fadeCnt = 0;
uint8_t LedFadeValue = 0;
uint8_t LedFadeDir = 0;
void LedFadeService(){	
	for (LedFadeIndex = 0;LedFadeIndex<16;LedFadeIndex++){
		LedDimmer(LedFadeIndex,LedFadeValue);	
	}
	if (fadeCnt > 50){		
		if (LedFadeDir == 0){
			LedFadeValue++;
			if (LedFadeValue >=50){
				LedFadeDir = 1;
			}
		}
		else {
			LedFadeValue--; 	
			if (LedFadeValue <= 5){
				LedFadeDir = 0;
			}
		}
		fadeCnt=0;
	}
	else {
		fadeCnt++;
	}	
	/*FadeValue[LedFadeValueIndex] = FadeValue[LedFadeValueIndex] - 6;
	if (FadeValue[LedFadeValueIndex] <= 0){
		FadeValue[LedFadeValueIndex] = 96;
	}	
	LedFadeValueIndex++;	
	if (LedFadeValueIndex >=16) LedFadeValueIndex = 0;
	
	if (fadeCnt > 10){					
		LedDimmer(LedFadeIndex,FadeValue[LedFadeIndex]);	
		LedFadeIndex++;
		if (LedFadeIndex>=16){
			LedFadeIndex = 0;
		}
		fadeCnt=0;
	}
	else {
		fadeCnt++;
	}	*/
}