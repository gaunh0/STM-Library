#include "Buzzer.h"

volatile uint16_t currentBuzzerOnPeriod;
volatile uint16_t currentBuzzerOffPeriod;
volatile uint16_t BuzzerOnPeriod;
volatile uint16_t BuzzerOffPeriod;
volatile uint8_t NumberOfBuzzerCycle;
volatile uint8_t  BuzzerState;

void BuzzerInitController(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = BUZZER_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

	BuzzerSound(0,0,0);
	BuzzerOff();
}

void BuzzerOn(){
	HAL_GPIO_WritePin(BUZZER_PORT,BUZZER_PIN,GPIO_PIN_SET);
	BuzzerState = BUZZER_ON;
}

void BuzzerOff(){
	HAL_GPIO_WritePin(BUZZER_PORT,BUZZER_PIN,GPIO_PIN_RESET);
	BuzzerState = BUZZER_OFF;
}

void BuzzerSound(uint16_t OnPeriod, uint16_t OffPeriod, uint8_t Time){    
	BuzzerOnPeriod = OnPeriod;    
	currentBuzzerOnPeriod =  OnPeriod;    
	BuzzerOffPeriod = OffPeriod;    
	currentBuzzerOffPeriod = OffPeriod;    
	NumberOfBuzzerCycle = Time;	
	BuzzerState = BUZZER_ON;
}

void BuzzerService(){    
	if (NumberOfBuzzerCycle == 0) {        
		BuzzerOff();
	}
	else {
		if (BuzzerState == BUZZER_ON)    {
			BuzzerOn();
			if (currentBuzzerOnPeriod>0) {	
				currentBuzzerOnPeriod--;	
			}
			else if (currentBuzzerOnPeriod == 0){            
				NumberOfBuzzerCycle--;           
				currentBuzzerOnPeriod = BuzzerOnPeriod;	            
				BuzzerOff();       
			}   
		}	   
		else {      
			BuzzerOff();       
			if (currentBuzzerOffPeriod>0){ 	
				currentBuzzerOffPeriod--;	
			}
			else if (currentBuzzerOffPeriod == 0){            
				currentBuzzerOffPeriod = BuzzerOffPeriod;	            
				BuzzerOn();       
			}    
		}	
	}
}


