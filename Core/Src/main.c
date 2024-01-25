/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "central_lock.h"
#include "HAL_flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern UART_HandleTypeDef huart1;
CentralLock_t centralLock;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	LockState_t currentLockState;
	LockState_t prevLockState;
	PowerMode_t powerMode;
	bool codeReceivedFlag;
	CENTRALLOCK_StatusTypeDef CL_State;
	AlarmState_t alarmState;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	CL_State = CentralLock_Init(&centralLock);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		alarmState = centralLock.alarmState;
		if (alarmState == ACTIVE) {
			/*! <Simulate the Alaram action> */
			while (centralLock.alarmState == ACTIVE) {
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
				HAL_GPIO_TogglePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin);
				HAL_Delay(500);
				HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
				HAL_GPIO_TogglePin(ALARM_LED_GPIO_Port, ALARM_LED_Pin);
				HAL_Delay(500);
			}
		}

		currentLockState = centralLock.currentLockState;
		prevLockState = centralLock.prevLockState;

		if (currentLockState != prevLockState) {
			/*! <The car owner has locked or unlocked the car without key-less entry> */

			/*! <Just to indicate that some even has been happened> */
			CL_State = CentralLock_ChangeModuleLedState(&centralLock,
					MODULE_LED_ON);

			CL_State = CentralLock_DoorChangeState(&centralLock,
					currentLockState, KEY);

			/*! <Prepare the central lock module for sleep mode> */
			CL_State = CentralLock_SetPowerMode(&centralLock, AWAKE);
		}
		codeReceivedFlag = centralLock.codeReceivedFlag;
		if (codeReceivedFlag) {
			/*! <The car owner has locked or unlocked the car with key-less entry> */

			CL_State = CentralLock_SetCodeReceivedFlag(&centralLock, false);

			/*! <Just to indicate that some even has been happened> */
			CL_State = CentralLock_ChangeModuleLedState(&centralLock,
					MODULE_LED_ON);

			/*! <check for the correctness of the code send> */
			CL_State = CentralLock_GetCodeStatus(&centralLock);

			if (CL_State == CENTRALLOCK_OK) {
				CentralLock_DoorChangeState(&centralLock, UNLOCKED, KEYLESS);
			} else if (CL_State == CENTRALLOCK_OUTOFRANGE_CODE) {
			} else if (CL_State == CENTRALLOCK_UNVALID_CODE) {

			} else {
			}
			CentralLock_ClearCodeBuffer(&centralLock);

			/*! <Prepare the central lock module for sleep mode> */
			CL_State = CentralLock_SetPowerMode(&centralLock, AWAKE);
		}

#if (MODULE_ENEBLE_SLEEP)
		powerMode = centralLock.powerMode;
		if (powerMode == AWAKE) {

			/*! <Set the current power state of the central lock module> */
			CentralLock_SetPowerMode(&centralLock, SLEEP);

			HAL_Delay(1000);

			/*! <Just to indicate that some even has been happened> */
			CL_State = CentralLock_ChangeModuleLedState(&centralLock,
					MODULE_LED_BLINK);

			/*! <Disable the SysTick timer interrupts to not interrupt the processor
			 *  each 1ms (by default) and get out of the sleep mode >
			 */
			HAL_SuspendTick();

			/*! <Enter Sleep mode> */
			HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
		}
#endif
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
