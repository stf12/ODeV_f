/**
 ******************************************************************************
 * @file    PushButtonTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 5, 2018
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

#include "PushButtonTask.h"
#include "PushButtonTask_vtbl.h"
#include "sysdebug.h"
#include "NucleoDriver.h"
#include "NucleoDriver_vtbl.h"
#include "gui/common/FrontendApplication.hpp"

#include "hid_report_parser.h"
#include "string.h"

#ifndef PB_TASK_CFG_STACK_DEPTH
#define PB_TASK_CFG_STACK_DEPTH      120
#endif

#ifndef PB_TASK_CFG_PRIORITY
#define PB_TASK_CFG_PRIORITY         (tskIDLE_PRIORITY+2)
#endif

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_PB, level, message)


/**
 * PushButtonTask Driver virtual table.
 */
static const AManagedTaskEx_vtbl s_xPushButtonTask_vtbl = {
    PushButtonTask_vtblHardwareInit,
    PushButtonTask_vtblOnCreateTask,
    PushButtonTask_vtblDoEnterPowerMode,
    PushButtonTask_vtblHandleError,
    PushButtonTask_vtblForceExecuteStep
};

/**
 * The only instance of the task object.
 */
static PushButtonTask s_xTaskObj;


// Private member function declaration
// ***********************************

/**
 * Execute one step of the task control loop while the system is in RUN mode.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return SYS_NO_EROR_CODE if success, a task specific error code otherwise.
 */
static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *_this);

/**
 * Task control function.
 *
 * @param pParams .
 */
static void PushButtonTaskRun(void *pParams);

void TestFreeGPIO();


// Inline function forward declaration
// ***********************************

#if defined (__GNUC__)
extern sys_error_code_t PushButtonTaskSetOutputQueue(PushButtonTask *_this, QueueHandle_t xQueue);
extern sys_error_code_t PushButtonTaskSetDriver(PushButtonTask *_this, IDriver *pxDriver);
#endif


// Public API definition
// *********************

AManagedTaskEx *PushButtonTaskAlloc() {
  // In this application there is only one Keyboard task,
  // so this allocator implement the singleton design pattern.

  // Initialize the super class
  AMTInitEx(&s_xTaskObj.super);

  s_xTaskObj.super.vptr = &s_xPushButtonTask_vtbl;

  return (AManagedTaskEx*)&s_xTaskObj;
}

// AManagedTask virtual functions definition
// ***********************************************

sys_error_code_t PushButtonTask_vtblHardwareInit(AManagedTask *_this, void *pParams) {
  assert_param(_this);
  UNUSED(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  // the driver is initialized by the task owner. This managed task use a driver
  // of type NucleoDriver in order to check the status of the push button and
  // put the system in low power mode.

  return xRes;
}

sys_error_code_t PushButtonTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *pObj = (PushButtonTask*)_this;

  pObj->m_bButtonArmed = FALSE;

  *pvTaskCode = PushButtonTaskRun;
  *pcName = "PB";
  *pnStackDepth = PB_TASK_CFG_STACK_DEPTH;
  *pParams = _this;
  *pxPriority = PB_TASK_CFG_PRIORITY;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblHandleError(AManagedTask *_this, SysEvent xError) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  return xRes;
}

sys_error_code_t PushButtonTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  PushButtonTask *pObj = (PushButtonTask*)_this;

  return xRes;
}


// Private function definition
// ***************************

static sys_error_code_t PushButtonTaskExecuteStepRun(PushButtonTask *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  boolean_t bIsButtonPressed = FALSE;

  if (_this->m_pxDriver) {
    AMTExSetInactiveState((AManagedTaskEx*)_this, TRUE);
    NucleoDriverWaitForButtonEvent((NucleoDriver*)_this->m_pxDriver, &bIsButtonPressed);
    AMTExSetInactiveState((AManagedTaskEx*)_this, FALSE);

    if (!bIsButtonPressed) {
      if (_this->m_bButtonArmed) {
        _this->m_bButtonArmed = FALSE;
        // enter low power mode
        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: increment counter.\r\n"));
//        SysEvent xEvent;
//        xEvent.nRawEvent = SYS_PM_MAKE_EVENT(SYS_PM_EVT_SRC_SW, SYS_PM_EVT_PARAM_ENTER_LP);
//        SysPostPowerModeEvent(xEvent);

        // Example of interaction with the GUI using the the TouchGFX Application and Model.
//        FrontendApplication* guiApp = static_cast<FrontendApplication*>(FrontendApplication::getInstance());
//        guiApp->getModel().incrementCounter(nIncrement);

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: Test free GPIO.\r\n"));
        TestFreeGPIO();

        if (_this->m_xOutputQueue) {
//          static HIDReport xReport02;
//          memset(&xReport02, 0, sizeof(HIDReport));
//          xReport02.reportID = HID_REPORT_ID_MOUSE;
//          xReport02.inputReport02.nDX = 0x25;
//
//          xQueueSendToBack(_this->m_xOutputQueue, &xReport02, pdMS_TO_TICKS(50));

//          static HIDReport xReport10;
//          memset(&xReport10, 0, sizeof(HIDReport));
//          xReport10.reportID = HID_REPORT_ID_KEYBOARD;
//          xReport10.inputReport10.nKeyCodeArray[0] = 0x16; // s for STF :)
//          xQueueSendToBack(_this->m_xOutputQueue, &xReport10, pdMS_TO_TICKS(50));
//
//          vTaskDelay(pdMS_TO_TICKS(500));
//          xReport10.inputReport10.nKeyCodeArray[0] = 0; // s for STF :)
//          xQueueSendToBack(_this->m_xOutputQueue, &xReport10, pdMS_TO_TICKS(50));
        }
      }
      else {
        _this->m_bButtonArmed = TRUE;

        SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: button armed.\r\n"));
      }
    }
  }

  return xRes;
}

static void PushButtonTaskRun(void *pParams) {
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  PushButtonTask *_this = (PushButtonTask*)pParams;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PB: start.\r\n"));

  IDrvStart(_this->m_pxDriver);

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
        xRes = PushButtonTaskExecuteStepRun(_this);
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

void TestFreeGPIO() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  HAL_PWREx_EnableVddIO2();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();


  /*Configure GPIO pins :
   * PI0 PI1 PI2 PI3 PI4 PI5 PI6 PI7 PI10 PI11 (10)  [KEY_IN]
   */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_0|GPIO_PIN_7|GPIO_PIN_6
                          |GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_11|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PB3 PB5 PB6 PB7 PB8 PB9 PB12 PB13 PB14 (9)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_8|GPIO_PIN_6
                          |GPIO_PIN_9|GPIO_PIN_7|GPIO_PIN_14|GPIO_PIN_12
                          |GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PA0 PA1 PA4 PA8 PA15 (5)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_8|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PH3 PH8 PH9 PH10 PH11 PH12 (6)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_9|GPIO_PIN_3|GPIO_PIN_10
                          |GPIO_PIN_11|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PG0 PG1 PG2 PG3 PG4 PG5 PG6 PG7 PG8 PG10 PG12 PG13 PG15 (13)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_15|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_8
                          |GPIO_PIN_13|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_5
                          |GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_1|GPIO_PIN_2
                          |GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PD4 PD5 PD6 PD7 PD13 (5)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7
                          |GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PC0 PC1 PC6 PC7 PC8 PC9 PC10 PC11 PC12 PC14 PC15 (11)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_12
                          |GPIO_PIN_8|GPIO_PIN_6|GPIO_PIN_15|GPIO_PIN_7
                          |GPIO_PIN_9|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PE5 PE6 (2)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins :
   * PF0 PF1 PF2 PF3 PF4 PF5 PF10 PF12 (8)
   */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_10|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
}
