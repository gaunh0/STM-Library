#include "Device.h"

uint8_t DoorState = 0;

void DeviceInitController()
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.Pin = DOOR_CONTROL_PIN;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(DOOR_CONTROL_PORT, &GPIO_InitStructure);
		
	DoorClose();
}

void DoorOpen(){
	HAL_GPIO_WritePin(DOOR_CONTROL_PORT,DOOR_CONTROL_PIN,GPIO_PIN_SET);						
	DoorState = DOOR_OPEN;
}
void DoorClose(){
	HAL_GPIO_WritePin(DOOR_CONTROL_PORT,DOOR_CONTROL_PIN,GPIO_PIN_RESET);
	DoorState = DOOR_CLOSE;
}

uint8_t GetDoorState(){
	return DoorState; 
}

