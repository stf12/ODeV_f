/**
 ******************************************************************************
 * @file    UtilityDriver.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Aug 23, 2020
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

#include "UtilityDriver.h"
#include "UtilityDriver_vtbl.h"
#include "FreeRTOS.h"
#include "mx.h"
#include "sysdebug.h"

#define UTL_TIMx                        TIM5
#define UTL_TIMx_CLK_ENABLE()           __HAL_RCC_TIM5_CLK_ENABLE()

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
 * UtilityDriver Driver virtual table.
 */
static const IDriver_vtbl s_xUtilityDriver_vtbl = {
    UtilityDriver_vtblInit,
    UtilityDriver_vtblStart,
    UtilityDriver_vtblStop,
    UtilityDriver_vtblDoEnterPowerMode,
    UtilityDriver_vtblReset
};


// Private member function declaration
// ***********************************


// Public API definition
// *********************

IDriver *UtilityDriverAlloc() {
  IDriver *pNewObj = (IDriver*)pvPortMalloc(sizeof(UtilityDriver));

  if (pNewObj == NULL) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("UtilityDriver - alloc failed.\r\n"));
  }
  else {
    pNewObj->vptr = &s_xUtilityDriver_vtbl;
  }

  return pNewObj;
}

sys_error_code_t UtilityDriver_vtblInit(IDriver *_this, void *pParams) {
  assert_param(_this);
//  assert_param(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilityDriver *pObj = (UtilityDriver*)_this;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  UTL_TIMx_CLK_ENABLE();
  /* Set TIMx instance */
  pObj->m_xTimer.Instance = UTL_TIMx;

  /* Initialize TIMx peripheral as follows:
       + Period = 10000 - 1
       + Prescaler = (SystemCoreClock/10000) - 1
       + ClockDivision = 0
       + Counter direction = Up
  */
  pObj->m_xTimer.Init.Period            = 0xFFFFFFFF;
  pObj->m_xTimer.Init.Prescaler         = 0;
  pObj->m_xTimer.Init.ClockDivision     = 0;
  pObj->m_xTimer.Init.CounterMode       = TIM_COUNTERMODE_UP;
  pObj->m_xTimer.Init.RepetitionCounter = 0;

  if (HAL_TIM_Base_Init(&pObj->m_xTimer) != HAL_OK) {
    xRes = SYS_UNDEFINED_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    sys_error_handler();
  }

  // configure LED2
  __HAL_RCC_GPIOD_CLK_ENABLE();
//  HAL_PWREx_EnableVddIO2();
  HAL_GPIO_WritePin(GPIOD, LED2_Pin, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  // configure the push button
  __HAL_RCC_GPIOE_CLK_ENABLE();

  // Initialize PE0 (USER_BUTTON)
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 15, 0);

  return xRes;
}

sys_error_code_t UtilityDriver_vtblStart(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilityDriver *pObj = (UtilityDriver*)_this;

  if (HAL_TIM_Base_Start(&pObj->m_xTimer) != HAL_OK) {
    xRes = SYS_UNDEFINED_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    sys_error_handler();
  }

  return xRes;
}

sys_error_code_t UtilityDriver_vtblStop(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilityDriver *pObj = (UtilityDriver*)_this;

  if (HAL_TIM_Base_Stop(&pObj->m_xTimer) != HAL_OK) {
    xRes = SYS_UNDEFINED_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    sys_error_handler();
  }
  pObj->m_xTimer.Instance->CNT = 0;

  return xRes;
}

sys_error_code_t UtilityDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  UtilityDriver *pObj = (UtilityDriver*)_this;

  if (eNewPowerMode == E_POWER_MODE_DATALOG) {
    UtilityDriver_vtblStart(_this);
  }
  else {
    UtilityDriver_vtblStop(_this);
  }

  return xRes;
}

sys_error_code_t UtilityDriver_vtblReset(IDriver *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  UtilityDriver *pObj = (UtilityDriver*)_this;

  return xRes;
}


// Private function definition
// ***************************
