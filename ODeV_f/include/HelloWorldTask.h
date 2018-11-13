/**
 ******************************************************************************
 * @file    HelloWorldTask.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 2, 2018
 *
 * @brief Blink the user LED of the NUCLEO board.
 *
 * This managed task is responsible to blink the user LED with a fixed period
 * of 1 second and a duty cycle of 50%.
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
#ifndef HELLOWORLDTASK_H_
#define HELLOWORLDTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTask.h"
#include "AManagedTaskVtbl.h"
#include "IDriver.h"
#include "IDriverVtbl.h"

/**
 * Create  type name for _HelloWorldTask.
 */
typedef struct _HelloWorldTask HelloWorldTask;

/**
 *  HelloWorldTask internal structure.
 */
struct _HelloWorldTask {
  /**
   * Base class object.
   */
  AManagedTask super;

  // Task variables should be added here.

  /**
   * The driver used by the task. The actual type is ::NucleoDriver.
   */
  IDriver *m_pxDriver;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of HelloWorldTask.
 *
 * @return a pointer to the generic obejct ::AManagedTask if success,
 * or NULL if out of memory error occurs.
 */
AManagedTask *HelloWorldTaskAlloc();

/**
 * Get a pointer to the driver used by the task.
 *
 * @param this [IN] specifies a pointer to a task object.
 * @return a pointer to the driver used by the task.
 */
inline IDriver *HelloWorldTaskGetDriver(HelloWorldTask *this);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
IDriver *HelloWorldTaskGetDriver(HelloWorldTask *this) {
  assert_param(this);

  return this->m_pxDriver;
}

#ifdef __cplusplus
}
#endif

#endif /* HELLOWORLDTASK_H_ */
