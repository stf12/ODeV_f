/**
 ******************************************************************************
 * @file    PushButtonTask.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 5, 2018
 *
 * @brief Handle the Push Button of the NUCLEO board.
 *
 * This managed task is responsible to handle the user button and to generate
 * the ::SysEvent in order to trigger the power mode transactions.
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
#ifndef PUSHBUTTONTASK_H_
#define PUSHBUTTONTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "IDriver.h"
#include "IDriver_vtbl.h"


/**
 * Create  type name for _PushButtonTask.
 */
typedef struct _PushButtonTask PushButtonTask;

/**
 *  PushButtonTask internal structure.
 */
struct _PushButtonTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * The driver used by the task. The actual type is ::NucleoDriver.
   */
  IDriver *m_pxDriver;

  /**
   * I want to put the system in low power mode on a button release event,
   * so this variable track when the user press the button, but it is not released yet.
   */
  boolean_t m_bButtonArmed;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of PushButtonTask.
 *
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *PushButtonTaskAlloc();

/**
 * Set the driver used by task to use the push button.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @param pDxriver [IN] specifies a pointer to a driver of type ::NuceloDriver.
 * @return SYS_NO_ERROR_CODE.
 */
inline sys_error_code_t PushButtonTaskSetDriver(PushButtonTask *_this, IDriver *pDxriver);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t PushButtonTaskSetDriver(PushButtonTask *_this, IDriver *pxDriver) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_pxDriver = pxDriver;

  return xRes;
}


#ifdef __cplusplus
}
#endif

#endif /* PUSHBUTTONTASK_H_ */
