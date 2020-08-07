/**
 ******************************************************************************
 * @file    UtilTask.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 23, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

#include "UtilTask.h"
#include "UtilTask_vtbl.h"
#include "HSD_json.h"
#include "hid_report_parser.h"
#include "mx.h"
#include "sysdebug.h"

// TODO: cange XXX with a short id for the task

#ifndef UTIL_TASK_CFG_STACK_DEPTH
#define UTIL_TASK_CFG_STACK_DEPTH              120
#endif

#ifndef UTIL_TASK_CFG_PRIORITY
#define UTIL_TASK_CFG_PRIORITY                 (tskIDLE_PRIORITY)
#endif

#ifndef UTIL_TASK_CFG_IN_QUEUE_ITEM_SIZE
#define UTIL_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(HIDReport)
#endif

#ifndef UTIL_TASK_CFG_IN_QUEUE_ITEM_COUNT
#define UTIL_TASK_CFG_IN_QUEUE_ITEM_COUNT      10
#endif

#undef  portDISABLE_INTERRUPTS
#undef  portENABLE_INTERRUPTS
#define portDISABLE_INTERRUPTS()               __asm volatile ("cpsid i" : : : "memory");
#define portENABLE_INTERRUPTS()                __asm volatile ("cpsie i" : : : "memory");__asm volatile ("isb 0xF":::"memory");

#define SYS_DEBUGF(level, message)             SYS_DEBUGF3(SYS_DBG_UTIL, level, message)


/**
 * UtilTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xUtilTask_vtbl = {
    UtilTask_vtblHardwareInit,
    UtilTask_vtblOnCreateTask,
    UtilTask_vtblDoEnterPowerMode,
    UtilTask_vtblHandleError,
    UtilTask_vtblForceExecuteStep
};

/**
 *  UtilTask internal structure.
 */
struct _UtilTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * Input queue used by other task to request services.
   */
  QueueHandle_t m_xInQueue;
};


/**
 * The only instance of the task object.
 */
static UtilTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t UtilTaskExecuteStepRun(UtilTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void UtilTaskRun(void *pParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
// Inline function defined inline in the header file UtilTask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *UtilTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xUtilTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t UtilTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  UtilTask *pObj = (UtilTask*)_this;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  UNUSED(_this);

  __HAL_RCC_GPIOE_CLK_ENABLE();

  // Initialize PE0 (USER_BUTTON)
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 15, 0);

  return xRes;
}

sys_error_code_t UtilTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *pObj = (UtilTask*)_this;

  // initialize the JSON library
  HSD_init_JSON(pvPortMalloc, vPortFree);

  // create the input queue
  pObj->m_xInQueue = xQueueCreate(UTIL_TASK_CFG_IN_QUEUE_ITEM_COUNT, UTIL_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInQueue == NULL) {
    xRes = SYS_UTIL_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UTIL_TASK_INIT_ERROR_CODE);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInQueue, "UTIL_Q");
#endif

  *pvTaskCode = UtilTaskRun;
  *pcName = "UTIL";
  *pnStackDepth = UTIL_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = UTIL_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t UtilTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  UtilTask *pObj = (UtilTask*)_this;

  return xRes;
}

sys_error_code_t UtilTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  UtilTask *pObj = (UtilTask*)_this;

  return xRes;
}

sys_error_code_t UtilTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *pObj = (UtilTask*)_this;
  HIDReport xReport = {
      .reportID = HID_REPORT_ID_FORCE_STEP,
  };

  if ((eActivePowerMode == E_POWER_MODE_RUN)) {
    xQueueSendToFront(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100));
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}

// Private function definition
// ***************************

static sys_error_code_t UtilTaskExecuteStepRun(UtilTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HIDReport xReport = {0};

  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (xQueueReceive(_this->m_xInQueue, &xReport, portMAX_DELAY) == pdTRUE) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
    if (xReport.reportID == HID_REPORT_ID_FORCE_STEP) {
      __NOP();
    }
  }

  return xRes;
}

static void UtilTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *_this = (UtilTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("UTIL: start.\r\n"));

  HAL_NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);

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
        xRes = UtilTaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_DATALOG:
      case E_POWER_MODE_AI:
      case E_POWER_MODE_DATALOG_AI:
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


// CubeMX integration
// ******************

void Util_PB_EXTI_Callback(uint16_t nPin) {
  if (nPin == USER_BUTTON_Pin) {
    // generate the system event.

    SysEvent xEvt = {
        .nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_PB, SYS_PM_EVT_PARAM_PRESSED)
    };
    SysPostPowerModeEvent(xEvt);
    // don't check the error code. For the moment we assume that we can loose a USER BUTTON PRessed event.
  }
}
