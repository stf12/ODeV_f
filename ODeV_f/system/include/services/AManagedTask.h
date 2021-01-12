/**
 ******************************************************************************
 * @file    AManagedTask.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 13, 2017
 * @brief   This file declare the Managed task Interface.
 *
 * TODO - STF - what is a managed task?
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

#ifndef INCLUDE_SERVICES_AMANAGEDTASK_H_
#define INCLUDE_SERVICES_AMANAGEDTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "syslowpower.h"
#include "FreeRTOS.h"

/**
 * Specifies the maximum number of errors that can be tracked by a managed task.
 */
#define MT_MAX_ERROR_COUNT       0x7

#ifndef MT_ALLOWED_ERROR_COUNT
/**
 * Specifies the maximum number of error a task can report before resetting the nIsTaskStillRunning flag.
 * If the AEM (::AppErrorManager) is used it will in turn trigger a system reset due to the WWDG.
 */
#define MT_ALLOWED_ERROR_COUNT   0x2
#endif


/**
 * Create  type name for _AManagedTask.
 */
typedef struct _AManagedTask AManagedTask;


// Public API declaration
//***********************

/**
 * Task specific function called by the framework to initialize
 * task related hardware resources. This function is called by the INIT task.
 *
 * @param this [IN] specifies a task object pointer.
 * @param pParams [IN] specifies a pointer to task specific hardware initialization parameters.
 * @return \a SYS_NO_ERROR_CODE if success, one of the following error codes otherwise:
 *         - SYS_OUT_OF_MEMORY_ERROR_CODE if is not possible to instantiate the driver object.
 *         - Other task specific error code
 */
inline sys_error_code_t AMTHardwareInit(AManagedTask *_this, void *pParams);

/**
 * Task specific function called by the framework before the task is created.
 * An application should use this function in order to perform task specific software initialization
 * and pass task specific parameters to the INIT task.
 *
 * @param _this [IN] specifies a task object pointer.
 * @param pvTaskCode [OUT] used by the application to specify the task main function.
 * @param pcName [OUT] used by the application to specify a descriptive name for the task.
 * @param pnStackDepth [OUT] used by the application to specify the task stack size.
 * @param pxPriority [OUT] used by the application to specify the task priority.
 * @param pParams [OUT] specifies a pointer to the parameters passed from the application to the task.
 * @return \a SYS_NO_ERROR_CODE if success, a task specific error code otherwise. If the function
 * fails the task creation process is stopped.
 */
inline sys_error_code_t AMTOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority);

/**
 * Task specific function called by the framework when the system is entering a specific power mode, in order to
 * implement the transaction in the power mode state machine.
 * This function is executed in the INIT task execution flow.
 * A managed task should modify its internal state to be ready to execute steps in the new power mode.
 * The implementation of the managed task control loop, provided with the framework template, suspend the task
 * after this function is called. The INIT task is in charge to resume the task when all the tasks are ready
 * and the new power mode become actual.
 *
 * @param _this [IN] specifies a task object pointer.
 * @param eActivePowerMode [IN] specifies the current power mode of the system.
 * @param eNewPowerMode [IN] specifies the new power mode that is to be activated by the system.
 * @return \a SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
 */
inline sys_error_code_t AMTDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * Called by the framework to handle a system wide error. This function is executed in the INIT task execution flow.
 *
 * @param _this [IN] specifies a task object pointer.
 * @param xError [IN] specifies a system error
 * @return \a SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
 */
inline sys_error_code_t AMTHandleError(AManagedTask *_this, SysEvent xError);

/**
 * Initialize a managed task structure. The application is responsible to allocate
 * a managed task in memory. This method must be called after the allocation.
 *
 * @param _this [IN] specifies a task object pointer.
 * @return \a SYS_NO_ERROR_CODE
 */
inline sys_error_code_t AMTInit(AManagedTask *_this);

/**
 * Utility function to retrieve the current power mode of the system.
 *
 * @return the current power mode of the system
 */
inline EPowerMode AMTGetSystemPowerMode();

/**
 * Notify the system that the task is still running. If an application error manage delegate is installed (_IApplicationErrorDelegate),
 * then a managed task must notify the system that it is working fine in order to prevent a system reset.
 *
 * @param _this [IN] specifies a task object pointer.
 * @param nStepError [IN] specifies an error code. Usually it is the error code reported during the task step execution.
 * @return \a SYS_NO_ERROR_CODE if success, a task specific error code otherwise.
 */
inline sys_error_code_t AMTNotifyIsStillRunning(AManagedTask *_this, sys_error_code_t nStepError);

/**
 * A managed task can handle an error during the step execution by itself. Another option is to let
 * the error navigate up to the main control loop of the task where it will be reported to the system
 * using the AMTNotifyIsStillRunning() function.
 * But if an error occurs and the managed task want to ignore the error and proceed with the step execution,
 * it should notify the system using this function before the error is overwritten. For example:
 *
 *     xRes = HCPExeuteCommand(&_this->m_xProtocol, xReport.outputReport11.nCommandID, NULL, _this->m_pxDriver);
 *     if (SYS_IS_ERROR_CODE(xRes)) {
 *       AMTReportErrOnStepExecution(_this, xRes);
 *     }
 *     // continue with the step execution.
 *
 * In this why the error is logged and the AED count the error when it check if the task is still running properly.
 *
 * @param _this [IN] specifies a task object pointer.
 * @param nStepError [IN] specifies an error code.
 */
inline void AMTReportErrOnStepExecution(AManagedTask *_this, sys_error_code_t nStepError);

/**
 * This function is a convenient method to call the call the system function SysResetAEDCounter() from a task code.
 *
 * @param _this [IN] specifies a task object pointer.
 */
inline void AMTResetAEDCounter(AManagedTask *_this);

/**
 * Check if the INIT task has requested a power mode switch.
 *
 * @param _this [IN] specifies a task object pointer.
 * @return `TRUE` if there is power mode switch pending request, `FALSE` otherwise.
 */
inline boolean_t AMTIsPowerModeSwitchPending(AManagedTask *_this);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICES_AMANAGEDTASK_H_ */
