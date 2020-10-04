/**
 ******************************************************************************
 * @file    sysinit.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Sep 6, 2016
 * @brief   System global initialization
 *
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#include "sysinit.h"
#include "syslowpower.h"
#include "sysdebug.h"
#include "NullErrorDelegate.h"
#include "SysDefPowerModeHelper.h"
#include "FreeRTOS.h"
//#include "task.h"//TODO: STF.Port - threadx
//#include "queue.h"//TODO: STF.Port - threadx
#include "tx_api.h"
#include "string.h"

#ifndef INIT_TASK_CFG_STACK_SIZE
#define INIT_TASK_CFG_STACK_SIZE               (140)
#endif
#define INIT_TASK_CFG_PRIORITY                 (configMAX_PRIORITIES - 1)
#define INIT_TASK_CFG_QUEUE_ITEM_SIZE          sizeof(SysEvent)
#ifndef INIT_TASK_CFG_QUEUE_LENGTH
#define INIT_TASK_CFG_QUEUE_LENGTH             16
#endif
#define INIT_TASK_CFG_PM_SWITCH_DELAY_MS       50

#ifndef INIT_TASK_CFG_ENABLE_BOOT_IF
#define INIT_TASK_CFG_ENABLE_BOOT_IF           0
#endif

#define SYS_DEBUGF(level, message) 			        SYS_DEBUGF3(SYS_DBG_INIT, level, message)

/**
 * Create a type name for _System.
 */
typedef struct _System System;

/**
 * The ODeV framework provide its services through the System object. It is an abstraction of an embedded application.
 */
struct _System{
  /**
   * Specifies the Init task handle.
   */
  TX_THREAD m_xInitTask;

  /**
   * Specifies the queue used to serialize the system request made by the application tasks.
   * The supported requests are:
   * - Power Mode Switch.
   * - Error.
   */
  QueueHandle_t m_xSysQueue;

  /**
   * Specifies the application specific error manager delegate object.
   */
  IApplicationErrorDelegate *m_pxAppErrorDelegate;

  /**
   * Specifies the application specific power mode helper object.
   */
  IAppPowerModeHelper *m_pxAppPowerModeHelper;

  /**
   *
   */
  void *pvFirstUnusedMemory;

#if INIT_TASK_CFG_ENABLE_BOOT_IF == 1
  /**
   * Specifies the application specific boot interface object.
   */
  IBoot *m_pxAppBootIF;
#endif

};


// Private variables for the Init Task.
// ************************************

/**
 * The only instance of System object.
 */
static System s_xTheSystem;

#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
/**
 * The FreeRTOS HEAP is allocated by the application so the system can initialize the heap memory at startup
 * in order to prevent some SRAM retention strange issue.
 */
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
#endif


// Private function declaration
// ****************************

/**
 * System Clock configuration procedure. It is provided by the CubeMX project.
 */
extern void SystemClock_Config(void);

/**
 * Configure the unused PIN in analog to minimize the power consumption, and enable the ultra low power mode.
 */
extern void SysPowerConfig();

/**
 * INIT task control loop. The INIT task is in charge of the system initialization.
 *
 * @param thread_input not used.
 */
static void InitTaskRun(ULONG thread_input);


// Public API definition
// *********************

__weak sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext) {
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);
  return SYS_TASK_INVALID_CALL_ERROR_CODE;
}

__weak sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext) {
  SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_TASK_INVALID_CALL_ERROR_CODE);
  return SYS_TASK_INVALID_CALL_ERROR_CODE;
}

sys_error_code_t SysInit(boolean_t bEnableBootIF) {
  // First step: initialize the minimum set of resources before passing the control
  // to the RTOS.

  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // Reset of all peripherals, Initializes the Flash interface and the Systick.
  HAL_Init();

  // Configure the system clock.
  SystemClock_Config();
  SysPowerConfig();

#if( configAPPLICATION_ALLOCATED_HEAP == 1 )
  // initialize the FreeRTOS heap.
  memset(ucHeap, 0, configTOTAL_HEAP_SIZE );
#endif

#if INIT_TASK_CFG_ENABLE_BOOT_IF == 1

  if (bEnableBootIF) {
    s_xTheSystem.m_pxAppBootIF = SysGetBootIF();
    if (s_xTheSystem.m_pxAppBootIF != NULL) {
      // initialize the BootIF
      xRes = IBootInit(s_xTheSystem.m_pxAppBootIF);
      // check the trigger condition
      if (!IBootCheckDFUTrigger(s_xTheSystem.m_pxAppBootIF)) {
        // prepare to jump to the main application
       uint32_t nAppAddress = IBootGetAppAdderss(s_xTheSystem.m_pxAppBootIF);
       xRes = IBootOnJampToApp(s_xTheSystem.m_pxAppBootIF, nAppAddress);
        if (!SYS_IS_ERROR_CODE(xRes) &&
            (((*(__IO uint32_t*)nAppAddress) & 0x2FFE0000 ) == 0x20000000)) {
          typedef void (*pFunction)(void);
          volatile uint32_t JumpAddress = *(__IO uint32_t*) (nAppAddress + 4);
          volatile pFunction JumpToApplication = (pFunction) JumpAddress;
          // initialize user application's Stack Pointer
          __set_MSP(*(__IO uint32_t*) nAppAddress);
          // jump to the user application
          JumpToApplication();
        }
      }
    }
  }

#endif  // INIT_TASK_CFG_ENABLE_BOOT_IF

#if (SYS_DBG_ENABLE_TA4 == 1)
  // The trace engine must be initialized here because in DEBUG configuration
  // a mutex object maybe created by the SysDebugInit.
   vTraceEnable(TRC_START_AWAIT_HOST);
#endif

#ifdef SYS_DEBUG
  SysDebugInit();
#ifdef DEBUG
  HAL_DBGMCU_EnableDBGStopMode();
  __HAL_DBGMCU_FREEZE_WWDG();
#endif // DEBUG
#endif // SYS_DEBUG

  // Clear the global error.
  SYS_CLEAR_ERROR();

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("System Initialization\r\n"));

  // Create the INIT task to complete the system initialization
  // after RTOS is started.

  // This is the case for FreeRTOS.
//  if (xTaskCreate(InitTaskRun, "INIT", INIT_TASK_CFG_STACK_SIZE, NULL, INIT_TASK_CFG_PRIORITY, &s_xTheSystem.m_xInitTask) != pdPASS) {
//    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
//    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
//  }

  // ThreadX use a different approach. After the scheduler is started, it will call the following function
  // that the application code must overwrite:
  // void tx_application_define(void *first_unused_memory)

  return xRes;
}

sys_error_code_t SysPostEvent(SysEvent xEvent) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  BaseType_t xResult;

  if (SYS_IS_ERROR_EVENT(xEvent)) {
    // notify the error delegate to allow a first response to critical errors.
    IAEDOnNewErrEvent(s_xTheSystem.m_pxAppErrorDelegate, xEvent);
  }

  if (SYS_IS_CALLED_FROM_ISR()) {
    xResult = xQueueSendToBackFromISR(s_xTheSystem.m_xSysQueue, &xEvent, NULL);
  }
  else {
    xResult = xQueueSendToBack(s_xTheSystem.m_xSysQueue, &xEvent, pdMS_TO_TICKS(50));
  }

  if (xResult == errQUEUE_FULL) {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INIT_TASK_POWER_MODE_NOT_ENABLE_ERROR_CODE);
    xRes = SYS_INIT_TASK_POWER_MODE_NOT_ENABLE_ERROR_CODE;
  }

  return xRes;
}

EPowerMode SysGetPowerMode() {
  return IapmhGetActivePowerMode(s_xTheSystem.m_pxAppPowerModeHelper);
}

sys_error_code_t SysTaskErrorHandler(AManagedTask *pxTask) {
  assert_param(pxTask);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // TODO: STF - how to handle the shutdown of the task?

  vTaskSuspend(pxTask->m_xThaskHandle);

  return xRes;
}

SysPowerStatus SysGetPowerStatus() {
  return IapmhGetPowerStatus(s_xTheSystem.m_pxAppPowerModeHelper);
}

void SysResetAEDCounter() {
  IAEDResetCounter(s_xTheSystem.m_pxAppErrorDelegate);
}

boolean_t SysEventsPending() {
  if (SYS_IS_CALLED_FROM_ISR()) {
    return uxQueueMessagesWaitingFromISR(s_xTheSystem.m_xSysQueue) > 0;
  }
  else {
    return uxQueueMessagesWaiting(s_xTheSystem.m_xSysQueue) > 0;
  }
}

__weak IApplicationErrorDelegate *SysGetErrorDelegate() {

  return NullAEDAlloc();
}

__weak IBoot *SysGetBootIF() {
  return NULL;
}

__weak IAppPowerModeHelper *SysGetPowerModeHelper() {

  return SysDefPowerModeHelperAlloc();
}


// Private functions definition
// ****************************

/**
 * INIT task control function.
 * The INIT task is the first created and running task. It is responsible to complete
 * the system initialization and to create all other system task.
 *
 * @param pParams not used
 */
static void InitTaskRun(ULONG thread_input) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  BaseType_t xRtosRes;
  UNUSED(thread_input);

  vTaskSuspendAll();

  // Create the queue for the system message.
  s_xTheSystem.m_xSysQueue = xQueueCreate(INIT_TASK_CFG_QUEUE_LENGTH, INIT_TASK_CFG_QUEUE_ITEM_SIZE);
  if (s_xTheSystem.m_xSysQueue == NULL) {
    // if s_xSysQueue is NULL then the execution is blocked by sys_error_handler().
    // see bugtabs4 #5265 (WGID:201282)
    sys_error_handler();
  }

#ifdef DEBUG
    vQueueAddToRegistry(s_xTheSystem.m_xSysQueue, "SYS_Q");
#endif

  // Check if the system has resumed from WWDG reset
  if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST) != RESET) {
    __asm volatile( "NOP" );

    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("INIT: start after WWDG reset!\r\n"));
  }
  // Check if the system has resumed from the Option Byte loading occurred
  //TODO: STF.Port - threadx - Check the Option Bytes for F4
//  if (__HAL_RCC_GET_FLAG(RCC_FLAG_OBLRST) != RESET) {
//    HAL_FLASH_OB_Lock();
//    HAL_FLASH_Lock();
//
//    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("INIT: start after OB reset!\r\n"));
//  }

  // check the reset flags
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("INIT: reset flags: 0x%x\r\n", READ_BIT(RCC->CSR, 0xFF000000)));

  // Clear reset flags in any case
  __HAL_RCC_CLEAR_RESET_FLAGS();

  // Get the default application error manager delegate
  s_xTheSystem.m_pxAppErrorDelegate = SysGetErrorDelegate();
  // initialize the application error manager delegate
  xRes = IAEDInit(s_xTheSystem.m_pxAppErrorDelegate, NULL);
  if (SYS_IS_ERROR_CODE(xRes)) {
    sys_error_handler();
  }

    // Get the default Power Mode Helper object
  s_xTheSystem.m_pxAppPowerModeHelper = SysGetPowerModeHelper();
  // Initialize the Power Mode Helper object
  xRes = IapmhInit(s_xTheSystem.m_pxAppPowerModeHelper);
  if (SYS_IS_ERROR_CODE(xRes)) {
    sys_error_handler();
  }

  // Allocate the global application context
  ApplicationContext xContext;
  // Initialize the context
  ACInit(&xContext);

  xRes = SysLoadApplicationContext(&xContext);
  if (xRes != SYS_NO_ERROR_CODE) {
    // it seems that there is no application to run!!!
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("INIT: no application tasks loaded!\r\n"));
    sys_error_handler();
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("INIT: added %i managed tasks.\r\n", ACGetTaskCount(&xContext)));

  // Initialize the hardware resource for all tasks
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("INIT: task hardware initialization.\r\n"));

  AManagedTask *pTask = ACGetFirstTask(&xContext);
  while (pTask != NULL && !SYS_IS_ERROR_CODE(xRes)) {
    xRes = AMTHardwareInit(pTask, NULL);
    if (SYS_IS_ERROR_CODE(xRes)) {
       SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INIT_TASK_FAILURE_ERROR_CODE);
       SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("\r\nINIT: system failure.\r\n"));
    }
    else {
      pTask = ACGetNextTask(&xContext, pTask);

      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("...\r\n"));
    }
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("DONE.\r\n"));

  // Create the application tasks
  TaskFunction_t pvTaskCode;
  const char *pcName;
  unsigned short nStackDepth;
  void *pTaskParams;
  UBaseType_t xPriority;

  pTask = ACGetFirstTask(&xContext);
  while (pTask != NULL && !SYS_IS_ERROR_CODE(xRes)) {
    xRes = AMTOnCreateTask(pTask, &pvTaskCode, &pcName, &nStackDepth, &pTaskParams, &xPriority);
    if (SYS_IS_ERROR_CODE(xRes)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INIT_TASK_FAILURE_ERROR_CODE);
      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("INIT: system failure.\r\n"));
    } else {
      xRtosRes = xTaskCreate(pvTaskCode, pcName, nStackDepth, pTaskParams, xPriority, &pTask->m_xThaskHandle);
      if(xRtosRes != pdPASS) {
        SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INIT_TASK_FAILURE_ERROR_CODE);
        SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("INIT: unable to create task %s.\r\n", pcName));
      }
    }
    pTask = ACGetNextTask(&xContext, pTask);
  }

  SysOnStartApplication(&xContext);
  IAEDOnStartApplication(s_xTheSystem.m_pxAppErrorDelegate, &xContext);

  SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("INIT: system initialized.\r\n"));

#if defined(DEBUG) || defined(SYS_DEBUG)
  if (SYS_DBG_LEVEL_SL >= g_sys_dbg_min_level) {
    size_t nFreeHeapSize = xPortGetFreeHeapSize();
    SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("INIT: free heap = %i.\r\n", nFreeHeapSize));
    SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("INIT: SystemCoreClock = %iHz.\r\n", SystemCoreClock));
  }
#endif

  xTaskResumeAll();

  // After the system initialization the INIT task is used to implement some system call
  // because it is the owner of the Application Context.
  // At the moment this is an initial implementation of a system level Power Management:
  // wait for a system level power mode request
  SysEvent xEvent;
  for (;;) {
    if (pdTRUE == xQueueReceive(s_xTheSystem.m_xSysQueue, &xEvent, portMAX_DELAY)) {
      EPowerMode eActivePowerMode = IapmhGetActivePowerMode(s_xTheSystem.m_pxAppPowerModeHelper);
      // check if it is a system error event
      if (SYS_IS_ERROR_EVENT(xEvent)) {
        IAEDProcessEvent(s_xTheSystem.m_pxAppErrorDelegate, &xContext, xEvent);
        // check if the system is in low power mode and it was waked up by a strange IRQ.
        if (IapmhIsLowPowerMode(s_xTheSystem.m_pxAppPowerModeHelper, eActivePowerMode)) {
          // if the system was wake up due to an error event, then wait the error is recovered before put the MCU in STOP
          if (!IAEDIsLastErrorPending(s_xTheSystem.m_pxAppErrorDelegate)) {
            // then put the system again in low power mode.
            IapmhDidEnterPowerMode(s_xTheSystem.m_pxAppPowerModeHelper, eActivePowerMode);
          }
        }
      }
      else {
        // it is a power mode event
        EPowerMode ePowerMode = IapmhComputeNewPowerMode(s_xTheSystem.m_pxAppPowerModeHelper, xEvent);
        if (ePowerMode != eActivePowerMode) {
          IapmhCheckPowerModeTransaction(s_xTheSystem.m_pxAppPowerModeHelper, eActivePowerMode, ePowerMode);

          SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("INIT: evt:src=%x evt:param=%x\r\n", xEvent.xEvent.nSource, xEvent.xEvent.nParam));

          // Forward the request to all managed tasks
          boolean_t bDelayPowerModeSwitch;
          do {
            bDelayPowerModeSwitch = FALSE;
            pTask = ACGetFirstTask(&xContext);
            for (; pTask!=NULL; pTask=ACGetNextTask(&xContext, pTask)) {
              // notify the task that the power mode is changing,
              // so the task will suspend.
              pTask->m_xStatus.nPowerModeSwitchPending = 1;
              if (pTask->m_xStatus.nPowerModeSwitchDone == 0) {
                if ((pTask->m_xStatus.nDelayPowerModeSwitch == 0)) {
                  AMTDoEnterPowerMode(pTask, eActivePowerMode, ePowerMode);
                  pTask->m_xStatus.nPowerModeSwitchDone = 1;
                  pTask->m_xStatus.nIsTaskStillRunning = 1;
                }
                else {
                  // check if it is an Extended Managed Task
                  if (pTask->m_xStatus.nReserved == 1) {
                    // force the step execution to prepare the task for the power mode switch.
                    AMTExForceExecuteStep((AManagedTaskEx*)pTask, eActivePowerMode);
                  }
                  bDelayPowerModeSwitch = TRUE;
                }
              }
            }

            if (bDelayPowerModeSwitch == TRUE) {
              vTaskDelay(pdMS_TO_TICKS(INIT_TASK_CFG_PM_SWITCH_DELAY_MS));
            }

          } while (bDelayPowerModeSwitch == TRUE);

          // Enter the specified power mode
          IapmhDidEnterPowerMode(s_xTheSystem.m_pxAppPowerModeHelper, ePowerMode);

          pTask = ACGetFirstTask(&xContext);
          for (; pTask!=NULL; pTask=ACGetNextTask(&xContext, pTask)) {
            pTask->m_xStatus.nPowerModeSwitchDone = 0;
            pTask->m_xStatus.nPowerModeSwitchPending = 0;
            vTaskResume(pTask->m_xThaskHandle);
          }
        }
        else {
          // check if the system is in a low power mode and it was waked up by a strange IRQ.
          if (IapmhIsLowPowerMode(s_xTheSystem.m_pxAppPowerModeHelper, eActivePowerMode)) {
            // then put the system again in low power mode.
            IapmhDidEnterPowerMode(s_xTheSystem.m_pxAppPowerModeHelper, ePowerMode);
          }
        }
      }
    }
  }
}


// ThreadX integration
// *******************

void tx_application_define(void *first_unused_memory) {
  UINT nRes = TX_SUCCESS;
  // create the INIT task.

  //  if (xTaskCreate(InitTaskRun, "INIT", INIT_TASK_CFG_STACK_SIZE, NULL, INIT_TASK_CFG_PRIORITY, &s_xTheSystem.m_xInitTask) != pdPASS) {
  //    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  //    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
  //  }

  s_xTheSystem.pvFirstUnusedMemory = first_unused_memory;
  nRes = tx_thread_create(&s_xTheSystem.m_xInitTask, "INIT", InitTaskRun, ODEV_MAGIC_NUMBER, s_xTheSystem.pvFirstUnusedMemory, INIT_TASK_CFG_STACK_SIZE * 4, INIT_TASK_CFG_PRIORITY, 3, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (nRes != TX_SUCCESS) {
    sys_error_handler();
  }
  s_xTheSystem.pvFirstUnusedMemory += INIT_TASK_CFG_STACK_SIZE * 4;
}
