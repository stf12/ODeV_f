/**
 ******************************************************************************
 * @file    App.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 2, 2018
 *
 * @brief   Define the Application main entry points
 *
 * The framework `weak` function are redefined in this file and they link
 * the application specific code with the framework.
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
#include "AppPowerModeHelper.h"
#include "SPIBusTask.h"
#include "ISM330DHCXTask.h"
#include "IIS3DWBTask.h"
#include "AITask.h"
#include "UtilTask.h"
#include "SDCardTask.h"

#include "HelloWorldTask.h"

/**
 * Application managed task.
 */
static AManagedTask *s_pxHelloWorldObj = NULL;

/**
 * SPI bus task object.
 */
static AManagedTaskEx *s_pxSPIBusObj = NULL;

/**
 * Sensor task object.
 */
static AManagedTaskEx *s_pxISM330DHCXObj = NULL;

/**
 * Sensor task object.
 */
static AManagedTaskEx *s_pxIIS3DWBObj = NULL;

/**
 * Predictive Maintenance task object.
 */
static AManagedTaskEx *s_pxPDMObj = NULL;

/**
 * SDCard task object.
 */
static AManagedTaskEx *s_pxSDCardObj = NULL;

/**
 * Utility task object.
 */
static AManagedTaskEx *s_pxUtilObj = NULL;


sys_error_code_t SysLoadApplicationContext(ApplicationContext *pAppContext) {
  assert_param(pAppContext);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;


  // Allocate the task objects
//  s_pxHelloWorldObj = HelloWorldTaskAlloc();
  s_pxSPIBusObj = SPIBusTaskAlloc();
  s_pxISM330DHCXObj = ISM330DHCXTaskAlloc();
  s_pxIIS3DWBObj = IIS3DWBTaskAlloc();
//  s_pxSDCardObj = SDCardTaskAlloc();
//  s_pxPDMObj = AITaskAlloc();
  s_pxUtilObj = UtilTaskAlloc();

  // Add the task object to the context.
////  xRes = ACAddTask(pAppContext, s_pxHelloWorldObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxISM330DHCXObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxIIS3DWBObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxSPIBusObj);
//  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxSDCardObj);
////  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxPDMObj);
  xRes = ACAddTask(pAppContext, (AManagedTask*)s_pxUtilObj);

  return xRes;
}

sys_error_code_t SysOnStartApplication(ApplicationContext *pAppContext) {
  UNUSED(pAppContext);

//  //connect the sensor task to the bus.
  SPIBusTaskConnectDevice((SPIBusTask*)s_pxSPIBusObj, ISM330DHCXTaskGetSensorIF((ISM330DHCXTask*)s_pxISM330DHCXObj));
  SPIBusTaskConnectDevice((SPIBusTask*)s_pxSPIBusObj, IIS3DWBTaskGetSensorIF((IIS3DWBTask*)s_pxIIS3DWBObj));
//
//  //add the AI task to the sensors as event listener
//  IEventListener *pxListener = AITaskGetEventListenrIF((AITask*)s_pxPDMObj);
////  IEventSrcAddEventListener(ISM330DHCXTaskGetEventSrcIF((ISM330DHCXTask*)s_pxISM330DHCXObj), pxListener);
////  IEventSrcAddEventListener(IIS3DWBTaskGetEventSrcIF((IIS3DWBTask*)s_pxIIS3DWBObj), pxListener);
//
//  //add the SD task to the sensors as event listener
//  pxListener = SDCardTaskGetEventListenrIF((SDCardTask*)s_pxSDCardObj);
//  IEventSrcAddEventListener(ISM330DHCXTaskGetEventSrcIF((ISM330DHCXTask*)s_pxISM330DHCXObj), pxListener);
//  IEventSrcAddEventListener(IIS3DWBTaskGetEventSrcIF((IIS3DWBTask*)s_pxIIS3DWBObj), pxListener);

  return SYS_NO_ERROR_CODE;
}

//IApplicationErrorDelegate *SysGetErrorDelegate() {
//  // Install the application error manager delegate.
//  static IApplicationErrorDelegate *s_pxErrDelegate = NULL;
//  if (s_pxErrDelegate == NULL) {
//    s_pxErrDelegate = AEMAlloc();
//  }
//
//  return s_pxErrDelegate;
//}

IAppPowerModeHelper *SysGetPowerModeHelper() {
  // Install the application power mode helper.
  static IAppPowerModeHelper *s_pxPowerModeHelper = NULL;
  if (s_pxPowerModeHelper == NULL) {
    s_pxPowerModeHelper = AppPowerModeHelperAlloc();
  }

  return s_pxPowerModeHelper;
}
