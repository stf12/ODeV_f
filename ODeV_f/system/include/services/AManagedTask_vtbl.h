/**
 ******************************************************************************
 * @file    AManagedTaskVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 13, 2017
 * @brief
 *
 * TODO - insert here the file description
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

#ifndef INCLUDE_SERVICES_AMANAGEDTASK_VTBL_H_
#define INCLUDE_SERVICES_AMANAGEDTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"
#include "tx_api.h"

/**
 * Create  type name for _IManagedTask_vtb.
 */
typedef struct _AManagedTask_vtbl AManagedTask_vtbl;

struct _AManagedTask_vtbl {
  sys_error_code_t (*HardwareInit)(AManagedTask *_this, void *pParams);
  sys_error_code_t (*OnCreateTask)(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName, VOID **pvStackStart, ULONG *pnStackSize, UINT *pnPriority, UINT *pnPreemptThreshold, ULONG *pnTimeSlice, ULONG *pnAutoStart, ULONG *pnParams);
  sys_error_code_t (*DoEnterPowerMode)(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
  sys_error_code_t (*HandleError)(AManagedTask *_this, SysEvent xError);
};

/**
 * Managed Task status field. This data is used to coordinate the power mode switch between the INIT task
 * and the application managed tasks.
 */
typedef struct _AMTStatus {
  /**
   * Set by task to delay a power mode switch. This allow a task to complete a step in its control loop
   * and put the task in a safe state before the power mode transaction.
   */
  uint8_t nDelayPowerModeSwitch: 1;

  /**
   * Set by INIT to signal a task about a pending power mode switch.
   */
  uint8_t nPowerModeSwitchPending: 1;

  /**
   * SET by INIT to mark a task ready for the power mode switch.
   * RESET by INIT at the end of teh power mode switch sequence.
   */
  uint8_t nPowerModeSwitchDone: 1;

  /**
   * Set by a managed task to notify the system that it is working fine. It is reset by the application error delegate.
   */
  uint8_t nIsTaskStillRunning: 1;

  /**
   * Count the error occurred during the task execution.
   */
  uint8_t nErrorCount: 2;

  /**
   * Specify if the task has been created suspended. It depends on the pnAutoStart parameter passed
   * during the task creation.
   */
  uint8_t nAutoStart: 1;

  uint8_t nReserved : 1;
} AMTStatus;

/**
 * A Managed Task a task integrated in the system. It defines a common interface for all application tasks.
 * All Managed Tasks belong to a linked list that is the ::_ApplicationContext.
 */
struct _AManagedTask {
  /**
   * Specifies  a pointer to the class virtual table.
   */
  const AManagedTask_vtbl *vptr;

  /**
   * Specify the native ThreadX task handle.
   */
//  TaskHandle_t m_xThaskHandle_;
  TX_THREAD m_xThaskHandle;

  /**
   *Specifies a pointer to the next managed task in the _ApplicationContext.
   */
  struct _AManagedTask *m_pNext;

  /**
   * Status flags.
   */
  AMTStatus m_xStatus;
};

extern EPowerMode SysGetPowerMode();


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t AMTHardwareInit(AManagedTask *_this, void *pParams) {
  return _this->vptr->HardwareInit(_this, pParams);
}

SYS_DEFINE_INLINE
inline sys_error_code_t AMTOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
    VOID **pvStackStart, ULONG *pnStackSize,
    UINT *pnPriority, UINT *pnPreemptThreshold,
    ULONG *pnTimeSlice, ULONG *pnAutoStart,
    ULONG *pnParams) {
  return _this->vptr->OnCreateTask(_this, pvTaskCode, pcName, pvStackStart, pnStackSize, pnPriority, pnPreemptThreshold, pnTimeSlice, pnAutoStart, pnParams);
}

SYS_DEFINE_INLINE
sys_error_code_t AMTDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  return _this->vptr->DoEnterPowerMode(_this, eActivePowerMode, eNewPowerMode);
}

SYS_DEFINE_INLINE
sys_error_code_t AMTHandleError(AManagedTask *_this, SysEvent xError) {
  return _this->vptr->HandleError(_this, xError);
}

SYS_DEFINE_INLINE
sys_error_code_t AMTInit(AManagedTask *_this) {
  _this->m_pNext = NULL;
//  _this->m_xThaskHandle = NULL;
  _this->m_xStatus.nDelayPowerModeSwitch = 1;
  _this->m_xStatus.nPowerModeSwitchPending = 0;
  _this->m_xStatus.nPowerModeSwitchDone = 0;
  _this->m_xStatus.nIsTaskStillRunning = 0;
  _this->m_xStatus.nErrorCount = 0;
  _this->m_xStatus.nAutoStart = 0;
  _this->m_xStatus.nReserved = 0;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
EPowerMode AMTGetSystemPowerMode() {
  return SysGetPowerMode();
}

SYS_DEFINE_INLINE
sys_error_code_t AMTNotifyIsStillRunning(AManagedTask *_this, sys_error_code_t nStepError) {

  if (SYS_IS_ERROR_CODE(nStepError) && (_this->m_xStatus.nErrorCount < MT_MAX_ERROR_COUNT - 1)) {
    _this->m_xStatus.nErrorCount++;
  }
  if (_this->m_xStatus.nErrorCount < MT_ALLOWED_ERROR_COUNT) {
    _this->m_xStatus.nIsTaskStillRunning = 1;
  }

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
void AMTResetAEDCounter(AManagedTask *_this) {
  SysResetAEDCounter();
}

SYS_DEFINE_INLINE
boolean_t AMTIsPowerModeSwitchPending(AManagedTask *_this) {
  return (boolean_t)_this->m_xStatus.nPowerModeSwitchPending;
}

SYS_DEFINE_INLINE
void AMTReportErrOnStepExecution(AManagedTask *_this, sys_error_code_t nStepError) {
  UNUSED(nStepError);

  if (_this->m_xStatus.nErrorCount < MT_MAX_ERROR_COUNT - 1) {
    _this->m_xStatus.nErrorCount++;
  }
}


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICES_AMANAGEDTASK_VTBL_H_ */
