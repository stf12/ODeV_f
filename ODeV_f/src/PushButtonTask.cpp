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
#include "gui/common/FrontendApplication.hpp"

#include "hid_report_parser.h"
#include "string.h"

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
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void PushButtonTaskRun(void *pParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
extern sys_error_code_t PushButtonTaskSetOutputQueue(PushButtonTask *_this, QueueHandle_t xQueue);
extern sys_error_code_t PushButtonTaskSetDriver(PushButtonTask *_this, IDriver *pxDriver);
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

sys_error_code_t PushButtonTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *pObj = (PushButtonTask*)_this;

  pObj->m_bButtonArmed = FALSE;

  *pvTaskCode = PushButtonTaskRun;
  *pcName = "PB";
  *pnStackDepth = PB_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = PB_TASK_CFG_PRIORITY;

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
        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: increment counter.\r\n"));
//        SysEvent xEvent;
//        xEvent.nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_SW, SYS_PM_EVT_PARAM_ENTER_LP);
//        SysPostPowerModeEvent(xEvent);

        // Example of interaction with the GUI using the the TouchGFX Application and Model.
        uint8_t nIncrement = 1;
        FrontendApplication* guiApp = static_cast<FrontendApplication*>(FrontendApplication::getInstance());
        guiApp->getModel().incrementCounter(nIncrement);

        if (_this->m_xOutputQueue) {
//          static HIDReport xReport02;
//          memset(&xReport02, 0, sizeof(HIDReport));
//          xReport02.reportID = HID_REPORT_ID_MOUSE;
//          xReport02.inputReport02.nDX = 0x25;
//
//          xQueueSendToBack(_this->m_xOutputQueue, &xReport02, pdMS_TO_TICKS(50));

//          static HIDReport xReport10;
//          memset(&xReport10, 0, sizeof(HIDReport));
//          xReport10.reportID = HID_REPORT_ID_KEYBOARD;
//          xReport10.inputReport10.nKeyCodeArray[0] = 0x16; // s for STF :)
//          xQueueSendToBack(_this->m_xOutputQueue, &xReport10, pdMS_TO_TICKS(50));
//
//          vTaskDelay(pdMS_TO_TICKS(500));
//          xReport10.inputReport10.nKeyCodeArray[0] = 0; // s for STF :)
//          xQueueSendToBack(_this->m_xOutputQueue, &xReport10, pdMS_TO_TICKS(50));
        }
      }
      else {
        _this->m_bButtonArmed = TRUE;

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: button armed.\r\n"));
      }
    }
  }

  return xRes;
}

static void PushButtonTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *_this = (PushButtonTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: start.\r\n"));

  IDrvStart(_this->m_pxDriver);

  for (;;) {

    // check if there is a pending power mode switch request
    if (_this->super.m_xStatus.nPowerModeSwitchPending == 1) {
      // clear the power mode switch delay because the task is ready to switch.
      taskENTER_CRITICAL();
        _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
      taskEXIT_CRITICAL();
      vTaskSuspend(NULL);
    }
    else {
      switch (AMTGetSystemPowerMode()) {
      case E_POWER_MODE_RUN:
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = PushButtonTaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_SLEEP_1:
        AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
        vTaskSuspend(_this->super.m_xThaskHandle);
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
