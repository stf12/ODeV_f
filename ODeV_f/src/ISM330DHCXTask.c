/**
 ******************************************************************************
 * @file    ISM330DHCXTask.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 7, 2020
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

#include "ISM330DHCXTask.h"
#include "ISM330DHCXTask_vtbl.h"
#include "hid_report_parser.h"
#include "SensorCommands.h"
#include "sensor_db.h"
#include "ISensorEventListener.h"
#include "ISensorEventListener_vtbl.h"
#include <string.h>
#include "sysdebug.h"

// TODO: change XXX with a short id for the task

#ifndef ISM330DHCX_TASK_CFG_STACK_DEPTH
#define ISM330DHCX_TASK_CFG_STACK_DEPTH              120
#endif

#ifndef ISM330DHCX_TASK_CFG_PRIORITY
#define ISM330DHCX_TASK_CFG_PRIORITY                 (tskIDLE_PRIORITY)
#endif

#ifndef ISM330DHCX_TASK_CFG_IN_QUEUE_LENGTH
#define ISM330DHCX_TASK_CFG_IN_QUEUE_LENGTH          10
#endif

#define ISM330DHCX_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(HIDReport)

#define ISM330DHCX_SPI_CS_Pin                        GPIO_PIN_13
#define ISM330DHCX_SPI_CS_GPIO_Port                  GPIOF
#define ISM330DHCX_SPI_CS_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOF_CLK_ENABLE()

#define ISM330DHCX_INT1_Pin                          GPIO_PIN_8
#define ISM330DHCX_INT1_GPIO_Port                    GPIOE
#define ISM330DHCX_INT1_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOE_CLK_ENABLE()
#define ISM330DHCX_INT1_EXTI_IRQn                    EXTI9_5_IRQn
#define ISM330DHCX_INT1_EXTI_LINE                    EXTI_LINE_8

#define ISM330DHCX_WRITE_BUFFER_SIZE                 (uint32_t)(16000)

#define SYS_DEBUGF(level, message)                   SYS_DEBUGF3(SYS_DBG_ISM330DHCX, level, message)


/**
 * ISM330DHCXTask virtual table.
 */
static const AManagedTaskEx_vtbl s_xISM330DHCXTask_vtbl = {
    ISM330DHCXTask_vtblHardwareInit,
    ISM330DHCXTask_vtblOnCreateTask,
    ISM330DHCXTask_vtblDoEnterPowerMode,
    ISM330DHCXTask_vtblHandleError,
    ISM330DHCXTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static ISM330DHCXTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t ISM330DHCXTaskExecuteStepRun(ISM330DHCXTask *_this);

/**
 * Execute one step of the task control loop while the system is in DATALOG mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t ISM330DHCXTaskExecuteStepDatalog(ISM330DHCXTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void ISM330DHCXTaskRun(void *pParams);

/**
 * Initialize the sensor according to the actual parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t ISM330DHCXTaskSensorInit(ISM330DHCXTask *_this);

/**
 * Read the data from the sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t ISM330DHCXTaskSensorReadData(ISM330DHCXTask *_this);

/**
 * Registerd the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t ISM330DHCXTaskSensorRegisterInDB(ISM330DHCXTask *_this);

/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t ISM330DHCXTaskSensorIsActive(const ISM330DHCXTask *_this);

/**
 * SPI CS Pin interrupt callback
 */
void ISM330DHCXTask_EXTI_Callback(uint16_t nPin);


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
static inline sys_error_code_t ISM330DHCXTaskPostReportToFront(ISM330DHCXTask *_this, HIDReport *pxReport);


#if defined (__GNUC__)
// Inline function defined inline in the header file ISM330DHCXTask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *ISM330DHCXTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xISM330DHCXTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

SPIBusIF *ISM330DHCXTaskGetSensorIF(ISM330DHCXTask *_this) {
  assert_param(_this);

  return &_this->m_xSensorIF;
}

IEventSrc *ISM330DHCXTaskGetEventSrcIF(ISM330DHCXTask *_this) {
  assert_param(_this);

  return _this->m_pxEventSrc;
}


// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t ISM330DHCXTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  ISM330DHCXTask *pObj = (ISM330DHCXTask*)_this;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  ISM330DHCX_SPI_CS_GPIO_CLK_ENABLE();
  ISM330DHCX_INT1_GPIO_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ISM330DHCX_SPI_CS_GPIO_Port, ISM330DHCX_SPI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : IIS3DWB_SPI_CS_Pin */
  GPIO_InitStruct.Pin = ISM330DHCX_SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ISM330DHCX_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STTS751_INT_Pin IIS3DWB_INT1_Pin */
  GPIO_InitStruct.Pin =  ISM330DHCX_INT1_Pin ;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ISM330DHCX_INT1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(ISM330DHCX_INT1_EXTI_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(ISM330DHCX_INT1_EXTI_IRQn); //TODO: STF - I want to listen the IRQ only after the initialization when the user tasks run
//  HAL_EXTI_GetHandle(&g_ism330dhcx_exti, ISM330DHCX_INT1_EXTI_LINE);
//  HAL_EXTI_RegisterCallback(&g_ism330dhcx_exti,  HAL_EXTI_COMMON_CB_ID, ISM330DHCXTask_EXTI_Callback);

  return xRes;
}

sys_error_code_t ISM330DHCXTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  ISM330DHCXTask *pObj = (ISM330DHCXTask*)_this;

  // Create task specific sw resources.

  pObj->m_xInQueue = xQueueCreate(ISM330DHCX_TASK_CFG_IN_QUEUE_LENGTH, ISM330DHCX_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInQueue == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInQueue, "ISM330DHCX_Q");
#endif

  xRes = SPIBusIFInit(&pObj->m_xSensorIF, 0, ISM330DHCX_SPI_CS_GPIO_Port, ISM330DHCX_SPI_CS_Pin);
  if (SYS_IS_ERROR_CODE(xRes)) {
    return xRes;
  }
  // set the SPIBusIF object as handle the IF connector because the SPIBus task
  // will use the handle to access the SPIBusIF.
  SPIBusIFSetHandle(&pObj->m_xSensorIF, &pObj->m_xSensorIF);

  // Initialize the EventSrc interface.
  // take the ownership of the interface.
  pObj->m_pxEventSrc = SensorEventSrcAlloc();
  if (pObj->m_pxEventSrc == NULL) {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return xRes;
  }
  IEventSrcInit(pObj->m_pxEventSrc);

  memset(pObj->m_pnSensorDataBuff, 0, sizeof(pObj->m_pnSensorDataBuff));

  *pvTaskCode = ISM330DHCXTaskRun;
  *pcName = "ISM330DHCX";
  *pnStackDepth = ISM330DHCX_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = ISM330DHCX_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t ISM330DHCXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  ISM330DHCXTask *pObj = (ISM330DHCXTask*)_this;

  if (eNewPowerMode == E_POWER_MODE_DATALOG) {
    //TODO: STF - I have to start the task only if the sensor is enabled!!!
    if (ISM330DHCXTaskSensorIsActive(pObj)) {
      HIDReport xReport = {
          .sensorReport.reportId = HID_REPORT_ID_SENSOR_CMD,
          .sensorReport.nCmdID = SENSOR_CMD_ID_START
      };

      if (xQueueSendToBack(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100)) != pdTRUE) {
        xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
      }
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: -> DATALOG\r\n"));
  }
  else if (eNewPowerMode == E_POWER_MODE_RUN) {
    // TODO: STF - power down

    HIDReport xReport = {
        .sensorReport.reportId = HID_REPORT_ID_SENSOR_CMD,
        .sensorReport.nCmdID = SENSOR_CMD_ID_STOP
    };

    if (xQueueSendToBack(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100)) != pdTRUE) {
      xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: -> RUN\r\n"));
  }


  return xRes;
}

sys_error_code_t ISM330DHCXTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  ISM330DHCXTask *pObj = (ISM330DHCXTask*)_this;


  return xRes;
}

sys_error_code_t ISM330DHCXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  ISM330DHCXTask *pObj = (ISM330DHCXTask*)_this;
  struct internalReportFE_t xReport = {
      .reportId = HID_REPORT_ID_FORCE_STEP,
      .nData = 0
  };

  if ((eActivePowerMode == E_POWER_MODE_RUN) || (eActivePowerMode == E_POWER_MODE_DATALOG)) {
    xRes = ISM330DHCXTaskPostReportToFront(pObj, (HIDReport*)&xReport);
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t ISM330DHCXTaskExecuteStepRun(ISM330DHCXTask *_this) {
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
        // disable the fifo
        ism330dhcx_fifo_gy_batch_set(pxSensorDrv, ISM330DHCX_GY_NOT_BATCHED);
        // disable the IRQs
        HAL_NVIC_DisableIRQ(ISM330DHCX_INT1_EXTI_IRQn);
      }
      break;

    default:
      break;
    }
  }

  return xRes;
}

static sys_error_code_t ISM330DHCXTaskExecuteStepDatalog(ISM330DHCXTask *_this) {
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

    case HID_REPORT_ID_ISM330DHCX:
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: new data.\r\n"));
      xRes = ISM330DHCXTaskSensorReadData(_this);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        // notify the listeners...
        SensorEvent xEvt;
        SensorEventInit((IEvent*)&xEvt, _this->m_pxEventSrc, _this->m_pnSensorDataBuff, ISM330DHCX_GY_SAMPLES_PER_IT * 6, 0);
        IEventSrcSendEvent(_this->m_pxEventSrc, (IEvent*)&xEvt, NULL);
      }
      break;

    case HID_REPORT_ID_SENSOR_CMD:
      if (xReport.sensorReport.nCmdID == SENSOR_CMD_ID_START) {
        // now I can use the sensor... let's initialize it.
        xRes = ISM330DHCXTaskSensorInit(_this);
        if (!SYS_IS_ERROR_CODE(xRes)) {
          // enable the IRQs
          HAL_NVIC_EnableIRQ(ISM330DHCX_INT1_EXTI_IRQn);
        }
      }
      break;

    default:
      // unwanted report
      xRes = SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_UNKNOWN_REPORT_ERROR_CODE);
    }
  }

  return xRes;
}

static void ISM330DHCXTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  ISM330DHCXTask *_this = (ISM330DHCXTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: start.\r\n"));

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.
  xRes = ISM330DHCXTaskSensorRegisterInDB(_this);
  if (SYS_IS_ERROR_CODE(xRes)) {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: unable to register with DB\r\n"));
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
        xRes = ISM330DHCXTaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_DATALOG:
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = ISM330DHCXTaskExecuteStepDatalog(_this);
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

static inline sys_error_code_t ISM330DHCXTaskPostReportToFront(ISM330DHCXTask *_this, HIDReport *pxReport) {
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

static sys_error_code_t ISM330DHCXTaskSensorInit(ISM330DHCXTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;

  uint8_t nReg0 = 0;
  int32_t nRetVal = 0;
  // if this variable need to persist then I move it in the managed task class declaration.
  ism330dhcx_pin_int1_route_t int1_route = {0};

  nRetVal = ism330dhcx_device_id_get(pxSensorDrv, (uint8_t *)&nReg0);
  if (!nRetVal) {
    SPIBusIFSetWhoAmI(&_this->m_xSensorIF, nReg0);
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: sensor - I am 0x%x.\r\n", nReg0));

  nRetVal = ism330dhcx_reset_set(pxSensorDrv, 1);
  do {
    ism330dhcx_reset_get(pxSensorDrv, &nReg0);
  } while(nReg0);
  nRetVal = ism330dhcx_i2c_interface_set(pxSensorDrv, ISM330DHCX_I2C_DISABLE);

  /* AXL FS */
  if(_this->m_xSensorCommonParam.pfFS[0] < 3.0f)
    ism330dhcx_xl_full_scale_set(pxSensorDrv, ISM330DHCX_2g);
  else if(_this->m_xSensorCommonParam.pfFS[0] < 5.0f)
    ism330dhcx_xl_full_scale_set(pxSensorDrv, ISM330DHCX_4g);
  else if(_this->m_xSensorCommonParam.pfFS[0] < 9.0f)
    ism330dhcx_xl_full_scale_set(pxSensorDrv, ISM330DHCX_8g);
  else if(_this->m_xSensorCommonParam.pfFS[0] < 17.0f)
    ism330dhcx_xl_full_scale_set(pxSensorDrv, ISM330DHCX_16g);

  /* GYRO FS */
  if(_this->m_xSensorCommonParam.pfFS[1] < 126.0f)
    ism330dhcx_gy_full_scale_set(pxSensorDrv, ISM330DHCX_125dps);
  else if(_this->m_xSensorCommonParam.pfFS[1] < 251.0f)
    ism330dhcx_gy_full_scale_set(pxSensorDrv, ISM330DHCX_250dps);
  else if(_this->m_xSensorCommonParam.pfFS[1] < 501.0f)
    ism330dhcx_gy_full_scale_set(pxSensorDrv, ISM330DHCX_500dps);
  else if(_this->m_xSensorCommonParam.pfFS[1] < 1001.0f)
    ism330dhcx_gy_full_scale_set(pxSensorDrv, ISM330DHCX_1000dps);
  else if(_this->m_xSensorCommonParam.pfFS[1] < 2001.0f)
    ism330dhcx_gy_full_scale_set(pxSensorDrv, ISM330DHCX_2000dps);
  else if(_this->m_xSensorCommonParam.pfFS[1] < 4001.0f)
    ism330dhcx_gy_full_scale_set(pxSensorDrv, ISM330DHCX_4000dps);

  nRetVal = ism330dhcx_fifo_watermark_set(pxSensorDrv, ISM330DHCX_WTM_LEVEL);

  ism330dhcx_pin_int1_route_get(pxSensorDrv, &int1_route);
  int1_route.int1_ctrl.int1_fifo_th = PROPERTY_ENABLE;
  nRetVal = ism330dhcx_pin_int1_route_set(pxSensorDrv, &int1_route);

  nRetVal = ism330dhcx_fifo_mode_set(pxSensorDrv, ISM330DHCX_STREAM_MODE);

  ism330dhcx_odr_xl_t ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_OFF;
  ism330dhcx_bdr_xl_t ism330dhcx_bdr_xl = ISM330DHCX_XL_NOT_BATCHED;
  ism330dhcx_odr_g_t ism330dhcx_odr_g = ISM330DHCX_GY_ODR_OFF;
  ism330dhcx_bdr_gy_t ism330dhcx_bdr_gy = ISM330DHCX_GY_NOT_BATCHED;

  if(_this->m_xSensorCommonParam.fODR < 13.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_12Hz5;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_12Hz5;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_12Hz5;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_12Hz5;
  }
  else if(_this->m_xSensorCommonParam.fODR < 27.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_26Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_26Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_26Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_26Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 53.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_52Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_52Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_52Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_52Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 105.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_104Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_104Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_104Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_104Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 209.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_208Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_208Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_208Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_208Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 418.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_417Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_417Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_417Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_417Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 834.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_833Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_833Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_833Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_833Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 1668.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_1667Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_1667Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_1667Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_1667Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 3334.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_3333Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_3333Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_3333Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_3333Hz;
  }
  else if(_this->m_xSensorCommonParam.fODR < 6668.0f)
  {
    ism330dhcx_odr_xl = ISM330DHCX_XL_ODR_6667Hz;
    ism330dhcx_bdr_xl = ISM330DHCX_XL_BATCHED_AT_6667Hz;
    ism330dhcx_odr_g = ISM330DHCX_GY_ODR_6667Hz;
    ism330dhcx_bdr_gy = ISM330DHCX_GY_BATCHED_AT_6667Hz;
  }

  if(_this->m_xSensorCommonParam.pbSubSensorActive[0])
  {
    nRetVal = ism330dhcx_xl_data_rate_set(pxSensorDrv, ism330dhcx_odr_xl);
    nRetVal = ism330dhcx_fifo_xl_batch_set(pxSensorDrv, ism330dhcx_bdr_xl);
  }
  if(_this->m_xSensorCommonParam.pbSubSensorActive[1])
  {
    ism330dhcx_gy_data_rate_set(pxSensorDrv, ism330dhcx_odr_g);
    ism330dhcx_fifo_gy_batch_set(pxSensorDrv, ism330dhcx_bdr_gy);
  }

  return xRes;
}

static sys_error_code_t ISM330DHCXTaskSensorReadData(ISM330DHCXTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;

  uint8_t nReg0 = 0;
  uint8_t nReg1 = 0;

  /* Check FIFO_WTM_IA anf fifo level. We do not use PID in order to avoid reading one register twice */
  ism330dhcx_read_reg(pxSensorDrv, ISM330DHCX_FIFO_STATUS1, &nReg0, 1);
  ism330dhcx_read_reg(pxSensorDrv, ISM330DHCX_FIFO_STATUS2, &nReg1, 1);

  uint16_t fifo_level = ((nReg1 & 0x03) << 8) + nReg0;

  //TODO: STF.Debug - need to add the timestamp
  if((nReg1) & 0x80  && (fifo_level >= ISM330DHCX_GY_SAMPLES_PER_IT) ) {
//    if(tim_value >= tim_value_old) {
//      period = tim_value - tim_value_old;
//    }
//    else {
//      period = tim_value + (0xFFFFFFFF - tim_value_old);
//    }
//
//    tim_value_old = tim_value;
//    ts_ism330dhcx += period;

    ism330dhcx_read_reg(pxSensorDrv, ISM330DHCX_FIFO_DATA_OUT_TAG, _this->m_pnSensorDataBuff, ISM330DHCX_GY_SAMPLES_PER_IT * 7);

    if(_this->m_pnSensorDataBuff[0]>>3 == 0x02 || !(_this->m_xSensorCommonParam.pbSubSensorActive[0]) || !(_this->m_xSensorCommonParam.pbSubSensorActive[1])) {
      /* First Sample in the fifo is AXL || 1 subsensor active only --> simply drop TAGS */
      int16_t * p16src = (int16_t *)_this->m_pnSensorDataBuff;
      int16_t * p16dest = (int16_t *)_this->m_pnSensorDataBuff;
      for (uint32_t i = 0; i < ISM330DHCX_GY_SAMPLES_PER_IT; i++) {
        p16src = (int16_t *)&((uint8_t *)(p16src))[1];
        *p16dest++ = *p16src++;
        *p16dest++ = *p16src++;
        *p16dest++ = *p16src++;
      }
    }
    else { /* First Sample in the fifo is NOT AXL && 2 subsensors active, rearrange data (TODO) */
      //TODO: STF - to be done
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("ISM330DHCX: rearrange new data: TBD.\r\n"));
    }
  }

  return xRes;
}

static sys_error_code_t ISM330DHCXTaskSensorRegisterInDB(ISM330DHCXTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  COM_Device_t *pxSDB = SDB_GetIstance();

  int32_t nID = SDB_AddSensor(pxSDB);
  if (nID != -1) {
    xRes = SYS_NO_ERROR_CODE;
    _this->m_nDBID = nID;

//    COM_DeviceDescriptor_t *tempDeviceDescriptor = SDB_GetDeviceDescriptor(pxSDB);
//     get_unique_id(tempDeviceDescriptor->serialNumber);
//     strcpy(tempDeviceDescriptor->alias, "STWIN_001");

      /* ISM330DHCX */


    COM_Sensor_t *pxSensor = SDB_GetSensor(pxSDB, _this->m_nDBID);

    /* SENSOR DESCRIPTOR */
    strcpy(pxSensor->sensorDescriptor.name, "ISM330DHCX");
    pxSensor->sensorDescriptor.dataType = DATA_TYPE_INT16;
    pxSensor->sensorDescriptor.ODR[0] = 12.5f;
    pxSensor->sensorDescriptor.ODR[1] = 26.0f;
    pxSensor->sensorDescriptor.ODR[2] = 52.0f;
    pxSensor->sensorDescriptor.ODR[3] = 104.0f;
    pxSensor->sensorDescriptor.ODR[4] = 208.0f;
    pxSensor->sensorDescriptor.ODR[5] = 417.0f;
    pxSensor->sensorDescriptor.ODR[6] = 833.0f;
    pxSensor->sensorDescriptor.ODR[7] = 1667.0f;
    pxSensor->sensorDescriptor.ODR[8] = 3333.0f;
    pxSensor->sensorDescriptor.ODR[9] = 6667.0f;
    pxSensor->sensorDescriptor.ODR[10] = COM_END_OF_LIST_FLOAT;
    pxSensor->sensorDescriptor.samplesPerTimestamp[0] = 0;
    pxSensor->sensorDescriptor.samplesPerTimestamp[1] = 1000;
    pxSensor->sensorDescriptor.nSubSensors = 2;

    /* SENSOR STATUS */
    pxSensor->sensorStatus.ODR = 104.0f; //1667.0f;
    pxSensor->sensorStatus.measuredODR = 0.0f;
    pxSensor->sensorStatus.initialOffset = 0.0f;
    pxSensor->sensorStatus.samplesPerTimestamp = 0;
    pxSensor->sensorStatus.isActive = 1;
    pxSensor->sensorStatus.usbDataPacketSize = 2048;
    pxSensor->sensorStatus.sdWriteBufferSize = ISM330DHCX_WRITE_BUFFER_SIZE;
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
    pxSensor->sensorStatus.subSensorStatus[0].sensitivity = 0.061f * pxSensor->sensorStatus.subSensorStatus[0].FS/2.0f;

      /* SUBSENSOR 1 DESCRIPTOR */
    pxSensor->sensorDescriptor.subSensorDescriptor[1].id = 1;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].sensorType = COM_TYPE_GYRO;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].dataPerSample = 3;
    strcpy(pxSensor->sensorDescriptor.subSensorDescriptor[1].unit, "mdps");
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[0] = 125.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[1] = 250.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[2] = 500.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[3] = 1000.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[4] = 2000.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[5] = 4000.0f;
    pxSensor->sensorDescriptor.subSensorDescriptor[1].FS[6] = COM_END_OF_LIST_FLOAT;

    /* SUBSENSOR 1 STATUS */
    pxSensor->sensorStatus.subSensorStatus[1].FS = 4000.0f;
    pxSensor->sensorStatus.subSensorStatus[1].isActive = 0;
    pxSensor->sensorStatus.subSensorStatus[1].sensitivity = 4.375f * pxSensor->sensorStatus.subSensorStatus[1].FS/125.0f;

    _this->m_xSensorCommonParam.fODR = pxSensor->sensorStatus.ODR;
    _this->m_xSensorCommonParam.pfFS[0] = pxSensor->sensorStatus.subSensorStatus[0].FS;
    _this->m_xSensorCommonParam.pfFS[1] = pxSensor->sensorStatus.subSensorStatus[1].FS;
    _this->m_xSensorCommonParam.pbSubSensorActive[0] = pxSensor->sensorStatus.subSensorStatus[0].isActive;
    _this->m_xSensorCommonParam.pbSubSensorActive[1] = pxSensor->sensorStatus.subSensorStatus[1].isActive;

		//TODO: STF.Porting - what to do with this?
    // it seems used only for LOG_ERROR
//    maxWriteTimeSensor[ism330dhcx_com_id] = 1000 * WRITE_BUFFER_SIZE_ISM330DHCX / (uint32_t)(ISM330DHCX_Init_Param.ODR * 12);
  }

  return xRes;
}

static boolean_t ISM330DHCXTaskSensorIsActive(const ISM330DHCXTask *_this) {
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

/**
 * SPI CS Pin interrupt callback
 */
void ISM330DHCXTask_EXTI_Callback(uint16_t nPin) {
  struct ism330dhcxReport_t xReport = {
      .reportId = HID_REPORT_ID_ISM330DHCX,
      .bDataReady = 1
  };

  if (s_xTaskObj.m_xInQueue != NULL) {
    if (pdTRUE != xQueueSendToBackFromISR(s_xTaskObj.m_xInQueue, &xReport, NULL)) {
      // unable to send the report. Signal the error
      sys_error_handler();
    }
  }
}

