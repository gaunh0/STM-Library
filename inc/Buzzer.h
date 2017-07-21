#include "stm32f1xx_hal.h"

#define BUZZER_PIN		GPIO_PIN_13
#define BUZZER_PORT		GPIOC

#define BUZZER_ON	1
#define BUZZER_OFF	0

void BuzzerInitController(void);
void BuzzerOn(void);
void BuzzerOff(void);
void BuzzerSound(uint16_t OnPeriod, uint16_t OffPeriod, uint8_t Time);
void BuzzerService(void);


