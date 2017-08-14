
#include "eeprom.h"
#include "eepromConfig.h"
#include "hal_def.h"

uint32_t	EEPROMPageBackup[_EEPROM_FLASH_PAGE_SIZE/4];
uint32_t ADDR_FLASH_PAGE[8] = {0x0801FC00,0x0801F800,0x0801F400,0x0801F000,0x0801EC00,0x0801E800,0x0801E400,0x0801E000};
//##########################################################################################################
//##########################################################################################################
//##########################################################################################################

// xoa page
bool	EE_Format(void) 
{
	uint32_t	error;
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef	flashErase;
	flashErase.NbPages=1;
	flashErase.Banks = FLASH_BANK_1;
	flashErase.PageAddress = _EEPROM_FLASH_PAGE_ADDRESS;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	if(HAL_FLASHEx_Erase(&flashErase,&error)==HAL_OK)
	{
		HAL_FLASH_Lock();
		if(error != 0xFFFFFFFF)
			return false;
		else
			return true;	
	}
	HAL_FLASH_Lock();
	return false;	
}
//##########################################################################################################
bool EE_Read(uint16_t VirtualAddress, uint32_t* Data)
{
	if(VirtualAddress >=	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;
	*Data =  (*(__IO uint32_t*)((VirtualAddress*4)+_EEPROM_FLASH_PAGE_ADDRESS));
	return true;
}
//##########################################################################################################
bool EE_Write(uint16_t VirtualAddress, uint32_t Data)
{
	if(VirtualAddress >=	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;

	if((*(__IO uint32_t*)((VirtualAddress*4)+_EEPROM_FLASH_PAGE_ADDRESS)) != 0xFFFFFFFF)
	{
		
		if( EE_Reads(0,(_EEPROM_FLASH_PAGE_SIZE/4),EEPROMPageBackup)==false)
		{
			HAL_FLASH_Lock();
			return false;
		}
		EEPROMPageBackup[VirtualAddress]=Data;
		EE_Format();
		
		HAL_FLASH_Unlock();
		for(uint16_t	i=0 ; i<_EEPROM_FLASH_PAGE_SIZE/4 ; i++)
		{
			if(EEPROMPageBackup[i]!=0xFFFFFFFF)
			{
				if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(i*4)+_EEPROM_FLASH_PAGE_ADDRESS,(uint64_t)EEPROMPageBackup[i])==false)
				{
					HAL_FLASH_Lock();
					return false;
				}
			}
		}
	}	
	HAL_FLASH_Unlock();
	if(Data!=0xFFFFFFFF)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(VirtualAddress*4)+_EEPROM_FLASH_PAGE_ADDRESS,(uint64_t)Data)==HAL_OK)
		{
			HAL_FLASH_Lock();
			return true;
		}
		else
		{
			HAL_FLASH_Lock();
			return false;
		}
	}
	HAL_FLASH_Lock();
	return true;

}
//##########################################################################################################
bool EE_Reads(uint16_t StartVirtualAddress,uint16_t HowMuchToRead,uint32_t* Data)
{
	if((StartVirtualAddress+HowMuchToRead) >	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;
	for(uint16_t	i=StartVirtualAddress ; i<HowMuchToRead ; i++)
	{
		*Data =  (*(__IO uint32_t*)((i*4)+_EEPROM_FLASH_PAGE_ADDRESS));
		Data++;
	}
	return true;
}
//##########################################################################################################
bool 	EE_Writes(uint16_t StartVirtualAddress,uint16_t HowMuchToWrite,uint32_t* Data)
{
	if((StartVirtualAddress+HowMuchToWrite) >	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;
	if( EE_Reads(0,(_EEPROM_FLASH_PAGE_SIZE/4),EEPROMPageBackup)==false)
		return false;
	for(uint16_t	i=StartVirtualAddress ; i<HowMuchToWrite+StartVirtualAddress ; i++)
	{
		EEPROMPageBackup[i]=*Data;	
		Data++;
	}	
	if(EE_Format()==false)
		return false;
	HAL_FLASH_Unlock();
	for(uint16_t	i=0 ; i<(_EEPROM_FLASH_PAGE_SIZE/4); i++)
	{
		if(EEPROMPageBackup[i]!=0xFFFFFFFF)
		{
			if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(i*4)+_EEPROM_FLASH_PAGE_ADDRESS,(uint64_t)EEPROMPageBackup[i])!=HAL_OK)
			{
				HAL_FLASH_Lock();
				return false;
			}
		}
	}
	HAL_FLASH_Lock();
	return true;
}
uint16_t EE_GetVirtualAddress(void)
{
  uint16_t index;
  uint32_t tmpData;
  for(index = 0;index < (_EEPROM_FLASH_PAGE_SIZE/4);index++)
    {
       EE_Read(index,&tmpData);
       if(tmpData == 0xFFFFFFFF)
       {
       return index;
       }
    }
}						   
bool EE_IsPageEmpty(uint32_t PageAddress)
{
  uint16_t index;
  uint32_t tmpData;
  for(index = 0;index < (_EEPROM_FLASH_PAGE_SIZE/4);index++)
    {
       EE_ReadCustomPage(PageAddress,index,&tmpData);
       if(tmpData == 0xFFFFFFFF)
       {
		      return true;
       }
    }
	return false;
}
bool EE_IsPageHaveData(uint32_t PageAddress)
{
  uint16_t index;
  uint32_t tmpData;
  for(index = 0;index < (_EEPROM_FLASH_PAGE_SIZE/4);index++)
    {
       EE_ReadCustomPage(PageAddress,index,&tmpData);
       if(tmpData != 0xFFFFFFFF)
       {
		      return true;
       }
    }
	return false;
}
bool EE_ReadCustomPage(uint32_t PageAddress,uint16_t VirtualAddress, uint32_t* Data)
{
	if(VirtualAddress >=	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;
	*Data =  (*(__IO uint32_t*)((VirtualAddress*4)+PageAddress));
	return true;	
}
bool EE_WriteCustomPage(uint32_t PageAddress,uint16_t VirtualAddress, uint32_t Data)
{
	if(VirtualAddress >=	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;

	if((*(__IO uint32_t*)((VirtualAddress*4)+PageAddress)) != 0xFFFFFFFF)
	{
		
		if( EE_Reads(0,(_EEPROM_FLASH_PAGE_SIZE/4),EEPROMPageBackup)==false)
		{
			HAL_FLASH_Lock();
			return false;
		}
		EEPROMPageBackup[VirtualAddress]=Data;
		EE_Format();
		
		HAL_FLASH_Unlock();
		for(uint16_t	i=0 ; i<_EEPROM_FLASH_PAGE_SIZE/4 ; i++)
		{
			if(EEPROMPageBackup[i]!=0xFFFFFFFF)
			{
				if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(i*4)+PageAddress,(uint64_t)EEPROMPageBackup[i])==false)
				{
					HAL_FLASH_Lock();
					return false;
				}
			}
		}
	}	
	HAL_FLASH_Unlock();
	if(Data!=0xFFFFFFFF)
	{
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(VirtualAddress*4)+PageAddress,(uint64_t)Data)==HAL_OK)
		{
			HAL_FLASH_Lock();
			return true;
		}
		else
		{
			HAL_FLASH_Lock();
			return false;
		}
	}
	HAL_FLASH_Lock();
	return true;

}
//##########################################################################################################
