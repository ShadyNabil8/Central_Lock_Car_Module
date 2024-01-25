/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CENTRAL_LOCK_LED_Pin GPIO_PIN_13
#define CENTRAL_LOCK_LED_GPIO_Port GPIOC
#define UNLOCK_ISR_Pin GPIO_PIN_0
#define UNLOCK_ISR_GPIO_Port GPIOA
#define UNLOCK_ISR_EXTI_IRQn EXTI0_IRQn
#define LOCK_ISR_Pin GPIO_PIN_1
#define LOCK_ISR_GPIO_Port GPIOA
#define LOCK_ISR_EXTI_IRQn EXTI1_IRQn
#define ALARM_Pin GPIO_PIN_2
#define ALARM_GPIO_Port GPIOA
#define ALARM_EXTI_IRQn EXTI2_IRQn
#define INSIDE_DOOR_ISR_Pin GPIO_PIN_3
#define INSIDE_DOOR_ISR_GPIO_Port GPIOA
#define INSIDE_DOOR_ISR_EXTI_IRQn EXTI3_IRQn
#define DOOR_PHYSICAL_STATE_Pin GPIO_PIN_11
#define DOOR_PHYSICAL_STATE_GPIO_Port GPIOB
#define DOOR0_Pin GPIO_PIN_12
#define DOOR0_GPIO_Port GPIOB
#define DOOR1_Pin GPIO_PIN_13
#define DOOR1_GPIO_Port GPIOB
#define DOOR2_Pin GPIO_PIN_14
#define DOOR2_GPIO_Port GPIOB
#define DOOR3_Pin GPIO_PIN_15
#define DOOR3_GPIO_Port GPIOB
#define ALARM_LED_Pin GPIO_PIN_8
#define ALARM_LED_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_11
#define BUZZER_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
