/**
 ******************************************************************************
 * @file    App.c
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
 * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
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

#include "sysdebug.h"
#include "ApplicationContext.h"
#include "HelloWorldTask.h"
#include "PushButtonTask.h"
#include "GuiTask.h"
#include "TscTask.h"
#include "HostComChannelTask.h"


static AManagedTask *s_pxHelloWorldObj = NULL;
static AManagedTaskEx *s_pxPushButtonObj = NULL;
static AManagedTaskEx *s_pxGuiTaskObj = NULL;
static AManagedTaskEx *s_pxTscTaskObj = NULL;
static AManagedTaskEx *s_pxHostComChannelTask = NULL;

//#include "usbd_core.h"
//#include "usbd_desc.h"
//#include "usbd_hid.h"
//USBD_HandleTypeDef USBD_Device;
//extern PCD_HandleTypeDef hpcd;


extern "C"
sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext) {
  assert_param(pAppContext);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // Allocate the task objects
  s_pxHelloWorldObj = HelloWorldTaskAlloc();
  s_pxPushButtonObj = PushButtonTaskAlloc();
  s_pxGuiTaskObj = GuiTaskAlloc();
  s_pxTscTaskObj = TscTaskAlloc();
  s_pxHostComChannelTask = HostComChannelTaskAlloc();

  // Add the task object to the context.
  xRes = ACAddTask(pAppContext, s_pxHelloWorldObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxPushButtonObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxTscTaskObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxGuiTaskObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxHostComChannelTask);

  return xRes;
}

extern "C"
sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext) {
  UNUSED(pAppContext);

  IDriver *pxNucleoDriver = HelloWorldTaskGetDriver((HelloWorldTask*)s_pxHelloWorldObj);
  PushButtonTaskSetDriver((PushButtonTask*)s_pxPushButtonObj, pxNucleoDriver);

//  // test the USB IP and the USD Device Library.
//
//  // Enable Power Clock
//  __HAL_RCC_PWR_CLK_ENABLE();
//
//  // Enable USB power on Pwrctrl CR2 register
//  HAL_PWREx_EnableVddUSB();
//
//  // Init Device Library
//  USBD_Init(&USBD_Device, &HID_Desc, 0);
//
//  // Add Supported Class
//  USBD_RegisterClass(&USBD_Device, (USBD_ClassTypeDef*)USBD_HID_CLASS);
//
//  // Start Device Process
//  USBD_Start(&USBD_Device);

  return SYS_NO_ERROR_CODE;
}

//extern "C"
//IApplicationErrorDelegate *SysGetErrorDelegate() {
//  // Install the application error manager delegate.
//  static IApplicationErrorDelegate *s_pxErrDelegate = NULL;
//  if (s_pxErrDelegate == NULL) {
//    s_pxErrDelegate = AEMAlloc();
//  }
//
//  return s_pxErrDelegate;
//}

//extern "C"
//IAppPowerModeHelper *SysGetPowerModeHelper() {
//  // Install the application power mode helper.
//  static IAppPowerModeHelper *s_pxPowerModeHelper = NULL;
//  if (s_pxPowerModeHelper == NULL) {
//    s_pxPowerModeHelper = AppPowerModeHelperAlloc();
//  }
//
//  return s_pxPowerModeHelper;
//}

