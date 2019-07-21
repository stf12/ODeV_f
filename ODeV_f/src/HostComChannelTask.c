/**
 ******************************************************************************
 * @file    HostComChannelTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Mar 8, 2018
 *
 * @brief
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

#include "usbd_desc.h"
#include "usbd_hid.h"
#include "HostComChannelTask.h"
#include "HostComChannelTask_vtbl.h"
#include "syslowpower.h"
#include "sysdebug.h"

#ifndef HCC_TASK_CFG_STACK_DEPTH
#define HCC_TASK_CFG_STACK_DEPTH                     256
#endif

#ifndef HCC_TASK_CFG_PRIORITY
#define HCC_TASK_CFG_PRIORITY                        (configMAX_PRIORITIES - 3)
#endif

#ifndef HCC_TASK_CFG_IN_QUEUE_LENGTH
#define HCC_TASK_CFG_IN_QUEUE_LENGTH                 3
#endif

#ifndef HCC_TASK_CFG_MAX_INACTIVE_STEPS
#define HCC_TASK_CFG_MAX_INACTIVE_STEPS              3
#endif

#define HCC_TASK_CFG_IN_QUEUE_ITEM_SIZE              sizeof(HIDReport)

#define HCC_TASK_CFG_MAX_TIMES_SEND_REPORT_BUSY      20
#define HCC_TASK_CFG_MAX_TIMES_SEND_REPORT_NO_BUS    5
#define HCC_TASK_CFG_KEY_REPORT_DELAY_MS             300
#define HCC_TASK_CFG_KEY_REPORT_DELAY_N              1

#define HCC_TASK_STEP_WAITING_TIME_MS                (pdMS_TO_TICKS(100))

#ifndef MT_CFG_START_DELAY_MS
#define MT_CFG_START_DELAY_MS                        1000
#endif

#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_HCC_TASK, level, message)


/**
 * HostComChannelTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xHostComChannelTask_vtbl = {
    HostComChannelTask_vtblHardwareInit,
    HostComChannelTask_vtblOnCreateTask,
    HostComChannelTask_vtblDoEnterPowerMode,
    HostComChannelTask_vtblHandleError,
    HostComChannelTask_vtblForceExecuteStep
};

/**
 * Create a type name for the USB Lib function used to send a report.
 */
typedef uint8_t (*SendReportF)(USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len);

/**
 * The only instance of the USB stack.
 */
static USBD_HandleTypeDef s_xUsbDevice;

/**
 * The only instance of the task object.
 */
static HostComChannelTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t HostComChannelTaskExecuteStepRun(HostComChannelTask *this);

/**
 * Task control function.
 *
 * @param pParams [IN] specifies a pointer to the task object.
 */
static void HostComChannelTaskRun(void *pParams);

/**
 * Empty the task's report queue.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t HostComChannelResetQueue(HostComChannelTask *this);

/**
 * Function used by the task to directly send a report to the host through the USB Lib API.
 * With this function the report is sent immediately to the USB stack.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param fSendReport [IN] specifies a low level function (exported by the USB lib) to use. It depend on the USB IF.
 * @param pnReport [IN] specifies a buffer with the report to send.
 * @param nReportlen [IN] specifies the size of the report to send.
 * @return SYS_NO_EROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t HostComChannelTaskSendReport(HostComChannelTask *this, SendReportF fSendReport, uint8_t *pnReport, uint16_t nReportlen);

/**
 * Private function used to post a report into the task queue. The report will be sent to the USB stack in the task control loop.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pnReport [IN] specifies a buffer with the report to send.
 * @param nReportlen [IN] specifies the size of the report to send.
 * @return SYS_NO_EROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t HostComChannelTaskPostReport(HostComChannelTask *this, uint8_t *pnReport, uint16_t nReportlen);

/**
 * Private function used to post a report into the front of the task queue. The report will be sent to the USB stack in the task control loop.
 * This is used to retransmit the report whe the USB bus is not resumed yet.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pnReport [IN] specifies a buffer with the report to send.
 * @param nReportlen [IN] specifies the size of the report to send.
 * @return SYS_NO_EROR_CODE if success, SYS_HCC_TASK_REPORT_LOST_ERROR_CODE code if the queue is full after 200ms.
 */
static inline sys_error_code_t HostComChannelTaskPostReportToFront(HostComChannelTask *this, uint8_t *pnReport, uint16_t nReportlen);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
extern QueueHandle_t HostComChannelGetInputReportQueue(AManagedTaskEx *this);
extern void HostComChannelSetUtilityCmdQueue(AManagedTaskEx *this, QueueHandle_t xQueue);
#endif


// Public API definition
// *********************

AManagedTaskEx *HostComChannelTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xHostComChannelTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t HostComChannelTask_vtblHardwareInit(AManagedTask *this, void *pParams) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  HostComChannelTask *pObj = (HostComChannelTask*)this;

//  // Enable Power Clock
//  __HAL_RCC_PWR_CLK_ENABLE();
//
//  // Enable USB power on Pwrctrl CR2 register
//  HAL_PWREx_EnableVddUSB();

  // Initialize Device Library, Add Supported Class and Start the library
  USBD_Init(&s_xUsbDevice, &g_xHidDesc, 0);
  USBD_RegisterClass(&s_xUsbDevice, (USBD_ClassTypeDef*)&USBD_HID);

  return xRes;
}

sys_error_code_t HostComChannelTask_vtblOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HostComChannelTask *pObj = (HostComChannelTask*)this;

  // Create task specific sw resources.
  pObj->m_xInputReportQueue = xQueueCreate(HCC_TASK_CFG_IN_QUEUE_LENGTH, HCC_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInputReportQueue == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    return xRes;
  }

  pObj->m_xMutex = xSemaphoreCreateRecursiveMutex();
  if (pObj->m_xMutex == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    return xRes;
  }

  pObj->m_pxHIDUSBHelper = (HIDUSBHelper*)HIDUSBHelperAlloc();
  if (pObj->m_pxHIDUSBHelper == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    return xRes;
  }
  // install the usb helper object into the ST USB stack
  IEventSrcInit((IEventSrc*)pObj->m_pxHIDUSBHelper);
  s_xUsbDevice.pUserData = pObj->m_pxHIDUSBHelper;

  pObj->m_xUtilityCmdQueue = NULL;
  pObj->m_nInactivityStepsCount = 0;
  pObj->m_bIs30sTimerStarted = FALSE;
  pObj->m_nDelayKeyReport = 0;
  pObj->m_nDelayLidReport = 0;
  pObj->m_nAttemptsToSendReport = 0;
  pObj->m_nStepDelay = HCC_TASK_STEP_WAITING_TIME_MS;

  *pvTaskCode = HostComChannelTaskRun;
  *pcName = "HCCT";
  *pnStackDepth = HCC_TASK_CFG_STACK_DEPTH;
  *pParams = this;
  *pxPriority = HCC_TASK_CFG_PRIORITY;

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInputReportQueue, "USB_Q");
#endif

  return xRes;
}

sys_error_code_t HostComChannelTask_vtblDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HostComChannelTask *pObj = (HostComChannelTask*)this;

//  SysPowerStatus xPowerStatus = SysGetPowerStatus();

  switch (eNewPowerMode) {
  case E_POWER_MODE_RUN:
    break;

  case E_POWER_MODE_SLEEP_1:
    xRes = HostComChannelResetQueue(pObj);
    break;
  }


  return xRes;
}

sys_error_code_t HostComChannelTask_vtblHandleError(AManagedTask *this, SysEvent xError) {
  UNUSED(this);
  UNUSED(xError);
  PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*)s_xUsbDevice.pData;

  if ((xError.xEvent.nSource == SYS_ERR_EVT_SRC_IAED) && (xError.xEvent.nParam == SYS_ERR_EVT_PARAM_EFT_TIMEOUT)) {
    // check if the USB bus is suspended
    if (USBD_IsBusSuspended()) {
      // try to resume the USB bus
      // Activate Remote wakeup
      HAL_PCD_ActivateRemoteWakeup((hpcd));
      // remote wakeup delay
      HAL_Delay(10);
      // Disable Remote wakeup
      HAL_PCD_DeActivateRemoteWakeup((hpcd));
    }
  }

  return SYS_NO_ERROR_CODE;
}


// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t HostComChannelTask_vtblForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode) {
  assert_param(this);
  HostComChannelTask *pObj = (HostComChannelTask*)this;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  struct internalReportFE_t xReport;

  if (eActivePowerMode == E_POWER_MODE_RUN) {
    xReport.nData = 0;
    xReport.reportId = HID_REPORT_ID_FORCE_STEP;

    xRes = HostComChannelTaskPostReportToFront(pObj, (uint8_t*)&xReport, 2);
  }
  else {
    vTaskResume(this->m_xThaskHandle);
  }

  return xRes;
}

sys_error_code_t HostComChannelAddEventListener(AManagedTaskEx *this, IEventListener *pxListener) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HostComChannelTask *pObj = (HostComChannelTask*)this;

  if (pxListener != NULL) {
   xRes = IEventSrcAddEventListener((IEventSrc*)pObj->m_pxHIDUSBHelper, pxListener);
  }

  return xRes;
}


// Private function definition
// ***************************

static void HostComChannelTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HostComChannelTask *this = (HostComChannelTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HCCT: start.\r\n"));

  vTaskDelay(pdMS_TO_TICKS(MT_CFG_START_DELAY_MS));

  USBD_Start(&s_xUsbDevice);

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
        xRes = HostComChannelTaskExecuteStepRun(this);
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

static sys_error_code_t HostComChannelTaskExecuteStepRun(HostComChannelTask *this) {
  assert_param(this);
  uint16_t nSize = 0;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

//  AMTExSetInactiveState((AManagedTaskEx*)this, this->m_bIs30sTimerStarted);
//  if (pdTRUE == xQueueReceive(this->m_xInputReportQueue, &this->m_xReport, this->m_nStepDelay)) {
//    AMTExSetInactiveState((AManagedTaskEx*)this, FALSE);
  if (pdTRUE == xQueueReceive(this->m_xInputReportQueue, &this->m_xReport, pdMS_TO_TICKS(100))) {

    if (this->m_bIs30sTimerStarted) {
      // stop the 30" timer
//      uint32_t nCommand = UTILITY_MAKE_COMMAND(UTILITY_CMD_ID_TIMER, UTILITY_CMD_PARAM_STOP);
//      if (this->m_xUtilityCmdQueue != NULL) {
//        xQueueSendToBack(this->m_xUtilityCmdQueue, &nCommand, pdMS_TO_TICKS(20));
//      }
      this->m_bIs30sTimerStarted = FALSE;
      this->m_nStepDelay = HCC_TASK_STEP_WAITING_TIME_MS;
    }

    // Send the report to the Host via USB
    if (this->m_xReport.reportID == HID_REPORT_ID_FORCE_STEP) {
      // do nothing. we need to change the power mode.
      __NOP();
    }
    else if (this->m_xReport.reportID == HID_REPORT_ID_MOUSE) {
      nSize = sizeof(this->m_xReport.inputReport02);
      if (pdTRUE == xSemaphoreTakeRecursive(this->m_xMutex, pdMS_TO_TICKS(100))) {
        if (!USBD_IsBusSuspended()) {
          USBD_HID_MouseSendReport(&s_xUsbDevice, (uint8_t*)&this->m_xReport.inputReport02, nSize);
        }
        xSemaphoreGiveRecursive(this->m_xMutex);
      }
    }
    else if (this->m_xReport.reportID == HID_REPORT_ID_KEYBOARD) {
      if (this->m_nDelayKeyReport) {
        vTaskDelay(pdMS_TO_TICKS(HCC_TASK_CFG_KEY_REPORT_DELAY_MS));
        this->m_nDelayKeyReport--;
      }
      nSize = 8;
      uint8_t *pnReport = ((uint8_t*)&this->m_xReport.inputReport10) + 1;
      xRes = HostComChannelTaskSendReport(this, USBD_HID_KeyboardSendReport, pnReport, nSize);

      }
    else if (this->m_xReport.reportID == HID_REPORT_ID_FN_KEYS_1) {
      // send the report via EP3
      nSize = 2; // sizeof(this->m_xReport.inputReport12);
      xRes = HostComChannelTaskSendReport(this, USBD_HID_CustomSendReport, (uint8_t*)&this->m_xReport.inputReport12, nSize);
    }
    else if (this->m_xReport.reportID == HID_REPORT_ID_FN_KEYS_2) {
      // send the report via EP3
      nSize = 2; // sizeof(this->m_xReport.inputReport14);
      xRes = HostComChannelTaskSendReport(this, USBD_HID_CustomSendReport, (uint8_t*)&this->m_xReport.inputReport14, nSize);
    }
  }
  else {
    if (++this->m_nInactivityStepsCount > HCC_TASK_CFG_MAX_INACTIVE_STEPS) {
      if (!this->m_bIs30sTimerStarted) {
//        uint32_t nCommand = UTILITY_MAKE_COMMAND(UTILITY_CMD_ID_TIMER, UTILITY_CMD_PARAM_RESET);
//        if (this->m_xUtilityCmdQueue != NULL) {
//          xQueueSendToBack(this->m_xUtilityCmdQueue, &nCommand, pdMS_TO_TICKS(20));
//        }
        this->m_bIs30sTimerStarted = TRUE;
        this->m_nStepDelay = portMAX_DELAY;
      }
      this->m_nInactivityStepsCount = 0;
    }
  }

  return xRes;
}

static sys_error_code_t HostComChannelResetQueue(HostComChannelTask *this){
  assert_param(this != NULL);

  xQueueGenericReset(this->m_xInputReportQueue, 0);

  return SYS_NO_ERROR_CODE;
}

static sys_error_code_t HostComChannelTaskSendReport(HostComChannelTask *this, SendReportF fSendReport, uint8_t *pnReport, uint16_t nReportlen) {
  assert_param(this);
  sys_error_code_t xRes = SYS_HCC_TASK_REPORT_LOST_ERROR_CODE;

  if (pdTRUE == xSemaphoreTakeRecursive(this->m_xMutex, pdMS_TO_TICKS(100))) {
    for (int i=0; i<HCC_TASK_CFG_MAX_TIMES_SEND_REPORT_BUSY; ++i) {
      if (!USBD_IsBusSuspended()) {
        if (USBD_OK == fSendReport(&s_xUsbDevice, pnReport, nReportlen)) {
          this->m_nAttemptsToSendReport = 0;
          xRes = SYS_NO_ERROR_CODE;
          break;
        }
        else {
          vTaskDelay(pdMS_TO_TICKS(5));
        }
      }
      else {
        // USB bus is still suspended
        if (this->m_nAttemptsToSendReport++ < HCC_TASK_CFG_MAX_TIMES_SEND_REPORT_NO_BUS) {
          xRes = HostComChannelTaskPostReportToFront(this, pnReport, nReportlen);
          if (!SYS_IS_ERROR_CODE(xRes)) {
            xRes = SYS_NO_ERROR_CODE;
            vTaskDelay(pdMS_TO_TICKS(100));
          }
        }
        else {
          // report is lost because the USB bus is still suspended
          this->m_nAttemptsToSendReport = 0;
        }
        break;
      }
    }

    xSemaphoreGiveRecursive(this->m_xMutex);
  }

  if (SYS_IS_ERROR_CODE(xRes)) {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    xRes = SYS_NO_ERROR_CODE;
  }

  return xRes;
}

static sys_error_code_t HostComChannelTaskPostReport(HostComChannelTask *this, uint8_t *pnReport, uint16_t nReportlen) {
  assert_param(this);
  assert_param(pnReport);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HIDReport xReport;

  memset(&xReport, 0, sizeof(HIDReport));
  memcpy(&xReport, pnReport, nReportlen);
  if (pdTRUE != xQueueSendToBack(this->m_xInputReportQueue, &xReport, pdMS_TO_TICKS(200))) {
    xRes = SYS_HCC_TASK_REPORT_LOST_ERROR_CODE;
    // this function is private and the caller will ignore this return code.
   }

  return xRes;
}

static inline sys_error_code_t HostComChannelTaskPostReportToFront(HostComChannelTask *this, uint8_t *pnReport, uint16_t nReportlen) {
  assert_param(this);
  assert_param(pnReport);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HIDReport xReport;

  memset(&xReport, 0, sizeof(HIDReport));
  memcpy(&xReport, pnReport, nReportlen);
  if (pdTRUE != xQueueSendToFront(this->m_xInputReportQueue, &xReport, pdMS_TO_TICKS(200))) {
    xRes = SYS_HCC_TASK_REPORT_LOST_ERROR_CODE;
    // this function is private and the caller will ignore this return code.
  }

  return xRes;
}
