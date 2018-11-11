/**
 ******************************************************************************
 * @file    AManagedTaskEx.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 2.0.0
 * @date    Jul 30, 2018
 *
 * @brief   This file declare the Managed task Interface.
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
#ifndef INCLUDE_SERVICES_AMANAGEDTASKEX_H_
#define INCLUDE_SERVICES_AMANAGEDTASKEX_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "AManagedTask.h"


/**
 * Create  type name for _AManagedTaskEx.
 */
typedef struct _AManagedTaskEx AManagedTaskEx;


// Public API declaration
//***********************

/**
 * Initialize a managed task structure. The application is responsible to allocate
 * a managed task in memory. This method must be called after the allocation.
 *
 * @param this [IN] specifies a task object pointer.
 * @return \a SYS_NO_ERROR_CODE
 */
inline sys_error_code_t AMTInitEx(AManagedTaskEx *this);

/**
 * Called by the framework to force the step execution.
 * During a step execution a task can be suspended waiting for an event or for other reason.
 * The In this situation the INIT task cannot complete the power mode switch, so it calls this method
 * in order to force the task to be ready for the power mode switch.
 *
 * @param this [IN] specifies a pointer to the object.
 * @param eActivePowerMode [IN] specifies the actual power mode
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t AMTExForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode);

/**
 * A task must use this method to notify the framework when it is entering the suspend state or
 * when is blocking on a event without a timeout. This state is called "inactive".
 * This information is used to the AED to resetting the system because it thinks that the task
 * is not responding.
 *
 * @param this [IN] specifies a pointer to the object.
 * @param bBlockedSuspended [IN] `TRUE` if a the task is entering the inactive state,
 *                                `FALSE` if a task is leaving the inactive state.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t AMTExSetInactiveState(AManagedTaskEx *this, boolean_t bBlockedSuspended);

/**
 * Check if the a managed task is inactive. A managed task is inactive when:
 * - it is in suspend state
 * - it is in blocked state without a timeout
 *
 * For more information about the task state see https://freertos.org/RTOS-task-states.html
 *
 * @param this [IN] specifies a pointer to the object.
 * @return `TRUE` if the task is in inactive state, `FALSE` otherwise.
 */
inline boolean_t AMTExIsTaskInactive(AManagedTaskEx *this);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_AMANAGEDTASKEX_H_ */
