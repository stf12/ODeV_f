/**
 ******************************************************************************
 * @file    AITask.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 22, 2020
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

#include "AITask.h"
#include "AITask_vtbl.h"
#include "CircularBuffer.h"
#include "hid_report_parser.h"
#include "timers.h"
#include "semphr.h"
#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sysdebug.h"

// TODO: cange XXX with a short id for the task

#ifndef AI_TASK_CFG_STACK_DEPTH
#define AI_TASK_CFG_STACK_DEPTH                120
#endif

#ifndef AI_TASK_CFG_PRIORITY
#define AI_TASK_CFG_PRIORITY                   (tskIDLE_PRIORITY+1)
#endif

#ifndef AI_TASK_CFG_IN_QUEUE_ITEM_SIZE
#define AI_TASK_CFG_IN_QUEUE_ITEM_SIZE         sizeof(struct aiReport_t)
#endif

#ifndef AI_TASK_CFG_IN_QUEUE_ITEM_COUNT
#define AI_TASK_CFG_IN_QUEUE_ITEM_COUNT        50
#endif

#define AI_TICK_TO_WAIT_IN_CMD_LOOP            portMAX_DELAY

#define AI_MAX_DATA_IDX                        (DATA_INPUT_USER * AXIS_NUMBER)
#define AI_DEFAULTSIMILARITY_THRESHOLD         90

#define AI_TASK_IS_SIGNAL_READY(pObj)          ((pObj)->m_nDataIdx >= AI_MAX_DATA_IDX)

#define SYS_DEBUGF(level, message)             SYS_DEBUGF3(SYS_DBG_AI, level, message)


/**
 * AITask virtual table.
 */
static const AManagedTaskEx_vtbl s_xAITask_vtbl = {
    AITask_vtblHardwareInit,
    AITask_vtblOnCreateTask,
    AITask_vtblDoEnterPowerMode,
    AITask_vtblHandleError,
    AITask_vtblForceExecuteStep
};

/**
 * AITask Sensor Event Listener IF virtual table.
 */
static const ISensorEventListener_vtbl s_xAITaskSEL_vtbl = {
    AITaskSEL_vtblOnStatusChange,
    AITaskSEL_vtblSetOwner,
    AITaskSEL_vtblGetOwner,
    AITaskSEL_vtblOnNewDataReady
};

/**
 * Specifies the execution context for the learning and detection phase.
 * It is a set of parameters controlling the execution of each phase.
 */
typedef struct _AICmdExecutionContext {

  /**
   * Specifies the duration in ms of the next phase (learning or detection)l.
   */
  TickType_t nTimerPeriodMS;

  /**
   * Specifies the number of signals (Frame) to use in the next phase.
   */
  uint32_t nSignals;

  /**
   * Specifies the NanoEdge AI function to use when a new signal is ready.
   * It can be on of:
   * - NanoEdgeAI_learn
   * - NanoEdgeAI_detect
   */
  uint8_t (*pfNeaiMode)(float data_input[]);

}AICmdExecutionContext;

typedef struct _AISensorListener {
  ISensorEventListener super;

  void *m_pxOwner;
} AISensorListener;

/**
 *  AITask internal structure.
 */
struct _AITask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * Specifies the task input queue used to send command to the task.
   */
  QueueHandle_t m_xInQueue;

  /**
   * Specifies a recursive mutex used to send a command in a synchronous way.
   */
  SemaphoreHandle_t m_xSyncCmdMutex;

  /**
   * Specifies a software timer used to stop the current detection or evaluation phase.
   */
  TimerHandle_t m_xStopTimer;

  /**
   * Specifies a pointer to a ::CircularBuffer used to get the data from the sensor and
   * asynchronously process them.
   */
  CircularBuffer *m_pxDataBuffer;

  /**
   * Specifies the buffer used by the data producer (e.g. the sensor task).
   */
  CBItem *m_pxProducerDataBuff;

  /**
   * Specifies the buffer used by the data consumer (e.g. the AITask to run the inference).
   */
  CBItem *m_pxConsumerDataBuff;

  /**
   * Specifies the index in the m_pxProducerDataBuff
   */
  uint32_t m_nDataIdx;

  /**
   * Specifies the state of the task. It can be "start" or "stop".
   */
  EAIState m_eState;

  /**
   * Used to count the frame during a detection or learning phase.
   */
  uint32_t m_nSignalCount;

  /**
   * Specifies the model sensitivity. A sensitivity value between 0 and 1 (excluded)
   * decreases the sensitivity of the AI model, while a value in between 1 and 100 increases it.
   */
  float m_fSensitivity;

  /**
   * Specifies the threshold to classify a signal as an anomaly based on the similarity percentage
   * returned by the NanoEdge AI detect algorithm.
   */
  uint8_t m_nThreshold;

  /**
   * Specifies the command context. The context can be modified any time. When the neai_start command is
   * execute a copy of the context is kept in the task stack and it is the actual context until the end
   * of the phase (learning / detecting).
   */
  AICmdExecutionContext m_xCmdContext;

  /**
   * Actual execution context. This is the execution context used during the learn or detect executoin phase.
   */
  AICmdExecutionContext m_xActualCmdContext;

  AISensorListener m_xSensorListener;
};

/**
 * The only instance of the task object.
 */
static AITask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t AITaskExecuteStepRun(AITask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void AITaskRun(void *pParams);

/**
 * Implements the AI_CMD_ID_SART command. It works according to the command execution context.
 * It start the timer for the time stamp, and the tasks for all the active sensors
 * according to the actual configuration.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param pxContext [IN] specify a pointer to the command execution context
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t AITaskStartImp(AITask *_this, const AICmdExecutionContext *pxContext);

/**
 * Implements the AI_CMD_ID_STOP command. It works according to the command execution context.
 * It stop the timer for the time stamp, and the tasks for all the active sensors
 * according to the actual configuration.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param pxContext [IN] specify a pointer to the command execution context
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
static sys_error_code_t AITaskStopImp(AITask *_this, const AICmdExecutionContext *pxContext);

/**
 * This function can be modified to specify the default commands execution context.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @return SYS_NO_ERROR_CODE
 */
static inline sys_error_code_t AITaskSetDefaultCmdExecutionContext(AITask *_this);

/**
 * Function called when the Stop timer expires.
 *
 * @param xTimer [IN] specifies an handle to the expired timer.
 */
static void AITimerStopCallbackFunction( TimerHandle_t xTimer);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
// Inline function defined inline in the header file AITask.h must be declared here as extern function.
#endif


// Public API definition
// *********************

AManagedTaskEx *AITaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xAITask_vtbl;
  s_xTaskObj.m_xSensorListener.super.vptr = &s_xAITaskSEL_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

sys_error_code_t AITaskSetMode(AITask *_this, const EAIMode eNewMode, TickType_t xTimeout) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  struct aiReport_t xCommand = {
      .reportId = HID_REPORT_ID_AI_CMD,
      .nCmdID = AI_CMD_ID_SET_MODE,
      .bSyncCmd = FALSE,
      .nParam = (uint32_t) eNewMode
  };

  if (SYS_IS_CALLED_FROM_ISR()) {
    if (xQueueSendToBackFromISR(_this->m_xInQueue, &xCommand, NULL) != pdPASS) {
      xRes = SYS_AI_TASK_IN_QUEUE_FULL_ERROR_CODE;
    }
  }
  else {
    if (xQueueSendToBack(_this->m_xInQueue, &xCommand, xTimeout) != pdTRUE) {
      xRes = SYS_TIMEOUT_ERROR_CODE;
    }
  }

  return xRes;
}

sys_error_code_t AITaskSendRawData(AITask *_this, const uint8_t *pnBuf, uint16_t nSize, double nTimeStamp) {
  assert_param(_this);
  assert_param(nSize <= DATA_INPUT_USER * AXIS_NUMBER * AI_RAW_AXIS_DATA_SIZE_B);
  assert_param(!( DATA_INPUT_USER % (nSize / AXIS_NUMBER / AI_RAW_AXIS_DATA_SIZE_B)));
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (_this->m_pxProducerDataBuff == NULL) {
    // try to get a new free buffer to acquire the next signal.
    xRes = CBGetFreeItemFromHead(_this->m_pxDataBuffer, &_this->m_pxProducerDataBuff);

    if (_this->m_pxProducerDataBuff != NULL) {
      // reset the index
      _this->m_nDataIdx = 0;
    }
  }

  if (_this->m_pxProducerDataBuff != NULL) {
    if(!AI_TASK_IS_SIGNAL_READY(_this)) {
      // copy the raw signal into the buffer to pass to Cartesiam.
      // we assume that m_fDataBuff size (that is the frame, or signal, size) is a multiple
      // of the number of sample read by the sensor (nSize/AI_RAW_AXIS_DATA_SIZE_B). So we start at pnBuf[_this->m_nDataIdx] and
      // copy all nSize data
      const uint32_t nDataCount = nSize / AXIS_NUMBER / AI_RAW_AXIS_DATA_SIZE_B;
      int16_t *pnSrc = (int16_t*)&pnBuf[0];
      float *pfDest = &(CBGetItemData(_this->m_pxProducerDataBuff)->pfBuff[_this->m_nDataIdx]);
      for (uint32_t i=0; i<nDataCount; ++i) {
        for (uint32_t j=0; j<AXIS_NUMBER; ++j) {
          pfDest[(i*3) + j] = ((float)*pnSrc++);
        }
      }
      // update m_nDataIdx for the next chunk of data
      _this->m_nDataIdx += (nDataCount * AXIS_NUMBER);

      // if the signal is ready, notify the task
      if (AI_TASK_IS_SIGNAL_READY(_this)) {
        // lock the acquisition task to avoid data new data form the sensor
        // will corrupt the signal before it is processed.
        // At the moment we lose data.
        _this->m_nDataIdx = AI_MAX_DATA_IDX + 1;

        // mark the data are ready to be consumed
        CBSetItemReady(_this->m_pxDataBuffer, _this->m_pxProducerDataBuff);
        // set the producer to NULL so we try to allocate a new buffer only the next time we have a new frame.
        // In this way the data consumer has more time to free a buffer
        _this->m_pxProducerDataBuff = NULL;

        // notify the AI task that there is a new signal to process.
        struct aiReport_t xCmd = {0};
        xCmd.reportId = HID_REPORT_ID_AI_CMD;
        xCmd.nCmdID = AI_CMD_ID_SIGNAL_READY;
        if (xQueueSendToBack(_this->m_xInQueue, &xCmd, pdMS_TO_TICKS(10)) != pdTRUE) {
          xRes = SYS_TIMEOUT_ERROR_CODE;
        }
      }
    }
  }

  return xRes;
}

EAIState AITaskGetState(const AITask *_this) {
  assert_param(_this);

  return _this->m_eState;
}

//sys_error_code_t AITaskGetExecutionContext(const AITask *_this, AIGetCmdResult *pExecutionContext) {
//  assert_param(_this);
//  assert_param(pExecutionContext);
//  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//
//  struct aiReport_t xCmd = {
//      .nCmdID = AI_CMD_ID_GET,
//      .bSyncCmd = TRUE,
//      .nParam = (uint32_t)(pExecutionContext)
//  };
//
//  BaseType_t xResult = pdPASS;
//  if (SYS_IS_CALLED_FROM_ISR()) {
//    xResult = xQueueSendToBackFromISR(_this->m_xInQueue, &xCmd, NULL);
//  }
//  else {
//    xResult = xQueueSendToBack(_this->m_xInQueue, &xCmd, pdMS_TO_TICKS(100));
//  }
//
//  if (pdTRUE != xResult) {
//    xRes = SYS_TIMEOUT_ERROR_CODE;
//  }
//  else {
//    // the message has been queued, wait for the result.
//    if (SYS_IS_CALLED_FROM_ISR()) {
//      xResult = xSemaphoreTakeFromISR(_this->m_xSyncCmdMutex, NULL);
//    }
//    else {
//      xResult = xSemaphoreTake(_this->m_xSyncCmdMutex, pdMS_TO_TICKS(100));
//    }
//
//    if (pdTRUE != xResult) {
//      xRes = SYS_TIMEOUT_ERROR_CODE;
//    }
//  }
//
//  return xRes;
//}

sys_error_code_t AITtaskStop(const AITask *_this, TickType_t xTimeout) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  struct aiReport_t xCommand = {0};
  xCommand.reportId = HID_REPORT_ID_AI_CMD;
  xCommand.nCmdID = AI_CMD_ID_STOP;

  if (SYS_IS_CALLED_FROM_ISR()) {
    if (pdTRUE != xQueueSendToBackFromISR(_this->m_xInQueue, &xCommand, NULL)) {
      xRes = SYS_TIMEOUT_ERROR_CODE;
    }
  }
  else {
    if (pdTRUE != xQueueSendToBack(_this->m_xInQueue, &xCommand, xTimeout)) {
      xRes = SYS_TIMEOUT_ERROR_CODE;
    }
  }

  return xRes;
}

sys_error_code_t AITtaskStart(const AITask *_this, TickType_t xTimeout) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  struct aiReport_t xCommand = {0};
  xCommand.reportId = HID_REPORT_ID_AI_CMD;
  xCommand.nCmdID = AI_CMD_ID_START;

  if (SYS_IS_CALLED_FROM_ISR()) {
    if (pdTRUE != xQueueSendToBackFromISR(_this->m_xInQueue, &xCommand, NULL)) {
      xRes = SYS_TIMEOUT_ERROR_CODE;
    }
  }
  else {
    if (pdTRUE != xQueueSendToBack(_this->m_xInQueue, &xCommand, xTimeout)) {
      xRes = SYS_TIMEOUT_ERROR_CODE;
    }
  }

  return xRes;
}

IEventListener *AITaskGetEventListenrIF(const AITask *_this) {
  assert_param(_this);

  return (IEventListener*)&_this->m_xSensorListener;
}


// AManagedTask virtual functions definition
// *****************************************

sys_error_code_t AITask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  AITask *pObj = (AITask*)_this;
  UNUSED(_this);

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("AI: no hw to initialize.\r\n"));

  return xRes;
}

sys_error_code_t AITask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  AITask *pObj = (AITask*)_this;

  // create the input queue
  pObj->m_xInQueue = xQueueCreate(AI_TASK_CFG_IN_QUEUE_ITEM_COUNT, AI_TASK_CFG_IN_QUEUE_ITEM_SIZE);
  if (pObj->m_xInQueue == NULL) {
    xRes = SYS_AI_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_AI_TASK_INIT_ERROR_CODE);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xInQueue, "AI_Q");
#endif

  // create the binary semaphore for the command execution: neai_get
  pObj->m_xSyncCmdMutex = xSemaphoreCreateBinary();
  if (pObj->m_xSyncCmdMutex == NULL) {
    xRes = SYS_AI_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_AI_TASK_INIT_ERROR_CODE);
    return xRes;
  }

#ifdef DEBUG
  vQueueAddToRegistry(pObj->m_xSyncCmdMutex, "AI_S");
#endif

  // create the software timer: one-shot timer. The period is changed in the START command execution.
  pObj->m_xStopTimer = xTimerCreate("AITim", 1, pdFALSE, _this, AITimerStopCallbackFunction);
  if (pObj->m_xStopTimer == NULL) {
    xRes = SYS_AI_TASK_INIT_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_AI_TASK_INIT_ERROR_CODE);
    return xRes;
  }

  // take the ownership of the Sensor Event IF
  IEventListenerSetOwner((IEventListener*)&pObj->m_xSensorListener, pObj);

  pObj->m_nDataIdx = 0;
  pObj->m_eState = E_AI_STOP;
  pObj->m_nSignalCount = 0;
  pObj->m_fSensitivity = 1.0;
  pObj->m_nThreshold = AI_DEFAULTSIMILARITY_THRESHOLD;
  // Set the default value of the command execution context
  AITaskSetDefaultCmdExecutionContext(pObj);
  // alloc the CircularBuffer object used to store and process the signals
  pObj->m_pxDataBuffer = CBAlloc();
  CBInit(pObj->m_pxDataBuffer);
  pObj->m_pxConsumerDataBuff = NULL;
  pObj->m_pxProducerDataBuff = NULL;

  *pvTaskCode = AITaskRun;
  *pcName = "AI";
  *pnStackDepth = AI_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = AI_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t AITask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  AITask *pObj = (AITask*)_this;

  return xRes;
}

sys_error_code_t AITask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  AITask *pObj = (AITask*)_this;

  return xRes;
}

sys_error_code_t AITask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  AITask *pObj = (AITask*)_this;

  return xRes;
}


// IIListener virtual functions definition
// ***************************************

sys_error_code_t AITaskSEL_vtblOnStatusChange(IListener *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UNUSED(_this);

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AI.SEL: not implemented OnStatusChange.\r\n"));

  return xRes;
}


// IEventListener virtual functions definition
// *******************************************

void AITaskSEL_vtblSetOwner(IEventListener *_this, void *pxOwner) {
  assert_param(_this);
  AISensorListener *pObj = (AISensorListener*)_this;

  pObj->m_pxOwner = pxOwner;
}

void *AITaskSEL_vtblGetOwner(IEventListener *_this) {
  assert_param(_this);
  AISensorListener *pxObj = (AISensorListener*)_this;

  return pxObj->m_pxOwner;
}


// ISensorEventListener virtual functions definition
// *************************************************

sys_error_code_t AITaskSEL_vtblOnNewDataReady(IEventListener *_this, const SensorEvent *pxEvt) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  AITask *pxOwner = (AITask*)IEventListenerGetOwner(_this);

  if (pxOwner != NULL) {
    xRes = AITaskSendRawData(pxOwner, pxEvt->pData, pxEvt->nDataSize, pxEvt->fTimeStamp);
    if (SYS_NO_ERROR_CODE != xRes) {
      SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("AI: error OnNewDataReady - code: 0x%x\r\n", xRes));
    }
  }

//  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("AI: new data: %d bytes.\r\n", pxEvt->nDataSize));

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t AITaskExecuteStepRun(AITask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  TickType_t xTimeoutInTick = AI_TICK_TO_WAIT_IN_CMD_LOOP;
  struct aiReport_t xCommand = {0};

  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  if (pdTRUE == xQueueReceive(_this->m_xInQueue, &xCommand, xTimeoutInTick)) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);
    if (xCommand.reportId == HID_REPORT_ID_AI_CMD) {
      switch (xCommand.nCmdID) {

      case AI_CMD_ID_SET_TIMER:
        _this->m_xCmdContext.nTimerPeriodMS = xCommand.nParam;
        break;

      case AI_CMD_ID_SET_MODE:
        _this->m_xCmdContext.pfNeaiMode = (EAIMode)xCommand.nParam == E_AI_DETECTION ? NanoEdgeAI_detect : NanoEdgeAI_learn;
        break;

      case AI_CMD_ID_SET_SIGNALS:
        _this->m_xCmdContext.nSignals = xCommand.nParam;
        break;

      case AI_CMD_ID_SET_SENSITIVITY:
        // The sensitivity can be changed dynamically at any time, without having to go through a new
        // learning phase. Indeed, sensitivity has no influence on the knowledge acquired during the
        //learning steps; it only plays a role in the detection step.
        _this->m_fSensitivity = xCommand.fParam;
        NanoEdgeAI_set_sensitivity(_this->m_fSensitivity);
        break;

      case AI_CMD_ID_SET_THRESHOLD:
        // The threshold can be changed dynamically at any time, without having to go through a new
        // execution phase.
        _this->m_nThreshold = xCommand.nParam;

        break;

      case AI_CMD_ID_GET:
//        {
//          AIGetCmdResult *pData = (AIGetCmdResult*)xCommand.nParam;
//          pData->nSignals = xContext.nSignals;
//          pData->nTimerPeriodMS = xContext.nTimerPeriodMS;
//          pData->fSensitivity = pxTask->m_fSensitivity;
//          pData->nThreshold = pxTask->m_nThreshold;
//          if (xCommand.bSyncCmd) {
//            // release the recursive mutex
//            if (xSemaphoreGive(_this->m_xSyncCmdMutex) != pdTRUE) {
//              xRes = SYS_AI_TASK_CMD_ERROR_CODE;
//            }
//          }
//        }
        break;

      case AI_CMD_ID_START:
        if (_this->m_eState == E_AI_STOP) {
          _this->m_eState = E_AI_START;
          // prepare the execution context
          if ((EAIMode)xCommand.nParam != E_AI_MODE_NONE) {
            _this->m_xCmdContext.pfNeaiMode = (EAIMode)xCommand.nParam == E_AI_DETECTION ? NanoEdgeAI_detect : NanoEdgeAI_learn;
          }
//          if (_this->m_xCmdContext.pfNeaiMode == NanoEdgeAI_learn){
//            LCD_LearnPanel();
//          }
//          else if (_this->m_xCmdContext.pfNeaiMode == NanoEdgeAI_detect){
//            LCD_DetectionPanel();
//          }
          // copy the application context into the local copy in the task stack.
          memcpy(&_this->m_xActualCmdContext, &_this->m_xCmdContext, sizeof(AICmdExecutionContext));
          // complete the rest of the command execution
          xRes = AITaskStartImp(_this, &_this->m_xActualCmdContext);
        }
        break;

      case AI_CMD_ID_STOP:
        if(_this->m_eState == E_AI_START) {
          _this->m_eState = E_AI_STOP;
//          LCD_IdlePanel();
          // complete the rest of the command execution
          xRes = AITaskStopImp(_this, &_this->m_xActualCmdContext);
        }
        break;

      case AI_CMD_ID_INIT:
        NanoEdgeAI_initialize();
        break;

      case AI_CMD_ID_SIGNAL_READY:
        // get a READY item from the the CircularBuffer
        xRes = CBGetReadyItemFromTail(_this->m_pxDataBuffer, &_this->m_pxConsumerDataBuff);
        if (_this->m_pxConsumerDataBuff != NULL) {
          uint8_t nSimilarity = 0;
          // learn about the signal
          nSimilarity = _this->m_xActualCmdContext.pfNeaiMode(CBGetItemData(_this->m_pxConsumerDataBuff)->pfBuff);

          // release the buffer as soon as possible
          CBReleaseItem(_this->m_pxDataBuffer, _this->m_pxConsumerDataBuff);
          _this->m_pxConsumerDataBuff = NULL;

          ++_this->m_nSignalCount;
          if (_this->m_xActualCmdContext.nSignals && !(_this->m_nSignalCount < _this->m_xActualCmdContext.nSignals)) {
//            StopExecutionPhases();
          }

          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("AI: %lu signal similarity = %d.\r\n", (unsigned long)_this->m_nSignalCount, nSimilarity));

          // Send the output in the console
//          if (_this->m_xActualCmdContext.pfNeaiMode == NanoEdgeAI_detect) {
//  #ifdef TEST_UI
//            test = test > 99 ? 0 :  test + 1 ;
//            nSimilarity = test;
//  #endif
//            // we are running in detection mode, so we print the similarity only if we have a problem
//            if (nSimilarity < _this->m_nThreshold) {
//              // unknown signal...
//              printf("%lu signal similarity = %d.\r\n", (unsigned long)_this->m_nSignalCount, nSimilarity);
//              if (nSimilarity > 20 ) {
//                LCD_DetectionPanelLight(LCD_ORANGE_LIGHT);
//              }
//              else{
//                LCD_DetectionPanelLight(LCD_RED_LIGHT);
//              }
//            }
//            else{
//              LCD_DetectionPanelLight(LCD_GREEN_LIGHT);
//            }
//              LCD_DetectionPanelSimilarity(nSimilarity);
//              LCD_DetectionPanelSignalCount(pxTask->m_nSignalCount);
//          }
//          else {
//            printf("%lu signal learning: %s.\r\n", (unsigned long)_this->m_nSignalCount, nSimilarity ? "SUCCESS" : "FAIL");
//            LCD_LearnPanelStatus(nSimilarity == 1);
//            LCD_LearnPanelSignalCount(pxTask->m_nSignalCount);
//          }

  #ifdef DEBUG
//          //TODO: STF.Begin - blink the LED for debug
//          if (++nLedPrescaler > 10) {
//            nLedPrescaler = 0;
//  //            BSP_LED_Toggle(LED_GREEN);
//            GPIOG->ODR ^= GPIO_PIN_10;
//          }
//          // STF.End
  #endif

        }
      break;

      default:
        break;
      }

      if (xRes != SYS_NO_ERROR_CODE) {
//        printf("\r\nAItaskRun() error 0x%x\r\n\r\n", (unsigned int)xRes);
        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("AI: step run error 0x%x\r\n", (unsigned int)xRes));
      }
    }
  }

  return xRes;
}

static void AITaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  AITask *_this = (AITask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("AI: start.\r\n"));

  // initialize the AI library
  NanoEdgeAI_initialize();

  // validate the frame size
  // the frame size depends DATA_INPUT_USER. Recommended values are power of 2.
  // If the frame size is bigger than 256 (the maximum depth of the sensor queue)
  // than it should be, at least, a multiple of 256 otherwise some sensor data are lost.
  const uint32_t nDataInputUser = DATA_INPUT_USER;
  if (!((nDataInputUser != 0) && ((nDataInputUser & (nDataInputUser - 1)) == 0))) {
    printf("\r\nWARNING: the buffer size (DATA_INPUT_USER), is not a power of 2.\r\n");
    // frame size is not a power of 2.
    if ((nDataInputUser < 256) || ((nDataInputUser % 256) == 0)) {
      // frame size is multiple of 256
      printf("         However it is a multiple of the sensor queue, so the application\r\n         may works fine.\r\n\r\r$ ");
    }
    else {
      printf("         Moreover it is not a multiple of the sensor queue, so the application\r\n         will not work as expected.\r\n\r\r$ ");
    }
  }

  //TODO: STF.Begin
  // send a fake command to simulate the 'start neai_lear' command.
  if (SYS_NO_ERROR_CODE != AITaskSetMode(_this, E_AI_LEARNING, pdMS_TO_TICKS(100))) {
    sys_error_handler();
  }
  if (SYS_NO_ERROR_CODE != AITtaskStart(_this, pdMS_TO_TICKS(100))) {
    sys_error_handler();
  }
  // STF.End

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
        xRes = AITaskExecuteStepRun(_this);
        taskENTER_CRITICAL();
          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

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

static sys_error_code_t AITaskStartImp(AITask *_this, const AICmdExecutionContext *pxContext) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_nSignalCount = 0;
  _this->m_nDataIdx = 0;

  // start sequence for the sensor and the rest of the app.
//  const uint32_t nSensors = COM_GetDeviceDescriptor()->nSensor;
//  SM_TIM_Start();
//
//  for (int i = 0; i < nSensors; i++) {
//    if(COM_GetSensorStatus(i)->isActive) {
//      StartSensorThread(i);
//      sensor_first_dataReady[i] = 1;
//    }
//  }

  if (pxContext->nTimerPeriodMS) {
    // start the software stop timer
    xTimerChangePeriod(_this->m_xStopTimer, pdMS_TO_TICKS(pxContext->nTimerPeriodMS), pdMS_TO_TICKS(200));
  }

  return xRes;
}

static sys_error_code_t AITaskStopImp(AITask *_this, const AICmdExecutionContext *pxContext) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // stop the software timer
  if (pxContext->nTimerPeriodMS) {
    xTimerStop(_this->m_xStopTimer, pdMS_TO_TICKS(200));
  }

//  // stop sequence for the sensor and the rest of the app.
//  const uint32_t nSensors = COM_GetDeviceDescriptor()->nSensor;
//
//  for (int i = 0; i < nSensors; i++) {
//    if(COM_GetSensorStatus(i)->isActive) {
//      StopSensorThread(i);
//      sensor_first_dataReady[i] = 0;
//    }
//  }
//
//  SM_TIM_Stop();

  // reset the object state

  // reset the CircularBuffer
  CBInit(_this->m_pxDataBuffer);
  _this->m_pxConsumerDataBuff = NULL;
  _this->m_pxProducerDataBuff = NULL;
  _this->m_nDataIdx = 0;

  _this->m_nSignalCount = 0;

//  printf("NanoEdge AI: stopped\r\n\r\n$ ");
//  fflush(stdout);

  return xRes;
}

static void AITimerStopCallbackFunction( TimerHandle_t xTimer) {
  AITask *pObj = (AITask*) pvTimerGetTimerID(xTimer);

  UNUSED(pObj);

//  StopExecutionPhases();
}


// Private inline function definition
// **********************************

static inline sys_error_code_t AITaskSetDefaultCmdExecutionContext(AITask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_xCmdContext.nTimerPeriodMS = 0; // timer not active
  _this->m_xCmdContext.nSignals = 0; // infinite samples
  _this->m_xCmdContext.pfNeaiMode = NanoEdgeAI_learn;

  return xRes;
}
