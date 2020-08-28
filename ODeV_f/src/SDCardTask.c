/**
#include <sensor_db.h>
 ******************************************************************************
 * @file    SDCardTask.c
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

#include "SDCardTask.h"
#include "SDCardTask_vtbl.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h"
#include "hid_report_parser.h"
#include "queue.h"
#include "HSD_json.h"
#include <string.h>
#include <stdio.h>
#include "sysdebug.h"

// TODO: change XXX with a short id for the task

#ifndef SDC_TASK_CFG_STACK_DEPTH
#define SDC_TASK_CFG_STACK_DEPTH        120
#endif

#ifndef SDC_TASK_CFG_PRIORITY
#define SDC_TASK_CFG_PRIORITY           (tskIDLE_PRIORITY)
#endif

#ifndef SDC_TASK_CFG_IN_QUEUE_ITEM_SIZE
#define SDC_TASK_CFG_IN_QUEUE_ITEM_SIZE sizeof(HIDReport)
#endif

#ifndef SDC_TASK_CFG_IN_QUEUE_ITEM_COUNT
#define SDC_TASK_CFG_IN_QUEUE_ITEM_COUNT 50
#endif

#define LOG_DIR_PREFIX                  "STM32_DL_"

/* Memory management macros */
#define HSD_malloc                      pvPortMalloc
#define HSD_calloc                      pvPortMalloc
#define HSD_free                        vPortFree
#define HSD_memset                      memset
#define HSD_memcpy                      memcpy

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_SDC, level, message)


/**
 * SDCardTask virtual table.
 */
static const AManagedTaskEx_vtbl s_xSDCardTask_vtbl = {
    SDCardTask_vtblHardwareInit,
    SDCardTask_vtblOnCreateTask,
    SDCardTask_vtblDoEnterPowerMode,
    SDCardTask_vtblHandleError,
    SDCardTask_vtblForceExecuteStep
};

/**
 * SDCardTask Sensor Event Listener IF virtual table.
 */
static const ISensorEventListener_vtbl s_xSDCardTaskSEL_vtbl = {
    SDCardTaskSEL_vtblOnStatusChange,
    SDCardTaskSEL_vtblSetOwner,
    SDCardTaskSEL_vtblGetOwner,
    SDCardTaskSEL_vtblOnNewDataReady
};

typedef struct _SDTSensorListener {
  ISensorEventListener super;

  void *m_pxOwner;
} SDTSensorListener;

/**
 *  SDCardTask internal structure.
 */
struct _SDCardTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * File system object for SD card logical drive
   */
  FATFS m_xSDFatFs;

  FIL m_xFileHandlers[COM_MAX_SENSORS];
  FIL m_xFileConfigHandler;
  FIL m_xFileConfigJSON;
  char m_pcSDPath[4]; /* SD card logical drive path */

  uint8_t *m_pnSDWriteBuffer[COM_MAX_SENSORS];
  uint32_t m_pnSDWriteBufferIdx[COM_MAX_SENSORS];

  boolean_t m_pnSensorFirstDataReady[COM_MAX_SENSORS];
  float m_pnSensorNSamplesAcc[COM_MAX_SENSORS];
  double m_pnOldTimeStamp[COM_MAX_SENSORS];
  uint16_t m_pnSensorNSamplesToTimestamp[COM_MAX_SENSORS];

  COM_Device_t m_xJSONDevice;

  /**
   * Input queue used to send HID message to the task.
   */
  QueueHandle_t m_xInQueue;

  SDTSensorListener m_xSensorListener;
};

/**
 * The only instance of the task object.
 */
static SDCardTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t SDTExecuteStepRun(SDCardTask *_this);

/**
 * Execute one step of the task control loop while the system is in DATALOG mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t SDTExecuteStepDatalog(SDCardTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void SDCardTaskRun(void *pParams);

/**
 * Initialize the SDCARD and the file system.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, SYS_SD_TASK_NO_SDCARD_ERROR_CODE if the SDCARD is not present or corrupted.
 */
static sys_error_code_t SDT_SD_Init(SDCardTask *_this);

/**
 * Deinitialize the SDCARD and the file system.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t SDT_SD_DeInit(SDCardTask *_this);

/**
 * SDCARD memory initialization. Performs the dynamic allocation for the m_pnSDWriteBuffer associated to each active sensor.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t SDTMemoryInit(SDCardTask *_this);

/**
 * SDCARD Task memory De-initialization.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t SDTMemoryDeinit(SDCardTask *_this);

/**
 * Execute the one time initialization when the task start. It check for the low battery condition and Initialize the SDCARD.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t SDTRuntimeInit(SDCardTask *_this);

/**
 * Create the file and folder in the SDCARD to store the data.
 * Create the memory buffers to handle the sensor data.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t SDTStartLogging(SDCardTask *_this);

/**
 * Create the file and folder in the SDCARD to store the data.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t SDTStopLogging(SDCardTask *_this);

static sys_error_code_t SDTReadJSON(SDCardTask *_this, char *serialized_string);

static sys_error_code_t SDTCreateJSON(SDCardTask *_this, char **serialized_string);

static uint32_t SDTGetLastDirNumber(SDCardTask *_this);

static sys_error_code_t SDTOpenFile(SDCardTask *_this, uint32_t id, const char *sensorName);

static sys_error_code_t SDTCloseFiles(SDCardTask *_this);

static sys_error_code_t SDTFlushBuffer(SDCardTask *_this, uint32_t id);

static sys_error_code_t SDTFillBuffer(SDCardTask *_this, uint8_t id, uint8_t *src, uint16_t srcSize);

static inline sys_error_code_t SDTWriteBuffer(SDCardTask *_this, uint32_t id, uint8_t *buffer, uint32_t size);

static inline sys_error_code_t SDTCloseFile(SDCardTask *_this, uint32_t id);

static inline sys_error_code_t SDTWriteConfigBuffer(SDCardTask *_this, uint8_t *buffer, uint32_t size);

static inline sys_error_code_t SDTTaskPostReportToFront(SDCardTask *_this, HIDReport *pxReport);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
// Inline function defined inline in the header file SDCardTask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *SDCardTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xSDCardTask_vtbl;
  s_xTaskObj.m_xSensorListener.super.vptr = &s_xSDCardTaskSEL_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

IEventListener *SDCardTaskGetEventListenrIF(const SDCardTask *_this) {
  assert_param(_this);

  return (IEventListener*)&_this->m_xSensorListener;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t SDCardTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SDCardTask *pObj = (SDCardTask*)_this;

  // This task doesn't have a IDriver at the moment because:
  // - it needs one PIN for the SDCARD detection
  // - the other SDCARD PINs are managed by the BSP driver generated by CubeMX.
  // - The BSP driver can be transformed or embedded in a IDrriver (TBD).

  BSP_PlatformSDDetectInit();

  return xRes;
}

sys_error_code_t SDCardTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SDCardTask *pObj = (SDCardTask*)_this;

  // create the input queue
  pObj->m_xInQueue = xQueueCreate(SDC_TASK_CFG_IN_QUEUE_ITEM_COUNT, SDC_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInQueue == NULL) {
    xRes = SYS_SD_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_INIT_ERROR_CODE);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInQueue, "SDT_Q");
#endif

  // take the ownership of the Sensor Event IF
  IEventListenerSetOwner((IEventListener*)&pObj->m_xSensorListener, pObj);

  *pvTaskCode = SDCardTaskRun;
  *pcName = "SDC";
  *pnStackDepth = SDC_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = SDC_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t SDCardTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SDCardTask *pObj = (SDCardTask*)_this;

  if (eNewPowerMode == E_POWER_MODE_DATALOG) {
    // prepare the files in the SDCARD
    xRes = SDTStartLogging(pObj);
    // reset the IN queue.
    xQueueReset(pObj->m_xInQueue);
  }
  else if (eNewPowerMode == E_POWER_MODE_RUN) {
    HIDReport xReport = {
        .sdReport.reportId = HID_REPORT_ID_SD_CMD,
        .sdReport.nCmdID = SDT_CMD_ID_STOP
    };

    if (xQueueSendToBack(pObj->m_xInQueue, &xReport, pdMS_TO_TICKS(100)) != pdTRUE) {
      xRes = SYS_APP_TASK_REPORT_LOST_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_APP_TASK_REPORT_LOST_ERROR_CODE);
    }
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDT: -> %d\r\n", eNewPowerMode));

  return xRes;
}

sys_error_code_t SDCardTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SDCardTask *pObj = (SDCardTask*)_this;

  return xRes;
}

sys_error_code_t SDCardTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SDCardTask *pObj = (SDCardTask*)_this;

  struct internalReportFE_t xReport = {
      .reportId = HID_REPORT_ID_FORCE_STEP,
      .nData = 0
  };

  if ((eActivePowerMode == E_POWER_MODE_RUN) || (eActivePowerMode == E_POWER_MODE_DATALOG)) {
    xRes = SDTTaskPostReportToFront(pObj, (HIDReport*)&xReport);
  }
  else {
    vTaskResume(_this->m_xThaskHandle);
  }

  return xRes;
}

// IIListener virtual functions definition
// ***************************************

sys_error_code_t SDCardTaskSEL_vtblOnStatusChange(IListener *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UNUSED(_this);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SDC.SEL: not implemented OnStatusChange.\r\n"));

  return xRes;
}


// IEventListener virtual functions definition
// *******************************************

void SDCardTaskSEL_vtblSetOwner(IEventListener *_this, void *pxOwner) {
  assert_param(_this);
  SDTSensorListener *pObj = (SDTSensorListener*)_this;

  pObj->m_pxOwner = pxOwner;
}

void *SDCardTaskSEL_vtblGetOwner(IEventListener *_this) {
  assert_param(_this);
  SDTSensorListener *pxObj = (SDTSensorListener*)_this;

  return pxObj->m_pxOwner;
}


// ISensorEventListener virtual functions definition
// *************************************************

sys_error_code_t SDCardTaskSEL_vtblOnNewDataReady(IEventListener *_this, const SensorEvent *pxEvt) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SDCardTask *pxOwner = (SDCardTask*)IEventListenerGetOwner(_this);
  COM_Device_t *pxSDB = SDB_GetIstance();

  if (pxOwner == NULL) {
    return xRes;
  }

  COM_SensorStatus_t * tempStatus = SDB_GetSensorStatus(pxSDB, pxEvt->nSensorID);
  COM_SensorDescriptor_t * tempDescriptor = SDB_GetSensorDescriptor(pxSDB, pxEvt->nSensorID);
  COM_SubSensorDescriptor_t *tempSubSensorDescriptor = NULL;
  uint16_t toSend = 0, nBytesPerSample = 0;

  if (pxOwner->m_pnSensorFirstDataReady[pxEvt->nSensorID]) {
    // Discard first set of sensor data
    pxOwner->m_pnSensorFirstDataReady[pxEvt->nSensorID] = 0;
    pxOwner->m_pnSensorNSamplesAcc[pxEvt->nSensorID] = 0.0f;
    tempStatus->initialOffset = pxEvt->fTimeStamp;
    pxOwner->m_pnOldTimeStamp[pxEvt->nSensorID] = pxEvt->fTimeStamp;
    pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] = tempStatus->samplesPerTimestamp;
  }
  else {
    if(tempDescriptor->dataType == DATA_TYPE_FLOAT || tempDescriptor->dataType == DATA_TYPE_INT32 || tempDescriptor->dataType == DATA_TYPE_UINT32)  {
      nBytesPerSample = 4;
    }
    else if(tempDescriptor->dataType == DATA_TYPE_UINT16 || tempDescriptor->dataType == DATA_TYPE_INT16) {
      nBytesPerSample = 2;
    }
    else if(tempDescriptor->dataType == DATA_TYPE_UINT8 || tempDescriptor->dataType == DATA_TYPE_INT8) {
      nBytesPerSample = 1;
    }

    uint8_t totalDataPerSample = 0;

    for (int i = 0; i < tempDescriptor->nSubSensors; i++) {
      if (tempStatus->subSensorStatus[i].isActive) {
        tempSubSensorDescriptor = SDB_GetSubSensorDescriptor(pxSDB, pxEvt->nSensorID, i);
        totalDataPerSample += tempSubSensorDescriptor->dataPerSample;
      }
    }

    nBytesPerSample *= totalDataPerSample;
    pxOwner->m_pnSensorNSamplesAcc[pxEvt->nSensorID] = (float)(pxEvt->nDataSize / nBytesPerSample);
    tempStatus->measuredODR = pxOwner->m_pnSensorNSamplesAcc[pxEvt->nSensorID]/(pxEvt->fTimeStamp - pxOwner->m_pnOldTimeStamp[pxEvt->nSensorID]);
    pxOwner->m_pnOldTimeStamp[pxEvt->nSensorID] = pxEvt->fTimeStamp;
    toSend = pxEvt->nDataSize/nBytesPerSample;

    uint8_t *pnBuff = (uint8_t *)pxEvt->pData;
    while (toSend > 0) {
      if(toSend < pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] || pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] == 0) {
        SDTFillBuffer(pxOwner, pxEvt->nSensorID , pnBuff, toSend * nBytesPerSample);

        if(pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] != 0) {
          pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] -= toSend;
        }
        toSend = 0;
      }
      else {
        SDTFillBuffer(pxOwner, pxEvt->nSensorID, pnBuff, pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] * nBytesPerSample);

        pnBuff += pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] * nBytesPerSample;
        toSend -= pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID];

        double newTS = pxEvt->fTimeStamp - ((1.0 / (double)tempStatus->measuredODR) * toSend);
        SDTFillBuffer(pxOwner, pxEvt->nSensorID, (uint8_t *)&newTS, 8);
        pxOwner->m_pnSensorNSamplesToTimestamp[pxEvt->nSensorID] = tempStatus->samplesPerTimestamp;
      }
    }

  }


  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t SDTExecuteStepRun(SDCardTask *_this) {
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

    case HID_REPORT_ID_SD_CMD:
      if (xReport.sdReport.nCmdID == SDT_CMD_ID_STOP) {
        // stop the datalog and close the files.
        xRes = SDTStopLogging(_this);
      }
      break;

    default:
      break;
    }
  }

  return xRes;
}

static sys_error_code_t SDTExecuteStepDatalog(SDCardTask *_this) {
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

    case HID_REPORT_ID_SD_CMD:
      if (xReport.sdReport.nCmdID == SDT_CMD_ID_START) {
        // start the datalog??
        // maybe better to get ready for the data logging during the PM transaction.
      }
      else if(xReport.sdReport.nCmdID == SDT_CMD_ID_FIRST_HALF_DATA_READY) {
        uint8_t sensor_id = xReport.sdReport.nParam;
        COM_SensorStatus_t * sensor_status = SDB_GetSensorStatus(SDB_GetIstance(), sensor_id);
        uint32_t buf_size = sensor_status->sdWriteBufferSize;
        SDTWriteBuffer(_this, sensor_id, _this->m_pnSDWriteBuffer[sensor_id], buf_size);
      }
      else if(xReport.sdReport.nCmdID == SDT_CMD_ID_SECOND_HALF_DATA_READY) {
        uint8_t sensor_id = xReport.sdReport.nParam;
        COM_SensorStatus_t * sensor_status = SDB_GetSensorStatus(SDB_GetIstance(), sensor_id);
        uint32_t buf_size = sensor_status->sdWriteBufferSize;
        SDTWriteBuffer(_this, sensor_id, (uint8_t*)(_this->m_pnSDWriteBuffer[sensor_id] + buf_size), buf_size);
      }
      break;

    default:
      break;
    }
  }

  return xRes;
}

static void SDCardTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SDCardTask *_this = (SDCardTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDC: start.\r\n"));
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDC: SDB size=%d.\r\n", sizeof(COM_Device_t)));

  xRes = SDTRuntimeInit(_this);
  if(SYS_IS_ERROR_CODE(xRes)) {
    sys_error_handler();
  }

  for (;;) {

    // check if there is a pending power mode switch request
    if (_this->super.m_xStatus.nPowerModeSwitchPending == 1) {
      // clear the power mode switch delay because the task is ready to switch.
      //
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
        xRes = SDTExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_DATALOG:
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = SDTExecuteStepDatalog(_this);
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

static sys_error_code_t SDT_SD_Init(SDCardTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (FATFS_LinkDriver(&SD_Driver, _this->m_pcSDPath) == 0) {
    // Register the file system object to the FatFs module
    if (f_mount(&_this->m_xSDFatFs, (TCHAR const*)_this->m_pcSDPath, 0) != FR_OK) {
      xRes = SYS_SD_TASK_NO_SDCARD_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_NO_SDCARD_ERROR_CODE);

      //TODO: STF -  for the moment block the application to copy the HSDatalog.
      sys_error_handler();
    }
  }
  else {
    xRes = SYS_SD_TASK_NO_SDCARD_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_NO_SDCARD_ERROR_CODE);

    //TODO: STF -  for the moment block the application to copy the HSDatalog.
    sys_error_handler();
  }



  return xRes;
}

static sys_error_code_t SDT_SD_DeInit(SDCardTask *_this) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (FATFS_UnLinkDriver(_this->m_pcSDPath) == 0) {
    // Unmount the file system object to the FatFs module
    if (f_mount(&_this->m_xSDFatFs, (TCHAR const*)_this->m_pcSDPath, 0) != FR_OK) {
      xRes = SYS_SD_TASK_NO_SDCARD_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_NO_SDCARD_ERROR_CODE);

      //TODO: STF -  for the moment block the application to copy the HSDatalog.
      sys_error_handler();
    }
  }
  else {
    xRes = SYS_SD_TASK_NO_SDCARD_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_NO_SDCARD_ERROR_CODE);

    //TODO: STF -  for the moment block the application to copy the HSDatalog.
    sys_error_handler();
  }

  return xRes;
}

static sys_error_code_t SDTRuntimeInit(SDCardTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (BSP_SD_IsDetected()) {
    char* config_JSON_string = NULL;

    xRes = SDT_SD_Init(_this);
    if (!SYS_IS_ERROR_CODE(xRes)) {
      // Check if a custom configuration JSON is available in the root folder of the SD Card
      if(f_open(&_this->m_xFileConfigJSON, "DeviceConfig.json", FA_OPEN_EXISTING | FA_READ) == FR_OK) {
        config_JSON_string = pvPortMalloc(f_size(&_this->m_xFileConfigJSON));
        f_gets(config_JSON_string, f_size(&_this->m_xFileConfigJSON), &_this->m_xFileConfigJSON);
        xRes = SDTReadJSON(_this, config_JSON_string);
        HSD_free(config_JSON_string);
        config_JSON_string = NULL;
        f_close(&_this->m_xFileConfigJSON);
      }
    }
//    else
//    {
//      Enable_Sensors(); //TODO: STF - SDT - how to replace this?
//    }
//    if (init_SD_peripheral != 0)
//    {
//      SDM_SD_DeInit(); //TODO: STF - SDT - why shall I deinit the SDCARD?
//      init_SD_peripheral = 0;
//    }
  }


  return xRes;
}

static sys_error_code_t SDTReadJSON(SDCardTask *_this, char *serialized_string) {
  assert_param(serialized_string);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  COM_Device_t *local_device;
  uint8_t ii;
  uint32_t size;
  local_device = SDB_GetIstance();
  size = sizeof(COM_Device_t);

  //TODO: STF - m_xJSONDevice (56 bytes) is used only here:
  // 1. Why we need a local copy instead of using the global instance?
  // 2. If we need a copy then memcpy is not good because it copy the pointer from the global instance
  memcpy(&_this->m_xJSONDevice, local_device, size);
  HSD_ParseDevice(serialized_string, &_this->m_xJSONDevice);

  for (ii = 0; ii < _this->m_xJSONDevice.deviceDescriptor.nSensor; ii++)
  {
//    update_sensorStatus(&local_device->sensors[ii]->sensorStatus, &_this->m_xJSONDevice.sensors[ii]->sensorStatus, ii); //TODO: STF - what to do ??
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDC: update sensor status %d.\r\n", ii));
  }

//  update_sensors_config(); //TODO: STF - what to do ??
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDC: update sensors config.\r\n"));

  return xRes;
}

static sys_error_code_t SDTCreateJSON(SDCardTask *_this, char **serialized_string) {
  assert_param(serialized_string);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  COM_Device_t *device;

  device = SDB_GetIstance();
  if (HSD_serialize_Device_JSON(device, serialized_string)) {
    xRes = SYS_SD_TASK_FILE_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_OP_ERROR_CODE);
  }

  return xRes;
}

static sys_error_code_t SDTStartLogging(SDCardTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // check if the SDCARD is detect.
  // TBD. For the moment I assume that the SDCAR is present.

  // Create the files for the logging session.
  COM_SensorStatus_t * sensor_status;
  COM_DeviceDescriptor_t * device_descriptor;
  COM_SensorDescriptor_t * sensor_descriptor;
  COM_Device_t *pxSDB = SDB_GetIstance();

  uint32_t i = 0, dir_n = 0;
  char dir_name[sizeof(LOG_DIR_PREFIX)+4];
  char file_name[50];

  device_descriptor = SDB_GetDeviceDescriptor(pxSDB);
  dir_n = SDTGetLastDirNumber(_this);
  dir_n++;

  sprintf(dir_name, "%s%03lu", LOG_DIR_PREFIX,(unsigned long) dir_n);

//  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDC: create dir %s", dir_name));

  FRESULT xFRes = f_mkdir(dir_name);
  if(xFRes != FR_OK) {
    xRes = SYS_SD_TASK_FILE_OPEN_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_OPEN_ERROR_CODE);
  }
  else {
    for(i=0;i<device_descriptor->nSensor;i++) {
      sensor_status = SDB_GetSensorStatus(pxSDB, i);

      if(sensor_status->isActive) {
        sensor_descriptor = SDB_GetSensorDescriptor(pxSDB, i);
        sprintf(file_name, "%s/%s", dir_name, sensor_descriptor->name);

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SDC: open log file %s\r\n", file_name));

        if(SYS_IS_ERROR_CODE(SDTOpenFile(_this, i, file_name))) {
          xRes = SYS_SD_TASK_FILE_OPEN_ERROR_CODE;
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_OPEN_ERROR_CODE);
          break;
        }
      }
    }

    if (!SYS_IS_ERROR_CODE(xRes)) {
      SDTMemoryInit(_this);

      for (int i=0; i<COM_MAX_SENSORS; ++i) {
        _this->m_pnSensorFirstDataReady[i] = TRUE;
        _this->m_pnOldTimeStamp[i] = 0.0;
        _this->m_pnSensorNSamplesAcc[i] = 0.0f;
        _this->m_pnSensorNSamplesToTimestamp[i] = 0;
      }
    }
  }

  return xRes;
}

static sys_error_code_t SDTStopLogging(SDCardTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

//  SM_TIM_Stop(); //TODO: STF.Porting to move in the UtilTask

  xRes = SDTCloseFiles(_this);
  xRes = SDTMemoryDeinit(_this);

  //TODO: STF.Porting - what shall I do ???
//  if (init_SD_peripheral != 0)
//  {
//    SDM_SD_DeInit();
//    init_SD_peripheral = 0;
//  }

  return xRes;
}

static uint32_t SDTGetLastDirNumber(SDCardTask *_this) {
  FRESULT fr;     /* Return value */
  DIR dj;         /* Directory search object */
  FILINFO fno;    /* File information */
  int dir_n = 0, tmp;
  char dir_name[sizeof(LOG_DIR_PREFIX)+1] = LOG_DIR_PREFIX;

  dir_name[sizeof(LOG_DIR_PREFIX)-1] = '*';  /* wildcard */
  dir_name[sizeof(LOG_DIR_PREFIX)] = 0;

  fr = f_findfirst(&dj, &fno, "", dir_name);  /* Start to search for matching directories */
  if(fno.fname[0])
  {
    tmp = strtol(&fno.fname[sizeof(LOG_DIR_PREFIX)],NULL,10);
    if(dir_n<tmp)
    {
      dir_n = tmp;
    }
  }

  /* Repeat while an item is found */
  while (fr == FR_OK && fno.fname[0])
  {
    fr = f_findnext(&dj, &fno);   /* Search for next item */
    if(fno.fname[0])
    {
      tmp = strtol(&fno.fname[sizeof(LOG_DIR_PREFIX)],NULL,10);
      if(tmp > dir_n)
      {
        dir_n = tmp;
      }
    }
  }

  f_closedir(&dj);

  return (uint32_t)dir_n;
}

static sys_error_code_t SDTOpenFile(SDCardTask *_this, uint32_t id, const char *sensorName) {
//	assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  UNUSED(_this);

  char file_name[50];

  sprintf(file_name, "%s%s", sensorName, ".dat");

  if(f_open(&_this->m_xFileHandlers[id], (char const*)file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
  {
    return 1;
  }

  return xRes; // OK
}

static sys_error_code_t SDTCloseFiles(SDCardTask *_this) {
  //  assert_param(_this);
    sys_error_code_t xRes = SYS_NO_ERROR_CODE;
    COM_SensorStatus_t * sensor_status = NULL;
    COM_DeviceDescriptor_t * device_descriptor = NULL;
    uint32_t id = 0, dir_n = 0;
    char dir_name[sizeof(LOG_DIR_PREFIX)+4];
    char file_name[50];
    char* JSON_string = NULL;
    COM_Device_t *pxSDB = SDB_GetIstance();

    UNUSED(_this);

    device_descriptor = SDB_GetDeviceDescriptor(pxSDB);

    /* Put all the sensors in "SUSPENDED" mode */
    //STF.Porting this must be done by each sensor task during the power mode switch.
//    for(id=0; id<device_descriptor->nSensor; id++) {
//      sensor_status = SDB_GetSensorStatus(id);
//
//      if(sensor_status->isActive) {
//        SDM_StopSensorThread(id);
//      }
//    }

    /* Flush remaining data and close the files  */
    for(id=0; id<device_descriptor->nSensor; id++) {
      sensor_status = SDB_GetSensorStatus(pxSDB, id);

      if(sensor_status->isActive) {
        SDTFlushBuffer(_this, id);
        xRes = SDTCloseFile(_this, id);
        if(SYS_IS_ERROR_CODE(xRes)) {
          break;
        }
      }
    }

    if (!SYS_IS_ERROR_CODE(xRes)) {
      dir_n = SDTGetLastDirNumber(_this);
      sprintf(dir_name, "%s%03ld", LOG_DIR_PREFIX, dir_n);
      sprintf(file_name, "%s/DeviceConfig.json", dir_name);

      if(f_open(&_this->m_xFileConfigHandler, (char const*)file_name, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
        xRes = SYS_SD_TASK_FILE_OPEN_ERROR_CODE;
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_OPEN_ERROR_CODE);
      }

      if (!SYS_IS_ERROR_CODE(xRes)) {
        SDTCreateJSON(_this, &JSON_string);
        SDTWriteConfigBuffer(_this, (uint8_t*)JSON_string, strlen(JSON_string));

        if (f_close(&_this->m_xFileConfigHandler)!= FR_OK) {
          xRes = SYS_SD_TASK_FILE_CLOSE_ERROR_CODE;
          SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_CLOSE_ERROR_CODE);
        }
      }

//      HSD_JSON_free(JSON_string); //TODO: STF.Porting - this function is present in v2.1.1, but the header is v2.0.0
      JSON_string = NULL;
    }


    return xRes;
}

static sys_error_code_t SDTMemoryInit(SDCardTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  COM_Device_t *pxSDB = SDB_GetIstance();
  COM_SensorStatus_t * sensor_status = NULL;
  COM_DeviceDescriptor_t * device_descriptor = NULL;
  uint32_t i;

  device_descriptor = SDB_GetDeviceDescriptor(pxSDB);

  for(i=0;i<device_descriptor->nSensor;i++)
  {
    sensor_status = SDB_GetSensorStatus(pxSDB, i);
    if(sensor_status->isActive)
    {
      _this->m_pnSDWriteBuffer[i] = HSD_malloc(sensor_status->sdWriteBufferSize*2);
      if(!_this->m_pnSDWriteBuffer[i])
      {
        sys_error_handler();
      }
    }
    else
    {
      _this->m_pnSDWriteBuffer[i] = 0;
    }
  }

  return xRes;
}

static sys_error_code_t SDTMemoryDeinit(SDCardTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  COM_Device_t *pxSDB = SDB_GetIstance();
  COM_SensorStatus_t * sensor_status = NULL;
  COM_DeviceDescriptor_t * device_descriptor = NULL;
  uint32_t i = 0;

  device_descriptor = SDB_GetDeviceDescriptor(pxSDB);

  for(i=0; i<device_descriptor->nSensor; i++) {
    sensor_status = SDB_GetSensorStatus(pxSDB, i);
    if(sensor_status->isActive && _this->m_pnSDWriteBuffer[i]!=0) {
      HSD_free(_this->m_pnSDWriteBuffer[i]);
      _this->m_pnSDWriteBuffer[i] = NULL;
    }
  }

  return xRes;
}

static sys_error_code_t SDTFlushBuffer(SDCardTask *_this, uint32_t id) {
	assert_param(_this);
	sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  uint32_t buf_size = 0;
  COM_Device_t *pxSDB = SDB_GetIstance();
  COM_SensorStatus_t * sensor_status = NULL;

  sensor_status = SDB_GetSensorStatus(pxSDB, id);
  buf_size = sensor_status->sdWriteBufferSize;

  if(_this->m_pnSDWriteBufferIdx[id]>0 && _this->m_pnSDWriteBufferIdx[id]<(buf_size-1)) {
    /* flush from the beginning */
    xRes = SDTWriteBuffer(_this, id, _this->m_pnSDWriteBuffer[id], _this->m_pnSDWriteBufferIdx[id]+1);
  }
  else if (_this->m_pnSDWriteBufferIdx[id]>(buf_size-1) && _this->m_pnSDWriteBufferIdx[id]<(buf_size*2-1)) {
    /* flush from half buffer */
    xRes =  SDTWriteBuffer(_this, id, (uint8_t *)(_this->m_pnSDWriteBuffer[id]+buf_size), _this->m_pnSDWriteBufferIdx[id]+1-buf_size);
  }

  _this->m_pnSDWriteBufferIdx[id] = 0;

	return xRes;
}

static sys_error_code_t SDTFillBuffer(SDCardTask *_this, uint8_t id, uint8_t *src, uint16_t srcSize) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  uint8_t *dst = _this->m_pnSDWriteBuffer[id];
  uint32_t dstP, srcP = 0;
  COM_SensorStatus_t * sensor_status = SDB_GetSensorStatus(SDB_GetIstance(), id);
  uint32_t dstSize, sdBufSize;
  HIDReport xReport = {
      .sdReport.reportId = HID_REPORT_ID_SD_CMD,
      .sdReport.nParam = id
  };

  dstP = _this->m_pnSDWriteBufferIdx[id];
  sdBufSize = sensor_status->sdWriteBufferSize;
  dstSize = sdBufSize*2;

  /* byte per byte copy */
  while(srcP < srcSize) {
    dst[dstP] = src[srcP];
    dstP++;
    srcP++;
    if(dstP >= dstSize) {
      dstP = 0;
    }
  }

  if((_this->m_pnSDWriteBufferIdx[id] < (dstSize/2)) && (dstP >= (dstSize/2))) {
    // first half full
    xReport.sdReport.nCmdID = SDT_CMD_ID_FIRST_HALF_DATA_READY;

  }
  else if(dstP < _this->m_pnSDWriteBufferIdx[id]) {
    // second half full
    xReport.sdReport.nCmdID = SDT_CMD_ID_SECOND_HALF_DATA_READY;
  }

  if (pdTRUE != xQueueSendToBack(_this->m_xInQueue, &xReport, pdMS_TO_TICKS(50))) {
    xRes = SYS_SD_TASK_DATA_LOST_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_DATA_LOST_ERROR_CODE);
  }

  _this->m_pnSDWriteBufferIdx[id] = dstP;

  return xRes;
}


// Inline function definition
// **************************

static inline
sys_error_code_t SDTWriteBuffer(SDCardTask *_this, uint32_t id, uint8_t *buffer, uint32_t size) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  uint32_t byteswritten = 0;

  if(f_write(&_this->m_xFileHandlers[id], buffer, size, (void *)&byteswritten) != FR_OK) {
    xRes = SYS_SD_TASK_FILE_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_OP_ERROR_CODE);
  }

  return xRes;
}

static inline
sys_error_code_t SDTCloseFile(SDCardTask *_this, uint32_t id) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (f_close(&_this->m_xFileHandlers[id]) != FR_OK) {
    xRes = SYS_SD_TASK_FILE_CLOSE_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_CLOSE_ERROR_CODE);
  }

  return xRes;
}

static inline
sys_error_code_t SDTWriteConfigBuffer(SDCardTask *_this, uint8_t *buffer, uint32_t size) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  uint32_t byteswritten = 0;

  if (f_write(&_this->m_xFileConfigHandler, buffer, size, (void *)&byteswritten) != FR_OK) {
    xRes = SYS_SD_TASK_FILE_OP_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_SD_TASK_FILE_OP_ERROR_CODE);
  }

  return xRes;
}

static inline sys_error_code_t SDTTaskPostReportToFront(SDCardTask *_this, HIDReport *pxReport) {
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
