#include "central_lock.h"

/* Section Variables -------------------------------------------------------------*/

/*! <Headers that are used in the checking of the code process>*/
static const uint8_t CodeHeaders[4] = { 0b01010101, 0b10101010, 0b00001111,
		0b11110000 };

/*! <UART module to receive the code>*/
extern UART_HandleTypeDef huart1;

/* Section Functions implementation -------------------------------------------------------------*/
/**
 * @brief  Initialize Central Lock Module
 * @note   This function initializes the Central Lock module by:
 * 			1. setting up its initial lock state with LOCK state.
 *         	2. setting up its initial power state with AWAKE state.
 *         	3. setting up reception flag with false.
 *         	4. Configuring parameters for code reception operations.
 *         	5. It fetches the old sequence number from Flash memory.
 *          6. starts the process of receiving lock codes in a non-blocking manner.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @retval None
 */
void CentralLock_Init(CentralLock_t *_centralLock) {

	/*! <We should here check the door if it is locked or not> */
	/*! <But for now it will be like this until I get more HW> */
	_centralLock->currentLockState = LOCKED;
	_centralLock->prevLockState = LOCKED;

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

/**
 * @brief  Change Central Lock Door State
 * @note   This function simulates the change in the state of the central lock doors by controlling
 *         the GPIO pins associated with each door. It updates the internal state variables,
 *         sets the current and previous lock states, and performs additional operations based on
 *         the provided state change source (e.g., keyless entry).
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @param  _currentState: The new state of the doors (LOCKED or UNLOCKED).
 * @param  _stateChangeSource: The source of the state change (e.g., KEYLESS).
 * @retval None
 */
void CentralLock_DoorChangeState(CentralLock_t *_centralLock,
		LockState_t _currentState, StateChangeSource_t _stateChangeSource) {

	/*! <Simulate locking and unlocking the doors> */
	HAL_GPIO_WritePin(DOOR0_GPIO_Port, DOOR0_Pin, _currentState);
	HAL_GPIO_WritePin(DOOR1_GPIO_Port, DOOR1_Pin, _currentState);
	HAL_GPIO_WritePin(DOOR2_GPIO_Port, DOOR2_Pin, _currentState);
	HAL_GPIO_WritePin(DOOR3_GPIO_Port, DOOR3_Pin, _currentState);

	/* Update internal lock state variables */
	CentralLock_SetCurrentLockState(_centralLock, _currentState);
	CentralLock_SetPrevLockState(_centralLock, _currentState);

	if (_stateChangeSource == KEYLESS) {
		/* Increment the number of operations and store the code if necessary */
		_centralLock->numOperations++;

		if (_centralLock->numOperations >= _centralLock->maxNpOperations)
			HAL_FlashStoreData(
					_centralLock->CodeBuffer + SEQUENCE_NUMBER_LENGTH,
					SEQUENCE_NUMBER_LENGTH, FLASH_START_ADDRESS);

		_centralLock->numOperations %= _centralLock->maxNpOperations;
	}
}

/**
 * @brief  Initiate Non-Blocking Code Reception for Central Lock
 * @note   This function initiates the non-blocking reception of a lock code using the UART communication.
 *         It leverages the Hardware Abstraction Layer (HAL) provided by STMicroelectronics.
 *         Upon reception, the code is stored in the internal buffer of the Central Lock module.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @retval None
 */
void CentralLock_ReceiveCodeNonBlocking(CentralLock_t *_centralLock) {
	HAL_UART_Receive_IT(&huart1, _centralLock->CodeBuffer, CODE_LENGTH);
}

/**
 * @brief  Set Current Lock State for Central Lock Module
 * @note   This function sets the current lock state of the central lock module to the provided state.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @param  _currentLockState: The new current lock state to be set (LOCKED or UNLOCKED).
 * @retval None
 */
void CentralLock_SetCurrentLockState(CentralLock_t *_centralLock,
		LockState_t _currentLockState) {
	_centralLock->currentLockState = _currentLockState;
}

/**
 * @brief  Set Previous Lock State for Central Lock Module
 * @note   This function sets the previous lock state of the central lock module to the provided state.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @param  _prevLockState: The new previous lock state to be set (LOCKED or UNLOCKED).
 * @retval None
 */
void CentralLock_SetPrevLockState(CentralLock_t *_centralLock,
		LockState_t _prevLockState) {
	_centralLock->prevLockState = _prevLockState;

}

/**
 * @brief  Clear Code Buffer for Central Lock Module
 * @note   This function clears the internal code buffer of the central lock module by setting
 *         each element to the null character ('\0').
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @retval None
 */
void CentralLock_ClearCodeBuffer(CentralLock_t *_centralLock) {
	for (int i = 0; i < CODE_LENGTH; i++) {
		_centralLock->CodeBuffer[i] = '\0';
	}
}

/**
 * @brief  Get Status of Received Code for Central Lock Module
 * @note   This function evaluates the status of the received lock code based on predefined
 *         headers, decryption, and sequence number validation. It returns the status indicating
 *         whether the code is valid, invalid, or out of range.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @retval CodeStatus_t: Status of the received lock code (VALID, UNVALID, or OUT_OF_RANGE).
 */
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
		_centralLock->currentSequenceNumber = decryptedSequenceNumber;
	}
	return VALID;
}

/**
 * @brief  Decrypt Lock Code for Central Lock Module
 * @note   This function decrypts the received lock code from the code buffer
 *         to obtain the sequence number.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @retval uint16_t: The decrypted sequence number.
 */
uint16_t CentralLock_DecryptCode(CentralLock_t *_centralLock) {
	uint16_t decryptedCode = 0;
	decryptedCode = _centralLock->CodeBuffer[SECOND_BYTE_IN_SEQ_NUM];
	decryptedCode = decryptedCode << FLASH_BYTE_SIZE;
	decryptedCode = decryptedCode
			| _centralLock->CodeBuffer[FIRST_BYTE_IN_SEQ_NUM];
	return decryptedCode;
}

/**
 * @brief  Change Central Lock Module LED State
 * @note   This function controls the state of the module's LED based on the specified LED state.
 *         If the state is MODULE_LED_BLINK, the LED will blink for a short duration; otherwise,
 *         the LED will be set to the provided state.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @param  _moduleLedState: The desired state of the module's LED (MODULE_LED_ON, MODULE_LED_OFF, MODULE_LED_BLINK).
 * @retval None
 */
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

/**
 * @brief  Set Power Mode for Central Lock Module
 * @note   This function sets the power mode of the central lock module to the specified mode.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @param  _mode: The desired power mode to be set (SLEEP, AWAKE).
 * @retval None
 */
void CentralLock_SetPowerMode(CentralLock_t *_centralLock, PowerMode_t _mode) {
	_centralLock->powerMode = _mode;
}

/**
 * @brief  Set Code Received Flag for Central Lock Module
 * @note   This function sets the internal code received flag of the central lock module
 *         based on the provided flag value.
 * @param  _centralLock: Pointer to the CentralLock_t structure representing the central lock module.
 * @param  _codeReceivedFlag: The flag indicating whether a code has been received (true) or not (false).
 * @retval None
 */
void CentralLock_SetCodeReceivedFlag(CentralLock_t *_centralLock,
bool _codeReceivedFlag) {
	_centralLock->codeReceivedFlag = _codeReceivedFlag;
}
