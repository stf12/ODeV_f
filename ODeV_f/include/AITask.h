/**
 ******************************************************************************
 * @file    AITask.h
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
#ifndef AITASK_H_
#define AITASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "ISensorEventListener.h"
#include "ISensorEventListener_vtbl.h"
#include "NanoEdgeAI.h"


// Command ID. These are all commands supported by the Predictive Maintenance Task.
// The task command queue is not directly accessible trough the public API, but the
// user must use the the CLI or the public API to send commands.
#define AI_CMD_ID_SET_MODE                ((uint16_t)0x0001)              ///< SET_MODE command ID. For more information see PMTaskSetMode() function.
#define AI_CMD_ID_SET_TIMER               ((uint16_t)0x0002)              ///< SET_TIMER command ID. Set a timer in ms to stop an execution phase.
#define AI_CMD_ID_SET_SIGNALS             ((uint16_t)0x0003)              ///< SET_SIGNALS command ID. Set the number of signals to process during an execution phase.
#define AI_CMD_ID_SET_THRESHOLD           ((uint16_t)0x0004)              ///< SET_THRESHOLD command ID. Set the threshold used by the AI library to classify the good and bad signals.
#define AI_CMD_ID_SET_SENSITIVITY         ((uint16_t)0x0005)              ///< SET_SENSITIVITY command ID. Set the sensitivity of the AI library.
#define AI_CMD_ID_GET                     ((uint16_t)0x0006)              ///< GET command ID. Get the value of the AI library parameters. For more information see PMTaskGetExecutionContext() function.
#define AI_CMD_ID_SIGNAL_READY            ((uint16_t)0x0007)              ///< SIGNAL_READY command ID. Used to notify the task that a new signal is ready in the ::CircularBuffer to be processed by the AI library.
#define AI_CMD_ID_START                   ((uint16_t)0x0008)              ///< START command ID. Start an execution phase according to the the ::PMCmdExecutionContext
#define AI_CMD_ID_STOP                    ((uint16_t)0x0009)              ///< STOP command ID. Stop the execution phase.
#define AI_CMD_ID_INIT                    ((uint16_t)0x000A)              ///< INIT command ID. Initialize the model of the AI library.
#define AI_CMD_ID_LAST_COMMAND            AI_CMD_ID_INIT                  ///< Specifies the last valid command ID.
#define AI_CMD_ID_INVALID                 (AI_CMD_ID_LAST_COMMAND + 1)    ///< Specifies the first invalid command ID.


#define AI_RAW_AXIS_DATA_SIZE_B           2 ///< data coming form LSM6DSO accelerometer are 16 bit
#define AI_MAX_SIGNALS_PARAM              0x100000U ///< Max number of signals to pass as parameter to the "neai_set signals <value>" command
#define AI_MAX_TIME_MS_PARAM              (1000U*60U*60U*24U*2U) ///< Man time in ms to pass to as parameter to the "neai_set timer <value>" command. It is 2 days.


/**
 * Create  type name for _AITask.
 */
typedef struct _AITask AITask;

/**
 * Specifies the execution mode for the task. The execution mode tell the task what to do when it is running
 * and new signals are provided by the sensor subsystem in order to be processed by the AI library.
 */
typedef enum _EAIode {
  E_AI_MODE_NONE = 0,//!< no execution mode is selected.
  E_AI_LEARNING,     //!< the AI library learn from the signals.
  E_AI_DETECTION     //!< the AI library detect the similarity of the signal with the learned model.
} EAIMode;

/**
 * Specifies the possible state of the task.
 */
typedef enum _EAIState {
  /**
   * The task waits for signals form the sensor subsystem in order to process them with the AI library.
   * When the task is in this state it is not possible to change the execution context.
   */
  E_AI_START = 0,

  /**
   *  The task does not process signals. In this state it is possible to change the execution context.
   */
  E_AI_STOP
} EAIState;


// Public API declaration
//***********************

/**
 * Allocate an instance of AITask.
 *
 * @return a pointer to the generic obejct ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *AITaskAlloc();

/**
 * Send an asynchronous PM_CMD_ID_SET_MODE command to the task input queue. It used to enable one of the two execution
 * phases regarding the Predictive Maintenance task:
 * - NanoEdgeAi learning
 * - NanoEdgeAi detection
 * This method can be called from an ISR
 * or a task code. In the first case, the timeout value is ignored. In the last case if the the task queue is not accessible
 * the method return SYS_PM_TASK_IN_QUEUE_FULL_ERROR_CODE.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param eNewMode [IN] specifies an operating mode (or the execution phase).
 * @param xTimeout [IN] specifies a timeout in tick.
 * @return SYS_NO_ERROR_CODE if success, or one the following error code:
 *         - SYS_PM_TASK_IN_QUEUE_FULL_ERROR_CODE if error and the method is called from an ISR.
 *         - SYS_TIMEOUT_ERROR_CODE if error and the method is called from a task code.
 */
sys_error_code_t AITaskSetMode(AITask *_this, const EAIMode eNewMode, TickType_t xTimeout);

/**
 * Synchronously send a buffer of raw data to the task object. The data are assembled to create a signal to be processed
 * by the AI library.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param pnBuf [IN] specifies the buffer with the raw data. it is formatted as
 * [D11, D12, ... D1n, D21, D22, D2n, ... Dm1, Dm2, ... Dmn] where
 * n is the number the number of axis (AXIS_NUMBER) and m is the frame size.
 * @param nSize [IN] specifies the size in byte of the buffer
 * @param nTimeStamp [IN] specifies the time stamp of the bunch of data
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t AITaskSendRawData(AITask *_this, const uint8_t *pnBuf, uint16_t nSize, double nTimeStamp);

/**
 * Get the actual state of the task object:
 * - E_AI_START: an execution phase is running.
 * - E_AI_STOP: no execution phase are running.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @return the actual state of the task object.
 */
EAIState AITaskGetState(const AITask *_this);

/**
 * Send a synchronous PM_CMD_ID_GET command to the task input queue. This method can be called from an ISR
 * or a task code. In the first case, the timeout value is ignored. In the last case if the the task queue is not accessible
 * the method return SYS_PM_TASK_IN_QUEUE_FULL_ERROR_CODE.
 *
 * If success, get a copy of the full execution context in the output buffer pExecutionContext.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param pExecutionContext [OUT] specifies a buffer to retrieve the execution context.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
//sys_error_code_t AITaskGetExecutionContext(const AITask *_this, PMGetCmdResult *pExecutionContext);

/**
 * Send an asynchronous PM_CMD_ID_STOP command into the task input queue.
 * This function can be called from an ISR or a task code.
 * In the first case, the timeout value is ignored. In the last case if the the task queue is not accessible
 * the method return a generic SYS_PM_TASK_IN_QUEUE_FULL_ERROR_CODE.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param xTimeout [IN] specifies a timeout in tick.
 * @return SYS_NO_ERROR_CODE if success, an application specific erro code otherwise.
 */
sys_error_code_t AITtaskStop(const AITask *_this, TickType_t xTimeout);

/**
 * Send an asynchronous PM_CMD_ID_START command into the task input queue.
 * This function can be called from an ISR or a task code.
 * In the first case, the timeout value is ignored. In the last case if the the task queue is not accessible
 * the method return a generic SYS_PM_TASK_IN_QUEUE_FULL_ERROR_CODE.
 *
 * @param _this [IN] specifies a pointer to the task object.
 * @param xTimeout [IN] specifies a timeout in tick.
 * @return SYS_NO_ERROR_CODE if success, an application specific erro code otherwise.
 */
sys_error_code_t AITtaskStart(const AITask *_this, TickType_t xTimeout);

/**
 * Get the ::ISensorEventListener interface of the task object.
 *
 * @param _this  [IN] specifies a pointer to the task object.
 * @return a pointer to the ::ISensorEventListener of the task object.
 */
IEventListener *AITaskGetEventListenrIF(const AITask *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* AITASK_H_ */
