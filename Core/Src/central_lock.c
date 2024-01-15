#include "central_lock.h"

/* Section Variables -------------------------------------------------------------*/

/*! <Headers that are used in the checking of the code process>*/
static const uint8_t CodeHeaders[4] = { 0b01010101, 0b10101010, 0b00001111,
		0b11110000 };

/*! <UART module to receive the code>*/
extern UART_HandleTypeDef huart1;

/* Section Functions implementation -------------------------------------------------------------*/
void CentralLock_Init(CentralLock_t *_centralLock) {
	/*!<Central lock module pin configuration>*/
	_centralLock->GPIOx_Doors_Port = GPIOB;
	_centralLock->GPIO_DoorArr[0] = GPIO_PIN_12;
	_centralLock->GPIO_DoorArr[1] = GPIO_PIN_13;
	_centralLock->GPIO_DoorArr[2] = GPIO_PIN_14;
	_centralLock->GPIO_DoorArr[3] = GPIO_PIN_15;

	/*! <We should here check the door if it is locked or not> */
	/*! <But for now it will be like this untill I get more HW> */
	_centralLock->CurrentLockState = LOCKED;
	_centralLock->PrevLockState = LOCKED;
	/*! <The current power state of the central lock module> */
	_centralLock->PowerMode = AWAKE;
	_centralLock->CodeReceived = false;

	_centralLock->MaxErrorInSequenceNumber = 100;

	_centralLock->NpOperations = 0;
	_centralLock->MaxNpOperations = 5;

	/*!<First, fetch the old sequence number from the flash memory>*/
	uint8_t fetchOldCodeBuffer[SEQUENCE_NUMBER_LENGTH] = { 0 };
	HAL_FlashReadData(fetchOldCodeBuffer, SEQUENCE_NUMBER_LENGTH,
	FLASH_START_ADDRESS);
	uint16_t oldCode = fetchOldCodeBuffer[1];
	oldCode = oldCode << FLASH_BYTE_SIZE;
	oldCode |= fetchOldCodeBuffer[0];
	_centralLock->CurrentSequenceNumber = oldCode;

	/*!<Start receiving data>*/
	CentralLock_ReceiveCodeNonBlocking(_centralLock);
}

void CentralLock_DoorChangeState(CentralLock_t *_centralLock,
		LockState_t _currentState, StateChangeSource_t _stateChangeSource) {

	for (int i = 0; i < 4; i++)
		HAL_GPIO_WritePin(_centralLock->GPIOx_Doors_Port,
				_centralLock->GPIO_DoorArr[i], _currentState);

	CentralLock_SetCurrentLockState(_centralLock, _currentState);
	CentralLock_SetPrevLockState(_centralLock, _currentState);

	if (_stateChangeSource == KEYLESS) {
		_centralLock->NpOperations++;

		if (_centralLock->NpOperations >= _centralLock->MaxNpOperations)
			HAL_FlashStoreData(
					_centralLock->CodeBuffer + SEQUENCE_NUMBER_LENGTH,
					SEQUENCE_NUMBER_LENGTH, FLASH_START_ADDRESS);

		_centralLock->NpOperations %= _centralLock->MaxNpOperations;
	}

}

void CentralLock_ReceiveCodeNonBlocking(CentralLock_t *_centralLock) {
	HAL_UART_Receive_IT(&huart1, _centralLock->CodeBuffer, CODE_LENGTH);
}

void CentralLock_SetCurrentLockState(CentralLock_t *_centralLock,
		LockState_t _currentLockState) {
	_centralLock->CurrentLockState = _currentLockState;
}
void CentralLock_SetPrevLockState(CentralLock_t *_centralLock,
		LockState_t _prevLockState) {
	_centralLock->PrevLockState = _prevLockState;

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
	uint16_t currentSequenceNumber = _centralLock->CurrentSequenceNumber;
	if (ABS(decryptedSequenceNumber - currentSequenceNumber)
			> _centralLock->MaxErrorInSequenceNumber) {
		return OUT_OF_RANGE;
	} else {
		CentralLock_UpdateCurrentSequenceNum(_centralLock,
				decryptedSequenceNumber);
	}
	return VALID;
}

static void CentralLock_UpdateCurrentSequenceNum(CentralLock_t *_centralLock,
		uint16_t _newSequenceNumber) {
	_centralLock->CurrentSequenceNumber = _newSequenceNumber;
}

static uint16_t CentralLock_DecryptCode(CentralLock_t *_centralLock) {
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
			HAL_GPIO_TogglePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin);
			HAL_Delay(BLINK_DELAY);
		}
		HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin,
		MODULE_BUILT_IN_LOW);
	} else {
		HAL_GPIO_WritePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin,
				_moduleLedState);
	}
}
void CentralLock_SetPowerMode(CentralLock_t *_centralLock, PowerMode_t _mode) {
	_centralLock->PowerMode = _mode;
}

void CentralLock_SetCodeReceivedFlag(CentralLock_t *_centralLock,
bool _codeReceived) {
	_centralLock->CodeReceived = _codeReceived;
}
