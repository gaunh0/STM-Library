#ifndef DEVICE_H
#define DEVICE_H

#include "stm32f1xx_hal.h"

#define DOOR_CLOSE		0
#define DOOR_OPEN		1

#define DOOR_CONTROL_PIN		GPIO_PIN_10
#define DOOR_CONTROL_PORT	GPIOB


void DeviceInitController(void);
void DoorClose(void);
void DoorOpen(void);
uint8_t GetDeviceState(void);

#endif
