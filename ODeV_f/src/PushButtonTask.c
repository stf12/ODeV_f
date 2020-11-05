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
#define PB_TASK_CFG_STACK_DEPTH      TX_MINIMUM_STACK
#endif

#ifndef PB_TASK_CFG_PRIORITY
#define PB_TASK_CFG_PRIORITY         (TX_MAX_PRIORITIES-3)
#endif

#define SYS_DEBUGF(level, message)   SYS_DEBUGF3(SYS_DBG_PB, level, message)


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
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *_this);

/**
 * Task control function.
 *
 * @param nParams .
 */
static void PushButtonTaskRun(ULONG nParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
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

sys_error_code_t PushButtonTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  UNUSED(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  // the driver is initialized by the task owner. This managed task use a driver
  // of type NucleoDriver in order to check the status of the push button and
  // put the system in low power mode.

  return xRes;
}

sys_error_code_t PushButtonTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
    VOID **pvStackStart, ULONG *pnStackSize,
    UINT *pnPriority, UINT *pnPreemptThreshold,
    ULONG *pnTimeSlice, ULONG *pnAutoStart,
    ULONG *pnParams)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *pObj = (PushButtonTask*)_this;

  pObj->m_bButtonArmed = FALSE;

  *pvTaskCode = PushButtonTaskRun;
  *pcName = "PB";
  *pnStackSize = PB_TASK_CFG_STACK_DEPTH;
  *pnParams = (ULONG)_this;
  *pnPriority = PB_TASK_CFG_PRIORITY;
  *pnPreemptThreshold = PB_TASK_CFG_PRIORITY;
  *pnTimeSlice = TX_NO_TIME_SLICE;
  *pnAutoStart = TX_AUTO_START;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  boolean_t bIsButtonPressed = FALSE;

  if (_this->m_pxDriver) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
    NucleoDriverWaitForButtonEvent((NucleoDriver*)_this->m_pxDriver, &bIsButtonPressed);
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);

    if (!bIsButtonPressed) {
      if (_this->m_bButtonArmed) {
        _this->m_bButtonArmed = FALSE;
        // enter low power mode
        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: enter LP mode.\r\n"));
        SysEvent xEvent;
        xEvent.nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_SW, SYS_PM_EVT_PARAM_ENTER_LP);
        SysPostPowerModeEvent(xEvent);
      }
      else {
        _this->m_bButtonArmed = TRUE;

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: button armed.\r\n"));
      }
    }
  }

  return xRes;
}

static void PushButtonTaskRun(ULONG nParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *_this = (PushButtonTask*)nParams;
  // define the vaiable to store the primary mask.
  UINT nOldPosture;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: start.\r\n"));

  IDrvStart(_this->m_pxDriver);

  for (;;) {

    // check if there is a pending power mode switch request
    if (_this->super.m_xStatus.nPowerModeSwitchPending == 1) {
      // clear the power mode switch delay because the task is ready to switch.
      nOldPosture = tx_interrupt_control(TX_INT_DISABLE);
        _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
      tx_interrupt_control(nOldPosture);
      tx_thread_suspend(&_this->super.m_xThaskHandle);
    }
    else {
      switch (AMTGetSystemPowerMode()) {
      case E_POWER_MODE_RUN:
        nOldPosture = tx_interrupt_control(TX_INT_DISABLE);
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        tx_interrupt_control(nOldPosture);
        xRes = PushButtonTaskExecuteStepRun(_this);
        nOldPosture = tx_interrupt_control(TX_INT_DISABLE);
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        tx_interrupt_control(nOldPosture);
        break;

      case E_POWER_MODE_SLEEP_1:
        AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
        tx_thread_suspend(&_this->super.m_xThaskHandle);
        AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
        break;
      }
    }

    // notify the system that the task is working fine.
    AMTNotifyIsStillRunning((AManagedTask*)_this, xRes);

#if (SYS_TRACE > 1)
    if (xRes != SYS_NO_ERROR_CODE) {
      sys_check_error_code(xRes);
      sys_error_handler();
    }
#endif
  }
}
