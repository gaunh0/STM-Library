/**
  ******************************************************************************
  * @file    Combros_Lib/Frame_1.h
  * @company COMBROS TECHNOLOGY CO., LTD
	* @team 	 Hardware Team
	* @author  Chau Tran
  * @version V1.0.0
  * @date    24-May-2016
  * @brief   This file contains all the functions prototypes for CRC checking.
  ******************************************************************************
  * @attention
  *
  * BASED ON THE CRC FIRMWARE FOR STM8S IN PROJECT REMOTERF - COMBROS
	*
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CRC_H
#define _CRC_H

#ifdef __cplusplus
extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx.h"
#include "main.h"

uint16_t CRC16Update(uint16_t crc, uint8_t data);
	uint16_t CalculateCRC16(uint8_t *data, uint8_t len);
unsigned short int CRC16 (unsigned char* puchMsg,unsigned short usDataLen );
unsigned int CRC16_2(unsigned char *buf, int len);

#ifdef __cplusplus
}
#endif

#endif /* _CRC_H */
