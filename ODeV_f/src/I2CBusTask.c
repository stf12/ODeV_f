/**
 ******************************************************************************
 * @file    I2CBusTask.c
 * @author  Stefano Oliveri
 * @version 1.0.0
 * @date    Aug 29, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 Stefano Oliveri</center></h2>
 *
 *
 ******************************************************************************
 */

#include "I2CBusTask.h"
#include "I2CBusTask_vtbl.h"
#include "I2CMasterDriver.h"
#include "I2CMasterDriver_vtbl.h"
#include "hid_report_parser.h"
#include "sysdebug.h"

#ifndef I2CBUS_TASK_CFG_STACK_DEPTH
#define I2CBUS_TASK_CFG_STACK_DEPTH        120
#endif

#ifndef I2CBUS_TASK_CFG_PRIORITY
#define I2CBUS_TASK_CFG_PRIORITY           (tskIDLE_PRIORITY)
#endif

#ifndef I2CBUS_TASK_CFG_INQUEUE_LENGTH
#define I2CBUS_TASK_CFG_INQUEUE_LENGTH     20
#endif

#define I2CBUS_OP_WAIT_MS                  50

#define SYS_DEBUGF(level, message)         SYS_DEBUGF3(SYS_DBG_I2CBUS, level, message)


/**
 * I2CBusTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xI2CBusTask_vtbl = {
    I2CBusTask_vtblHardwareInit,
    I2CBusTask_vtblOnCreateTask,
    I2CBusTask_vtblDoEnterPowerMode,
    I2CBusTask_vtblHandleError,
    I2CBusTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static I2CBusTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t I2CBusTaskExecuteStepRun(I2CBusTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void I2CBusTaskRun(void *pParams);

static int32_t I2CBusTaskWrite(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);
static int32_t I2CBusTaskRead(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
#endif


// Public API definition
// *********************

AManagedTaskEx *I2CBusTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xI2CBusTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

sys_error_code_t I2CBusTaskConnectDevice(I2CBusTask *_this, I2CBusIF *pxBusIF) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL) {
    pxBusIF->m_xConnector.pfReadReg = I2CBusTaskRead;
    pxBusIF->m_xConnector.pfWriteReg = I2CBusTaskWrite;
    _this->m_nConnectedDevices++;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I2CBUS: connected device: %d\r\n", _this->m_nConnectedDevices));
  }
  else {
    xRes = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return xRes;
}

sys_error_code_t I2CBusTaskDisconnectDevice(I2CBusTask *_this, I2CBusIF *pxBusIF) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (pxBusIF != NULL) {
    pxBusIF->m_xConnector.pfReadReg = I2CBusNullRW;
    pxBusIF->m_xConnector.pfWriteReg = I2CBusNullRW;
    _this->m_nConnectedDevices--;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I2CBUS: connected device: %d\r\n", _this->m_nConnectedDevices));
  }
  else {
    xRes = SYS_INVALID_PARAMETER_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
  }

  return xRes;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t I2CBusTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  I2CBusTask *pObj = (I2CBusTask*)_this;

  pObj->m_pxDriver = I2CMasterDriverAlloc();
  if (pObj->m_pxDriver == NULL) {
    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("I2CBus task: unable to alloc driver object.\r\n"));
    xRes = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
  }
  else {
    xRes = IDrvInit((IDriver*)pObj->m_pxDriver, NULL);
    if (SYS_IS_ERROR_CODE(xRes)) {
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("I2CBus task: error during driver initialization\r\n"));
    }
  }

  return xRes;
}

sys_error_code_t I2CBusTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  I2CBusTask *pObj = (I2CBusTask*)_this;

  // initialize the software resources.
  pObj->m_xInQueue = xQueueCreate(I2CBUS_TASK_CFG_INQUEUE_LENGTH, HidReportGetSize(HID_REPORT_ID_I2C_BUS_READ));
  if (pObj->m_xInQueue != NULL) {

#ifdef DEBUG
    vQueueAddToRegistry(pObj->m_xInQueue, "I2CBUS_Q");
#endif

    pObj->m_nConnectedDevices = 0;

    *pvTaskCode = I2CBusTaskRun;
    *pcName = "I2CBUS";
    *pnStackDepth = I2CBUS_TASK_CFG_STACK_DEPTH;
    *pParams = _this;
    *pxPriority = I2CBUS_TASK_CFG_PRIORITY;
  }
  else {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  return xRes;
}

sys_error_code_t I2CBusTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CBusTask *pObj = (I2CBusTask*)_this;

  return xRes;
}

sys_error_code_t I2CBusTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CBusTask *pObj = (I2CBusTask*)_this;

  return xRes;
}

sys_error_code_t I2CBusTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  I2CBusTask *pObj = (I2CBusTask*)_this;

  // to resume the task we send a fake empty message.
  HIDReport xReport = {
      .reportID = HID_REPORT_ID_FORCE_STEP
  };
  if ((eActivePowerMode == E_POWER_MODE_RUN) || (eActivePowerMode == E_POWER_MODE_DATALOG)) {
    if (pdTRUE != xQueueSendToFront(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100))) {

      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I2CBUS: unable to resume the task.\r\n"));

      xRes = SYS_I2CBUS_TASK_RESUME_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I2CBUS_TASK_RESUME_ERROR_CODE);
    }
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t I2CBusTaskExecuteStepRun(I2CBusTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  struct i2cIOReport_t xMsg = {0};
  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (pdTRUE == xQueueReceive(_this->m_xInQueue, &xMsg, portMAX_DELAY)) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
    switch (xMsg.reportId) {
    case HID_REPORT_ID_FORCE_STEP:
      __NOP();
      // do nothing. I need only to resume the task.
      break;

    case HID_REPORT_ID_I2C_BUS_READ:

      //TODO: STF.Porting - how do we handle this scenario?
//      if (msg->sensorHandler->WhoAmI == 0xBCU && msg->readSize > 1)
//      {
//        autoInc = 0x80;
//      }

      I2CMasterDriverSetDeviceAddr((I2CMasterDriver*)_this->m_pxDriver, xMsg.pxSensor->m_nAddress);
      xRes = IIODrvRead(_this->m_pxDriver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        xRes = I2CBusIFNotifyIOComplete(xMsg.pxSensor);
      }
      break;

    case HID_REPORT_ID_I2C_BUS_WRITE:
      I2CMasterDriverSetDeviceAddr((I2CMasterDriver*)_this->m_pxDriver, xMsg.pxSensor->m_nAddress);
      xRes = IIODrvWrite(_this->m_pxDriver, xMsg.pnData, xMsg.nDataSize, xMsg.nRegAddr);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        xRes = I2CBusIFNotifyIOComplete(xMsg.pxSensor);
      }
      break;

    default:
      //TODO: STF -  need to notify the error
      break;
    }
  }

  return xRes;
}

static void I2CBusTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  I2CBusTask *_this = (I2CBusTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I2C: start.\r\n"));

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I2CBUS: start the driver.\r\n"));
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
        xRes = I2CBusTaskExecuteStepRun(_this);
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

static int32_t I2CBusTaskWrite(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize) {
  assert_param(pxSensor);
  I2CBusIF *pxI2CSensor = (I2CBusIF *)pxSensor;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  struct i2cIOReport_t xMsg = {
      .reportId = HID_REPORT_ID_I2C_BUS_WRITE,
      .pxSensor = pxI2CSensor,
      .nRegAddr = nRegAddr,
      .pnData = pnData,
      .nDataSize = nSize
  };

  if (s_xTaskObj.m_xInQueue != NULL) {
    if (SYS_IS_CALLED_FROM_ISR()) {
      // we cannot read and write in the I2C BUS from an ISR. Notify the error
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I2CBUS_TASK_IO_ERROR_CODE);
      xRes = SYS_I2CBUS_TASK_IO_ERROR_CODE;
    }
    else {
      if (pdTRUE != xQueueSendToBack(s_xTaskObj.m_xInQueue, &xMsg, I2CBUS_OP_WAIT_MS)) {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I2CBUS_TASK_IO_ERROR_CODE);
        xRes = SYS_I2CBUS_TASK_IO_ERROR_CODE;
      }
    }
  }

  if (!SYS_IS_ERROR_CODE(xRes)) {
    // suspend the sensor task.
    xRes = I2CBusIFWaitIOComplete(pxI2CSensor);
  }

  return xRes;
}

static int32_t I2CBusTaskRead(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize) {
  assert_param(pxSensor);
  I2CBusIF *pxI2CSensor = (I2CBusIF *)pxSensor;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  struct i2cIOReport_t xMsg = {
      .reportId = HID_REPORT_ID_I2C_BUS_READ,
      .pxSensor = pxI2CSensor,
      .nRegAddr = nRegAddr,
      .pnData = pnData,
      .nDataSize = nSize
  };

  if (s_xTaskObj.m_xInQueue != NULL) {
    if (SYS_IS_CALLED_FROM_ISR()) {
      // we cannot read and write in the I2C BUS from an ISR. Notify the error
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I2CBUS_TASK_IO_ERROR_CODE);
      xRes = SYS_I2CBUS_TASK_IO_ERROR_CODE;
    }
    else {
      if (pdTRUE != xQueueSendToBack(s_xTaskObj.m_xInQueue, &xMsg, I2CBUS_OP_WAIT_MS)) {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_I2CBUS_TASK_IO_ERROR_CODE);
        xRes = SYS_I2CBUS_TASK_IO_ERROR_CODE;
      }
    }
  }

  if (!SYS_IS_ERROR_CODE(xRes)) {
    xRes = I2CBusIFWaitIOComplete(pxI2CSensor);
  }

  return xRes;
}
