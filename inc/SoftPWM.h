#include "stm32f1xx_hal.h"

void TimerInitController(void); 
void LedDimmer(uint8_t led,uint8_t _value);
void LedFadeValueInit(void);
void LedFade(uint8_t led);
void SoftPWMInitController(void);
void SoftPWMService(void);
void LedFadeService(void);
