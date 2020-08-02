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

//Select the SystemClock_Config
//#define SystemClock_Config_SensorTile SystemClock_Config
#define SystemClock_Config_MX SystemClock_Config

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

void SystemClock_Config_MX(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST) != HAL_OK)
  {
    sys_error_handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 30;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    sys_error_handler();
  }

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    sys_error_handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_SDMMC1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_HSI48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    sys_error_handler();
  }

  //TODO: STF.Begin
//  /* Enable Power Clock*/
//  __HAL_RCC_PWR_CLK_ENABLE();
//  HAL_PWREx_EnableVddUSB();
//  HAL_PWREx_EnableVddIO2();
//  //BSP_Enable_DCDC2();
//  GPIO_InitTypeDef  GPIO_InitStruct;
//
//  __HAL_RCC_GPIOE_CLK_ENABLE();
//
//  /* Configure the GPIO_LED pin */
//  GPIO_InitStruct.Pin = GPIO_PIN_13;
//  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
//
//  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
//
//  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
  // STF.End

}

/**
 * @brief  System Clock Configuration
 *         The system Clock is configured as follow :
 *            System Clock source            = PLL (MSI)
 *            SYSCLK(Hz)                     = 80000000
 *            HCLK(Hz)                       = 80000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 1
 *            APB2 Prescaler                 = 1
 *            MSI Frequency(Hz)              = 48000000
 *            PLL_M                          = 6
 *            PLL_N                          = 40
 *            PLL_P                          = 7
 *            PLL_R                          = 4
 *            PLL_Q                          = 4
 *            Flash Latency(WS)              = 4
 * @param  None
 * @retval None
 */
void SystemClock_Config_SensorTile(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
#if SENSING1_USE_USB
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;
#endif /* SENSING1_USE_USB */

  /* Enable access to the backup domain */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  /* RTC Clock selection can be changed only if the Backup Domain is reset */
  __HAL_RCC_BACKUPRESET_FORCE();
  __HAL_RCC_BACKUPRESET_RELEASE();

  /* set low drive on LSE to reduce power consumption */
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /* Enable the LSE Oscilator and Disable the LSI Oscillator */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    sys_error_handler();
  }

  /* Enable the CSS interrupt in case LSE signal is corrupted or not present */
  HAL_RCCEx_DisableLSECSS();

  /* Enable MSI Oscillator and activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState            = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange       = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM            = 6;
  RCC_OscInitStruct.PLL.PLLN            = 40;
  RCC_OscInitStruct.PLL.PLLP            = 7;
  RCC_OscInitStruct.PLL.PLLQ            = 4;
  RCC_OscInitStruct.PLL.PLLR            = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    sys_error_handler();
  }

  /* Enable MSI Auto-calibration through LSE */
  HAL_RCCEx_EnableMSIPLLMode();

#if SENSING1_USE_USB
  /* Select MSI output as USB clock source */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_MSI;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
#endif /* SENSING1_USE_USB */

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK |
                                      RCC_CLOCKTYPE_HCLK |
                                      RCC_CLOCKTYPE_PCLK1 |
                                      RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
    sys_error_handler();
  }
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

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
    sys_error_handler();
  }

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
  __HAL_RCC_WAKEUPSTOP_CLK_CONFIG(RCC_STOP_WAKEUPCLOCK_HSI);


//  // This function is called in the early step of the system initialization.
//  // All the PINs used by the application are reconfigured later by the application tasks.
//
//  GPIO_InitTypeDef GPIO_InitStruct;
//
//  // Configure all GPIO as analog to reduce current consumption on non used IOs
//  // Enable GPIOs clock
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();
//  __HAL_RCC_GPIOC_CLK_ENABLE();
//  __HAL_RCC_GPIOD_CLK_ENABLE();
//  __HAL_RCC_GPIOH_CLK_ENABLE();
//
//  // Configure GPIO pins : PA0 PA1 PA2 PA3 PA4 PA5
//  //                       PA6 PA7 PA8 PA9 PA10
//  //                       PA11 PA12 PA15
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6
//                          |GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
//                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
//  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
//  GPIO_InitStruct.Pull = GPIO_NOPULL;
//  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
//
//  // Configure GPIO pins : PB0 PB1 PB2 PB3 PB4 PB5
//  //                       PB6 PB7 PB7 PB8 PB9 PB10
//  //                       PB11 PB12 PB13 PB14 PB15
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4
//                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9
//                          |GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13
//                          |GPIO_PIN_14|GPIO_PIN_15;
//  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//  // Configure GPIO pins : PC0 PC1 PC2 PC3
//  //                       PC4 PC5 PC6 PC7
//  //                       PC8 PC9 PC10 PC11
//  //                       PC12 PC13
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
//                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
//                          |GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
//                          |GPIO_PIN_12|GPIO_PIN_13;
//  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
//
//  // Configure GPIO pin : PD2
//  GPIO_InitStruct.Pin = GPIO_PIN_2;
//  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
//
//  // Configure GPIO pin : PH0 PH1 PH3
//  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_3;
//  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
//
//  // Disable GPIOs clock
//  __HAL_RCC_GPIOA_CLK_DISABLE();
//  __HAL_RCC_GPIOB_CLK_DISABLE();
//  __HAL_RCC_GPIOC_CLK_DISABLE();
//  __HAL_RCC_GPIOD_CLK_DISABLE();
//  __HAL_RCC_GPIOH_CLK_DISABLE();
}

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  // System interrupt init
  // MemoryManagement_IRQn interrupt configuration
  HAL_NVIC_SetPriority(MemoryManagement_IRQn, 0, 0);
  // BusFault_IRQn interrupt configuration
  HAL_NVIC_SetPriority(BusFault_IRQn, 0, 0);
  // UsageFault_IRQn interrupt configuration
  HAL_NVIC_SetPriority(UsageFault_IRQn, 0, 0);
  // SVCall_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SVCall_IRQn, 0, 0);
  // DebugMonitor_IRQn interrupt configuration
  HAL_NVIC_SetPriority(DebugMonitor_IRQn, 0, 0);
  // PendSV_IRQn interrupt configuration
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
  // SysTick_IRQn interrupt configuration
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

// Private function definition
// ***************************
