#include "central_lock.h"

/* Section Variables -------------------------------------------------------------*/

/*! <Headers that are used in the checking of the code process>*/
static const uint8_t CodeHeaders[4] = { 0b01010101, 0b10101010, 0b00001111,
		0b11110000 };

/*! <UART module to receive the code>*/
extern UART_HandleTypeDef huart1;

/* Section Functions implementation -------------------------------------------------------------*/
void CentralLock_Init(CentralLock_t *_centralLock) {

	/*! <We should here check the door if it is locked or not> */
	/*! <But for now it will be like this untill I get more HW> */
	_centralLock->currentLockState = LOCKED;
	_centralLock->prevLockState = LOCKED;

	/*! <The current power state of the central lock module> */
	_centralLock->powerMode = AWAKE;

	_centralLock->codeReceivedFlag = false;

	_centralLock->maxErrorInSequenceNumber = 100;

	_centralLock->numOperations = 0;
	_centralLock->maxNpOperations = 5;

	/*!<First, fetch the old sequence number from the flash memory>*/
	uint8_t fetchOldCodeBuffer[SEQUENCE_NUMBER_LENGTH] = { 0 };
	HAL_FlashReadData(fetchOldCodeBuffer, SEQUENCE_NUMBER_LENGTH,
	FLASH_START_ADDRESS);
	uint16_t oldCode = fetchOldCodeBuffer[1];
	oldCode = oldCode << FLASH_BYTE_SIZE;
	oldCode |= fetchOldCodeBuffer[0];
	_centralLock->currentSequenceNumber = oldCode;

	/*!<Start receiving data>*/
	CentralLock_ReceiveCodeNonBlocking(_centralLock);
}

void CentralLock_DoorChangeState(CentralLock_t *_centralLock,
		LockState_t _currentState, StateChangeSource_t _stateChangeSource) {

	/*! <Simulate locking and unlocking the doors> */
	HAL_GPIO_WritePin(DOOR0_GPIO_Port, DOOR0_Pin, _currentState);
	HAL_GPIO_WritePin(DOOR1_GPIO_Port, DOOR1_Pin, _currentState);
	HAL_GPIO_WritePin(DOOR2_GPIO_Port, DOOR2_Pin, _currentState);
	HAL_GPIO_WritePin(DOOR3_GPIO_Port, DOOR3_Pin, _currentState);

	CentralLock_SetCurrentLockState(_centralLock, _currentState);
	CentralLock_SetPrevLockState(_centralLock, _currentState);

	if (_stateChangeSource == KEYLESS) {
		_centralLock->numOperations++;

		if (_centralLock->numOperations >= _centralLock->maxNpOperations)
			HAL_FlashStoreData(
					_centralLock->CodeBuffer + SEQUENCE_NUMBER_LENGTH,
					SEQUENCE_NUMBER_LENGTH, FLASH_START_ADDRESS);

		_centralLock->numOperations %= _centralLock->maxNpOperations;
	}

}

void CentralLock_ReceiveCodeNonBlocking(CentralLock_t *_centralLock) {
	HAL_UART_Receive_IT(&huart1, _centralLock->CodeBuffer, CODE_LENGTH);
}

void CentralLock_SetCurrentLockState(CentralLock_t *_centralLock,
		LockState_t _currentLockState) {
	_centralLock->currentLockState = _currentLockState;
}
void CentralLock_SetPrevLockState(CentralLock_t *_centralLock,
		LockState_t _prevLockState) {
	_centralLock->prevLockState = _prevLockState;

}

void CentralLock_ClearCodeBuffer(CentralLock_t *_centralLock) {
	for (int i = 0; i < CODE_LENGTH; i++) {
		_centralLock->CodeBuffer[i] = '\0';
	}
}

CodeStatus_t CentralLock_GetCodeStatus(CentralLock_t *_centralLock) {
	for (int i = 0; i < CODE_LENGTH; i++) {
		for (int i = 0; i < 2; i++) {
			if ((_centralLock->CodeBuffer[i] != CodeHeaders[i])
					|| (_centralLock->CodeBuffer[CODE_LENGTH - 1 - i]
							!= CodeHeaders[3 - i])) {
				return UNVALID;
			}
		}
	}
	uint16_t decryptedSequenceNumber = CentralLock_DecryptCode(_centralLock);
	uint16_t currentSequenceNumber = _centralLock->currentSequenceNumber;
	if (ABS(decryptedSequenceNumber - currentSequenceNumber)
			> _centralLock->maxErrorInSequenceNumber) {
		return OUT_OF_RANGE;
	} else {
		CentralLock_UpdateCurrentSequenceNum(_centralLock,
				decryptedSequenceNumber);
	}
	return VALID;
}

void CentralLock_UpdateCurrentSequenceNum(CentralLock_t *_centralLock,
		uint16_t _newSequenceNumber) {
	_centralLock->currentSequenceNumber = _newSequenceNumber;
}

uint16_t CentralLock_DecryptCode(CentralLock_t *_centralLock) {
	uint16_t decryptedCode = 0;
	decryptedCode = _centralLock->CodeBuffer[SECOND_BYTE_IN_SEQ_NUM];
	decryptedCode = decryptedCode << FLASH_BYTE_SIZE;
	decryptedCode = decryptedCode
			| _centralLock->CodeBuffer[FIRST_BYTE_IN_SEQ_NUM];
	return decryptedCode;
}

void CentralLock_ChangeModuleLedState(CentralLock_t *_centralLock,
		ModuleLedState_t _moduleLedState) {
	if (_moduleLedState == MODULE_LED_BLINK) {
		uint8_t i = 0;
		for (i = 0U; i < 16; i++) {
			HAL_GPIO_TogglePin(CENTRAL_LOCK_LED_GPIO_Port,
					CENTRAL_LOCK_LED_Pin);
			HAL_Delay(BLINK_DELAY);
		}
		HAL_GPIO_WritePin(CENTRAL_LOCK_LED_GPIO_Port, CENTRAL_LOCK_LED_Pin,
		MODULE_BUILT_IN_LOW);
	} else {
		HAL_GPIO_WritePin(CENTRAL_LOCK_LED_GPIO_Port, CENTRAL_LOCK_LED_Pin,
				_moduleLedState);
	}
}
void CentralLock_SetPowerMode(CentralLock_t *_centralLock, PowerMode_t _mode) {
	_centralLock->powerMode = _mode;
}

void CentralLock_SetCodeReceivedFlag(CentralLock_t *_centralLock,
bool _codeReceivedFlag) {
	_centralLock->codeReceivedFlag = _codeReceivedFlag;
}
