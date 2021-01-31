/**
 ******************************************************************************
 * @file    UtilityDriver.h
 * @author  Stefano Oliveri
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
#ifndef INCLUDE_DRIVERS_UTILITYDRIVER_H_
#define INCLUDE_DRIVERS_UTILITYDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IDriver.h"
#include "IDriver_vtbl.h"
#include "mx.h"


/**
 * Create  type name for _UtilityDriver.
 */
typedef struct _UtilityDriver UtilityDriver;

/**
 *  UtilityDriver internal structure.
 */
struct _UtilityDriver {
  /**
   * Base class object.
   */
  IDriver super;

  // Driver variables should be added here.

  /**
   * Hardware timer used to compute the time stamp.
   */
  TIM_HandleTypeDef m_xTimer;
};

/**
 * Initialization parameters for the driver.
 */
typedef struct _UtilityDriverParams {
  uint32_t nParam;
}UtilityDriverParams;


// Public API declaration
//***********************

/**
 * Allocate an instance of UtilityDriver. The driver is allocated
 * in the FreeRTOS heap.
 *
 * @return a pointer to the generic interface ::IDriver if success,
 * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
 */
IDriver *UtilityDriverAlloc();

/**
 * Get the time stamp. It is implemented as the counter of a 16-bit timer.
 *
 * @param _this [IN] specifies a pointer to an ::UtilityDriver object
 * @return the time stamp.
 */
inline uint32_t UtilityDrvGetTimeStamp(UtilityDriver *_this);

/**
 * Enable or disable the push button.
 *
 * @param _this [IN] specifies a pointer to an ::UtilityDriver object
 * @param bEnable [IN] TRUE to enable the push button, FALSE to disable it.
 * @return SYS_NO_ERROR_CODE
 */
inline sys_error_code_t UtilityDrvEnablePushButton(UtilityDriver *_this, boolean_t bEnable);

/**
 *  Toggle the user led LED1.
 *
 * @param _this [IN] specifies a pointer to an ::UtilityDriver object
 * @return SYS_NO_ERROR_CODE
 */
inline sys_error_code_t UtilityDrvToggleLED(UtilityDriver *_this);

/**
 * Set the user LED1 value to on or off.
 *
 * @param _this  [IN] specifies a pointer to an ::UtilityDriver object
 * @param bOn specifies the value of the user LED LED1.
 * @return SYS_NO_ERROR_CODE
 */
inline sys_error_code_t UtilityDrvSetLED(UtilityDriver *_this, boolean_t bOn);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
inline uint32_t UtilityDrvGetTimeStamp(UtilityDriver *_this) {
  assert_param(_this);

  return _this->m_xTimer.Instance->CNT;
}

SYS_DEFINE_INLINE
sys_error_code_t UtilityDrvEnablePushButton(UtilityDriver *_this, boolean_t bEnable) {
  UNUSED(_this);

  if (bEnable) {
    HAL_NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);
  }
  else {
    HAL_NVIC_DisableIRQ(USER_BUTTON_EXTI_IRQn);
  }

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
sys_error_code_t UtilityDrvToggleLED(UtilityDriver *_this) {
  UNUSED(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  LED1_GPIO_Port->ODR ^= LED1_Pin;

  return xRes;
}

SYS_DEFINE_INLINE
sys_error_code_t UtilityDrvSetLED(UtilityDriver *_this, boolean_t bOn) {
  UNUSED(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (bOn) {
    LED1_GPIO_Port->BSRR = LED1_Pin;
  }
  else {
    LED1_GPIO_Port->BRR = LED1_Pin;
  }

  return xRes;
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_UTILITYDRIVER_H_ */
