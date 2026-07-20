/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define ROT1_Pin GPIO_PIN_1
#define ROT1_GPIO_Port GPIOA
#define ROT2_Pin GPIO_PIN_2
#define ROT2_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_4
#define DC_GPIO_Port GPIOA
#define SW1_Pin GPIO_PIN_0
#define SW1_GPIO_Port GPIOB
#define SPI_CS2_Pin GPIO_PIN_10
#define SPI_CS2_GPIO_Port GPIOB
#define SPI_CS1_Pin GPIO_PIN_11
#define SPI_CS1_GPIO_Port GPIOB
#define BLK_Pin GPIO_PIN_8
#define BLK_GPIO_Port GPIOA
#define Display_RES_Pin GPIO_PIN_9
#define Display_RES_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define SD_SPI_HANDLE hspi1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
