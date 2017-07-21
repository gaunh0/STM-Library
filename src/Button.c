#include "Button.h"
#include "RFID.h"
#define BUTTON_TIME_BOUND	1000
volatile uint8_t ButtonValue[4] = {0,0,0,0};

void ButtonInitController(){
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pin = BUTTON1_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BUTTON1_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
	GPIO_InitStructure.Pin 	= BUTTON2_PIN;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(BUTTON2_PORT, &GPIO_InitStructure);
}

uint8_t button_timer_cnt = 0;
void ButtonService(){
	if (button_timer_cnt > 10){		
		if ((HAL_GPIO_ReadPin(BUTTON1_PORT, BUTTON1_PIN)) == GPIO_PIN_SET){	
			ButtonValue[3] |= BUTTON1_MASK;
		}	
		else {
			ButtonValue[3] &= ~(BUTTON1_MASK);
		}

		if ((HAL_GPIO_ReadPin(BUTTON2_PORT, BUTTON2_PIN))== GPIO_PIN_SET){
			ButtonValue[3] |= BUTTON2_MASK;
		}	
		else {
			ButtonValue[3] &= ~(BUTTON2_MASK);
		}

		if ((HAL_GPIO_ReadPin(RFID_IRQ_PORT, RFID_IRQ_PIN)) == GPIO_PIN_SET){	
			ButtonValue[3] |= 0x04;
		}	
		else {
			ButtonValue[3] &= ~(0x04);
		}
		
		ButtonValue[1] = ButtonValue[2];
		ButtonValue[2] = ButtonValue[3];
		if ((ButtonValue[1] == ButtonValue[2])&&(ButtonValue[2] == ButtonValue[3])){
			ButtonValue[0] = ButtonValue[3];
		}	
		else {
			ButtonValue[0]  = 0;
		}
		button_timer_cnt = 0;
	}
	else {
	    button_timer_cnt ++;
	}
}

uint8_t GetButtonValue(){
	return ButtonValue[0];
}

uint8_t Button1Pressed(){
	if ((GetButtonValue() & BUTTON1_MASK) == BUTTON1_MASK){
		return PRESSED;
	}
	else {
		return RELEASED;
	}
}
uint8_t Button2Pressed(){
	if ((GetButtonValue() & BUTTON2_MASK) == BUTTON2_MASK){
		return PRESSED;
	}
	else{
		return RELEASED;
	}
}
uint8_t AllButtonReleased(){
	if (GetButtonValue() == 0){
		return 1;
	}
	else return 0;
}	

