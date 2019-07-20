/**
 ******************************************************************************
 * @file    sysinit_mx.c
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
#include "syserror.h"

/**
 * This type group together the components of the clock three to be modified during
 * a power mode change.
 */
typedef struct _system_clock_t{
  uint32_t latency;
  RCC_OscInitTypeDef osc;
  RCC_ClkInitTypeDef clock;
  RCC_PeriphCLKInitTypeDef periph_clock;
} system_clock_t;

/**
 * It is used to save and restore the system clock during the power mode switch.
 */
static system_clock_t system_clock;

// Private member function declaration
// ***********************************


// Public API definition
// *********************

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
//  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  //Initializes the CPU, AHB and APB busses clocks
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    sys_error_handler();
  }

  //Initializes the CPU, AHB and APB busses clocks
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    sys_error_handler();
  }
//  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
//  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
//  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
//  {
//    sys_error_handler();
//  }
}

void SystemClock_Backup(void)
{
  HAL_RCC_GetOscConfig(&(system_clock.osc));
  HAL_RCC_GetClockConfig(&(system_clock.clock), &(system_clock.latency));
  HAL_RCCEx_GetPeriphCLKConfig(&(system_clock.periph_clock));
}

/**
  * @brief  Restore original clock parameters
  * @retval Process result
  *         @arg SMPS_OK or SMPS_KO
  */
void SystemClock_Restore(void)
{
  /* SEQUENCE:
   *   1. Set PWR regulator to SCALE1
   *   2. PLL ON
   *   3. Set SYSCLK source to PLL
   *
   * NOTE:
   *   Do not change or update the base-clock source (e.g. MSI and LSE)
   */

  if (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_CFGR_SWS_PLL) {
    if (HAL_RCC_OscConfig(&(system_clock.osc)) != HAL_OK) {
      sys_error_handler();
    }
  }

  if (HAL_RCC_ClockConfig(&(system_clock.clock), system_clock.latency) != HAL_OK) {
    sys_error_handler();
  }
}

void SysPowerConfig() {
  // Select HSI as system clock source after Wake Up from Stop mode
//  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);


  // This function is called in the early step of the system initialization.
  // All the PINs used by the application are reconfigured later by the application tasks.

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configure all GPIO as analog to reduce current consumption on non used IOs
  // Enable GPIOs clock
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
//  __HAL_RCC_GPIOF_CLK_ENABLE();

  // Configure GPIO pins : PA0 PA1 PA2 PA3 PA4 PA5
  //                       PA6 PA7 PA8 PA9 PA10
  //                       PA11 PA12 PA15
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configure GPIO pins : PB0 PB1 PB2 PB3 PB4 PB5
  //                       PB6 PB7 PB7 PB8 PB9 PB10
  //                       PB11 PB12 PB13 PB14 PB15
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
                          |GPIO_PIN_14|GPIO_PIN_15;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  // Configure GPIO pins : PC0 PC1 PC2 PC3
  //                       PC4 PC5 PC6 PC7
  //                       PC8 PC9 PC10 PC11
  //                       PC12 PC13 PC14
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure GPIO pin : PD2
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  // Configure GPIO pin : PF0 PF1
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
//  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  // Disable GPIOs clock
  __HAL_RCC_GPIOA_CLK_DISABLE();
  __HAL_RCC_GPIOB_CLK_DISABLE();
  __HAL_RCC_GPIOC_CLK_DISABLE();
  __HAL_RCC_GPIOD_CLK_DISABLE();
//  __HAL_RCC_GPIOF_CLK_DISABLE();
}

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  // System interrupt init
  // PendSV_IRQn interrupt configuration
  HAL_NVIC_SetPriority(PendSV_IRQn, 3, 0);
  // SysTick_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SysTick_IRQn, 3, 0);
}

// Private function definition
// ***************************
