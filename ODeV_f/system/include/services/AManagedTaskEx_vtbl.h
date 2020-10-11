/**
 ******************************************************************************
 * @file    AManagedTaskEx_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 2.0.0
 * @date    Jul 30, 2018
 *
 * @brief
 *
 * TODO - insert here the file description
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
#ifndef INCLUDE_SERVICES_AMANAGEDTASKEX_VTBL_H_
#define INCLUDE_SERVICES_AMANAGEDTASKEX_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"
#include "FreeRTOS.h"
//#include "task.h" //TODO: STF.Port - threadx

#include "AManagedTask_vtbl.h"

/**
 * Create  type name for _IManagedTask_vtb.
 */
typedef struct _AManagedTaskEx_vtbl AManagedTaskEx_vtbl;

struct _AManagedTaskEx_vtbl {
  sys_error_code_t (*HardwareInit)(AManagedTask *_this, void *pParams);
  sys_error_code_t (*OnCreateTask)(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority);
  sys_error_code_t (*DoEnterPowerMode)(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
  sys_error_code_t (*HandleError)(AManagedTask *_this, SysEvent xError);
  sys_error_code_t (*ForceExecuteStep)(AManagedTaskEx *_this, EPowerMode eActivePowerMode);
};

/**
 * Managed Task extended status field. This data is used to coordinate the power mode switch between the INIT task
 * and the application managed tasks.
 */
typedef struct _AMTStatusEx {
  uint8_t nIsWaitingNoTimeout : 1;

  uint8_t nUnused: 6;
  uint8_t nReserved : 1;
} AMTStatusEx;

/**
 * A Managed Task a task integrated in the system. It defines a common interface for all application tasks.
 * All Managed Tasks belong to a linked list that is the ::_ApplicationContext.
 */
struct _AManagedTaskEx {
  /**
   * Specifies  a pointer to the class virtual table.
   */
  const AManagedTaskEx_vtbl *vptr;

  /**
   * Specify the native FreeRTOS task handle.
   */
  TaskHandle_t m_xThaskHandle;

  /**
   *Specifies a pointer to the next managed task in the _ApplicationContext.
   */
  struct _AManagedTaskEx *m_pNext;

  /**
   * Status flags.
   */
  AMTStatus m_xStatus;

  /**
   * Extended status flags.
   */
  AMTStatusEx m_xStatusEx;
};

extern EPowerMode SysGetPowerMode();

// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t AMTExForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  return _this->vptr->ForceExecuteStep(_this, eActivePowerMode);
}

SYS_DEFINE_INLINE
sys_error_code_t AMTInitEx(AManagedTaskEx *_this) {
  AManagedTaskEx *pObj = (AManagedTaskEx*)_this;

  _this->m_pNext = NULL;
  _this->m_xThaskHandle = NULL;
  _this->m_xStatus.nDelayPowerModeSwitch = 1;
  _this->m_xStatus.nPowerModeSwitchPending = 0;
  _this->m_xStatus.nPowerModeSwitchDone = 0;
  _this->m_xStatus.nIsTaskStillRunning = 0;
  _this->m_xStatus.nErrorCount = 0;
  _this->m_xStatus.nReserved = 1; // this identifies the task as an AManagedTaskEx.
  pObj->m_xStatusEx.nIsWaitingNoTimeout = 0;
  pObj->m_xStatusEx.nUnused = 0;
  pObj->m_xStatusEx.nReserved = 0;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
sys_error_code_t AMTExSetInactiveState(AManagedTaskEx *_this, boolean_t bBlockedSuspended) {
  assert_param(_this);

  _this->m_xStatusEx.nIsWaitingNoTimeout = bBlockedSuspended;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
boolean_t AMTExIsTaskInactive(AManagedTaskEx *_this){
  assert_param(_this);

  return (boolean_t)_this->m_xStatusEx.nIsWaitingNoTimeout;
}


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICES_AMANAGEDTASKEX_VTBL_H_ */
