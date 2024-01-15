/*
 * central_lock.h
 *
 *  Created on: Jan 7, 2024
 *      Author: shady
 */

#ifndef INC_CENTRAL_LOCK_H_
#define INC_CENTRAL_LOCK_H_

/* Section Includes -------------------------------------------------------------*/
#include "gpio.h"
#include "time.h"
#include "HAL_flash.h"

/* Section macros -------------------------------------------------------------*/
#define CODE_LENGTH 6
#define SEQUENCE_NUMBER_LENGTH 2
#define FIRST_BYTE_IN_SEQ_NUM 2
#define SECOND_BYTE_IN_SEQ_NUM 3
#define FLASH_START_ADDRESS 0x0801FC00
#define ABS(x) ((x)>0?(x):-(x))

/* Section typedefines -------------------------------------------------------------*/
typedef struct {
	GPIO_TypeDef *GPIOx_Doors_Port;
	uint16_t GPIO_DoorArr[4];
	uint16_t GPIO_Led_Pin;
} CentralLock_t;

typedef enum {
	LOCKED, UNLOCKED
} LockState_t;

typedef enum {
	VALID, UNVALID, OUT_OF_RANGE
} CodeStatus_t;

/* Section Functions prototypes -------------------------------------------------------------*/
void CentralLock_DoorChangeState(CentralLock_t *CentralLock, LockState_t currentState);
static void CentralLock_UpdateCurrentSequenceNum(uint16_t _newSequenceNumber);
void CentralLock_SetCurrentLockState(LockState_t CurrentState);
void CentralLock_SetPrevLockState(LockState_t PrevState);
void CentralLock_OpenDoors(CentralLock_t *CentralLock);
uint8_t CentralLock_GetCodePortion(uint8_t portion);
static uint16_t CentralLock_GetCurrentSequenceNum();
void CentralLock_Init(CentralLock_t *CentralLock);
uint8_t CentralLock_GetCodeHeader(uint8_t header);
LockState_t CentralLock_GetCurrentLockState();
LockState_t CentralLock_GetPrevLockState();
void CentralLock_ReceiveCodeNonBlocking();
static uint16_t CentralLock_DecryptCode();
CodeStatus_t CentralLock_GetCodeStatus();
uint8_t CentralLock_GetCurSequenceNum();
void CentralLock_IncCurSequenceNum();
void CentralLock_RstCurSequenceNum();
void CentralLock_ClearCodeBuffer();
#endif /* INC_CENTRAL_LOCK_H_ */
