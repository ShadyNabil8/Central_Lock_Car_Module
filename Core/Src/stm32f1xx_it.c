/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32f1xx_it.c
 * @brief   Interrupt Service Routines.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "central_lock.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ABS(x) ((x)>0?(x):-(x))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
const uint32_t debounceDelay = 500;
volatile uint32_t lastDebounceTime = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int debug = 0;
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern UART_HandleTypeDef huart1;
extern CentralLock_t CentralLock;
/* USER CODE BEGIN EV */
extern UART_HandleTypeDef huart1;
extern volatile LockState_t LockState;
extern volatile uint8_t receivedBytes;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
	/* USER CODE BEGIN NonMaskableInt_IRQn 0 */

	/* USER CODE END NonMaskableInt_IRQn 0 */
	/* USER CODE BEGIN NonMaskableInt_IRQn 1 */
	while (1) {
	}
	/* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
	/* USER CODE BEGIN HardFault_IRQn 0 */

	/* USER CODE END HardFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_HardFault_IRQn 0 */
		/* USER CODE END W1_HardFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
	/* USER CODE BEGIN MemoryManagement_IRQn 0 */

	/* USER CODE END MemoryManagement_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
		/* USER CODE END W1_MemoryManagement_IRQn 0 */
	}
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void) {
	/* USER CODE BEGIN BusFault_IRQn 0 */

	/* USER CODE END BusFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_BusFault_IRQn 0 */
		/* USER CODE END W1_BusFault_IRQn 0 */
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
	/* USER CODE BEGIN UsageFault_IRQn 0 */

	/* USER CODE END UsageFault_IRQn 0 */
	while (1) {
		/* USER CODE BEGIN W1_UsageFault_IRQn 0 */
		/* USER CODE END W1_UsageFault_IRQn 0 */
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
	/* USER CODE BEGIN SVCall_IRQn 0 */

	/* USER CODE END SVCall_IRQn 0 */
	/* USER CODE BEGIN SVCall_IRQn 1 */

	/* USER CODE END SVCall_IRQn 1 */
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
	/* USER CODE BEGIN DebugMonitor_IRQn 0 */

	/* USER CODE END DebugMonitor_IRQn 0 */
	/* USER CODE BEGIN DebugMonitor_IRQn 1 */

	/* USER CODE END DebugMonitor_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
	/* USER CODE BEGIN PendSV_IRQn 0 */

	/* USER CODE END PendSV_IRQn 0 */
	/* USER CODE BEGIN PendSV_IRQn 1 */

	/* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
	/* USER CODE BEGIN SysTick_IRQn 0 */

	/* USER CODE END SysTick_IRQn 0 */
	HAL_IncTick();
	/* USER CODE BEGIN SysTick_IRQn 1 */

	/* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles EXTI line0 interrupt.
 */
void EXTI0_IRQHandler(void) {
	/* USER CODE BEGIN EXTI0_IRQn 0 */

	/* USER CODE END EXTI0_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(UNLOCK_ISR_Pin);
	/* USER CODE BEGIN EXTI0_IRQn 1 */
	uint32_t currentMillis = Millis();
	if (ABS(currentMillis - lastDebounceTime) >= debounceDelay) {
		HAL_GPIO_TogglePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin);
		CentralLock_SetCurrentLockState(UNLOCKED);
		lastDebounceTime = currentMillis;
	}
	/* USER CODE END EXTI0_IRQn 1 */
}

/**
 * @brief This function handles EXTI line1 interrupt.
 */
void EXTI1_IRQHandler(void) {
	/* USER CODE BEGIN EXTI1_IRQn 0 */

	/* USER CODE END EXTI1_IRQn 0 */
	HAL_GPIO_EXTI_IRQHandler(LOCK_ISR_Pin);
	/* USER CODE BEGIN EXTI1_IRQn 1 */
	uint32_t currentMillis = Millis();
	if (ABS(currentMillis - lastDebounceTime) >= debounceDelay) {
		HAL_GPIO_TogglePin(BUILT_IN_LED_GPIO_Port, BUILT_IN_LED_Pin);
		CentralLock_SetCurrentLockState(LOCKED);
		lastDebounceTime = currentMillis;
	}
	/* USER CODE END EXTI1_IRQn 1 */
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void) {
	/* USER CODE BEGIN USART1_IRQn 0 */

	/* USER CODE END USART1_IRQn 0 */
	HAL_UART_IRQHandler(&huart1);
	/* USER CODE BEGIN USART1_IRQn 1 */
	/*
	 * This Function is triggered each time a byte is received.
	 */
	debug++;
	/* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */
/**
 * @brief  Rx Transfer completed callbacks.
 * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	/*
	 * This Function is triggered only when all data are received.
	 */
//	uint8_t sequenceNumber = CentralLock_GetCurSequenceNum();
//	uint8_t codePortion = CentralLock_GetCodePortion(sequenceNumber);
//	// This mean we receive a header not a real code.
//	if (sequenceNumber == 0x0 || sequenceNumber == 0x01
//			|| sequenceNumber == 0x04 || sequenceNumber == 0x05) {
//		uint8_t codeHeader = CentralLock_GetCodeHeader(sequenceNumber);
//		// This means that the received byte is a header and also a correct header in sequence.
//		if (codeHeader == codePortion) {
//			HAL_UART_Transmit(&huart1, &codeHeader, 1, 5);
//			// If the received code portion is the last portion of a code, then we received all the code correctly.
//			if (sequenceNumber == 0x05) {
//				CentralLock_UnlockDoors(&CentralLock);
//			}
//			CentralLock_IncCurSequenceNum();
//		}
//		// If the header received was not in correct sequence.
//		else {
//			CentralLock_RstCurSequenceNum();
//			// Send something
//		}
//
//	}
//	// We receive a real code.
//	else {
//		CentralLock_IncCurSequenceNum();
//		HAL_UART_Transmit(&huart1, &codePortion, 1, 5);
//
//	}
//	//CentralLock_IncCurSequenceNum();
	CodeStatus_t status = CentralLock_GetCodeStatus();
	if (status == VALID) {

	} else {
	}
	CentralLock_ClearCodeBuffer();
	CentralLock_ReceiveCodeNonBlocking();
}
/* USER CODE END 1 */
