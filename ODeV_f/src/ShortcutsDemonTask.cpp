/**
 ******************************************************************************
 * @file    ShortcutsDemonTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Jan 28, 2019
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

#include "ShortcutsDemonTask.h"
#include "ShortcutsDemonTask_vtbl.h"
#include "sysdebug.h"
#include "hid_report_parser.h"


#ifndef SCD_TASK_CFG_STACK_DEPTH
#define SCD_TASK_CFG_STACK_DEPTH              240
#endif

#ifndef SCD_TASK_CFG_PRIORITY
#define SCD_TASK_CFG_PRIORITY                 (tskIDLE_PRIORITY)
#endif

#ifndef SCD_TASK_CFG_IN_QUEUE_LENGTH
#define SCD_TASK_CFG_IN_QUEUE_LENGTH          10
#endif

#define SCD_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(odev::AShortcut*)
#define SCD_TASK_CFG_KEY_DELAY_MS             10

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_SCD, level, message)


/**
 * ShortcutsDemonTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xShortcutsDemonTask_vtbl = {
    ShortcutsDemonTask_vtblHardwareInit,
    ShortcutsDemonTask_vtblOnCreateTask,
    ShortcutsDemonTask_vtblDoEnterPowerMode,
    ShortcutsDemonTask_vtblHandleError,
    ShortcutsDemonTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static ShortcutsDemonTask s_xTaskObj = {0};


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t ShortcutsDemonTaskExecuteStepRun(ShortcutsDemonTask *_this);

/**
 * Task control function.
 *
 * @param pParams [IN] specifies a pointer to the task object.
 */
static void ShortcutsDemonTaskRun(void *pParams);

/**
 * If the key code is a modifier, then it set the value of the modifier in the report and return `TRUE`.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param nKeyCode [IN] specifies a key code.
 * @param pxReport [IN] specifies the ::HIDReport to modify.
 * @param bSet [IN] specifies the value of the modifier key.
 * @return `TRUE` if the key code is modifier key, `FALSE` otherwise.
 */
static boolean_t ShortcutsDemonTaskCheckModifierKey(ShortcutsDemonTask *_this, uint8_t nKeyCode, HIDReport *pxReport, boolean_t bVal);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
// Inline function defined inline in the header file ShortcutsDemonTask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *ShortcutsDemonTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xShortcutsDemonTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t ShortcutsDemonTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  ShortcutsDemonTask *pObj = (ShortcutsDemonTask*)_this;

  return xRes;
}

volatile size_t nTemp = 0;
sys_error_code_t ShortcutsDemonTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  ShortcutsDemonTask *pObj = (ShortcutsDemonTask*)_this;

  pObj->m_xOutputQueue = NULL;
  pObj->m_xShortcutsInQueue = xQueueCreate(SCD_TASK_CFG_IN_QUEUE_LENGTH, SCD_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xShortcutsInQueue == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
  }
  else {

#ifdef DEBUG
    vQueueAddToRegistry(pObj->m_xShortcutsInQueue, "SCD_Q");
#endif

    *pvTaskCode = ShortcutsDemonTaskRun;
    *pcName = "SCD";
    *pnStackDepth = SCD_TASK_CFG_STACK_DEPTH;
    *pParams = _this;
    *pxPriority = SCD_TASK_CFG_PRIORITY;
  }

  return xRes;
}

sys_error_code_t ShortcutsDemonTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  ShortcutsDemonTask *pObj = (ShortcutsDemonTask*)this;

  return xRes;
}

sys_error_code_t ShortcutsDemonTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  ShortcutsDemonTask *pObj = (ShortcutsDemonTask*)this;

  return xRes;
}

sys_error_code_t ShortcutsDemonTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  ShortcutsDemonTask *pObj = (ShortcutsDemonTask*)_this;

  return xRes;
}

sys_error_code_t ShortcutsDemonTaskPostShortcuts(ShortcutsDemonTask *_this, odev::AShortcut *pxShortcut) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // parameter validation
  if ((pxShortcut == NULL) || (_this->m_xShortcutsInQueue == NULL)) {
    xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }
  else {
    if (pdTRUE != xQueueSendToBack(_this->m_xShortcutsInQueue, &pxShortcut, pdMS_TO_TICKS(100))) {
      xRes = SYS_TASK_QUEUE_FULL_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    }
  }

  return xRes;
}

sys_error_code_t ShortcutsDemontaskSetOutputQueue(ShortcutsDemonTask *_this, QueueHandle_t xQueue) {
  assert_param(_this);

  _this->m_xOutputQueue = xQueue;

  return SYS_NO_ERROR_CODE;
}

// Private function definition
// ***************************

static sys_error_code_t ShortcutsDemonTaskExecuteStepRun(ShortcutsDemonTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  odev::AShortcut *pxShortcut = NULL;
  HIDReport xReport = {0};
  xReport.reportID = HID_REPORT_ID_KEYBOARD;

  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (pdTRUE == xQueueReceive(_this->m_xShortcutsInQueue, &pxShortcut, portMAX_DELAY)) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);

    //send the key report one by one every SCD_TASK_CFG_KEY_DELAY_MS ms.

    // send the key pressed event.
    if (_this->m_xOutputQueue != NULL) {
      uint8_t nKeyCount = pxShortcut->GetKeyCount();
      for (int i=0; i<nKeyCount; i++) {
        if (!ShortcutsDemonTaskCheckModifierKey(_this, (*pxShortcut)[i], &xReport, TRUE)) {
          xReport.inputReport10.nKeyCodeArray[i] = (*pxShortcut)[i];
        }
        xQueueSendToBack(_this->m_xOutputQueue, &xReport, pdMS_TO_TICKS(100));
        vTaskDelay(SCD_TASK_CFG_KEY_DELAY_MS);
      }

      // send teh key release event
      for (int i=nKeyCount; i>0; i--) {
        if (!ShortcutsDemonTaskCheckModifierKey(_this, (*pxShortcut)[i-1], &xReport, FALSE)) {
          xReport.inputReport10.nKeyCodeArray[i-1] = 0;
        }
        xQueueSendToBack(_this->m_xOutputQueue, &xReport, pdMS_TO_TICKS(100));
        vTaskDelay(SCD_TASK_CFG_KEY_DELAY_MS);
      }
    }
  }
  else {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
  }

  return xRes;
}

static void ShortcutsDemonTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  ShortcutsDemonTask *_this = (ShortcutsDemonTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SCD: start.\r\n"));

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
        xRes = ShortcutsDemonTaskExecuteStepRun(_this);
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

static boolean_t ShortcutsDemonTaskCheckModifierKey(ShortcutsDemonTask *_this, uint8_t nKeyCode, HIDReport *pxReport, boolean_t bVal) {
  assert_param(_this);
  assert_param(pxReport);
  boolean_t bRes = TRUE;

  switch (nKeyCode) {
  case KC_LEFT_GUI:
    pxReport->inputReport10.nLeftGUI = bVal;
  break;
  case KC_LEFT_SHIFT:
    pxReport->inputReport10.nLeftShift = bVal;
  break;
  case KC_LEFT_CTRL:
    pxReport->inputReport10.nLeftCtrl = bVal;
  break;
  case KC_LEFT_ALT:
    pxReport->inputReport10.nLeftAlt = bVal;
  break;
  case KC_RIGHT_GUI:
    pxReport->inputReport10.nRightGUI = bVal;
  break;
  case KC_RIGHT_SHIFT:
    pxReport->inputReport10.nRightShift = bVal;
   break;
  case KC_RIGHT_ALT:
    pxReport->inputReport10.nRightAlt = bVal;
   break;
  case KC_RIGHT_CTRL:
    pxReport->inputReport10.nRightCtrl = bVal;
    break;
  default:
    // the key code is not a modifier.
    bRes = FALSE;
    break;
  }

  return bRes;
}
