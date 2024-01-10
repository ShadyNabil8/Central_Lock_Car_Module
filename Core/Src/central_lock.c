#include "central_lock.h"
#include  <stdbool.h>

static volatile LockState_t CurrentLockState;
static volatile LockState_t PrevLockState;
static uint8_t CodeBuffer[CODE_LENGTH];
static volatile uint16_t CurrentSequenceNumber = 0;
bool LockStateChanged = false;
// static const uint16_t MaxSequenceNumber = 1000;
static const uint16_t MaxErrorInSequenceNumber = 99;
static const uint8_t CodeHeaders[4] = { 0b01010101, 0b10101010, 0b00001111,
		0b11110000 };
extern UART_HandleTypeDef huart1;
void CentralLock_Init(CentralLock_t *CentralLock) {
	CentralLock->GPIOx_Doors_Port = GPIOB;
	CentralLock->GPIO_DoorArr[0] = GPIO_PIN_12;
	CentralLock->GPIO_DoorArr[1] = GPIO_PIN_13;
	CentralLock->GPIO_DoorArr[2] = GPIO_PIN_14;
	CentralLock->GPIO_DoorArr[3] = GPIO_PIN_15;

	CurrentLockState = LOCKED;
	PrevLockState = LOCKED;
	LockStateChanged = false;

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
	decryptedCode = CodeBuffer[2];
	decryptedCode = decryptedCode << CODE_LENGTH;
	decryptedCode = decryptedCode | CodeBuffer[3];
	return decryptedCode;
}
