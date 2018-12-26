/**
 ******************************************************************************
 * @file    NucleoDriver.h
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
 * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
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
#ifndef INCLUDE_DRIVERS_NUCLEODRIVER_H_
#define INCLUDE_DRIVERS_NUCLEODRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IDriver.h"
#include "IDriverVtbl.h"
#include "FreeRTOS.h"
#include "semphr.h"


#define LD1_Pin                         GPIO_PIN_15
#define LD1_GPIO_Port                   GPIOB
#define LD4_Pin                         GPIO_PIN_15
#define LD4_GPIO_Port                   GPIOH
#define B1_Pin                          GPIO_PIN_0
#define B1_GPIO_Port                    GPIOB


/**
 * Create  type name for _NucleoDriver.
 */
typedef struct _NucleoDriver NucleoDriver;

/**
 *  NucleoDriver internal structure.
 */
struct _NucleoDriver {
  /**
   * Base class object.
   */
  IDriver super;

  // Driver variables should be added here.

  /**
   * Specifies if the push button is pressed or not.
   */
  volatile boolean_t m_bPB1Pressed;

  /**
   * Specifies the semaphore used to synchronize the driver with a task.
   */
  SemaphoreHandle_t m_xSyncObj;
};

/**
 * Initialization parameters for the driver.
 */
typedef struct _NucleoDriverParams {
  uint32_t nParam;
}NucleoDriverParams;


// Public API declaration
//***********************

/**
 * Allocate an instance of NucleoDriver. The driver is allocated
 * in the FreeRTOS heap.
 *
 * @return a pointer to the generic interface ::IDriver if success,
 * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
 */
IDriver *NucleoDriverAlloc();

/**
 * Turn the LED ON or OFF
 *
 * @param _this [IN] specifies a pointer to a driver object.
 * @param bON [IN] specifies the status of the LED. `TRUE` to turn the LED ON, `FALSE` otherwise.
 * @return SYS_NO_ERROR_CODE
 */
inline sys_error_code_t NucleoDriverSetLed(NucleoDriver *_this, boolean_t bON);

/**
 * Toggle the LED.
 *
 * @param _this [IN] specifies a pointer to a driver object.
 * @return SYS_NO_ERROR_CODE
 */
inline sys_error_code_t NucleoDriverToggleLed(NucleoDriver *_this);

/**
 * Get the status of the push button.
 *
 * @param _this [IN] specifies a pointer to a driver object.
 * @return `TRUE` if the push button is pressed, `FALSE` otherwise
 */
inline boolean_t NucleoDriverIsButtonPressed(NucleoDriver *_this);

/**
 * Block the calling task until the push button is pressed or released.
 *
 * @param _this [IN] specifies a pointer to a driver object.
 * @param pbButtonPressed [OUT] specify the status of the push button.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t NucleoDriverWaitForButtonEvent(NucleoDriver *_this, boolean_t *pbButtonPressed);

sys_error_code_t NucleoDriverStopWaitingForButtonEvent(NucleoDriver *_this);

// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t NucleoDriverSetLed(NucleoDriver *_this, boolean_t bON) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UNUSED(_this);

  HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, (GPIO_PinState)bON);

  return xRes;
}

SYS_DEFINE_INLINE
sys_error_code_t NucleoDriverToggleLed(NucleoDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UNUSED(_this);

  HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);

  return xRes;
}

SYS_DEFINE_INLINE
boolean_t NucleoDriverIsButtonPressed(NucleoDriver *_this) {
  assert_param(_this);

  return _this->m_bPB1Pressed;
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_NUCLEODRIVER_H_ */
