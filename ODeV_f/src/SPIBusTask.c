/**
 ******************************************************************************
 * @file    SPIBusTask.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 3, 2020
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

#include "SPIBusTask.h"
#include "SPIBusTask_vtbl.h"
#include "SPIMasterDriver.h"
#include "SPIMasterDriver_vtbl.h"
#include "hid_report_parser.h"
#include "sysdebug.h"


#ifndef SPIBUS_TASK_CFG_STACK_DEPTH
#define SPIBUS_TASK_CFG_STACK_DEPTH        120
#endif

#ifndef SPIBUS_TASK_CFG_PRIORITY
#define SPIBUS_TASK_CFG_PRIORITY           (tskIDLE_PRIORITY+3)
#endif

#ifndef SPIBUS_TASK_CFG_INQUEUE_LENGTH
#define SPIBUS_TASK_CFG_INQUEUE_LENGTH     20
#endif

#define SPIBUS_OP_WAIT_MS                  50

#define SYS_DEBUGF(level, message)         SYS_DEBUGF3(SYS_DBG_SPIBUS, level, message)


/**
 * SPIBusTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xSPIBusTask_vtbl = {
    SPIBusTask_vtblHardwareInit,
    SPIBusTask_vtblOnCreateTask,
    SPIBusTask_vtblDoEnterPowerMode,
    SPIBusTask_vtblHandleError,
    SPIBusTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static SPIBusTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t SPIBusTaskExecuteStepRun(SPIBusTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void SPIBusTaskRun(void *pParams);

static int32_t SPIBusTaskWrite(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);
static int32_t SPIBusTaskRead(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
// Inline function defined inline in the header file SPIBusTask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *SPIBusTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xSPIBusTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

sys_error_code_t SPIBusTaskConnectDevice(SPIBusTask *_this, SPIBusIF *pxBusIF) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL) {
    pxBusIF->m_xConnector.pfReadReg = SPIBusTaskRead;
    pxBusIF->m_xConnector.pfWriteReg = SPIBusTaskWrite;
    _this->m_nConnectedDevices++;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: connected device: %d\r\n", _this->m_nConnectedDevices));
  }
  else {
    xRes = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return xRes;
}

sys_error_code_t SPIBusTaskDisconnectDevice(SPIBusTask *_this, SPIBusIF *pxBusIF) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL) {
    pxBusIF->m_xConnector.pfReadReg = SPIBusNullRW;
    pxBusIF->m_xConnector.pfWriteReg = SPIBusNullRW;
    _this->m_nConnectedDevices--;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: connected device: %d\r\n", _this->m_nConnectedDevices));
  }
  else {
    xRes = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return xRes;
}


// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t SPIBusTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIBusTask *pObj = (SPIBusTask*)_this;

  pObj->m_pxDriver = SPIMasterDriverAlloc();
  if (pObj->m_pxDriver == NULL) {
    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("SPIBus task: unable to alloc driver object.\r\n"));
    xRes = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
  }
  else {
    xRes = IDrvInit((IDriver*)pObj->m_pxDriver, NULL);
    if (SYS_IS_ERROR_CODE(xRes)) {
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("SPIBus task: error during driver initialization\r\n"));
    }
  }

  return xRes;
}

sys_error_code_t SPIBusTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIBusTask *pObj = (SPIBusTask*)_this;

  // initialize the software resources.
  pObj->m_xInQueue = xQueueCreate(SPIBUS_TASK_CFG_INQUEUE_LENGTH, HidReportGetSize(HID_REPORT_ID_SPI_BUS_READ));
  if (pObj->m_xInQueue != NULL) {

#ifdef DEBUG
    vQueueAddToRegistry(pObj->m_xInQueue, "SPI_Q");
#endif

    pObj->m_nConnectedDevices = 0;

    *pvTaskCode = SPIBusTaskRun;
    *pcName = "SPIBUS";
    *pnStackDepth = SPIBUS_TASK_CFG_STACK_DEPTH;
    *pParams = _this;
    *pxPriority = SPIBUS_TASK_CFG_PRIORITY;
  }
  else {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  return xRes;
}

sys_error_code_t SPIBusTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIBusTask *pObj = (SPIBusTask*)_this;

//  xQueueReset(pObj->m_xInQueue);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: -> %d\r\n", eNewPowerMode));

  return xRes;
}

sys_error_code_t SPIBusTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIBusTask *pObj = (SPIBusTask*)_this;

  return xRes;
}

sys_error_code_t SPIBusTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIBusTask *pObj = (SPIBusTask*)_this;

  // to resume the task we send a fake empty message.
  HIDReport xReport = {
      .reportID = HID_REPORT_ID_FORCE_STEP
  };
  if ((eActivePowerMode == E_POWER_MODE_RUN) || (eActivePowerMode == E_POWER_MODE_DATALOG)) {
    if (pdTRUE != xQueueSendToFront(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100))) {

      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIBUS: unable to resume the task.\r\n"));

      xRes = SYS_SPIBUS_TASK_RESUME_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_RESUME_ERROR_CODE);
    }
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}

// Private function definition
// ***************************

static sys_error_code_t SPIBusTaskExecuteStepRun(SPIBusTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  struct spiIOReport_t xMsg = {0};
  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (pdTRUE == xQueueReceive(_this->m_xInQueue, &xMsg, portMAX_DELAY)) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
    switch (xMsg.reportId) {
    case HID_REPORT_ID_FORCE_STEP:
      __NOP();
      // do nothing. I need only to resume the task.
      break;

    case HID_REPORT_ID_SPI_BUS_READ:
      SPIMasterDriverSelectDevice((SPIMasterDriver*)_this->m_pxDriver, xMsg.pxSensor->m_pxSSPinPort, xMsg.pxSensor->m_nSSPin);
      xRes = IIODrvRead(_this->m_pxDriver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
      SPIMasterDriverDeselectDevice((SPIMasterDriver*)_this->m_pxDriver, xMsg.pxSensor->m_pxSSPinPort, xMsg.pxSensor->m_nSSPin);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        xRes = SPIBusIFNotifyIOComplete(xMsg.pxSensor);
      }
      break;

    case HID_REPORT_ID_SPI_BUS_WRITE:
      SPIMasterDriverSelectDevice((SPIMasterDriver*)_this->m_pxDriver, xMsg.pxSensor->m_pxSSPinPort, xMsg.pxSensor->m_nSSPin);
      xRes = IIODrvWrite(_this->m_pxDriver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
      SPIMasterDriverDeselectDevice((SPIMasterDriver*)_this->m_pxDriver, xMsg.pxSensor->m_pxSSPinPort, xMsg.pxSensor->m_nSSPin);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        xRes = SPIBusIFNotifyIOComplete(xMsg.pxSensor);
      }
      break;

    default:
      //TODO: STF -  need to notify the error
      break;
    }
  }

  return xRes;
}

static void SPIBusTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIBusTask *_this = (SPIBusTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: start.\r\n"));

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIBUS: start the driver.\r\n"));
  xRes = IDrvStart((IDriver*)_this->m_pxDriver);
  if (SYS_IS_ERROR_CODE(xRes)) {
    sys_error_handler();
  }


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
      case E_POWER_MODE_DATALOG:
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = SPIBusTaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

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

static int32_t SPIBusTaskWrite(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize) {
  assert_param(pxSensor);
  SPIBusIF *pxSPISensor = (SPIBusIF *)pxSensor;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  uint8_t nAutoInc = 0x00;
  struct spiIOReport_t xMsg = {
      .reportId = HID_REPORT_ID_SPI_BUS_WRITE,
      .pxSensor = pxSPISensor,
      .nRegAddr = nRegAddr | nAutoInc,
      .pnData = pnData,
      .nDataSize = nSize
  };

  if (s_xTaskObj.m_xInQueue != NULL) {
    if (SYS_IS_CALLED_FROM_ISR()) {
      // we cannot read and write in the SPI BUS from an ISR. Notify the error
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
      xRes = SYS_SPIBUS_TASK_IO_ERROR_CODE;
    }
    else {
      if (pdTRUE != xQueueSendToBack(s_xTaskObj.m_xInQueue, &xMsg, SPIBUS_OP_WAIT_MS)) {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
        xRes = SYS_SPIBUS_TASK_IO_ERROR_CODE;
      }
    }
  }

  if (!SYS_IS_ERROR_CODE(xRes)) {
    // suspend the sensor task.
    xRes = SPIBusIFWaitIOComplete(pxSPISensor);
  }

  return xRes;
}

static int32_t SPIBusTaskRead(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize) {
  assert_param(pxSensor);
  SPIBusIF *pxSPISensor = (SPIBusIF *)pxSensor;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  uint8_t nAutoInc = 0x00;
  struct spiIOReport_t xMsg = {
      .reportId = HID_REPORT_ID_SPI_BUS_READ,
      .pxSensor = pxSPISensor,
      .nRegAddr = nRegAddr | 0x80 | nAutoInc,
      .pnData = pnData,
      .nDataSize = nSize
  };

  if (s_xTaskObj.m_xInQueue != NULL) {
    if (SYS_IS_CALLED_FROM_ISR()) {
      // we cannot read and write in the SPI BUS from an ISR. Notify the error
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
      xRes = SYS_SPIBUS_TASK_IO_ERROR_CODE;
    }
    else {
      if (pdTRUE != xQueueSendToBack(s_xTaskObj.m_xInQueue, &xMsg, SPIBUS_OP_WAIT_MS)) {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SPIBUS_TASK_IO_ERROR_CODE);
        xRes = SYS_SPIBUS_TASK_IO_ERROR_CODE;
      }
    }
  }

  if (!SYS_IS_ERROR_CODE(xRes)) {
    xRes = SPIBusIFWaitIOComplete(pxSPISensor);
  }

  return xRes;
}
