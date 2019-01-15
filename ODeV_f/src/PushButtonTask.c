/**
 ******************************************************************************
 * @file    PushButtonTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 5, 2018
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

#include "PushButtonTask.h"
#include "PushButtonTask_vtbl.h"
#include "sysdebug.h"
#include "NucleoDriver.h"
#include "NucleoDriver_vtbl.h"

#ifndef PB_TASK_CFG_STACK_DEPTH
#define PB_TASK_CFG_STACK_DEPTH      120
#endif

#ifndef PB_TASK_CFG_PRIORITY
#define PB_TASK_CFG_PRIORITY         (tskIDLE_PRIORITY+2)
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_PB, level, message)


/**
 * PushButtonTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xPushButtonTask_vtbl = {
    PushButtonTask_vtblHardwareInit,
    PushButtonTask_vtblOnCreateTask,
    PushButtonTask_vtblDoEnterPowerMode,
    PushButtonTask_vtblHandleError,
    PushButtonTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static PushButtonTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void PushButtonTaskRun(void *pParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
sys_error_code_t PushButtonTaskSetDriver(PushButtonTask *_this, IDriver *pxDriver);
#endif


// Public API definition
// *********************

AManagedTaskEx *PushButtonTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xPushButtonTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t PushButtonTask_vtblHardwareInit(AManagedTask *this, void *pParams) {
  assert_param(this);
  UNUSED(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)this;

  // the driver is initialized by the task owner. This managed task use a driver
  // of type NucleoDriver in order to check the status of the push button and
  // put the system in low power mode.

  return xRes;
}

sys_error_code_t PushButtonTask_vtblOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *pObj = (PushButtonTask*)this;

  pObj->m_bButtonArmed = FALSE;

  *pvTaskCode = PushButtonTaskRun;
  *pcName = "PB";
  *pnStackDepth = PB_TASK_CFG_STACK_DEPTH;
  *pParams = this;
  *pxPriority = PB_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)this;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblHandleError(AManagedTask *this, SysEvent xError) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)this;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)this;

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *this) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  boolean_t bIsButtonPressed = FALSE;

  if (this->m_pxDriver) {
    AMTExSetInactiveState((AManagedTaskEx*)this, TRUE);
    NucleoDriverWaitForButtonEvent((NucleoDriver*)this->m_pxDriver, &bIsButtonPressed);
    AMTExSetInactiveState((AManagedTaskEx*)this, FALSE);

    if (!bIsButtonPressed) {
      if (this->m_bButtonArmed) {
        this->m_bButtonArmed = FALSE;
        // enter low power mode
        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: enter LP mode.\r\n"));
        SysEvent xEvent;
        xEvent.nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_SW, SYS_PM_EVT_PARAM_ENTER_LP);
        SysPostPowerModeEvent(xEvent);
      }
      else {
        this->m_bButtonArmed = TRUE;

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: button armed.\r\n"));
      }
    }
  }

  return xRes;
}

static void PushButtonTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *this = (PushButtonTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: start.\r\n"));

  IDrvStart(this->m_pxDriver);

  for (;;) {

    // check if there is a pending power mode switch request
    if (this->super.m_xStatus.nPowerModeSwitchPending == 1) {
      // clear the power mode switch delay because the task is ready to switch.
      taskENTER_CRITICAL();
        this->super.m_xStatus.nDelayPowerModeSwitch = 0;
      taskEXIT_CRITICAL();
      vTaskSuspend(NULL);
    }
    else {
      switch (AMTGetSystemPowerMode()) {
      case E_POWER_MODE_RUN:
        taskENTER_CRITICAL();
          this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = PushButtonTaskExecuteStepRun(this);
        taskENTER_CRITICAL();
          this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_SLEEP_1:
        AMTExSetInactiveState((AManagedTaskEx*)this, TRUE);
        vTaskSuspend(this->super.m_xThaskHandle);
        AMTExSetInactiveState((AManagedTaskEx*)this, FALSE);
        break;
      }
    }

    // notify the system that the task is working fine.
    AMTNotifyIsStillRunning((AManagedTask*)this, xRes);

#if (SYS_TRACE > 1)
    if (xRes != SYS_NO_ERROR_CODE) {
      sys_check_error_code(xRes);
      sys_error_handler();
    }
#endif
  }
}
