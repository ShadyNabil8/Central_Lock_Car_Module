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

/* Private function prototypes -----------------------------------------------*/
void HAL_StoreToFlash(const uint8_t *_srcDataBuffer, uint32_t _dataSize,
		uint32_t _flashAddress);
void HAL_ReadFromFlash(uint8_t *_destDataAddress, uint32_t _numberOfBytes,
		uint32_t _startReadAddress);
void HAL_EraseFlash(uint32_t _pageAddress, uint32_t _numOfPages);

#endif /* INC_HAL_FLASH_H_ */
