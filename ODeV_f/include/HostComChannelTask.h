/**
 ******************************************************************************
 * @file    HostComChannelTask.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Mar 8, 2018
 *
 * @brief USB subsystem.
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
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
#ifndef HOSTCOMCHANNELTASK_H_
#define HOSTCOMCHANNELTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "HID-USBHelper.h"
#include "queue.h"
#include "semphr.h"
#include "hid_report_parser.h"

//#include "StubUSBListener.h"
//#include "StubUSBListener_vtbl.h"

/**
 * Create  type name for _HostComChannelTask.
 */
typedef struct _HostComChannelTask HostComChannelTask;

/**
 *  HostComChannelTask internal structure.
 */
struct _HostComChannelTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  HIDUSBHelper *m_pxHIDUSBHelper;

  QueueHandle_t m_xInputReportQueue;

  QueueHandle_t m_xUtilityCmdQueue;

  SemaphoreHandle_t m_xMutex;

  HIDReport m_xReport;

  int8_t m_nInactivityStepsCount;
  boolean_t m_bIs30sTimerStarted;
  uint8_t m_nDelayKeyReport;
  uint8_t m_nDelayLidReport;
  uint8_t m_nAttemptsToSendReport;

  /**
   * Specifies the time to wait for an event in the step execution.
   * After the suspend timer is started we can wait for infinite time.
   */
  TickType_t m_nStepDelay;

//  StubUSBListener *m_pxUSBListener;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of HostComChannelTask.
 *
 * @return a pointer to the generic obejct ::AManagedTask if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *HostComChannelTaskAlloc();

/**
 * Add an event listener object to this event source.
 *
 * @param _this [IN]specifies a task object pointer.
 * @param pxListener [IN] specifies a pointer to an event listener.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t HostComChannelAddEventListener(AManagedTaskEx *_this, IEventListener *pxListener);

/**
 * Get the queue used to delivery the input report to the task. Each reeport is send to the HOST via USB.
 *
 * @param _this [IN]specifies a task object pointer.
 * @return the input report queue.
 */
inline QueueHandle_t HostComChannelGetInputReportQueue(AManagedTaskEx *_this);

/**
 * Set the queue used by the task to use the functionality exposed by the Utility task.
 *
 * @param _this [IN]specifies a task object pointer.
 * @param xQueue [IN] specifies a queue.
 */
inline void HostComChannelSetUtilityCmdQueue(AManagedTaskEx *_this, QueueHandle_t xQueue);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
QueueHandle_t HostComChannelGetInputReportQueue(AManagedTaskEx *_this) {
  assert_param(_this);
  HostComChannelTask *pObj = (HostComChannelTask*)_this;

  return pObj->m_xInputReportQueue;
}

SYS_DEFINE_INLINE
void HostComChannelSetUtilityCmdQueue(AManagedTaskEx *_this, QueueHandle_t xQueue) {
  assert_param(_this);
  HostComChannelTask *pObj = (HostComChannelTask*)_this;

  pObj->m_xUtilityCmdQueue = xQueue;
}

#ifdef __cplusplus
}
#endif

#endif /* HOSTCOMCHANNELTASK_H_ */
