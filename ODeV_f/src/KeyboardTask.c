/**
 ******************************************************************************
 * @file    KeyboardTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 5, 2016
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

#include "KeyboardTask.h"
#include "KeyboardTaskVtbl.h"
#include "KeyboardKeyCodesMap.h"
#include "IApplicationErrorDelegate.h"
#include "IApplicationErrorDelegateVtbl.h"
#include "sysinit.h"
#include "sysdebug.h"
#include "semphr.h"

#define KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED		6

#define KEYBOARD_TASK_IS_FX_KEY(nKeyCode) (((nKeyCode) >= KC_FN1) && ((nKeyCode) <= KC_FN12))

#define KEYBOARD_TASK_IS_FN_KEY(nKeyCode) ((nKeyCode==KC_FN2) || (nKeyCode==KC_FN3) || (nKeyCode==KC_FN6) || \
                                           (nKeyCode==KC_FN7) || (nKeyCode==KC_FN8) || (nKeyCode==KC_FN9) || \
                                           (nKeyCode==KC_FN12))

#define KEYBOARD_TASK_HAS_SHORTCUT(nKeyCode) ((nKeyCode==KC_FN1) || (nKeyCode==KC_FN4)  || (nKeyCode==KC_FN5)        || \
                                             (nKeyCode==KC_FN10) || (nKeyCode==KC_FN11) || (nKeyCode==KC_RIGHT_CTRL) || \
                                             (nKeyCode==KC_083)  || (nKeyCode==KC_075)  || (nKeyCode==KC_084)        || \
                                             (nKeyCode==KC_079)  || (nKeyCode==KC_089))

#define KEYBOARD_TASK_LEFT_GUI_MASK       ((uint8_t)0x01)
#define KEYBOARD_TASK_LEFT_CTRL_MASK      ((uint8_t)0x02)

// Keyboard Task section
#ifndef KEYBOARD_TASK_CFG_STACK_DEPTH
#define KEYBOARD_TASK_CFG_STACK_DEPTH      120
#endif

#ifndef KEYBOARD_TASK_CFG_PRIORITY
#define KEYBOARD_TASK_CFG_PRIORITY         tskIDLE_PRIORITY
#endif

#define KEYBOARD_TASK_CFG_ERR_FR_PRIORITY  1

#ifndef MT_CFG_START_DELAY_MS
#define MT_CFG_START_DELAY_MS              1000
#endif

#define KEYBOARD_TASK_KC_FORCE_STEP        0xFE

#define SYS_DEBUGF(level, message) 			SYS_DEBUGF3(SYS_DBG_KEYBOARD_TASK, level, message)

static const IErrFirstResponder_vtbl s_xKeyboardErrFR_vtbl = {
    KeyboardErrFRSetOwner,
    KeyboardErrFRGetOwner,
    KeyboardErrFRNewError
};

static const AManagedTaskEx_vtbl s_xKeyboardTask_vtbl = {
    KeyboardHardwareInit,
    KeyboardOnCreateTask,
    KeyboardDoEnterPowerMode,
    KeyboardHandleError,
    Keyboard_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static KeyboardTask s_xTaskObj;


// Private function forward declaration
// ************************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t KeyboardTaskExecuteStepRun(KeyboardTask *this);

/**
 * Task control function.
 *
 * @param pParams not used.
 */
static void KeyboardTaskRun(void *pParams);

/**
 * Remap a key code in a FN shortcut.
 *
 * @param this [IN] specifies a pointer to task object.
 * @param pxEvent [IN] specifies a pointer to a KeyEvent object.
 */
static void KeyboardTaskSetShortcut(KeyboardTask *this, KeyEvent *pxEvent);

/**
 * Check if the key event is related to FN or FN Lock and update the the m_bFnPressed member.
 *
 * @param this [IN] specifies a pointer to task object.
 * @param pxEvent [IN] specifies a pointer to a KeyEvent object.
 *
 * @return the updated value of m_bFnPressed.
 */
static boolean_t KeyboardTaskUpdateFn(KeyboardTask *this, const KeyEvent *pxEvent);

/**
 * Initialize the value of the task's variable.
 *
 * @param this [IN] specifies a pointer to task object.
 */
static void KeyboardTaskResetState(KeyboardTask *this);

/**
 * Process a key event in the following cases:
 * - The key is a shortcut linked to FN and FN is active.
 * - The key is a normal key.
 *
 * @param this  [IN] specifies a pointer to task object.
 * @param pxEvent [IN] specifies a pointer to a key event.
 * @param pbIsShortcut [OUT] \a TRUE is the key event has been processed as a shortcut, \a FALSE otherwise.
 * @return \a TRUE if a new report is ready to be sent, \a FALSE otherwise.
 */
static boolean_t KeyboardDefaultKeyEvent(KeyboardTask *this, KeyEvent *pxEvent, boolean_t *pbIsShortcut);

/**
 * Release the modifiers. It sends a report to the report delivery queue.
 * It is used when a key event is a key release and the key is a shortcut.
 *
 * @param this  [IN] specifies a pointer to task object.
 * @param pxEvent [IN] specifies a pointer to a key event.
 */
static void KeyboardReleaseShortcutModifiers(KeyboardTask *this, KeyEvent *pxEvent);


// Inline function farward declaration
// ***********************************

#if defined (__GNUC__)
extern void KeyboardSetUtilityCmdQueue(AManagedTaskEx *this, QueueHandle_t xQueue);
#endif


// Public API implementation
// *************************

AManagedTaskEx *KeyboardTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xKeyboardTask_vtbl;
  s_xTaskObj.m_xErrFirstResponder.super.vptr = &s_xKeyboardErrFR_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

sys_error_code_t KeyboardHardwareInit(AManagedTask *this, void *pParams) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *pObj = (KeyboardTask*)this;

  pObj->m_pxDriver = KeyMatrixDrvAlloc();
  if (pObj->m_pxDriver == NULL) {
    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("Keyboard task: unable to alloc driver object.\r\n"));
    xRes = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
  }
  else {
    xRes = IDrvInit(pObj->m_pxDriver, NULL);
    if (SYS_IS_ERROR_CODE(xRes)) {
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("Keyboard task: error during driver initialization\r\n"));
    }
  }

  return xRes;
}

sys_error_code_t KeyboardOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *pObj = (KeyboardTask*)this;

  // Initialize the KeyScanner service
  KeyScannerInit(&pObj->m_xKeyScanner, pObj->m_pxDriver);
  // Initialize the object state
  pObj->m_nSimultaneousKeysPressedCount = 0;
  pObj->m_bFnActive = FALSE;
  pObj->m_bFnLock = FALSE;
  pObj->m_bFxIsPressed = FALSE;
  pObj->m_pxDelegate = NULL;
  pObj->m_xUtilityCmdQueue = NULL;

  // Initialize the first responder object
  IErrFirstResponderSetOwner((IErrFirstResponder*)&pObj->m_xErrFirstResponder, this);
  IAEDAddFirstResponder(SysGetErrorDelegate(), (IErrFirstResponder*)&pObj->m_xErrFirstResponder, KEYBOARD_TASK_CFG_ERR_FR_PRIORITY);

  *pvTaskCode = KeyboardTaskRun;
  *pcName = "KEYD";
  *pnStackDepth = KEYBOARD_TASK_CFG_STACK_DEPTH;
  *pParams = this;
  *pxPriority = KEYBOARD_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t KeyboardSetReportDeliveryQueue(AManagedTaskEx *this, QueueHandle_t xQueue) {
  assert_param(this);
  KeyboardTask *pObj = (KeyboardTask*)this;

  pObj->m_xReportDeliveryQueue = xQueue;

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t KeyboardSetFNDelegate(AManagedTaskEx *this, IFNDelegate *pxDelegate) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *pObj = (KeyboardTask*)this;

  pObj->m_pxDelegate = pxDelegate;

  return xRes;
}

sys_error_code_t KeyboardDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode){
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *pObj = (KeyboardTask*)this;

  xRes = IDrvDoEnterPowerMode(pObj->m_pxDriver, eActivePowerMode, eNewPowerMode);

  switch (eNewPowerMode) {
//  case E_POWER_MODE_INACTIVE:
//    // reset the key scanner
//    xRes = KeyScannerReset(&pObj->m_xKeyScanner);
//    xRes = KeyScannerResetQueue(&pObj->m_xKeyScanner);
//    KeyboardTaskResetState(pObj);
//    break;

  case E_POWER_MODE_RUN:
    break;

  case E_POWER_MODE_SLEEP_1:
    // reset the key scanner
    xRes = KeyScannerReset(&pObj->m_xKeyScanner);
    xRes = KeyScannerResetQueue(&pObj->m_xKeyScanner);
    KeyboardTaskResetState(pObj);
    break;

//  case E_POWER_MODE_SLEEP_2:
//    break;
  }

  return xRes;
}

sys_error_code_t KeyboardHandleError(AManagedTask *this, SysEvent xError) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *pObj = (KeyboardTask*)this;

  if (xError.xEvent.nSource == SYS_ERR_EVT_SRC_IAED) {
    switch (xError.xEvent.nParam) {
      case SYS_ERR_EVT_PARAM_EFT:
        KeyMatrixDrvSetDebounceFilter(pObj->m_pxDriver, KMDRV_CFG_DEBOUNCE_FILTER_WITH_EFT);
        break;

      case SYS_ERR_EVT_PARAM_EFT_TIMEOUT:
        KeyMatrixDrvSetDebounceFilter(pObj->m_pxDriver, KMDRV_CFG_DEBOUNCE_FILTER);
        break;
    }
    if (AMTGetSystemPowerMode() == E_POWER_MODE_RUN) {
      IDrvStart(pObj->m_pxDriver);
    }
  }

  return xRes;
}

sys_error_code_t Keyboard_vtblForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *pObj = (KeyboardTask*)this;

  if ((eActivePowerMode == E_POWER_MODE_RUN)) {
    KeyEvent xEvent;
    xEvent.nKeyCode = KEYBOARD_TASK_KC_FORCE_STEP;
    KeyScannerForceKeyEvent(&pObj->m_xKeyScanner, &xEvent);
  }
  else {
    vTaskResume(this->m_xThaskHandle);
  }

  return xRes;
}

// IErrFirstResponder virtual functions definition
// ***********************************************

void KeyboardErrFRSetOwner(IErrFirstResponder *this, void *pxOwner) {
  assert_param(this);

  ((KeyboardFirstResponder*)this)->m_pOwner = pxOwner;
}

void *KeyboardErrFRGetOwner(IErrFirstResponder *this) {
  assert_param(this);

  return ((KeyboardFirstResponder*)this)->m_pOwner;
}

sys_error_code_t KeyboardErrFRNewError(IErrFirstResponder *this, SysEvent xError, boolean_t bIsCalledFromISR) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if ((xError.xEvent.nSource == SYS_ERR_EVT_SRC_IAED) &&
      ((xError.xEvent.nParam == SYS_ERR_EVT_PARAM_EFT) || (xError.xEvent.nParam == SYS_ERR_EVT_PARAM_EFT_TIMEOUT))) {
    // IFT error detected. Stop the driver to increase the debounce count.
    KeyboardTask *pOwner = (KeyboardTask*)IErrFirstResponderGetOwner(this);
    xRes = IDrvStop(pOwner->m_pxDriver);
  }

  return xRes;
}

// Private function implementation
// *******************************

static void KeyboardTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyboardTask *this = (KeyboardTask*)pParams;

  vTaskDelay(pdMS_TO_TICKS(MT_CFG_START_DELAY_MS));

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("Keyboard_Task: start.\r\n"));

  // Start the low level driver.
  xRes = IDrvStart(this->m_pxDriver);

  // prepare the report. The ID is HID_REPORT_ID_KEYBOARD (0x10) for a KEY event.
  this->m_xReport.inputReport10.reportId = HID_REPORT_ID_KEYBOARD;

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
        xRes = KeyboardTaskExecuteStepRun(this);
        taskENTER_CRITICAL();
          this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

//      case E_POWER_MODE_INACTIVE:
//      case E_POWER_MODE_SLEEP_2:
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

  UNUSED(xRes);
}

static sys_error_code_t KeyboardTaskExecuteStepRun(KeyboardTask *this) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  KeyEvent xEvent;
  boolean_t bIsShortcut = FALSE;

  // Wait for a keyboard event
  AMTExSetInactiveState((AManagedTaskEx*)this, TRUE);
  xRes = KeyScannerWaitForKeyevent(&this->m_xKeyScanner, &xEvent, portMAX_DELAY);
  AMTExSetInactiveState((AManagedTaskEx*)this, FALSE);

//		SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("K%i:%x\r\n", xEvent.nKeyPressed, xEvent.nKeyCode));

  // send the report to the USB task.
  if (xEvent.nKeyCode != KEYBOARD_TASK_KC_FORCE_STEP) {
    boolean_t bSendReport = FALSE;

    // There is one Key pressed/released event to process. There are four cases:
    // 1. The key is FN or FN Lock
    // 2. The key is a function key linked to FN and FN is active.
    // 3. The key is a shortcut linked to FN and FN is active.
    // 4. The key is a normal key.

    if ((xEvent.nKeyCode == KC_FN) || (xEvent.nKeyCode == KC_FN_LOCK)) {
      // case 1: the key is FN or FN Lock.
      // Update the Fn status and forward to the delegate.

      if (!KeyboardTaskUpdateFn(this, &xEvent)) {
        // the Fn key is released. If there are modifier keys set due to a shortcut I reset the modifiers.
        uint8_t nTemp = this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED];
        if (nTemp & KEYBOARD_TASK_LEFT_GUI_MASK) {
          if(this->m_xReport.inputReport10.nKeyCodeArray[0] != 0){
            // if Fx key is still pressed and Fn key is released
            this->m_bFxIsPressed = TRUE;
            this->m_xReport.inputReport10.nKeyCodeArray[0] = 0;
          }
          else{
            this->m_xReport.inputReport10.nLeftGUI = 0;
          }
          bSendReport = TRUE;
        }
        if (nTemp & KEYBOARD_TASK_LEFT_CTRL_MASK) {
          this->m_xReport.inputReport10.nLeftCtrl = 0;
          bSendReport = TRUE;
        }
        this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] = 0;
      }
    }  // end of case 1: the key is FN or FN Lock.
    else if (this->m_bFnActive && KEYBOARD_TASK_IS_FN_KEY(xEvent.nKeyCode)) {
      // case 2: the key is a function key linked to FN and FN is active.
      if (this->m_pxDelegate != NULL) {
        // Is a function key. Forwards the event to the delegate and stop processing.
        if (!xEvent.bKeyPressed) {
          // check if the key was pressed before FN. See bugwoa #5615
          for (int i=0; i<KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED; ++i) {
            if (this->m_xReport.inputReport10.nKeyCodeArray[i] == xEvent.nKeyCode) {
              this->m_xReport.inputReport10.nKeyCodeArray[i] = 0;
              --this->m_nSimultaneousKeysPressedCount;
              bSendReport = TRUE;
            }
          }
          if (!bSendReport) {
            // the key release event has not been consumed yet. Forward to the FNDelegate
            IFNDelegateOnKeyReleased(this->m_pxDelegate, xEvent.nKeyCode);
          }
        }
        else {
          IFNDelegateOnKeyPressed(this->m_pxDelegate, xEvent.nKeyCode);
        }

      }
    }
    else {
      // case 3 and 4:
      // The key is a shortcut linked to FN and FN is active. Or
      // The key is a normal key.
      switch (xEvent.nKeyCode) {
        // Check if is a modifier key.
        case KC_LEFT_GUI:
          this->m_xReport.inputReport10.nLeftGUI = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_LEFT_SHIFT:
          this->m_xReport.inputReport10.nLeftShift = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_LEFT_CTRL:
          this->m_xReport.inputReport10.nLeftCtrl = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_LEFT_ALT:
          this->m_xReport.inputReport10.nLeftAlt = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_RIGHT_GUI:
          this->m_xReport.inputReport10.nRightGUI = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_RIGHT_SHIFT:
          this->m_xReport.inputReport10.nRightShift = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_RIGHT_ALT:
          this->m_xReport.inputReport10.nRightAlt = xEvent.bKeyPressed;
          bSendReport = TRUE;
        break;
        case KC_RIGHT_CTRL:
          // This is a special case because it is the only key that depends on FN.
          // It is a modifiers if FN is not active, but it is the KEY MENU if FN is active
          if (!this->m_bFnActive) {
            this->m_xReport.inputReport10.nRightCtrl = xEvent.bKeyPressed;
            bSendReport = TRUE;
            break;
          }
        default:
          // add the key code
          bSendReport = KeyboardDefaultKeyEvent(this, &xEvent, &bIsShortcut);
          break;
      }
    }
    if (bSendReport && this->m_xReportDeliveryQueue) {
      xQueueSendToBack(this->m_xReportDeliveryQueue, &(this->m_xReport), pdMS_TO_TICKS(250));
      // If the queue is not free after 250ms the report is not sent.

      // If it is a key release event and the key is a shortcut, then send a second report
      // in order to release the modifiers.
      if (!xEvent.bKeyPressed && bIsShortcut) {
        KeyboardReleaseShortcutModifiers(this, &xEvent);
      }
    }
  }
  else {
    xRes = SYS_NO_ERROR_CODE;
  }

  return xRes;
}

static boolean_t KeyboardTaskUpdateFn(KeyboardTask *this, const KeyEvent *pxEvent) {
  assert_param(this);
  assert_param(pxEvent);

  boolean_t bFn = pxEvent->nKeyCode == KC_FN ? pxEvent->bKeyPressed : FALSE;
  if ((pxEvent->nKeyCode == KC_FN_LOCK) && !pxEvent->bKeyPressed) {
    this->m_bFnLock = !this->m_bFnLock;
  }

  boolean_t bFnActive = bFn || this->m_bFnLock;
  if (bFnActive != this->m_bFnActive) {
    // if the FN status is changed:
    this->m_bFnActive = bFnActive;
    // 1. notify the delegate
    if (this->m_pxDelegate != NULL) {
      IFNDelegateOnFnStatusChanged(this->m_pxDelegate, bFnActive);
    }
  }

  return this->m_bFnActive;
}

static void KeyboardTaskSetShortcut(KeyboardTask *this, KeyEvent *pxEvent) {
  assert_param(this);
  assert_param(pxEvent);
  boolean_t FN_Key_pressed = FALSE;

  switch (pxEvent->nKeyCode) {
    // for F1, F4, F5, F10, F11 and others I have to change the key code and set the LEFT_GUI flag.
  case KC_FN1:
    FN_Key_pressed = TRUE;
    this->m_xReport.inputReport10.nLeftGUI = 1;
    this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] |= KEYBOARD_TASK_LEFT_GUI_MASK;
    pxEvent->nKeyCode = KC_024;
    break;
  case KC_FN4:
    FN_Key_pressed = TRUE;
    this->m_xReport.inputReport10.nLeftGUI = 1;
    this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] |= KEYBOARD_TASK_LEFT_GUI_MASK;
    pxEvent->nKeyCode = KC_026;
    break;
  case KC_FN5:
    //FN_Key_pressed = TRUE;
    this->m_xReport.inputReport10.nLeftGUI = 1;
    this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] |= KEYBOARD_TASK_LEFT_GUI_MASK;
    this->m_xReport.inputReport10.nLeftCtrl = 1;
    this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] |= KEYBOARD_TASK_LEFT_CTRL_MASK;
    pxEvent->nKeyCode = KC_F24;
    break;
  case KC_FN10:
    FN_Key_pressed = TRUE;
    this->m_xReport.inputReport10.nLeftGUI = 1;
    this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] |= KEYBOARD_TASK_LEFT_GUI_MASK;
    pxEvent->nKeyCode = KC_016;
    break;
  case KC_FN11:
    FN_Key_pressed = TRUE;
    this->m_xReport.inputReport10.nLeftGUI = 1;
    this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] |= KEYBOARD_TASK_LEFT_GUI_MASK;
    pxEvent->nKeyCode = KC_033;
    break;

  case KC_RIGHT_CTRL:
    pxEvent->nKeyCode = KC_MENU;
    break;
  case KC_083:	// up arrow
    pxEvent->nKeyCode = KC_PAGEUP;
    break;
  case KC_084:	// down arrow
    pxEvent->nKeyCode = KC_PAGEDOWN;
    break;
  case KC_079:	// left arrow
    pxEvent->nKeyCode = KC_HOME;
    break;
  case KC_089:	// right arrow
    pxEvent->nKeyCode = KC_END;
    break;
  case KC_075:
    pxEvent->nKeyCode = KC_PRINTSCREEN;
    break;
  }
  if (FN_Key_pressed && this->m_xReportDeliveryQueue) {
    xQueueSendToBack(this->m_xReportDeliveryQueue, &(this->m_xReport), pdMS_TO_TICKS(250));
    // If the queue is not free after 250ms the report is not sent.
  }
}

static void KeyboardTaskResetState(KeyboardTask *this) {
  assert_param(this);

  this->m_nSimultaneousKeysPressedCount = 0;
  // don't reset m_bFnActive and m_bFnLock because we have to restore the FN status when the
  // system goes in RUN.
  this->m_bFnActive = this->m_bFnLock;

  for (uint8_t i=0; i< KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED; i++) {
    this->m_xReport.inputReport10.nKeyCodeArray[i] = 0;
    this->m_pnFnShortcutMap[i] = 0;
  }
  this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] = 0;
  this->m_xReport.reportID = HID_REPORT_ID_KEYBOARD;
  this->m_xReport.inputReport10.nLeftAlt = 0;
  this->m_xReport.inputReport10.nLeftAlt = 0;
  this->m_xReport.inputReport10.nLeftGUI = 0;
  this->m_xReport.inputReport10.nLeftShift = 0;
  this->m_xReport.inputReport10.nRightAlt = 0;
  this->m_xReport.inputReport10.nRightCtrl = 0;
  this->m_xReport.inputReport10.nRightGUI = 0;
  this->m_xReport.inputReport10.nRightShift = 0;
}

static boolean_t KeyboardDefaultKeyEvent(KeyboardTask *this, KeyEvent *pxEvent, boolean_t *pbIsShortcut) {
  assert_param(this);
  assert_param(pbIsShortcut);
  boolean_t bSendReport = FALSE;

  // add the key code
  if (pxEvent->bKeyPressed && (this->m_nSimultaneousKeysPressedCount < KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED)) {
    boolean_t bKeyAlreadyInReport = FALSE;
    // check if the key is already in the report
    for (int i=0; i<KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED; ++i) {
      if (this->m_xReport.inputReport10.nKeyCodeArray[i] == pxEvent->nKeyCode) {
        bKeyAlreadyInReport = TRUE;
        break;
      }
    }
    if (!bKeyAlreadyInReport) {
      // search for a free position in the key code array
      for (int i=0; i<KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED; ++i) {
        if (this->m_xReport.inputReport10.nKeyCodeArray[i] == 0) {
          if (KEYBOARD_TASK_HAS_SHORTCUT(pxEvent->nKeyCode) && (this->m_bFnActive)) {
            // if the key has a shortcut and the FN key is active, then remap the key code.
            this->m_pnFnShortcutMap[i] = pxEvent->nKeyCode;
            KeyboardTaskSetShortcut(this, pxEvent);
            *pbIsShortcut = TRUE;
          }
          this->m_xReport.inputReport10.nKeyCodeArray[i] = pxEvent->nKeyCode;
          ++this->m_nSimultaneousKeysPressedCount;
          bSendReport = TRUE;
          break;
        }
      }
    }
  }
  else if (!pxEvent->bKeyPressed) {
    for (int i=0; i<KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED; ++i) {
      if (this->m_pnFnShortcutMap[i] == pxEvent->nKeyCode) {
        this->m_pnFnShortcutMap[i] = 0;
        this->m_xReport.inputReport10.nKeyCodeArray[i] = 0;
        // special case combination FN+F1:
        // if I clean only the key, then, when the user release FN, Windows display the START menu.
        // But if FN Lock is active then Windows works fine.
        if(this->m_bFxIsPressed){
          // if Fn key is released before.
          this->m_xReport.inputReport10.nLeftGUI = 0;
          this->m_bFxIsPressed = FALSE;
        }
        /*if ((pxEvent->nKeyCode == KC_FN1) && !this->m_bFnLock) {
          this->m_xReport.inputReport10.nLeftGUI = 0;
        }*/
        --this->m_nSimultaneousKeysPressedCount;
        *pbIsShortcut = TRUE;
        bSendReport = TRUE;
      }
      else if (this->m_xReport.inputReport10.nKeyCodeArray[i] == pxEvent->nKeyCode) {
        this->m_xReport.inputReport10.nKeyCodeArray[i] = 0;
        --this->m_nSimultaneousKeysPressedCount;
        bSendReport = TRUE;
      }
    }
  }

  return bSendReport;
}
static void KeyboardReleaseShortcutModifiers(KeyboardTask *this, KeyEvent *pxEvent) {
  assert_param(this);
  assert_param(pxEvent);
  boolean_t bSendReport = FALSE;

  // the Fn key is released. If there are modifier keys set due to a shortcut I reset the modifiers.
  uint8_t nTemp = this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED];
  if (nTemp & KEYBOARD_TASK_LEFT_GUI_MASK) {
    if(this->m_xReport.inputReport10.nKeyCodeArray[0] != 0){
      // if Fx key is still pressed and Fn key is released
      this->m_bFxIsPressed = TRUE;
      this->m_xReport.inputReport10.nKeyCodeArray[0] = 0;
    }
    else{
      this->m_xReport.inputReport10.nLeftGUI = 0;
    }
    bSendReport = TRUE;
  }
  if (nTemp & KEYBOARD_TASK_LEFT_CTRL_MASK) {
    this->m_xReport.inputReport10.nLeftCtrl = 0;
    bSendReport = TRUE;
  }
  this->m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED] = 0;

  if (bSendReport && this->m_xReportDeliveryQueue) {
    xQueueSendToBack(this->m_xReportDeliveryQueue, &(this->m_xReport), pdMS_TO_TICKS(250));
  }
}
