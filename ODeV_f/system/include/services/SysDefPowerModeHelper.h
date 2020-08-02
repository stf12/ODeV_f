/**
 ******************************************************************************
 * @file    SysDefPowerModeHelper.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Oct 31, 2018
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
#ifndef INCLUDE_SERVICES_SYSDEFPOWERMODEHELPER_H_
#define INCLUDE_SERVICES_SYSDEFPOWERMODEHELPER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IAppPowerModeHelper.h"
#include "IAppPowerModeHelper_vtbl.h"


/**
 * Create  type name for _SysDefPowerModeHelper.
 */
typedef struct _SysDefPowerModeHelper SysDefPowerModeHelper;

/**
 * Internal state of the Default Power Mode Helper.
 */
struct _SysDefPowerModeHelper {

  /**
   * Base class object.
   */
  IAppPowerModeHelper *super;

  /**
   *
   */
  SysPowerStatus m_xStatus;
};

// Public API declaration
//***********************

/**
 * Allocate an instance of SysDefPowerModeHelper. It is allocated in the FreeRTOS heap.
 *
 * @return a pointer to the generic interface ::IApplicationErrorDelegate if success,
 * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
 */
IAppPowerModeHelper * SysDefPowerModeHelperAlloc();


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_SYSDEFPOWERMODEHELPER_H_ */
