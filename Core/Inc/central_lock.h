/*
 * central_lock.h
 *
 *  Created on: Jan 7, 2024
 *      Author: shady
 */

#ifndef INC_CENTRAL_LOCK_H_
#define INC_CENTRAL_LOCK_H_

#include "gpio.h"
#include "time.h"

#define CODE_LENGTH 6

typedef struct {
	GPIO_TypeDef *GPIOx_Doors_Port;
	uint16_t GPIO_DoorArr[4];
	uint16_t GPIO_Led_Pin;
} CentralLock_t;

typedef enum {
	LOCKED, UNLOCKED
} LockState_t;

typedef enum {
	VALID, UNVALID
} CodeStatus_t;

void CentralLock_Init(CentralLock_t *CentralLock);
void CentralLock_DoorChangeState(CentralLock_t *CentralLock, LockState_t state);
void CentralLock_UnlockDoors(CentralLock_t *CentralLock);
void CentralLock_LockDoors(CentralLock_t *CentralLock);
void CentralLock_ReceiveCodeNonBlocking();
LockState_t CentralLock_GetCurrentLockState();
LockState_t CentralLock_GetPrevLockState();
void CentralLock_SetCurrentLockState(LockState_t CurrentState);
void CentralLock_SetPrevLockState(LockState_t PrevState);
uint8_t CentralLock_GetCurSequenceNum();
void CentralLock_IncCurSequenceNum();
void CentralLock_RstCurSequenceNum();
uint8_t CentralLock_GetCodePortion(uint8_t portion);
uint8_t CentralLock_GetCodeHeader(uint8_t header);
void CentralLock_OpenDoors(CentralLock_t *CentralLock);
void CentralLock_ClearCodeBuffer();
CodeStatus_t CentralLock_GetCodeStatus();
#endif /* INC_CENTRAL_LOCK_H_ */
