/**
 ******************************************************************************
 * @file    ShortcutsDemonTask.h
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
#ifndef SHORTCUTSDEMONTASK_H_
#define SHORTCUTSDEMONTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "queue.h"
#include <IShortcut.h>


/**
 * Create  type name for _ShortcutsDemonTask.
 */
typedef struct _ShortcutsDemonTask ShortcutsDemonTask;

/**
 *  ShortcutsDemonTask internal structure.
 */
struct _ShortcutsDemonTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  /**
   * This queue receives the shortcuts to be sent to the host.
   */
  QueueHandle_t m_xShortcutsInQueue;

  /**
   * This queue is used to deliver the keys report sequences to the host communication channel task.
   * N.B. when there is the key scanner, then the key must delivered as a key event to the keyboard task.
   */
  QueueHandle_t m_xOutputQueue;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of ShortcutsDemonTask.
 *
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *ShortcutsDemonTaskAlloc();

/**
 * Post a shortcut in the task input queue.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @param pxShortcut [IN] specifies a pointer to a shortcut to be inserted in the queue. Note that because
 *        it is a pointer the caller must guarantee that the shortcut object persist.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t ShortcutsDemonTaskPostShortcuts(ShortcutsDemonTask *_this, odev::IShortcut *pxShortcut);

/**
 * Set the queue used by the task to deliver the key report sequences to the host communication task.
 *
 * @param _this  [IN] specifies a pointer to a task object.
 * @param xQueue [IN] specifies a queue.
 * @return SYS_NO_ERROR_CODE.
 */
sys_error_code_t ShortcutsDemontaskSetOutputQueue(ShortcutsDemonTask *_this, QueueHandle_t xQueue);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SHORTCUTSDEMONTASK_H_ */
