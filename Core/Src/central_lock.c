#include "central_lock.h"

static volatile LockState_t CurrentLockState;
static volatile LockState_t PrevLockState;
static volatile uint8_t CodeBuffer[CODE_LENGTH];
static volatile uint8_t currentSequenceNumber = 0;
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

	CentralLock_ReceiveCodeNonBlocking();
}

void CentralLock_DoorChangeState(CentralLock_t *CentralLock, LockState_t state) {
	if (state == UNLOCKED)
		CentralLock_UnlockDoors(CentralLock);
	else if (state == LOCKED)
		CentralLock_LockDoors(CentralLock);

	/*
	 * Do more stuff here if needed.
	 */

}

void CentralLock_UnlockDoors(CentralLock_t *CentralLock) {
	for (int i = 0; i < 4; i++)
		HAL_GPIO_WritePin(CentralLock->GPIOx_Doors_Port,
				CentralLock->GPIO_DoorArr[i], UNLOCKED);
}
void CentralLock_LockDoors(CentralLock_t *CentralLock) {

	for (int i = 0; i < 4; i++)
		HAL_GPIO_WritePin(CentralLock->GPIOx_Doors_Port,
				CentralLock->GPIO_DoorArr[i], LOCKED);
}

void CentralLock_ReceiveCodeNonBlocking() {
	uint8_t srquenceNum = CentralLock_GetCurSequenceNum();
	HAL_UART_Receive_IT(&huart1, CodeBuffer + srquenceNum, CODE_LENGTH);
}

LockState_t CentralLock_GetCurrentLockState() {
	return CurrentLockState;
}
LockState_t CentralLock_GetPrevLockState() {
	return PrevLockState;
}

void CentralLock_SetCurrentLockState(LockState_t CurrentState) {
	CurrentLockState = CurrentState;
}
void CentralLock_SetPrevLockState(LockState_t PrevState) {
	PrevLockState = PrevState;
}

uint8_t CentralLock_GetCurSequenceNum() {
	return currentSequenceNumber;
}
void CentralLock_IncCurSequenceNum() {
	currentSequenceNumber++;
	currentSequenceNumber %= CODE_LENGTH;
}

void CentralLock_RstCurSequenceNum() {
	currentSequenceNumber = 0;
}
uint8_t CentralLock_GetCodePortion(uint8_t portion) {
	return CodeBuffer[portion];
}
uint8_t CentralLock_GetCodeHeader(uint8_t header) {
	return CodeHeaders[header];
}
void CentralLock_OpenDoors(CentralLock_t *CentralLock) {
	CentralLock_DoorChangeState(CentralLock, UNLOCKED);
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
}

