#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdbool.h>
#include "stm32f1xx_hal.h"


//################################################################################################################
bool 	EE_Init(void);
bool	EE_Format(void);
bool 	EE_Read(uint16_t VirtualAddress, uint32_t* Data);
bool 	EE_Write(uint16_t VirtualAddress, uint32_t Data);
bool	EE_Reads(uint16_t StartVirtualAddress,uint16_t HowMuchToRead,uint32_t* Data);
bool 	EE_Writes(uint16_t StartVirtualAddress,uint16_t HowMuchToWrite,uint32_t* Data);
uint16_t EE_GetVirtualAddress(void);
bool EE_IsPageEmpty(uint32_t PageAddress);
bool EE_ReadCustomPage(uint32_t PageAddress,uint16_t VirtualAddress, uint32_t* Data);
bool EE_WriteCustomPage(uint32_t PageAddress,uint16_t VirtualAddress, uint32_t Data);
//################################################################################################################

#endif
