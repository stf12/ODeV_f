/**
 ******************************************************************************
 * @file    KeyboardTask.h
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

#ifndef KEYBOARDTASK_H_

#define KEYBOARDTASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "hid_report_parser.h"
#include "KeyScanner.h"
#include "IFNDelegate.h"
#include "IFNDelegateVtbl.h"
#include "IErrorFirstResponder.h"
#include "IErrorFirstResponderVtbl.h"
#include "queue.h"

#define KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED		6

/**
 * Creates a type name for _KeyboardTask.
 */
typedef struct _KeyboardTask KeyboardTask;

/**
 * First responder class to handle system errors as soon as they they are generated.
 */
typedef struct _KeyboardFirstResponder {
  /**
   * Base interface.
   */
  IErrFirstResponder super;

  /**
   * First responder owner;
   */
  void *m_pOwner;
} KeyboardFirstResponder;

/**
 * This structure specifies the internal state of a task object.
 */
struct _KeyboardTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  /**
   * It specifies a pointer to a driver object used by the task.
   * The task has the responsibility to allocate the driver and initialize it using
   * the function IDrvInit().
   */
  IDriver *m_pxDriver;

  /**
   * The key scanner object provides the services to analyze the key matrix data
   * coming from the low level driver and synthesize the high level event.
   */
  KeyScanner m_xKeyScanner;

  /**
   * First responder object that implement the ::IErrFirstResponder interface.
   */
  KeyboardFirstResponder m_xErrFirstResponder;

  /**
   * Specify the queue used by the task to delivery the input reports received from the Device.
   * It must be set using the KeyboardSetReportDeliveryQueue API function.
   * If the queue is NULL the input reports are not delivered.
   */
  QueueHandle_t m_xReportDeliveryQueue;

  /**
   * Specify the queue used to access the service provided by the Utility task.
   */
  QueueHandle_t m_xUtilityCmdQueue;

  /**
   * Specifies the input report for the key event.
   */
  HIDReport m_xReport;

  /**
   * The function key delegate. This object receive notification about
   * the function keys: FN, FN Lock, F2, F3, F6, F7, F8, F9, F12.
   * The delegate doesn't receive notification about the shortcut: F1, F4, F5, F10, F11.
   */
  IFNDelegate *m_pxDelegate;

  /**
   * Specifies the number of simultaneous keys pressed.
   */
  uint8_t m_nSimultaneousKeysPressedCount;

  /**
   * Specifies if the virtual FN key is active or not. It depend on FN Lock and FN.
   */
  boolean_t m_bFnActive;

  /**
   * Specifies the FN Lock status.
   */
  boolean_t m_bFnLock;

  /**
    * Specifies the Fx(x=1,4,10,11) is still pressed.
    */
  boolean_t m_bFxIsPressed;

  /**
   * Store the Fx shortcuts (FN + Fx_shortcuts) pressed and not released plus the modifiers
   * bits depending on FN.
   */
  uint8_t m_pnFnShortcutMap[KEYBOARD_TASK_MAX_SIMULTANEOUS_KEYS_PRESSED + 1];

};


/**
 * Instantiate an object of KeyboardTask type.
 *
 * @return a pointer to the the new created object or NULL if out of memory error occurs.
 */
AManagedTaskEx *KeyboardTaskAlloc();

/**
 * Set the queue used to delivery the Input reports received from the Device.
 *
 * @param _this [IN] specifies a pointer to task object.
 * @param xQueue [IN] specifies a queue
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t KeyboardSetReportDeliveryQueue(AManagedTaskEx *_this, QueueHandle_t xQueue);

/**
 * Set the Function Key delegate object.
 *
 * @param _this [IN] specifies a pointer to task object.
 * @param pxDelegate [IN] specifies an object implementing the IFNDelegate interface, or NULL to remove the delegate.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t KeyboardSetFNDelegate(AManagedTaskEx *_this, IFNDelegate *pxDelegate);

/**
 * Set the queue used by the task to use the functionality exposed by the Utility task.
 *
 * @param _this [IN]specifies a task object pointer.
 * @param xQueue [IN] specifies a queue.
 */
inline void KeyboardSetUtilityCmdQueue(AManagedTaskEx *_this, QueueHandle_t xQueue);


// Inline function definition.
// ***************************

SYS_DEFINE_INLINE
void KeyboardSetUtilityCmdQueue(AManagedTaskEx *_this, QueueHandle_t xQueue) {
  assert_param(_this);
  KeyboardTask *pObj = (KeyboardTask*)_this;

  pObj->m_xUtilityCmdQueue = xQueue;
}


#ifdef __cplusplus
}
#endif


#endif /* KEYBOARDTASK_H_ */
