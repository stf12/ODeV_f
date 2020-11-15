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
#include "UtilityDriver.h"
#include "UtilityDriver_vtbl.h"
#include "HSD_json.h"
#include "hid_report_parser.h"
#include "mx.h"
#include "sysmem.h"
#include "sysdebug.h"

// TODO: cange XXX with a short id for the task

#ifndef UTIL_TASK_CFG_STACK_DEPTH
#define UTIL_TASK_CFG_STACK_DEPTH              120
#endif

#ifndef UTIL_TASK_CFG_PRIORITY
#define UTIL_TASK_CFG_PRIORITY                 (TX_MAX_PRIORITIES - 1)
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
   * Specifies the driver used by the task. It is an instance of ::UtilityDriver
   */
  IDriver *m_pxDriver;

  /**
   * Input queue used by other task to request services.
   */
  TX_QUEUE m_xInQueue;
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
 * @param nParams .
 */
static void UtilTaskRun(ULONG nParams);


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

uint32_t UtilTaskGetTimeStamp(UtilTask *_this) {
  assert_param(_this);

  return UtilityDrvGetTimeStamp((UtilityDriver*)_this->m_pxDriver);
}

uint32_t UtilGetTimeStamp() {
  return UtilityDrvGetTimeStamp((UtilityDriver*)s_xTaskObj.m_pxDriver);
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t UtilTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *pObj = (UtilTask*)_this;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  pObj->m_pxDriver = UtilityDriverAlloc();
  if (pObj->m_pxDriver == NULL) {
    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("UTIL task: unable to alloc driver object.\r\n"));
    xRes = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
  }
  else {
    xRes = IDrvInit(pObj->m_pxDriver, NULL);
    if (SYS_IS_ERROR_CODE(xRes)) {
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("UTIL task: error during driver initialization\r\n"));
    }
  }

  //TODO: STF -  Need to move into UtilityDriver
  __HAL_RCC_GPIOE_CLK_ENABLE();

  // Initialize PE0 (USER_BUTTON)
  GPIO_InitStruct.Pin = USER_BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_BUTTON_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(USER_BUTTON_EXTI_IRQn, 15, 0);

  return xRes;
}

sys_error_code_t UtilTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
    VOID **pvStackStart, ULONG *pnStackSize,
    UINT *pnPriority, UINT *pnPreemptThreshold,
    ULONG *pnTimeSlice, ULONG *pnAutoStart,
    ULONG *pnParams)
{
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *pObj = (UtilTask*)_this;

  // initialize the JSON library
  HSD_init_JSON(SysAlloc, SysFree);

  // create the input queue
  uint16_t nItemSize = UTIL_TASK_CFG_IN_QUEUE_ITEM_SIZE;
  VOID *pvQueueItemsBuff = SysAlloc(UTIL_TASK_CFG_IN_QUEUE_ITEM_COUNT * nItemSize);
  if (pvQueueItemsBuff == NULL) {
    xRes = SYS_UTIL_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UTIL_TASK_INIT_ERROR_CODE);
    return xRes;
  }

  if (TX_SUCCESS != tx_queue_create(&pObj->m_xInQueue, "UTIL_Q", nItemSize / 4, pvQueueItemsBuff, UTIL_TASK_CFG_IN_QUEUE_ITEM_COUNT * nItemSize)) {
    xRes = SYS_SD_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_INIT_ERROR_CODE);
    return xRes;
  }

  *pvTaskCode = UtilTaskRun;
  *pcName = "UTIL";
  *pvStackStart = NULL; // allocate the task stack in the system memory pool
  *pnStackSize = UTIL_TASK_CFG_STACK_DEPTH;
  *pnParams = (ULONG)_this;
  *pnPriority = UTIL_TASK_CFG_PRIORITY;
  *pnPreemptThreshold = UTIL_TASK_CFG_PRIORITY;
  *pnTimeSlice = TX_NO_TIME_SLICE;
  *pnAutoStart = TX_AUTO_START;

  return xRes;
}

sys_error_code_t UtilTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *pObj = (UtilTask*)_this;

  // propagate the call to the driver object.
  IDrvDoEnterPowerMode(pObj->m_pxDriver, eActivePowerMode, eNewPowerMode);

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
    tx_queue_front_send(&pObj->m_xInQueue, &xReport, AMT_MS_TO_TICKS(100));
  }
  else {
    tx_thread_resume(&_this->m_xThaskHandle);
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
  if (tx_queue_receive(&_this->m_xInQueue, &xReport, TX_WAIT_FOREVER) == TX_SUCCESS) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
    if (xReport.reportID == HID_REPORT_ID_FORCE_STEP) {
      __NOP();
    }
  }

  return xRes;
}

static void UtilTaskRun(ULONG nParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UtilTask *_this = (UtilTask*)nParams;
  UINT nPosture = TX_INT_ENABLE;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("UTIL: start.\r\n"));

  HAL_NVIC_EnableIRQ(USER_BUTTON_EXTI_IRQn);

  for (;;) {

    // check if there is a pending power mode switch request
    if (_this->super.m_xStatus.nPowerModeSwitchPending == 1) {
      // clear the power mode switch delay because the task is ready to switch.
      nPosture = tx_interrupt_control(TX_INT_DISABLE);
        _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
      tx_interrupt_control(nPosture);
      tx_thread_suspend(&_this->super.m_xThaskHandle);
    }
    else {
      switch (AMTGetSystemPowerMode()) {
      case E_POWER_MODE_RUN:
        nPosture = tx_interrupt_control(TX_INT_DISABLE);
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        tx_interrupt_control(nPosture);
        xRes = UtilTaskExecuteStepRun(_this);
        nPosture = tx_interrupt_control(TX_INT_DISABLE);
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        tx_interrupt_control(nPosture);;
        break;

      case E_POWER_MODE_DATALOG:
      case E_POWER_MODE_AI:
      case E_POWER_MODE_DATALOG_AI:
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
