/**
 ******************************************************************************
 * @file    sysdebug_mx.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.1
 * @date    Nov 13, 2017
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#include "systp.h"

#define UART_TX_Pin GPIO_PIN_9
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_10
#define UART_RX_GPIO_Port GPIOA

// Forward function declaration
// ****************************
void sys_error_handler(void);

// Private member function declaration
// ***********************************


// Public API definition
// *********************

void MX_USART_DBG_Init(UART_HandleTypeDef* uartHandle) {
  uartHandle->Instance = USART1;
  uartHandle->Init.BaudRate = 115200;
  uartHandle->Init.WordLength = UART_WORDLENGTH_8B;
  uartHandle->Init.StopBits = UART_STOPBITS_1;
  uartHandle->Init.Parity = UART_PARITY_NONE;
  uartHandle->Init.Mode = UART_MODE_TX_RX;
  uartHandle->Init.HwFlowCtl = UART_HWCONTROL_NONE;
  uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
  uartHandle->Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  uartHandle->AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(uartHandle) != HAL_OK) {
    sys_error_handler();
  }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /**USART2 GPIO Configuration
    PA9             ------> USART2_TX
    PA10             ------> USART2_RX
    */
    GPIO_InitStruct.Pin = UART_TX_Pin|UART_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(UART_TX_GPIO_Port, &GPIO_InitStruct);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA9     ------> USART2_TX
    PA10     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, UART_TX_Pin|UART_RX_Pin);
  }
}

void MX_TIM6_Init(TIM_HandleTypeDef* tim_baseHandle)
{
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  tim_baseHandle->Instance = TIM6;
  tim_baseHandle->Init.Prescaler = 399;
  tim_baseHandle->Init.CounterMode = TIM_COUNTERMODE_UP;
  tim_baseHandle->Init.Period = 1;
  tim_baseHandle->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(tim_baseHandle) != HAL_OK)
  {
    sys_error_handler();
  }

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(tim_baseHandle, &sMasterConfig) != HAL_OK)
  {
    sys_error_handler();
  }

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM6)
  {
    /* TIM6 clock enable */
    __HAL_RCC_TIM6_CLK_ENABLE();

    /* TIM6 interrupt Init */
    HAL_NVIC_SetPriority(TIM6_IRQn, 3, 0);
//    HAL_NVIC_EnableIRQ(TIM6_IRQn);
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM6)
  {
    /* Peripheral clock disable */
    __HAL_RCC_TIM6_CLK_DISABLE();

    /* TIM6 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM6_IRQn);
  }
}

// Private function definition
// ***************************
