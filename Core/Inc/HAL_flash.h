/*
 * HAL_flash.h
 *
 *  Created on: Jan 12, 2024
 *      Author: shady
 */

#ifndef INC_HAL_FLASH_H_
#define INC_HAL_FLASH_H_

/* Private includes ----------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
#define FLASH_HALF_WORD_SIZE 2
#define FLASH_WORD_SIZE 4
#define FLASH_BYTE_SIZE 8
#define FLASH_SIZE_OF_PAGE 1024

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef HAL_FlashStoreData(const uint8_t *_srcDataBuffer, uint32_t _dataSize,
		uint32_t _flashAddress);
void HAL_FlashReadData(uint8_t *_destDataAddress, uint32_t _numberOfBytes,
		uint32_t _startReadAddress);
HAL_StatusTypeDef HAL_FlashErase(uint32_t _typeErase, uint32_t _eraseStartAddress,
		uint32_t _numOfPages);

#endif /* INC_HAL_FLASH_H_ */
