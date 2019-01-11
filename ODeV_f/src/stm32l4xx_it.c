/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "EXTIPinMap.h"
#include "stm32l4r9i_eval.h"



// Forward function declarations
// ****************************

extern void xPortSysTickHandler(void);

/**
 * Map one EXTI to n callback based on the GPIO PIN.
 */
static inline void ExtiDefISR();


// External variables
// ******************

extern SD_HandleTypeDef  hsd_eval;
extern I2C_HandleTypeDef hi2c_eval;

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
/*            Cortex-M4 Processor Interruption and Exception Handlers         */
/******************************************************************************/

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
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line0 interrupt.
*/
void EXTI0_IRQHandler (void)
{
  HAL_GPIO_EXTI_IRQHandler(MFX_INT_PIN); // GUI
}

/**
* @brief This function handles EXTI line2 interrupt.
*/
void EXTI2_IRQHandler (void)
{
//  // GUI
//#if defined(USE_GVO_390x390)
//  HAL_GPIO_EXTI_IRQHandler(TS_DSI_INT_PIN);
//#else
//  HAL_GPIO_EXTI_IRQHandler(TS_RGB_INT_PIN);
//#endif

  ExtiDefISR();
}


// GUI specific ISR
// ****************

void EXTI15_10_IRQHandler(void)
{
//  if(__HAL_GPIO_EXTI_GET_FLAG(WAKEUP_BUTTON_PIN) != RESET)
//  {
//    HAL_GPIO_EXTI_IRQHandler(WAKEUP_BUTTON_PIN);
//  }

  ExtiDefISR();
}

/**
 * @brief  This function handles SDMMC interrupt request.
 * @param  None
 * @retval None
 */
void SDMMC1_IRQHandler(void)
{
  HAL_SD_IRQHandler(&hsd_eval);
}

/**
  * @brief  This function handles I2C transfer global interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hi2c_eval);
}

/**
  * @brief  This function handles I2C error global interrupt request.
  * @param  None
  * @retval None
  */
void I2C2_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hi2c_eval);
}

/**
  * @brief  This function handles LTDC error global interrupt request.
  * @param  None
  * @retval None
  */
void LTDC_ER_IRQHandler(void)
{
  configASSERT(0);
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
