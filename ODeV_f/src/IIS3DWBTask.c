/**
 ******************************************************************************
 * @file    IIS3DWBTask.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 10, 2020
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

#include "IIS3DWBTask.h"
#include "IIS3DWBTask_vtbl.h"
#include "hid_report_parser.h"
#include "SensorCommands.h"
#include "sensor_db.h"
#include "ISensorEventListener.h"
#include "ISensorEventListener_vtbl.h"
#include <string.h>
#include "sysdebug.h"

// TODO: change XXX with a short id for the task

#ifndef IIS3DWB_TASK_CFG_STACK_DEPTH
#define IIS3DWB_TASK_CFG_STACK_DEPTH              120
#endif

#ifndef IIS3DWB_TASK_CFG_PRIORITY
#define IIS3DWB_TASK_CFG_PRIORITY                 (tskIDLE_PRIORITY)
#endif

#ifndef IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH
#define IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH          20
#endif

#define IIS3DWB_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(HIDReport)

#define IIS3DWB_SPI_CS_Pin                        GPIO_PIN_5
#define IIS3DWB_SPI_CS_GPIO_Port                  GPIOF
#define IIS3DWB_SPI_CS_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()

#define IIS3DWB_INT1_Pin                          GPIO_PIN_14
#define IIS3DWB_INT1_GPIO_Port                    GPIOE
#define IIS3DWB_INT1_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOE_CLK_ENABLE()
#define IIS3DWB_INT1_EXTI_IRQn                    EXTI15_10_IRQn
#define IIS3DWB_INT1_EXTI_LINE                    EXTI_LINE_14

#define IIS3DWB_WRITE_BUFFER_SIZE                 (uint32_t)(32000)

#define SYS_DEBUGF(level, message)                SYS_DEBUGF3(SYS_DBG_IIS3DWB, level, message)


/**
 * IIS3DWBTask virtual table.
 */
static const AManagedTaskEx_vtbl s_xIIS3DWBTask_vtbl = {
    IIS3DWBTask_vtblHardwareInit,
    IIS3DWBTask_vtblOnCreateTask,
    IIS3DWBTask_vtblDoEnterPowerMode,
    IIS3DWBTask_vtblHandleError,
    IIS3DWBTask_vtblForceExecuteStep
};

/**
 *  IIS3DWBTask internal structure.
 */
struct _IIS3DWBTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * SPI IF object used to connect the sensor task to the SPI bus.
   */
  SPIBusIF m_xSensorIF;

  /**
   * Specifies sensor parameters used to initialize the sensor.
   */
  SensorInitParam m_xSensorCommonParam;

  /**
   * Specifies the sensor ID to access the sensor configuration inside the sensor DB.
   */
  uint8_t m_nDBID;

  /**
   * Synchronization object used to send command to the task.
   */
  QueueHandle_t m_xInQueue;

  /**
   * Buffer to store the data read from the sensor
   */
  uint8_t m_pnSensorDataBuff[IIS3DWB_SAMPLES_PER_IT * 7];

  /**
   * ::IEventSrc interface implementation for this class.
   */
  IEventSrc *m_pxEventSrc;
};


/**
 * The only instance of the task object.
 */
static IIS3DWBTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t IIS3DWBTaskExecuteStepRun(IIS3DWBTask *_this);

/**
 * Execute one step of the task control loop while the system is in DATALOG mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t IIS3DWBTaskExecuteStepDatalog(IIS3DWBTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void IIS3DWBTaskRun(void *pParams);

/**
 * Initialize the sensor according to the actual parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t IIS3DWBTaskSensorInit(IIS3DWBTask *_this);

/**
 * Read the data from the sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t IIS3DWBTaskSensorReadData(IIS3DWBTask *_this);

/**
 * Register the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t IIS3DWBTaskSensorRegisterInDB(IIS3DWBTask *_this);

/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t IIS3DWBTaskSensorIsActive(const IIS3DWBTask *_this);

/**
 * SPI CS Pin interrupt callback
 */
void IIS3DWBTask_EXTI_Callback(uint16_t nPin);


// Inline function forward declaration
// ***********************************

/**
 * Private function used to post a report into the front of the task queue.
 * Used to resume the task when the required by the INIT task.
 *
 * @param this [IN] specifies a pointer to the task object.
 * @param pxReport [IN] specifies a report to send.
 * @return SYS_NO_EROR_CODE if success, SYS_APP_TASK_REPORT_LOST_ERROR_CODE.
 */
static inline sys_error_code_t IIS3DWBTaskPostReportToFront(IIS3DWBTask *_this, HIDReport *pxReport);

#if defined (__GNUC__)
// Inline function defined inline in the header file IIS3DWBTask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *IIS3DWBTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xIIS3DWBTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

SPIBusIF *IIS3DWBTaskGetSensorIF(IIS3DWBTask *_this) {
  assert_param(_this);

  return &_this->m_xSensorIF;
}

IEventSrc *IIS3DWBTaskGetEventSrcIF(IIS3DWBTask *_this) {
  assert_param(_this);

  return (IEventSrc*)_this->m_pxEventSrc;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t IIS3DWBTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  IIS3DWBTask *pObj = (IIS3DWBTask*)_this;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  IIS3DWB_SPI_CS_GPIO_CLK_ENABLE();
  IIS3DWB_INT1_GPIO_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IIS3DWB_SPI_CS_GPIO_Port, IIS3DWB_SPI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : IIS3DWB_SPI_CS_Pin */
  GPIO_InitStruct.Pin = IIS3DWB_SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(IIS3DWB_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STTS751_INT_Pin IIS3DWB_INT1_Pin */
  GPIO_InitStruct.Pin =  IIS3DWB_INT1_Pin ;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
//  HAL_EXTI_GetHandle(&iis3dwb_exti, EXTI_LINE_14);
//  HAL_EXTI_RegisterCallback(&iis3dwb_exti,  HAL_EXTI_COMMON_CB_ID, IIS3DWB_Int_Callback);

  return xRes;
}

sys_error_code_t IIS3DWBTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  IIS3DWBTask *pObj = (IIS3DWBTask*)_this;

  // Create task specific sw resources.

  pObj->m_xInQueue = xQueueCreate(IIS3DWB_TASK_CFG_IN_QUEUE_LENGTH, IIS3DWB_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInQueue == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInQueue, "IIS3DWB_Q");
#endif

  xRes = SPIBusIFInit(&pObj->m_xSensorIF, 0, IIS3DWB_SPI_CS_GPIO_Port, IIS3DWB_SPI_CS_Pin);
  if (SYS_IS_ERROR_CODE(xRes)) {
    return xRes;
  }
  // set the SPIBusIF object as handle the IF connector because the SPIBus task
  // will use the handle to access the SPIBusIF.
  SPIBusIFSetHandle(&pObj->m_xSensorIF, &pObj->m_xSensorIF);

  // Initialize the EventSrc interface.
  pObj->m_pxEventSrc = SensorEventSrcAlloc();
  if (pObj->m_pxEventSrc == NULL) {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return xRes;
  }
  IEventSrcInit(pObj->m_pxEventSrc);

  memset(pObj->m_pnSensorDataBuff, 0, sizeof(pObj->m_pnSensorDataBuff));
  pObj->m_nDBID = 0xFF;

  *pvTaskCode = IIS3DWBTaskRun;
  *pcName = "IIS3DWB";
  *pnStackDepth = IIS3DWB_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = IIS3DWB_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t IIS3DWBTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  IIS3DWBTask *pObj = (IIS3DWBTask*)_this;

  if (eNewPowerMode == E_POWER_MODE_DATALOG) {
    if (IIS3DWBTaskSensorIsActive(pObj)) {
      HIDReport xReport = {
          .sensorReport.reportId = HID_REPORT_ID_SENSOR_CMD,
          .sensorReport.nCmdID = SENSOR_CMD_ID_START
      };

      if (xQueueSendToBack(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100)) != pdTRUE) {
        xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: -> DATALOG\r\n"));
  }
  else if (eNewPowerMode == E_POWER_MODE_RUN) {
    //TODO: STF - power down

    HIDReport xReport = {
        .sensorReport.reportId = HID_REPORT_ID_SENSOR_CMD,
        .sensorReport.nCmdID = SENSOR_CMD_ID_STOP
    };

    if (xQueueSendToBack(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100)) != pdTRUE) {
      xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: -> RUN\r\n"));
  }

  return xRes;
}

sys_error_code_t IIS3DWBTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  IIS3DWBTask *pObj = (IIS3DWBTask*)_this;


  return xRes;
}

sys_error_code_t IIS3DWBTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  IIS3DWBTask *pObj = (IIS3DWBTask*)_this;

  struct internalReportFE_t xReport = {
      .reportId = HID_REPORT_ID_FORCE_STEP,
      .nData = 0
  };

  if ((eActivePowerMode == E_POWER_MODE_RUN) || (eActivePowerMode == E_POWER_MODE_DATALOG)) {
    uint8_t nCount = 3;
    while(nCount) {
      xRes = IIS3DWBTaskPostReportToFront(pObj, (HIDReport*)&xReport);
      if (SYS_IS_ERROR_CODE(xRes)) {
        // we are in a power mode switch but we can't resume the task because the in queue is full!
        // try to reset the queue
        nCount--;
        xQueueReset(pObj->m_xInQueue);
      }
      else {
        nCount = 0;
      }
    }
    if (uxQueueMessagesWaiting(pObj->m_xSensorIF.m_xSyncObj)) {
      SPIBusIFNotifyIOComplete(&pObj->m_xSensorIF);
    }
    _this->m_xStatus.nDelayPowerModeSwitch = 0;
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t IIS3DWBTaskExecuteStepRun(IIS3DWBTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HIDReport xReport = {};
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;

  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (pdTRUE == xQueueReceive(_this->m_xInQueue, &xReport, portMAX_DELAY)) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);

    switch (xReport.reportID) {
    case HID_REPORT_ID_FORCE_STEP:
      // do nothing. I need only to resume.
      __NOP();
      break;

    case HID_REPORT_ID_SENSOR_CMD:
      if (xReport.sensorReport.nCmdID == SENSOR_CMD_ID_STOP) {
        // disabe the fifo
        iis3dwb_fifo_xl_batch_set(pxSensorDrv, IIS3DWB_XL_NOT_BATCHED);
        // disable the IRQs
        HAL_NVIC_DisableIRQ(IIS3DWB_INT1_EXTI_IRQn);
      }
      break;

    case HID_REPORT_ID_IIS3DWB:
      // date ready but we are in RUN (no datalog, no AI) so we simple skip the repot.
      break;

    default:
      // unwanted report
      xRes = SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE);

      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB: unexpected report in Run: %i\r\n", xReport.reportID));
    }
  }

  return xRes;
}

static sys_error_code_t IIS3DWBTaskExecuteStepDatalog(IIS3DWBTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HIDReport xReport = {};

  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (pdTRUE == xQueueReceive(_this->m_xInQueue, &xReport, portMAX_DELAY)) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);

    switch (xReport.reportID) {
    case HID_REPORT_ID_FORCE_STEP:
      // do nothing. I need only to resume.
      __NOP();
      break;

    case HID_REPORT_ID_IIS3DWB:
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: new data.\r\n"));
      xRes = IIS3DWBTaskSensorReadData(_this);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        // notify the listeners...
        SensorEvent xEvt;
        SensorEventInit((IEvent*)&xEvt, _this->m_pxEventSrc, _this->m_pnSensorDataBuff, IIS3DWB_SAMPLES_PER_IT * 6, 0, _this->m_nDBID);
        IEventSrcSendEvent(_this->m_pxEventSrc, (IEvent*)&xEvt, NULL);
      }
      break;

    case HID_REPORT_ID_SENSOR_CMD:
      if (xReport.sensorReport.nCmdID == SENSOR_CMD_ID_START) {
        xRes = IIS3DWBTaskSensorInit(_this);
        if (!SYS_IS_ERROR_CODE(xRes)) {
          // enable the IRQs
          HAL_NVIC_EnableIRQ(IIS3DWB_INT1_EXTI_IRQn);
        }
      }
      break;

    default:
      // unwanted report
      xRes = SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE);

      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("IIS3DWB: unexpected report in Datalog: %i\r\n", xReport.reportID));
      break;
    }
  }

  return xRes;
}

static void IIS3DWBTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  IIS3DWBTask *_this = (IIS3DWBTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: start.\r\n"));

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.
  xRes = IIS3DWBTaskSensorRegisterInDB(_this);
  if (SYS_IS_ERROR_CODE(xRes)) {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: unable to register with DB\r\n"));
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
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = IIS3DWBTaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_DATALOG:
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = IIS3DWBTaskExecuteStepDatalog(_this);
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

static inline sys_error_code_t IIS3DWBTaskPostReportToFront(IIS3DWBTask *_this, HIDReport *pxReport) {
  assert_param(_this);
  assert_param(pxReport);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (SYS_IS_CALLED_FROM_ISR()) {
    if (pdTRUE != xQueueSendToFrontFromISR(_this->m_xInQueue, pxReport, NULL)) {
      xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }
  else {
    if (pdTRUE != xQueueSendToFront(_this->m_xInQueue, pxReport, pdMS_TO_TICKS(100))) {
      xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      // this function is private and the caller will ignore this return code.
    }
  }

  return xRes;
}

static sys_error_code_t IIS3DWBTaskSensorInit(IIS3DWBTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;

  uint8_t nReg0 = 0;
  int32_t nRetVal = 0;
  // if this variable need to persist then I move it in the managed task class declaration.
  iis3dwb_pin_int1_route_t pin_int1_route = {0};

  nRetVal = iis3dwb_device_id_get(pxSensorDrv, (uint8_t *)&nReg0);
  if (!nRetVal) {
    SPIBusIFSetWhoAmI(&_this->m_xSensorIF, nReg0);
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("IIS3DWB: sensor - I am 0x%x.\r\n", nReg0));

  // reset the sensor
  nRetVal = iis3dwb_reset_set(pxSensorDrv, 1);
  do {
    iis3dwb_reset_get(pxSensorDrv, &nReg0);
  } while(nReg0);

  //TODO: STF - what is this?
  iis3dwb_read_reg(pxSensorDrv, IIS3DWB_CTRL1_XL, (uint8_t *)&nReg0, 1);
  nReg0 |= 0xA0;
  iis3dwb_write_reg(pxSensorDrv, IIS3DWB_CTRL1_XL, (uint8_t *)&nReg0, 1);

  /*Disable the I2C interfacer and set fifo in continuous / stream mode*/
  iis3dwb_i2c_interface_set(pxSensorDrv, IIS3DWB_I2C_DISABLE);
  iis3dwb_fifo_mode_set(pxSensorDrv, IIS3DWB_STREAM_MODE);

  /*Set watermark*/
  iis3dwb_fifo_watermark_set(pxSensorDrv, IIS3DWB_WTM_LEVEL);
  /*Data Ready pulse mode*/
  iis3dwb_data_ready_mode_set(pxSensorDrv, IIS3DWB_DRDY_PULSED);

  /*Set full scale*/
  if(_this->m_xSensorCommonParam.pfFS[0] < 3.0f)
    iis3dwb_xl_full_scale_set(pxSensorDrv, IIS3DWB_2g);
  else if(_this->m_xSensorCommonParam.pfFS[0] < 5.0f)
    iis3dwb_xl_full_scale_set(pxSensorDrv, IIS3DWB_4g);
  else if(_this->m_xSensorCommonParam.pfFS[0] < 9.0f)
    iis3dwb_xl_full_scale_set(pxSensorDrv, IIS3DWB_8g);
  else if(_this->m_xSensorCommonParam.pfFS[0] < 17.0f)
    iis3dwb_xl_full_scale_set(pxSensorDrv, IIS3DWB_16g);

  /*Set 2nd stage filter*/
  iis3dwb_xl_hp_path_on_out_set(pxSensorDrv,IIS3DWB_LP_5kHz);
  /* FIFO_WTM_IA routing on pin INT1 */
  iis3dwb_pin_int1_route_get(pxSensorDrv, &pin_int1_route);
  *(uint8_t*)&(pin_int1_route.int1_ctrl) = 0;
  *(uint8_t*)&(pin_int1_route.md1_cfg) = 0;
  pin_int1_route.int1_ctrl.int1_fifo_th = PROPERTY_ENABLE;
  iis3dwb_pin_int1_route_set(pxSensorDrv, &pin_int1_route);

  /*Enable writing to FIFO*/
  iis3dwb_fifo_xl_batch_set(pxSensorDrv, IIS3DWB_XL_BATCHED_AT_26k7Hz);

  return xRes;
}

static sys_error_code_t IIS3DWBTaskSensorReadData(IIS3DWBTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;

  uint8_t nReg0 = 0;
  uint8_t nReg1 = 0;
//  int32_t nRetVal = 0;
  uint16_t fifo_level = 0;

  /* Check FIFO_WTM_IA anf fifo level. We do not use PID in order to avoid reading one register twice */
  iis3dwb_read_reg(pxSensorDrv, IIS3DWB_FIFO_STATUS1, &nReg0, 1);
  iis3dwb_read_reg(pxSensorDrv, IIS3DWB_FIFO_STATUS2, &nReg1, 1);
  fifo_level = ((nReg1 & 0x03) << 8) + nReg0;

  //TODO: STF.Debug - need to add the timestamp
  if((nReg1) & 0x80  && (fifo_level >= IIS3DWB_SAMPLES_PER_IT) ) {
//    if(tim_value >= tim_value_old)
//    {
//      period = tim_value - tim_value_old;
//    }
//    else
//    {
//      period = tim_value + (0xFFFFFFFF - tim_value_old);
//    }
//
//    tim_value_old = tim_value;
//    ts_iis3dwb +=  period;

    iis3dwb_read_reg(pxSensorDrv, IIS3DWB_FIFO_DATA_OUT_TAG, (uint8_t *)_this->m_pnSensorDataBuff, IIS3DWB_SAMPLES_PER_IT * 7);

    /* Arrange Data */
    int16_t * p16src = (int16_t *)_this->m_pnSensorDataBuff;
    int16_t * p16dest = (int16_t *)_this->m_pnSensorDataBuff;
    for (uint16_t i=0; i < IIS3DWB_SAMPLES_PER_IT; i++) {
      p16src = (int16_t *)&((uint8_t *)(p16src))[1];
      *p16dest++ = *p16src++;
      *p16dest++ = *p16src++;
      *p16dest++ = *p16src++;
    }
//    IIS3DWB_Data_Ready((uint8_t *)iis3dwb_mem, IIS3DWB_SAMPLES_PER_IT * 6, (double)ts_iis3dwb/(double)SystemCoreClock);
  }

  return xRes;
}

static sys_error_code_t IIS3DWBTaskSensorRegisterInDB(IIS3DWBTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  COM_Device_t *pxSDB = SDB_GetIstance();

  int32_t nID = SDB_AddSensor(pxSDB);
  if (nID != -1) {
    xRes = SYS_NO_ERROR_CODE;
    _this->m_nDBID = nID;

//    COM_DeviceDescriptor_t *tempDeviceDescriptor = SDB_GetDeviceDescriptor(pxSDB);
//    get_unique_id(tempDeviceDescriptor->serialNumber);
//    strcpy(tempDeviceDescriptor->alias, "STWIN_001");

      /* IIS3DWB */

    COM_Sensor_t *pxSensor = SDB_GetSensor(pxSDB, _this->m_nDBID);

    /* SENSOR DESCRIPTOR */
    strcpy(pxSensor->sensorDescriptor.name, "IIS3DWB");
    pxSensor->sensorDescriptor.dataType = DATA_TYPE_INT16;
    pxSensor->sensorDescriptor.ODR[0] = 26667.0f;
    pxSensor->sensorDescriptor.ODR[1] = COM_END_OF_LIST_FLOAT;  /* Terminate list */
    pxSensor->sensorDescriptor.samplesPerTimestamp[0] = 0;
    pxSensor->sensorDescriptor.samplesPerTimestamp[1] = 1000;
    pxSensor->sensorDescriptor.nSubSensors = 1;

    /* SENSOR STATUS */
    pxSensor->sensorStatus.ODR = 26667.0f;
    pxSensor->sensorStatus.measuredODR = 0.0f;
    pxSensor->sensorStatus.initialOffset = 0.0f;
    pxSensor->sensorStatus.samplesPerTimestamp = 1000;
    pxSensor->sensorStatus.isActive = 1;
    pxSensor->sensorStatus.usbDataPacketSize = 3000;
    pxSensor->sensorStatus.sdWriteBufferSize = IIS3DWB_WRITE_BUFFER_SIZE;
    pxSensor->sensorStatus.comChannelNumber = -1;

    /* SUBSENSOR 0 DESCRIPTOR */
    pxSensor->sensorDescriptor.subSensorDescriptor[0].id = 0;
    pxSensor->sensorDescriptor.subSensorDescriptor[0].sensorType = COM_TYPE_ACC;
    pxSensor->sensorDescriptor.subSensorDescriptor[0].dataPerSample = 3;
    strcpy(pxSensor->sensorDescriptor.subSensorDescriptor[0].unit, "g");
    pxSensor->sensorDescriptor.subSensorDescriptor[0].FS[0] = 2.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[0].FS[1] = 4.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[0].FS[2] = 8.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[0].FS[3] = 16.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[0].FS[4] = COM_END_OF_LIST_FLOAT;

    /* SUBSENSOR 0 STATUS */
    pxSensor->sensorStatus.subSensorStatus[0].FS = 16.0f;
    pxSensor->sensorStatus.subSensorStatus[0].isActive = 1;
    pxSensor->sensorStatus.subSensorStatus[0].sensitivity = 0.061f *  pxSensor->sensorStatus.subSensorStatus[0].FS/2;

    _this->m_xSensorCommonParam.fODR = pxSensor->sensorStatus.ODR;
    _this->m_xSensorCommonParam.pfFS[0] = pxSensor->sensorStatus.subSensorStatus[0].FS;
    _this->m_xSensorCommonParam.pbSubSensorActive[0] = pxSensor->sensorStatus.subSensorStatus[0].isActive;

    //TODO: STF.Porting - what to do with this?
    // it seems used only for LOG_ERROR
//    maxWriteTimeSensor[iis3dwb_com_id] = 1000 * WRITE_BUFFER_SIZE_IIS3DWB / (uint32_t)(IIS3DWB_Init_Param.ODR * 6);
  }

  return xRes;
}

static boolean_t IIS3DWBTaskSensorIsActive(const IIS3DWBTask *_this) {
  assert_param(_this);
  boolean_t bRes = FALSE;

  COM_Sensor_t *pxSensor = SDB_GetSensor(SDB_GetIstance(), _this->m_nDBID);
  if (pxSensor != NULL) {
    // check first the sensor status
    if (pxSensor->sensorStatus.isActive) {
      // check if at least one sub sensor is active
      for (uint8_t i=0; i<pxSensor->sensorDescriptor.nSubSensors; ++i) {
        if (pxSensor->sensorStatus.subSensorStatus[i].isActive) {
          bRes = TRUE;
          break;
        }
      }
    }
  }

  return bRes;
}


// CubeMX integration
// ******************

void IIS3DWBTask_EXTI_Callback(uint16_t nPin) {
  struct ism330dhcxReport_t xReport = {
      .reportId = HID_REPORT_ID_IIS3DWB,
      .bDataReady = 1
  };

  if (s_xTaskObj.m_xInQueue != NULL && !AMTIsPowerModeSwitchPending((AManagedTask*)&s_xTaskObj)) {
    if (pdTRUE != xQueueSendToBackFromISR(s_xTaskObj.m_xInQueue, &xReport, NULL)) {
      // unable to send the report. Signal the error
      sys_error_handler();
    }
  }
}
