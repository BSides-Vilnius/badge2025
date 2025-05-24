/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025, BSides Vilnius
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
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
#include "stm32l0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "testing.h"
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
#define GAUGE_MEASURE_Pin GPIO_PIN_3
#define GAUGE_MEASURE_GPIO_Port GPIOA
#define SAO_CK_Pin GPIO_PIN_5
#define SAO_CK_GPIO_Port GPIOA
#define GAUGE_DISABLED_Pin GPIO_PIN_6
#define GAUGE_DISABLED_GPIO_Port GPIOA
#define SAO_SD_Pin GPIO_PIN_7
#define SAO_SD_GPIO_Port GPIOA
#define BTN_D_Pin GPIO_PIN_10
#define BTN_D_GPIO_Port GPIOB
#define BTN_D_EXTI_IRQn EXTI4_15_IRQn
#define LED_CK_Pin GPIO_PIN_13
#define LED_CK_GPIO_Port GPIOB
#define LED_SD_Pin GPIO_PIN_15
#define LED_SD_GPIO_Port GPIOB
#define BTN_A_Pin GPIO_PIN_8
#define BTN_A_GPIO_Port GPIOB
#define BTN_A_EXTI_IRQn EXTI4_15_IRQn
#define BTN_M_Pin GPIO_PIN_9
#define BTN_M_GPIO_Port GPIOB
#define BTN_M_EXTI_IRQn EXTI4_15_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
