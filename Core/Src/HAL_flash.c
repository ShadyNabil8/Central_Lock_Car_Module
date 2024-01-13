/*
 * HAL_flash.c
 *
 *  Created on: Jan 12, 2024
 *      Author: shady
 */

#include "HAL_flash.h"

/**
 * @brief  Store Data in Flash Memory with Erase
 * @note   This function enables the storage of data in the Flash memory of an STM32 microcontroller.
 *         It utilizes the Hardware Abstraction Layer (HAL) provided by STMicroelectronics.
 *         The Flash memory is first erased in pages, and then the specified data is written.
 * @param  _srcDataBuffer: Pointer to the source data buffer.
 * @param  _dataSize: Size of the data to be stored (in bytes).
 * @param  _flashStartAddress: Starting address in Flash memory where the data will be written.
 * @retval HAL_StatusTypeDef: Status of the last Flash operation.
 */
HAL_StatusTypeDef HAL_FlashStoreData(const uint8_t *_srcDataBuffer,
		uint32_t _dataSize, uint32_t _flashStartAddress) {

	uint8_t index = 0;

	/*! <Convert the size of the byte-based data to a half-word-based size> */
	uint32_t halfWords = _dataSize / FLASH_HALF_WORD_SIZE
			+ (uint32_t) ((_dataSize % FLASH_HALF_WORD_SIZE) != 0);

	/*! <Cuaculate the number of pages needed to store this size of data>*/
	uint32_t numPages = _dataSize / FLASH_SIZE_OF_PAGE
			+ (uint32_t) ((_dataSize % FLASH_SIZE_OF_PAGE) != 0);

	/*! <Unlock the flash memory to unlock the FPEC(Flash program and erase controller) block(registers)> */
	/*! <This block of registers handles the [program and erase] operations of the Flash memory.> */
	/*! <We need to access the FLASH_CR register to access the PG bit.> */
	HAL_FLASH_Unlock();

	/*! <Variable to track the last operation status>*/
	HAL_StatusTypeDef lastOpStatus = HAL_OK;

	/*! <Erase the Flash memory pages corresponding to the specified data size>*/
	lastOpStatus = HAL_FlashErase(FLASH_TYPEERASE_PAGES, _flashStartAddress,
			numPages);

	/*!<Variable to track the index which the byte we read will be stored>*/
	uint32_t dataSizeinBytes = 0;

	/*! <The half word to be written>*/
	uint16_t halfWord = 0;

	/*! <Start writing> */
	for (index = 0U; (index < halfWords) && (dataSizeinBytes < _dataSize);
			index++) {
		/*! <Construct a 16-bit half-word from two consecutive bytes in the source data buffer>*/
		halfWord = 0;
		halfWord = _srcDataBuffer[dataSizeinBytes++];
		halfWord |= (((uint16_t) (_srcDataBuffer[dataSizeinBytes++]))
				<< FLASH_BYTE_SIZE);
		/*! <Program the half-word to the Flash memory at the specified address>*/
		lastOpStatus = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
				_flashStartAddress + (FLASH_HALF_WORD_SIZE * index), halfWord);
	}

	/*! <Lock the Flash memory to disable write and erase operations> */
	HAL_FLASH_OB_Lock();

	return lastOpStatus;
}

/**
 * @brief  Read Data from Flash Memory
 * @note   This function allows reading data from the Flash memory of an STM32 microcontroller.
 *         It retrieves data stored in Flash and copies it to the specified destination buffer.
 *         The Hardware Abstraction Layer (HAL) provided by STMicroelectronics is utilized.
 * @param  _destDataAddress: Pointer to the destination buffer where the read data will be stored.
 * @param  _numberOfBytes: Number of bytes to read from Flash memory.
 * @param  _startReadAddress: Starting address in Flash memory from where the reading will commence.
 * @retval None
 */
void HAL_FlashReadData(uint8_t *_destDataAddress, uint32_t _numberOfBytes,
		uint32_t _startReadAddress) {

	uint8_t index = 0;
	uint8_t innerIndex = 0;

	/*! <Determine the number of words inside a buffer and read this buffer byte by byte>*/
	uint32_t numberOfWords = _numberOfBytes / FLASH_WORD_SIZE
			+ (uint32_t) (_numberOfBytes % FLASH_WORD_SIZE != 0);

	/*!<Variable to track the index which the byte we read will be stored>*/
	uint32_t indexOfStoredByte = 0;

	/*!<Variable to carry the fetched word to convert it into bytes later>*/
	uint32_t fetchedWord = 0;

	/*! <Store the fetched words in the byte-based buffer>*/
	for (index = 0U; index < numberOfWords; index++) {
		fetchedWord = *(((uint32_t*) _startReadAddress) + index);
		for (innerIndex = 0U;
				(innerIndex < FLASH_WORD_SIZE)
						&& (indexOfStoredByte < _numberOfBytes); innerIndex++) {
			/*! <Extract bytes from the fetched word and store them in the destination buffer> */
			_destDataAddress[indexOfStoredByte] = (uint8_t) (fetchedWord
					>> (FLASH_BYTE_SIZE * innerIndex));
			indexOfStoredByte++;
		}
	}
}

/**
 * @brief  Erase Flash Memory Pages
 * @note   This function initiates the erasure of specified pages in the Flash memory of an STM32 microcontroller.
 *         It utilizes the Hardware Abstraction Layer (HAL) provided by STMicroelectronics.
 * @param  _typeErase: Type of erasure (Mass erase or page erase).
 * @param  _eraseStartAddress: Starting address in Flash memory where the erasure will commence.
 * @param  _numOfPages: Number of consecutive Flash memory pages to be erased.
 * @retval HAL_StatusTypeDef: Status of the last Flash erase operation.
 */
HAL_StatusTypeDef HAL_FlashErase(uint32_t _typeErase,
		uint32_t _eraseStartAddress, uint32_t _numOfPages) {

	/* EraseInit configuration*/
	FLASH_EraseInitTypeDef pageEraseInit;
	pageEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	pageEraseInit.PageAddress = _eraseStartAddress;
	pageEraseInit.NbPages = _numOfPages;

	/*! <Pointer to the start address of the faulty page in case of an error during erasing>*/
	/*! <0xFFFFFFFF means that all the pages have been correctly erased>*/
	uint32_t faultPageAddress = 0;

	/*! <Variable to track the last operation status>*/
	HAL_StatusTypeDef lastOpStatus = HAL_OK;

	/*! <Start the erase operation>*/
	lastOpStatus = HAL_FLASHEx_Erase(&pageEraseInit, &faultPageAddress);

	/* Here we can check for the fault page if there is>*/

	return lastOpStatus;
}

