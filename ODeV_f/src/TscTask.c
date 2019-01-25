/**
 ******************************************************************************
 * @file    TscTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Dec 21, 2018
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

#include "TscTask.h"
#include "TscTask_vtbl.h"
#include "GuiConfig.h"
#include "stm32l4r9i_eval.h"
#include "stm32l4r9i_eval_io.h"
#include "stm32l4r9i_eval_ts.h"
#include "sysdebug.h"

#ifndef TSC_TASK_CFG_STACK_DEPTH
#define TSC_TASK_CFG_STACK_DEPTH      150
#endif

#ifndef TSC_TASK_CFG_PRIORITY
#define TSC_TASK_CFG_PRIORITY         (tskIDLE_PRIORITY+4)
#endif

#define SYS_DEBUGF(level, message)    SYS_DEBUGF3(SYS_DBG_TSC, level, message)

/**
 * Specifies the resources used between the task and the ISR.
 */
typedef struct _HardwareResources {
  /**
   * Synchronization queue.
   */
  QueueHandle_t xVsyncQueue;
} HardwareResource;

/**
 * TscTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xTscTask_vtbl = {
    TscTask_vtblHardwareInit,
    TscTask_vtblOnCreateTask,
    TscTask_vtblDoEnterPowerMode,
    TscTask_vtblHandleError,
    TscTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static TscTask s_xTaskObj;

/**
 * Resources used between task and IRS.
 */
static HardwareResource s_xHardwareResources = {0};


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t TscTaskExecuteStepRun(TscTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void TscTaskRun(void *pParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
#endif


// Public API definition
// *********************

AManagedTaskEx *TscTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xTscTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t TscTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  TscTask *pObj = (TscTask*)_this;

  if (BSP_TS_Init(GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT) == TS_OK) {
    /* Enable TS interrupt */
    if (BSP_TS_ITConfig() != TS_OK) {
      sys_error_handler();
    }
    //      TouchTaskInit();
  }
  else {
    sys_error_handler();
  }

  return xRes;
}

sys_error_code_t TscTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  TscTask *pObj = (TscTask*)_this;

  pObj->m_xVsyncQueue = xQueueCreate(1, 1);
  if (pObj->m_xVsyncQueue == NULL) {
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }
  else {
    // Initialize the hardware resource.
    s_xHardwareResources.xVsyncQueue = pObj->m_xVsyncQueue;

    *pvTaskCode = TscTaskRun;
    *pcName = "TSC";
    *pnStackDepth = TSC_TASK_CFG_STACK_DEPTH;
    *pParams = _this;
    *pxPriority = TSC_TASK_CFG_PRIORITY;
  }

  return xRes;
}

sys_error_code_t TscTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  TscTask *pObj = (TscTask*)_this;

  return xRes;
}

sys_error_code_t TscTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  TscTask *pObj = (TscTask*)_this;

  return xRes;
}

sys_error_code_t TscTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  TscTask *pObj = (TscTask*)_this;

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t TscTaskExecuteStepRun(TscTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  uint8_t nDummy = 0x5a;

  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  xQueueReceive(_this->m_xVsyncQueue, &nDummy, portMAX_DELAY);
  AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);

//  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("TSC: step!\r\n"));

  // Update TS data
  // TODO: STF - To be checked for the display 480x272.
  // The default operation mode is in polling for this touch screen.
//  BSP_TS_UpdateState();

  return xRes;
}

static void TscTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  TscTask *_this = (TscTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("TSC: start.\r\n"));

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
        xRes = TscTaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

//      case E_POWER_MODE_INACTIVE:
//      case E_POWER_MODE_SLEEP_2:
      case E_POWER_MODE_SLEEP_1:
        vTaskDelay(pdMS_TO_TICKS(100));
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

/**
  * @brief  interrupt detection
  * @param  None
  * @retval None
  */
void TouchDetectInt(uint16_t nPin)
{
  UNUSED(nPin);

  static uint8_t nDummy = 0x5a;
  if (s_xHardwareResources.xVsyncQueue != NULL) {
    /* Release the semaphore if the thread was created prior to this call */
    portBASE_TYPE px = pdFALSE;
    xQueueSendFromISR(s_xHardwareResources.xVsyncQueue, &nDummy, &px);
    portEND_SWITCHING_ISR(px);
  }
}
