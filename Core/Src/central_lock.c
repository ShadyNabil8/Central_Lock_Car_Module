#include "central_lock.h"
#include  <stdbool.h>

/* Section Variables -------------------------------------------------------------*/

/*! <Variable to carry the current lock state of the car (locked or unlocked)>*/
static volatile LockState_t CurrentLockState;

/*! <Variable to carry the previous lock state of the car (locked or unlocked)>*/
static volatile LockState_t PrevLockState;

/*! <Variable to carry the number of lock/unlock operations since the module is on>*/
static volatile uint8_t NpOperations = 0;

/*! <Variable to carry the maximum number of lock/unlock operation before storing the sequence number in the flash memory>*/
static volatile const uint8_t MaxNpOperations = 5;

/*! <Buffer to store the code for further processing>*/
static uint8_t CodeBuffer[CODE_LENGTH];

/*! <Variable to carry the current sequence number that is synchronized with the car key>*/
static volatile uint16_t CurrentSequenceNumber = 0;

/*! <If the difference between the sequence number fetcher from the car key and the current sequence number is
 * greater than 99, then the code is not valid and the module will not unlock the car for security purpose,
 * or the car key and the module need to be reprogrammed. by the car owner in case of key is pressed frequently far from the car.
 * This logic is used to secure the car against the hacking if someone tried to send a random code.
 * >*/
static const uint16_t MaxErrorInSequenceNumber = 99;

/*! <Headers that are used in the checking of the code process>*/
static const uint8_t CodeHeaders[4] = { 0b01010101, 0b10101010, 0b00001111,
		0b11110000 };

/*! <UART module to receive the code>*/
extern UART_HandleTypeDef huart1;

/* Section Functions implementation -------------------------------------------------------------*/
void CentralLock_Init(CentralLock_t *CentralLock) {
	/*!<Central lock module pin configuration>*/
	CentralLock->GPIOx_Doors_Port = GPIOB;
	CentralLock->GPIO_DoorArr[0] = GPIO_PIN_12;
	CentralLock->GPIO_DoorArr[1] = GPIO_PIN_13;
	CentralLock->GPIO_DoorArr[2] = GPIO_PIN_14;
	CentralLock->GPIO_DoorArr[3] = GPIO_PIN_15;

	CurrentLockState = LOCKED;
	PrevLockState = LOCKED;

	/*!<First, fetch the old sequence number from the flash memory>*/
	uint8_t fetchOldCodeBuffer[SEQUENCE_NUMBER_LENGTH] = { 0 };
	HAL_FlashReadData(fetchOldCodeBuffer, SEQUENCE_NUMBER_LENGTH,
	FLASH_START_ADDRESS);
	uint16_t oldCode = fetchOldCodeBuffer[1];
	oldCode = oldCode << FLASH_BYTE_SIZE;
	oldCode |= fetchOldCodeBuffer[0];
	CurrentSequenceNumber = oldCode;

	/*!<Start receiving data>*/
	CentralLock_ReceiveCodeNonBlocking();
}

void CentralLock_DoorChangeState(CentralLock_t *CentralLock,
		LockState_t _currentState) {
	LockState_t _prevLockState = CentralLock_GetPrevLockState();
	if (_prevLockState != _currentState) {
		for (int i = 0; i < 4; i++)
			HAL_GPIO_WritePin(CentralLock->GPIOx_Doors_Port,
					CentralLock->GPIO_DoorArr[i], _currentState);
		CentralLock_SetCurrentLockState(_currentState);
		CentralLock_SetPrevLockState(_currentState);
	}

	NpOperations++;

	if (NpOperations >= MaxNpOperations)
		HAL_FlashStoreData(CodeBuffer + SEQUENCE_NUMBER_LENGTH,
		SEQUENCE_NUMBER_LENGTH, FLASH_START_ADDRESS);

	NpOperations %= MaxNpOperations;
}

void CentralLock_ReceiveCodeNonBlocking() {
	HAL_UART_Receive_IT(&huart1, CodeBuffer, CODE_LENGTH);
}

LockState_t CentralLock_GetCurrentLockState() {
	return CurrentLockState;
}
LockState_t CentralLock_GetPrevLockState() {
	return PrevLockState;
}

void CentralLock_SetCurrentLockState(LockState_t _currentLockState) {
	CurrentLockState = _currentLockState;
}
void CentralLock_SetPrevLockState(LockState_t _prevLockState) {
	PrevLockState = _prevLockState;
}

void CentralLock_IncCurSequenceNum() {
	CurrentSequenceNumber++;
	CurrentSequenceNumber %= CODE_LENGTH;
}

void CentralLock_RstCurSequenceNum() {
	CurrentSequenceNumber = 0;
}
uint8_t CentralLock_GetCodePortion(uint8_t _portion) {
	return CodeBuffer[_portion];
}
uint8_t CentralLock_GetCodeHeader(uint8_t _header) {
	return CodeHeaders[_header];
}

void CentralLock_ClearCodeBuffer() {
	for (int i = 0; i < CODE_LENGTH; i++) {
		CodeBuffer[i] = '\0';
	}
}
CodeStatus_t CentralLock_GetCodeStatus() {
	for (int i = 0; i < CODE_LENGTH; i++) {
		for (int i = 0; i < 2; i++) {
			if ((CodeBuffer[i] != CodeHeaders[i])
					|| (CodeBuffer[CODE_LENGTH - 1 - i] != CodeHeaders[3 - i])) {
				return UNVALID;
			}
		}
	}
	uint16_t decryptedSequenceNumber = CentralLock_DecryptCode();
	uint16_t currentSequenceNumber = CentralLock_GetCurrentSequenceNum();
	if (ABS(decryptedSequenceNumber - currentSequenceNumber)
			> MaxErrorInSequenceNumber) {
		return OUT_OF_RANGE;
	} else {
		CentralLock_UpdateCurrentSequenceNum(decryptedSequenceNumber);
	}
	return VALID;
}

static uint16_t CentralLock_GetCurrentSequenceNum() {
	return CurrentSequenceNumber;
}
static void CentralLock_UpdateCurrentSequenceNum(uint16_t _newSequenceNumber) {
	CurrentSequenceNumber = _newSequenceNumber;
}

static uint16_t CentralLock_DecryptCode() {
	uint16_t decryptedCode = 0;
	decryptedCode = CodeBuffer[SECOND_BYTE_IN_SEQ_NUM];
	decryptedCode = decryptedCode << FLASH_BYTE_SIZE;
	decryptedCode = decryptedCode | CodeBuffer[FIRST_BYTE_IN_SEQ_NUM];
	return decryptedCode;
}

