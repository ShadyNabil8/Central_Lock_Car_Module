#include "central_lock.h"

LockState_t LockState;
LockState_t TempLockState;

void CentralLock_Init(CentralLock_t *CentralLock) {
	CentralLock->GPIOx_Doors_Port = GPIOB;
	CentralLock->GPIO_DoorArr[0] = GPIO_PIN_12;
	CentralLock->GPIO_DoorArr[1] = GPIO_PIN_13;
	CentralLock->GPIO_DoorArr[2] = GPIO_PIN_14;
	CentralLock->GPIO_DoorArr[3] = GPIO_PIN_15;
	LockState = LOCKED;
	TempLockState = LOCKED;
}

void CentralLock_DoorChangeState(CentralLock_t *CentralLock, LockState_t state) {
	for (int i = 0; i < 4; i++)
		HAL_GPIO_WritePin(CentralLock->GPIOx_Doors_Port,
				CentralLock->GPIO_DoorArr[i], state);
}
