/**
 ******************************************************************************
 * @file    NucleoDriver.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 2, 2018
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

#include "NucleoDriver.h"
#include "NucleoDriver_vtbl.h"
#include "FreeRTOS.h"
#include "sysdebug.h"

#ifndef NUCLEO_DRV_CFG_IRQ_PRIORITY
#define NUCLEO_DRV_CFG_IRQ_PRIORITY    2
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
 * NucleoDriver Driver virtual table.
 */
static const IDriver_vtbl s_xNucleoDriver_vtbl = {
    NucleoDriver_vtblInit,
    NucleoDriver_vtblStart,
    NucleoDriver_vtblStop,
    NucleoDriver_vtblDoEnterPowerMode,
    NucleoDriver_vtblWrite,
    NucleoDriver_vtblRead
};

/**
 * Data used by the low leve HAL API.
 */
typedef struct _HardwareResources {
  volatile boolean_t *pbPB1Pressed;
  SemaphoreHandle_t xSyncObj;
} HardwareResources;

/**
 * Specifies the resources used by the hardware (the low level HAL API like ISR, etc.)
 */
static HardwareResources s_xHardwareResources;


// Inline function forward declaration
// ***********************************

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
extern sys_error_code_t NucleoDriverToggleLed(NucleoDriver *this);
extern sys_error_code_t NucleoDriverSetLed(NucleoDriver *this, boolean_t bON);
extern boolean_t NucleoDriverIsButtonPressed(NucleoDriver *this);
#endif

// Private member function declaration
// ***********************************


// Public API definition
// *********************

IDriver *NucleoDriverAlloc() {
  IDriver *pNewObj = (IDriver*)pvPortMalloc(sizeof(NucleoDriver));

  if (pNewObj == NULL) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("NucleoDriver - alloc failed.\r\n"));
  }
  else {
    pNewObj->vptr = &s_xNucleoDriver_vtbl;
  }

  return pNewObj;
}

sys_error_code_t NucleoDriver_vtblInit(IDriver *_this, void *pParams) {
  assert_param(_this);
  UNUSED(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  NucleoDriver *pObj = (NucleoDriver*)_this;
  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Configure GPIO pin : LD4
  GPIO_InitStruct.Pin = LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD4_GPIO_Port, &GPIO_InitStruct);

  // Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, NUCLEO_DRV_CFG_IRQ_PRIORITY, 0);

  // Configure the software resource
  pObj->m_bPB1Pressed = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) != GPIO_PIN_SET;
  pObj->m_xSyncObj = xSemaphoreCreateBinary();
  if (pObj == NULL) {
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(xRes);
  }

  return xRes;
}

sys_error_code_t NucleoDriver_vtblStart(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  NucleoDriver *pObj = (NucleoDriver*)_this;

  // set the hardware resources
  __disable_irq();
  s_xHardwareResources.pbPB1Pressed = &pObj->m_bPB1Pressed;
  s_xHardwareResources.xSyncObj = pObj->m_xSyncObj;
  __enable_irq();

  HAL_NVIC_ClearPendingIRQ(EXTI4_15_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);

  return xRes;
}

sys_error_code_t NucleoDriver_vtblStop(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UNUSED(_this);
//  NucleoDriver *pObj = (NucleoDriver*)_this;

  // release the hardware resources
  __disable_irq();
  s_xHardwareResources.pbPB1Pressed = NULL;
  s_xHardwareResources.xSyncObj = NULL;
  __enable_irq();

  HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);

  return xRes;
}

sys_error_code_t NucleoDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  NucleoDriver *pObj = (NucleoDriver*)_this;

  return xRes;
}

sys_error_code_t NucleoDriver_vtblWrite(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  NucleoDriver *pObj = (NucleoDriver*)_this;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("NucleoDRV: Write method not used by _this driver.\r\n"));

  return xRes;
}

sys_error_code_t NucleoDriver_vtblRead(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  NucleoDriver *pObj = (NucleoDriver*)_this;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("NucleoDRV: Read method not used by _this driver.\r\n"));

  return xRes;
}


sys_error_code_t NucleoDriverWaitForButtonEvent(NucleoDriver *_this, boolean_t *pbButtonPressed) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  xSemaphoreTake(_this->m_xSyncObj, portMAX_DELAY);
  *pbButtonPressed = _this->m_bPB1Pressed;

  return xRes;
}

sys_error_code_t NucleoDriverStopWaitingForButtonEvent(NucleoDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  xSemaphoreGive(s_xHardwareResources.xSyncObj);

  return xRes;
}


// Private function definition
// ***************************


// CubeMx Integration
// ******************

/**
 * EXTI ISR [PIN10 - PIN15]
 */
void EXTI4_15_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == B1_Pin) {
    if (s_xHardwareResources.pbPB1Pressed != NULL) {
      *s_xHardwareResources.pbPB1Pressed = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) != GPIO_PIN_SET;
      EPowerMode eActivePowerMode = SysGetPowerMode();
      if (eActivePowerMode == E_POWER_MODE_RUN) {
        xSemaphoreGiveFromISR(s_xHardwareResources.xSyncObj, NULL);
      }
    }
  }
}
