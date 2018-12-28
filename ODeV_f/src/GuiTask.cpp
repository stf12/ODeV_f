/**
 ******************************************************************************
 * @file    GuiTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Dec 19, 2018
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

#include "GuiTask.h"
#include "GuiTask_vtbl.h"
#include "sysdebug.h"

#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/hal/HAL.hpp>

#ifndef GUI_TASK_CFG_STACK_DEPTH
#define GUI_TASK_CFG_STACK_DEPTH      (950)
#endif

#ifndef GUI_TASK_CFG_PRIORITY
#define GUI_TASK_CFG_PRIORITY         (tskIDLE_PRIORITY+3)
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_GUI, level, message)


/**
 * GuiTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xGuiTask_vtbl = {
    GuiTask_vtblHardwareInit,
    GuiTask_vtblOnCreateTask,
    GuiTask_vtblDoEnterPowerMode,
    GuiTask_vtblHandleError,
    GuiTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static GuiTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
//static sys_error_code_t GuiTaskExecuteStepRun(GuiTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void GuiTaskRun(void *pParams);


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
#endif


// Public API definition
// *********************

AManagedTaskEx *GuiTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xGuiTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t GuiTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  GuiTask *pObj = (GuiTask*)_this;

  touchgfx::hw_init_odev();
  touchgfx::touchgfx_init();

  return xRes;
}

sys_error_code_t GuiTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  GuiTask *pObj = (GuiTask*)_this;

  *pvTaskCode = GuiTaskRun;
  *pcName = "GUI";
  *pnStackDepth = GUI_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = GUI_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t GuiTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  GuiTask *pObj = (GuiTask*)_this;

  return xRes;
}

sys_error_code_t GuiTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  GuiTask *pObj = (GuiTask*)_this;

  return xRes;
}

sys_error_code_t GuiTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  GuiTask *pObj = (GuiTask*)_this;

  return xRes;
}


// Private function definition
// ***************************

//static sys_error_code_t GuiTaskExecuteStepRun(GuiTask *_this) {
//  assert_param(_this);
//  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//
//  return xRes;
//}

static void GuiTaskRun(void *pParams) {
//  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  GuiTask *_this = (GuiTask*)pParams;
  UNUSED(pParams);

  vTaskDelay(pdMS_TO_TICKS(2000));

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("GUI: start.\r\n"));

//  AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
  touchgfx::HAL::getInstance()->taskEntry();

//  while (1) {
//    vTaskSuspend(NULL);
//  }

//  for (;;) {
//
//    // check if there is a pending power mode switch request
//    if (_this->super.m_xStatus.nPowerModeSwitchPending == 1) {
//      // clear the power mode switch delay because the task is ready to switch.
//      taskENTER_CRITICAL();
//        _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
//      taskEXIT_CRITICAL();
//      vTaskSuspend(NULL);
//    }
//    else {
//      switch (AMTGetSystemPowerMode()) {
//      case E_POWER_MODE_RUN:
//        taskENTER_CRITICAL();
//          _this->super.m_xStatus.nDelayPowerModeSwitch = 1;
//        taskEXIT_CRITICAL();
//        xRes = GuiTaskExecuteStepRun(_this);
//        taskENTER_CRITICAL();
//          _this->super.m_xStatus.nDelayPowerModeSwitch = 0;
//        taskEXIT_CRITICAL();
//        break;
//
//      case E_POWER_MODE_INACTIVE:
//      case E_POWER_MODE_SLEEP_2:
//      case E_POWER_MODE_SLEEP_1:
//        vTaskDelay(pdMS_TO_TICKS(100));
//        break;
//      }
//    }
//
//    // notify the system that the task is working fine.
//    AMTNotifyIsStillRunning((AManagedTask*)_this, xRes);
//
//#if (SYS_TRACE > 1)
//    if (xRes != SYS_NO_ERROR_CODE) {
//      sys_check_error_code(xRes);
//      sys_error_handler();
//    }
//#endif
//  }
}
