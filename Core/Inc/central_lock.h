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
#include  <stdbool.h>

/* Section macros -------------------------------------------------------------*/
#define CODE_LENGTH 6

#define SEQUENCE_NUMBER_LENGTH 2

#define FIRST_BYTE_IN_SEQ_NUM 2

#define SECOND_BYTE_IN_SEQ_NUM 3

#define BLINK_DELAY 100

#define MODULE_BUILT_IN_HIGH  0

#define MODULE_BUILT_IN_LOW  1

/*! <Carry the maximum number of lock/unlock operation before storing the sequence number in the flash memory>*/
#define MAX_NUMBER_OPERATIONS 20

/*! <If the difference between the sequence number fetcher from the car key and the current sequence number is
 * greater than 99, then the code is not valid and the module will not unlock the car for security purpose,
 * or the car key and the module need to be reprogrammed. by the car owner in case of key is pressed frequently far from the car.
 * This logic is used to secure the car against the hacking if someone tried to send a random code.
 * >*/
#define MAX_ERROR_SEQUENCE_NUMBER 99

#define FLASH_START_ADDRESS 0x0801FC00

/* Section macro functions -------------------------------------------------------------*/
#define ABS(x) ((x)>0?(x):-(x))

/* Section typedefines -------------------------------------------------------------*/
typedef enum {
	LOCKED, UNLOCKED
} LockState_t;

typedef enum {
	VALID, UNVALID, OUT_OF_RANGE
} CodeStatus_t;

typedef enum {
	AWAKE, SLEEP
} PowerMode_t;

typedef struct {
	/*! <Variable to carry the current lock state of the car (locked or unlocked)>*/
	LockState_t currentLockState;
	/*! <Variable to carry the previous lock state of the car (locked or unlocked)>*/
	LockState_t prevLockState;
	/*! <Variable to indicate if the code is sent or not>*/
	bool codeReceivedFlag;
	/*! <Variable to carry the current power mode of the module (sleep / awake) >*/
	PowerMode_t powerMode;
	/*! <Variable to carry the number of lock/unlock operations since the module is on>*/
	uint8_t numOperations;
	/*! <Buffer to store the code for further processing>*/
	uint8_t CodeBuffer[CODE_LENGTH];
	/*! <Variable to carry the current sequence number that is synchronized with the car key>*/
	uint16_t currentSequenceNumber;
} CentralLock_t;

typedef enum {
	MODULE_LED_ON, MODULE_LED_OFF, MODULE_LED_BLINK
} ModuleLedState_t;

typedef enum {
	KEY, KEYLESS
} StateChangeSource_t;

/* Section Functions prototypes -------------------------------------------------------------*/
void CentralLock_Init(CentralLock_t *_centralLock);

void CentralLock_DoorChangeState(CentralLock_t *_centralLock,
		LockState_t _currentState, StateChangeSource_t _stateChangeSource);

void CentralLock_ReceiveCodeNonBlocking(CentralLock_t *_centralLock);

void CentralLock_SetCurrentLockState(CentralLock_t *_centralLock,
		LockState_t _currentLockState);

void CentralLock_SetPrevLockState(CentralLock_t *_centralLock,
		LockState_t _prevLockState);

void CentralLock_ClearCodeBuffer(CentralLock_t *_centralLock);

CodeStatus_t CentralLock_GetCodeStatus(CentralLock_t *_centralLock);

uint16_t CentralLock_DecryptCode(CentralLock_t *_centralLock);

void CentralLock_ChangeModuleLedState(CentralLock_t *_centralLock,
		ModuleLedState_t _moduleLedState);

void CentralLock_SetPowerMode(CentralLock_t *_centralLock, PowerMode_t _mode);

void CentralLock_SetCodeReceivedFlag(CentralLock_t *_centralLock,
bool _codeReceivedFlag);

#endif /* INC_CENTRAL_LOCK_H_ */
