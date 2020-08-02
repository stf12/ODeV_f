/**
 ******************************************************************************
 * @file    HelloWorldTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 2, 2018
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
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

#include "HelloWorldTask.h"
#include "HelloWorldTask_vtbl.h"
#include "sysdebug.h"


#ifndef HW_TASK_CFG_STACK_DEPTH
#define HW_TASK_CFG_STACK_DEPTH         120
#endif

#ifndef HW_TASK_CFG_PRIORITY
#define HW_TASK_CFG_PRIORITY            (tskIDLE_PRIORITY+1)
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_HW, level, message)


/**
 * HelloWorldTask Driver virtual table.
 */
static const AManagedTask_vtbl s_xHelloWorldTask_vtbl = {
  HelloWorldTask_vtblHardwareInit,
  HelloWorldTask_vtblOnCreateTask,
  HelloWorldTask_vtblDoEnterPowerMode,
  HelloWorldTask_vtblHandleError,
};

/**
 * The only instance of the task object.
 */
static HelloWorldTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t HelloWorldTaskExecuteStepRun(HelloWorldTask *this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void HelloWorldTaskRun(void *pParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
#endif


// Public API definition
// *********************

AManagedTask *HelloWorldTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInit(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xHelloWorldTask_vtbl;

  return (AManagedTask*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t HelloWorldTask_vtblHardwareInit(AManagedTask *this, void *pParams) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HelloWorldTask *pObj = (HelloWorldTask*)this;

//  pObj->m_pxDriver = NucleoDriverAlloc();
//  if (pObj->m_pxDriver == NULL) {
//    SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("HW task: unable to alloc driver object.\r\n"));
//    xRes = SYS_GET_LAST_LOW_LEVEL_ERROR_CODE();
//  }
//  else {
//    xRes = IDrvInit(pObj->m_pxDriver, NULL);
//    if (SYS_IS_ERROR_CODE(xRes)) {
//      SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("Keyboard task: error during driver initialization\r\n"));
//    }
//  }

  return xRes;
}

sys_error_code_t HelloWorldTask_vtblOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  HelloWorldTask *pObj = (HelloWorldTask*)this;


 *pvTaskCode = HelloWorldTaskRun;
  *pcName = "HW";
  *pnStackDepth = HW_TASK_CFG_STACK_DEPTH;
  *pParams = this;
  *pxPriority = HW_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t HelloWorldTask_vtblDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HelloWorldTask *pObj = (HelloWorldTask*)this;

  if (eNewPowerMode == E_POWER_MODE_SLEEP_1) {
//    NucleoDriverSetLed((NucleoDriver*)pObj->m_pxDriver, FALSE);
  }

  return xRes;
}

sys_error_code_t HelloWorldTask_vtblHandleError(AManagedTask *this, SysEvent xError) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  HelloWorldTask *pObj = (HelloWorldTask*)this;

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t HelloWorldTaskExecuteStepRun(HelloWorldTask *this) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  vTaskDelay(pdMS_TO_TICKS(1000));
//  NucleoDriverToggleLed((NucleoDriver*)this->m_pxDriver);
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("Hello STWIN!!\r\n"));
  __NOP();
  __NOP();

  return xRes;
}

static void HelloWorldTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  HelloWorldTask *this = (HelloWorldTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HW: start.\r\n"));

  for (;;) {

    // check if there is a pending power mode switch request
    if (this->super.m_xStatus.nPowerModeSwitchPending == 1) {
      // clear the power mode switch delay because the task is ready to switch.
      taskENTER_CRITICAL();
        this->super.m_xStatus.nDelayPowerModeSwitch = 0;
      taskEXIT_CRITICAL();
      vTaskSuspend(NULL);
    }
    else {
      switch (AMTGetSystemPowerMode()) {
      case E_POWER_MODE_RUN:
        taskENTER_CRITICAL();
          this->super.m_xStatus.nDelayPowerModeSwitch = 1;
        taskEXIT_CRITICAL();
        xRes = HelloWorldTaskExecuteStepRun(this);
        taskENTER_CRITICAL();
          this->super.m_xStatus.nDelayPowerModeSwitch = 0;
        taskEXIT_CRITICAL();
        break;

      case E_POWER_MODE_SLEEP_1:
        vTaskDelay(pdMS_TO_TICKS(100));
        break;
      }
    }

    // notify the system that the task is working fine.
    AMTNotifyIsStillRunning((AManagedTask*)this, xRes);

#if (SYS_TRACE > 1)
    if (xRes != SYS_NO_ERROR_CODE) {
      sys_check_error_code(xRes);
      sys_error_handler();
    }
#endif
  }
}
