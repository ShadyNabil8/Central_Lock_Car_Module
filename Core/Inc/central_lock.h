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

#define IS_LOCKSTATE(STATE) ((STATE == LOCKED) || (STATE == UNLOCKED))

#define IS_NULL(PTR) (NULL == PTR)

#define IS_ERROR(VAL) (VAL == CENTRALLOCK_ERROR)

#define IS_LED_STATE(STATE) ((STATE == MODULE_LED_ON) || (STATE == MODULE_LED_OFF) || (STATE == MODULE_LED_BLINK))

#define IS_CHANGE_STATE_SRC(SRC) ((SRC == KEY) || (SRC == KEYLESS))

#define IS_POWER_MODE(MODE) ((MODE == AWAKE) || (MODE == SLEEP))

/* Section typedefines -------------------------------------------------------------*/
/**
 * @brief  HAL Status structures definition
 */
typedef enum {
	CENTRALLOCK_OK = 0x00U,
	CENTRALLOCK_ERROR = 0x01U,
	CENTRALLOCK_UNVALID_CODE = 0x02U,
	CENTRALLOCK_OUTOFRANGE_CODE = 0x04U,
} CENTRALLOCK_StatusTypeDef;

typedef enum {
	LOCKED, UNLOCKED
} LockState_t;

typedef enum {
	VALID, UNVALID, OUT_OF_RANGE
} CodeStatus_t;

typedef enum {
	AWAKE, SLEEP
} PowerMode_t;

typedef enum {
	MODULE_LED_ON, MODULE_LED_OFF, MODULE_LED_BLINK
} ModuleLedState_t;

typedef enum {
	KEY, KEYLESS
} StateChangeSource_t;

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

/* Section Functions prototypes -------------------------------------------------------------*/
CENTRALLOCK_StatusTypeDef CentralLock_Init(CentralLock_t *_centralLock);

CENTRALLOCK_StatusTypeDef CentralLock_DoorChangeState(
		CentralLock_t *_centralLock, LockState_t _currentState,
		StateChangeSource_t _stateChangeSource);

CENTRALLOCK_StatusTypeDef CentralLock_ReceiveCodeNonBlocking(
		CentralLock_t *_centralLock);

CENTRALLOCK_StatusTypeDef CentralLock_SetCurrentLockState(
		CentralLock_t *_centralLock, LockState_t _currentLockState);

CENTRALLOCK_StatusTypeDef CentralLock_SetPrevLockState(
		CentralLock_t *_centralLock, LockState_t _prevLockState);

CENTRALLOCK_StatusTypeDef CentralLock_ClearCodeBuffer(
		CentralLock_t *_centralLock);

CENTRALLOCK_StatusTypeDef CentralLock_GetCodeStatus(CentralLock_t *_centralLock);

CENTRALLOCK_StatusTypeDef CentralLock_DecryptCode(CentralLock_t *_centralLock,
		uint16_t *_decryptedCode);

CENTRALLOCK_StatusTypeDef CentralLock_ChangeModuleLedState(
		CentralLock_t *_centralLock, ModuleLedState_t _moduleLedState);

CENTRALLOCK_StatusTypeDef CentralLock_SetPowerMode(CentralLock_t *_centralLock,
		PowerMode_t _mode);

CENTRALLOCK_StatusTypeDef CentralLock_SetCodeReceivedFlag(
		CentralLock_t *_centralLock,
		bool _codeReceivedFlag);

#endif /* INC_CENTRAL_LOCK_H_ */
