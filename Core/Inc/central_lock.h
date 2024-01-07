/*
 * central_lock.h
 *
 *  Created on: Jan 7, 2024
 *      Author: shady
 */

#ifndef INC_CENTRAL_LOCK_H_
#define INC_CENTRAL_LOCK_H_

#include "gpio.h"

typedef struct {
	GPIO_TypeDef *GPIOx_Doors_Port;
	uint16_t GPIO_DoorArr[4];
	uint16_t GPIO_Led_Pin;
} CentralLock_t;

typedef enum {
	LOCKED, UNLOCKED
} LockState_t;

void CentralLock_Init(CentralLock_t *CentralLock);
void CentralLock_DoorChangeState(CentralLock_t *CentralLock, LockState_t state);

#endif /* INC_CENTRAL_LOCK_H_ */
