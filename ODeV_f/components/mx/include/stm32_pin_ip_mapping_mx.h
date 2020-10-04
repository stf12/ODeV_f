/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : stm32_pin_ip_mapping_mx.h
  * @brief          : Header PIN and IP mapping.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_PIN_IP_MAPPING_MX_H
#define __STM32_PIN_IP_MAPPING_MX_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

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

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin                          GPIO_PIN_13
#define B1_GPIO_Port                    GPIOC
#define USART_TX_Pin                    GPIO_PIN_2
#define USART_TX_GPIO_Port              GPIOA
#define USART_RX_Pin                    GPIO_PIN_3
#define USART_RX_GPIO_Port              GPIOA
#define LD2_Pin                         GPIO_PIN_5
#define LD2_GPIO_Port                   GPIOA
#define TMS_Pin                         GPIO_PIN_13
#define TMS_GPIO_Port                   GPIOA
#define TCK_Pin                         GPIO_PIN_14
#define TCK_GPIO_Port                   GPIOA
#define SWO_Pin                         GPIO_PIN_3
#define SWO_GPIO_Port                   GPIOB
#define SYS_DBG_TP1_Pin                 GPIO_PIN_0
#define SYS_DBG_TP1_GPIO_Port           GPIOC
#define SYS_DBG_TP2_Pin                 GPIO_PIN_1
#define SYS_DBG_TP2_GPIO_Port           GPIOC
/* USER CODE BEGIN Private defines */
// USART used for the Debug Log
#define SYS_DBG_USART                   huart2
#define SYS_DBG_USART_MX_INIT()         MX_USART2_UART_Init()
// Test Point PINs
#define SYS_DBG_TP1_PIN                 SYS_DBG_TP1_Pin
#define SYS_DBG_TP1_PORT                SYS_DBG_TP1_GPIO_Port
#define SYS_DBG_TP2_PIN                 SYS_DBG_TP2_Pin
#define SYS_DBG_TP2_PORT                SYS_DBG_TP2_GPIO_Port
#define SYS_DBG_TP_CLK_ENABLE()         __HAL_RCC_GPIOH_CLK_ENABLE()
// TIMer for the runtime statistics
#define SYS_DBG_TIM                     htim11
#define SYS_DBG_TIM_IRQ_N               TIM1_TRG_COM_TIM11_IRQn
#define SYS_DBG_TIM_INIT()              MX_TIM11_Init()
// Re-map the HAL error handler to the framework error function
void sys_error_handler(void); // forward function declaration
#define Error_Handler()                 sys_error_handler()
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_PIN_IP_MAPPING_MX_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
