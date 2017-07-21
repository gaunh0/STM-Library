#include "LED.h"

void LedInitController(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = LEDRED_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LEDRED_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = LEDGREEN_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(LEDGREEN_PORT, &GPIO_InitStructure);
	
	LedOff(LEDRED);
	LedOff(LEDGREEN);

	 SetLedDisplayFlag();
}


void LedOff(uint8_t LedIndex){
	switch (LedIndex){
		case LEDRED:
			HAL_GPIO_WritePin(LEDRED_PORT,LEDRED_PIN,GPIO_PIN_SET);
			break;
		case LEDGREEN:
			HAL_GPIO_WritePin(LEDGREEN_PORT,LEDGREEN_PIN,GPIO_PIN_SET);
			break;
		default:
			break;
	}
}
void LedOn(uint8_t LedIndex){
	switch (LedIndex){
		case LEDRED:
			HAL_GPIO_WritePin(LEDRED_PORT,LEDRED_PIN,GPIO_PIN_RESET);
			break;
		case LEDGREEN:
			HAL_GPIO_WritePin(LEDGREEN_PORT,LEDGREEN_PIN,GPIO_PIN_RESET);
			break;	
		default:
			break;
	}
}
void LedToggle(uint8_t LedIndex){
	switch (LedIndex){
		case LEDRED:
			HAL_GPIO_TogglePin(LEDRED_PORT,LEDRED_PIN);
			break;
		case LEDGREEN:
			HAL_GPIO_TogglePin(LEDGREEN_PORT,LEDGREEN_PIN);
			break;
		default:
			break;
	}
}

uint8_t LedDisplayFlag = 0;
uint16_t LedTimerCounter = 0;
uint8_t LedIndex = 0;
void SetLedDisplayFlag(){
	LedDisplayFlag = 1;
}
void ClearLedDisplayFlag(){
	LedDisplayFlag = 0;
}

#define LED_CHANGE_TIME	400
void LedScanDisplay(){
	if (LedDisplayFlag == 1){
		switch (LedIndex){
			case 0:
				LedTimerCounter++;
				if (LedTimerCounter>LED_CHANGE_TIME){
					LedTimerCounter = 0;
					LedIndex++;
				}
				break;
			case 1:
				LedTimerCounter++;
				if (LedTimerCounter>LED_CHANGE_TIME){
					LedTimerCounter = 0;
					LedIndex++;
				}
				break;
			case 2:
				LedTimerCounter++;
				if (LedTimerCounter>LED_CHANGE_TIME){
					LedTimerCounter = 0;
					LedIndex++;
				}
				break;
			case 3:
				LedTimerCounter++;
				if (LedTimerCounter>LED_CHANGE_TIME){
					LedTimerCounter = 0;
					LedIndex++;
					if (LedIndex >=4){
						LedIndex = 0;
					}
				}
				break;	
			default:
				LedIndex = 0;
				break;
		}		
	}
	else {
		LedTimerCounter = 0;
	}
}


