/**
 ******************************************************************************
 * @file    HTS221Task.c
 * @author  STMicroelectronics - AIS - MCD Team
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

#include "HTS221Task.h"
#include "HTS221Task_vtbl.h"
#include "hid_report_parser.h"
#include "SensorCommands.h"
#include "sensor_db.h"
#include "ISensorEventListener.h"
#include "ISensorEventListener_vtbl.h"
#include "UtilTask.h"
#include <string.h>
#include "sysdebug.h"

// TODO: change XXX with a short id for the task

#ifndef HTS221_TASK_CFG_STACK_DEPTH
#define HTS221_TASK_CFG_STACK_DEPTH              120
#endif

#ifndef HTS221_TASK_CFG_PRIORITY
#define HTS221_TASK_CFG_PRIORITY                 (tskIDLE_PRIORITY)
#endif

#ifndef HTS221_TASK_CFG_IN_QUEUE_LENGTH
#define HTS221_TASK_CFG_IN_QUEUE_LENGTH          10
#endif

#define HTS221_TASK_CFG_IN_QUEUE_ITEM_SIZE       sizeof(HIDReport)

#define HTS221_INT1_Pin                          GPIO_PIN_6
#define HTS221_INT1_GPIO_Port                    GPIOG
#define HTS221_INT1_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOG_CLK_ENABLE()
#define HTS221_INT_GPIO_ADDITIONAL()             HAL_PWREx_EnableVddIO2()
#define HTS221_INT1_EXTI_IRQn                    EXTI9_5_IRQn
#define HTS221_INT1_EXTI_LINE                    EXTI_LINE_6

#define HTS221_WRITE_BUFFER_SIZE                 (uint32_t)(16000)

#define SYS_DEBUGF(level, message)               SYS_DEBUGF3(SYS_DBG_HTS221, level, message)


/**
 * HTS221Task virtual table.
 */
static const AManagedTaskEx_vtbl s_xHTS221Task_vtbl = {
    HTS221Task_vtblHardwareInit,
    HTS221Task_vtblOnCreateTask,
    HTS221Task_vtblDoEnterPowerMode,
    HTS221Task_vtblHandleError,
    HTS221Task_vtblForceExecuteStep
};

/**
 *  HTS221Task internal structure.
 */
struct _HTS221Task {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * I2C IF object used to connect the sensor task to the I2C bus.
   */
  I2CBusIF m_xSensorIF;

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
//  uint8_t m_pnSensorDataBuff[HTS221_GY_SAMPLES_PER_IT * 7];

  float x0_t;
  float y0_t;
  float x1_t;
  float y1_t;
  float x0_h;
  float y0_h;
  float x1_h;
  float y1_h;

  /**
   * ::IEventSrc interface implementation for this class.
   */
  IEventSrc *m_pxEventSrc;

  /**
   * Specifies the time stamp in tick.
   */
  uint32_t m_nTimeStampTick;

  /**
   * Used during the time stamp computation to manage the overflow of the hardware timer.
   */
  uint32_t m_nOldTimeStampTick;

  /**
   * Specifies the time stamp linked with the sensor data.
   */
  uint64_t m_nTimeStamp;
};


/**
 * The only instance of the task object.
 */
static HTS221Task s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t HTS221TaskExecuteStepRun(HTS221Task *_this);

/**
 * Execute one step of the task control loop while the system is in DATALOG mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t HTS221TaskExecuteStepDatalog(HTS221Task *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void HTS221TaskRun(void *pParams);

/**
 * Initialize the sensor according to the actual parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t HTS221TaskSensorInit(HTS221Task *_this);

/**
 * Read the data from the sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t HTS221TaskSensorReadData(HTS221Task *_this);

/**
 * Register the sensor with the global DB and initialize the default parameters.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise
 */
static sys_error_code_t HTS221TaskSensorRegisterInDB(HTS221Task *_this);

/**
 * Check if the sensor is active. The sensor is active if at least one of the sub sensor is active.
 * @param _this [IN] specifies a pointer to a task object.
 * @return TRUE if the sensor is active, FALSE otherwise.
 */
static boolean_t HTS221TaskSensorIsActive(const HTS221Task *_this);

/**
 * Sensor Pin interrupt callback
 */
void HTS221Task_EXTI_Callback(uint16_t nPin);


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
static inline sys_error_code_t HTS221TaskPostReportToFront(HTS221Task *_this, HIDReport *pxReport);


#if defined (__GNUC__)
// Inline function defined inline in the header file HTS221Task.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *HTS221TaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xHTS221Task_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

I2CBusIF *HTS221TaskGetSensorIF(HTS221Task *_this) {
  assert_param(_this);

  return &_this->m_xSensorIF;
}

IEventSrc *HTS221TaskGetEventSrcIF(HTS221Task *_this) {
  assert_param(_this);

  return _this->m_pxEventSrc;
}


// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t HTS221Task_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  HTS221Task *pObj = (HTS221Task*)_this;
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  HTS221_INT_GPIO_ADDITIONAL();
  HTS221_INT1_GPIO_CLK_ENABLE();

  /*Configure GPIO pins : STTS751_INT_Pin  */
  GPIO_InitStruct.Pin =  HTS221_INT1_Pin ;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(HTS221_INT1_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(HTS221_INT1_EXTI_IRQn, 5, 0);
//  HAL_NVIC_EnableIRQ(HTS221_INT1_EXTI_IRQn); //TODO: STF - I want to listen the IRQ only after the initialization when the user tasks run
//  HAL_EXTI_GetHandle(&g_ism330dhcx_exti, HTS221_INT1_EXTI_LINE);
//  HAL_EXTI_RegisterCallback(&g_ism330dhcx_exti,  HAL_EXTI_COMMON_CB_ID, HTS221Task_EXTI_Callback);

  return xRes;
}

sys_error_code_t HTS221Task_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HTS221Task *pObj = (HTS221Task*)_this;

  // Create task specific sw resources.

  pObj->m_xInQueue = xQueueCreate(HTS221_TASK_CFG_IN_QUEUE_LENGTH, HTS221_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInQueue == NULL) {
    xRes = SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInQueue, "HTS221_Q");
#endif

  xRes = I2CBusIFInit(&pObj->m_xSensorIF, HTS221_ID, HTS221_I2C_ADDRESS);
  if (SYS_IS_ERROR_CODE(xRes)) {
    return xRes;
  }
  // set the I2CBusIF object as handle the IF connector because the SPIBus task
  // will use the handle to access the I2CBusIF.
  I2CBusIFSetHandle(&pObj->m_xSensorIF, &pObj->m_xSensorIF);

  // Initialize the EventSrc interface.
  // take the ownership of the interface.
  pObj->m_pxEventSrc = SensorEventSrcAlloc();
  if (pObj->m_pxEventSrc == NULL) {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    return xRes;
  }
  IEventSrcInit(pObj->m_pxEventSrc);

  pObj->m_nDBID = 0xFF;
  pObj->m_nTimeStampTick = 0;
  pObj->m_nOldTimeStampTick = 0;
  pObj->m_nTimeStamp = 0;
  pObj->x0_t = 0.0f;
  pObj->y0_t = 0.0f;
  pObj->x1_t = 0.0f;
  pObj->y1_t = 0.0f;
  pObj->x0_h = 0.0f;
  pObj->y0_h = 0.0f;
  pObj->x1_h = 0.0f;
  pObj->y1_h = 0.0f;

  *pvTaskCode = HTS221TaskRun;
  *pcName = "HTS221";
  *pnStackDepth = HTS221_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = HTS221_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t HTS221Task_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HTS221Task *pObj = (HTS221Task*)_this;

  if (eNewPowerMode == E_POWER_MODE_DATALOG) {
    if (HTS221TaskSensorIsActive(pObj)) {
      HIDReport xReport = {
          .sensorReport.reportId = HID_REPORT_ID_SENSOR_CMD,
          .sensorReport.nCmdID = SENSOR_CMD_ID_START
      };

      if (xQueueSendToBack(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100)) != pdTRUE) {
        xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
      }

      // reset the variables for the time stamp computation.
      pObj->m_nTimeStampTick = 0;
      pObj->m_nOldTimeStampTick = 0;
      pObj->m_nTimeStamp = 0;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: -> DATALOG\r\n"));
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

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: -> RUN\r\n"));
  }


  return xRes;
}

sys_error_code_t HTS221Task_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  HTS221Task *pObj = (HTS221Task*)_this;


  return xRes;
}

sys_error_code_t HTS221Task_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HTS221Task *pObj = (HTS221Task*)_this;

  HIDReport xReport = {
      .internalReportFE.reportId = HID_REPORT_ID_FORCE_STEP,
      .internalReportFE.nData = 0
  };

  if ((eActivePowerMode == E_POWER_MODE_RUN) || (eActivePowerMode == E_POWER_MODE_DATALOG)) {
    if (AMTExIsTaskInactive(_this)) {
      xRes = HTS221TaskPostReportToFront(pObj, (HIDReport*)&xReport);
    }
    else {
      _this->m_xStatus.nDelayPowerModeSwitch = 0;
    }
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t HTS221TaskExecuteStepRun(HTS221Task *_this) {
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
        ism330dhcx_fifo_gy_batch_set(pxSensorDrv, HTS221_GY_NOT_BATCHED);
        // disable the IRQs
        HAL_NVIC_DisableIRQ(HTS221_INT1_EXTI_IRQn);
      }
      break;

    default:
      break;
    }
  }

  return xRes;
}

static sys_error_code_t HTS221TaskExecuteStepDatalog(HTS221Task *_this) {
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

    case HID_REPORT_ID_HTS221:
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: new data.\r\n"));
      xRes = HTS221TaskSensorReadData(_this);
      if (!SYS_IS_ERROR_CODE(xRes)) {
        // update the time stamp
        uint32_t nPeriod = 0;
        if (_this->m_nTimeStampTick >= _this->m_nOldTimeStampTick) {
          nPeriod = _this->m_nTimeStampTick - _this->m_nOldTimeStampTick;
        }
        else {
          // overflow of the hw timer
          nPeriod = _this->m_nTimeStampTick + (0xFFFFFFFF -_this->m_nOldTimeStampTick);
        }
        _this->m_nOldTimeStampTick = _this->m_nTimeStampTick;
        _this->m_nTimeStamp += nPeriod;
        // notify the listeners...
        double fTimeStamp = (double)_this->m_nTimeStamp/(double)(SystemCoreClock);
        SensorEvent xEvt;
        SensorEventInit((IEvent*)&xEvt, _this->m_pxEventSrc, _this->m_pnSensorDataBuff, HTS221_GY_SAMPLES_PER_IT * 6, fTimeStamp, _this->m_nDBID);
        IEventSrcSendEvent(_this->m_pxEventSrc, (IEvent*)&xEvt, NULL);

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: ts = %f\r\n", (float)fTimeStamp));
      }
      break;

    case HID_REPORT_ID_SENSOR_CMD:
      if (xReport.sensorReport.nCmdID == SENSOR_CMD_ID_START) {
        xRes = HTS221TaskSensorInit(_this);
        if (!SYS_IS_ERROR_CODE(xRes)) {
          // enable the IRQs
          HAL_NVIC_EnableIRQ(HTS221_INT1_EXTI_IRQn);
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

static void HTS221TaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HTS221Task *_this = (HTS221Task*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: start.\r\n"));

  // At this point all system has been initialized.
  // Execute task specific delayed one time initialization.
  xRes = HTS221TaskSensorRegisterInDB(_this);
  if (SYS_IS_ERROR_CODE(xRes)) {
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: unable to register with DB\r\n"));
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
        xRes = HTS221TaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_DATALOG:
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = HTS221TaskExecuteStepDatalog(_this);
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

static inline sys_error_code_t HTS221TaskPostReportToFront(HTS221Task *_this, HIDReport *pxReport) {
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

static sys_error_code_t HTS221TaskSensorInit(HTS221Task *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;

  uint8_t nReg0 = 0;
  int32_t nRetVal = 0;
  hts221_axis1bit16_t coeff = {0};

  nRetVal = hts221_device_id_get(pxSensorDrv, (uint8_t *)&nReg0);
  if (!nRetVal) {
    SPIBusIFSetWhoAmI(&_this->m_xSensorIF, nReg0);
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: sensor - I am 0x%x.\r\n", nReg0));

  //reset the sensor
  hts221_power_on_set(pxSensorDrv, PROPERTY_DISABLE);
  hts221_boot_set(pxSensorDrv, PROPERTY_ENABLE);

  /* Set BDU*/
  hts221_block_data_update_set(pxSensorDrv, PROPERTY_ENABLE);
  /* Enable Interrupt */
  hts221_drdy_on_int_set(pxSensorDrv, PROPERTY_ENABLE);
  /* Set Data Rate */

  if(_this->m_xSensorCommonParam.fODR < 2.0f)
    hts221_data_rate_set(pxSensorDrv, HTS221_ODR_1Hz);
  else if(_this->m_xSensorCommonParam.fODR < 8.0f)
    hts221_data_rate_set(pxSensorDrv, HTS221_ODR_7Hz);
  else if(_this->m_xSensorCommonParam.fODR < 13.0f)
    hts221_data_rate_set(pxSensorDrv, HTS221_ODR_12Hz5);

  /* Get calibration values (only first time) */
  hts221_temp_adc_point_0_get(pxSensorDrv, coeff.u8bit);
  _this->x0_t = (float)coeff.i16bit;

  hts221_temp_deg_point_0_get(pxSensorDrv, coeff.u8bit);
  _this->y0_t = (float)coeff.u8bit[0];

  hts221_temp_adc_point_1_get(pxSensorDrv, coeff.u8bit);
  _this->x1_t = (float)coeff.i16bit;

  hts221_temp_deg_point_1_get(pxSensorDrv, coeff.u8bit);
  _this->y1_t = (float)coeff.u8bit[0];

  hts221_hum_adc_point_0_get(pxSensorDrv, coeff.u8bit);
  _this->x0_h = (float)coeff.i16bit;

  hts221_hum_rh_point_0_get(pxSensorDrv, coeff.u8bit);
  _this->y0_h = (float)coeff.u8bit[0];

  hts221_hum_adc_point_1_get(pxSensorDrv, coeff.u8bit);
  _this->x1_h = (float)coeff.i16bit;

  hts221_hum_rh_point_1_get(pxSensorDrv, coeff.u8bit);
  _this->y1_h = (float)coeff.u8bit[0];

  /* Power Up */
  hts221_power_on_set(pxSensorDrv, PROPERTY_ENABLE);

  //TODO: STF - why read the data now ??
//  hts221_temperature_raw_get(pxSensorDrv, data_raw_temperature.u8bit);
//  hts221_humidity_raw_get(pxSensorDrv, data_raw_humidity.u8bit);

  return xRes;
}

static sys_error_code_t HTS221TaskSensorReadData(HTS221Task *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  stmdev_ctx_t *pxSensorDrv = (stmdev_ctx_t*) &_this->m_xSensorIF.m_xConnector;
  uint8_t nReg0 = 0;
  uint8_t nReg1 = 0;
  hts221_axis1bit16_t data_raw_humidity = {0};
  hts221_axis1bit16_t data_raw_temperature = {0};

  float dataOut[2] = {0.0f, 0.0f};

  hts221_temperature_raw_get(pxSensorDrv, data_raw_temperature.u8bit);
  // Apply calibration */ /* To be optimized eventually
  dataOut[0] = (((_this->y1_t - _this->y0_t) * (float)(data_raw_temperature.i16bit)) + ((_this->x1_t * _this->y0_t) - (_this->x0_t * _this->y1_t))) / (_this->x1_t - _this->x0_t);

  if((nReg1) & 0x80  && (fifo_level >= HTS221_GY_SAMPLES_PER_IT) ) {
    ism330dhcx_read_reg(pxSensorDrv, HTS221_FIFO_DATA_OUT_TAG, _this->m_pnSensorDataBuff, HTS221_GY_SAMPLES_PER_IT * 7);

    if(_this->m_pnSensorDataBuff[0]>>3 == 0x02 || !(_this->m_xSensorCommonParam.pbSubSensorActive[0]) || !(_this->m_xSensorCommonParam.pbSubSensorActive[1])) {
      /* First Sample in the fifo is AXL || 1 subsensor active only --> simply drop TAGS */
      int16_t * p16src = (int16_t *)_this->m_pnSensorDataBuff;
      int16_t * p16dest = (int16_t *)_this->m_pnSensorDataBuff;
      for (uint32_t i = 0; i < HTS221_GY_SAMPLES_PER_IT; i++) {
        p16src = (int16_t *)&((uint8_t *)(p16src))[1];
        *p16dest++ = *p16src++;
        *p16dest++ = *p16src++;
        *p16dest++ = *p16src++;
      }
    }
    else { /* First Sample in the fifo is NOT AXL && 2 subsensors active, rearrange data (TODO) */
      //TODO: STF - to be done
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HTS221: rearrange new data: TBD.\r\n"));
    }
  }

  return xRes;
}

static sys_error_code_t HTS221TaskSensorRegisterInDB(HTS221Task *_this) {
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

      /* HTS221 */

    COM_Sensor_t *pxSensor = SDB_GetSensor(pxSDB, _this->m_nDBID);

    /* SENSOR DESCRIPTOR */
    strcpy(pxSensor->sensorDescriptor.name, "HTS221");
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
    pxSensor->sensorStatus.sdWriteBufferSize = HTS221_WRITE_BUFFER_SIZE;
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
//    maxWriteTimeSensor[ism330dhcx_com_id] = 1000 * WRITE_BUFFER_SIZE_HTS221 / (uint32_t)(HTS221_Init_Param.ODR * 12);
  }

  return xRes;
}

static boolean_t HTS221TaskSensorIsActive(const HTS221Task *_this) {
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
void HTS221Task_EXTI_Callback(uint16_t nPin) {
  struct hts221Report_t xReport = {
      .reportId = HID_REPORT_ID_HTS221,
      .bDataReady = 1
  };

  if (s_xTaskObj.m_xInQueue != NULL) {
    if (pdTRUE != xQueueSendToBackFromISR(s_xTaskObj.m_xInQueue, &xReport, NULL)) {
      // unable to send the report. Signal the error
      sys_error_handler();
    }
    s_xTaskObj.m_nTimeStampTick = UtilGetTimeStamp();
  }
}

