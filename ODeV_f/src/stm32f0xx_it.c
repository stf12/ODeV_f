/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
// Includes
// ********
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "EXTIPinMap.h"


// Forward function declarations
// ****************************

extern void xPortSysTickHandler(void);

/**
 * Map one EXTI to n callback based on the GPIO PIN.
 */
static inline void ExtiDefISR();


// External variables
// ******************

extern PCD_HandleTypeDef g_hpcd;

EXTI_DECLARE_PIN2F_MAP()


// Private function definition
void ExtiDefISR() {
  EXTIPin2CallbckMap xMap = EXTI_GET_P2F_MAP();
  for (int i=0; xMap[i].pfCallback != NULL; i++) {
    if (__HAL_GPIO_EXTI_GET_IT(xMap[i].nPin)) {
      /* EXTI line interrupt detected */
      __HAL_GPIO_EXTI_CLEAR_IT(xMap[i].nPin);
      xMap[i].pfCallback(xMap[i].nPin);
    }
  }
}

/******************************************************************************/
/*           Cortex-M0 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1) {
  }
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
  {
#endif  /* INCLUDE_xTaskGetSchedulerState */
    xPortSysTickHandler();
#if (INCLUDE_xTaskGetSchedulerState  == 1 )
  }
#endif  /* INCLUDE_xTaskGetSchedulerState */
}


/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles USB global interrupt / USB wake-up interrupt through EXTI line 18.
  */
void USB_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&g_hpcd);
}

void EXTI0_1_IRQHandler(void)
{
  ExtiDefISR();
}

void EXTI2_3_IRQHandler(void)
{
  ExtiDefISR();
}
void EXTI4_15_IRQHandler(void)
{
  ExtiDefISR();
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
