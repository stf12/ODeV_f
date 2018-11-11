/**
 ******************************************************************************
 * @file    NullErrorDelegate.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 14, 2017
 *
 * @brief   Empty implementation of the IApplicationErrorDelegate IF
 *
 * When installed this delegate disable the error managment subsystem.
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
#ifndef INCLUDE_SERVICES_NULLERRORDELEGATE_H_
#define INCLUDE_SERVICES_NULLERRORDELEGATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IApplicationErrorDelegate.h"
#include "IApplicationErrorDelegateVtbl.h"

/**
 * Create a type name for _NuLLErrorDelegate
 */
typedef struct _NuLLErrorDelegate NullErrorDelegate;

/**
 * Define the NULL error manager delegate. It does not implement any error management.
 */
struct _NuLLErrorDelegate {
  /**
   * Base class object.
   */
  IApplicationErrorDelegate super;
};



// Public API declaration
//***********************

/**
 * Allocate an instance of ::NullErrorDelegate.
 * Initialize the object virtual table.
 *
 * @return
 */
IApplicationErrorDelegate *NullAEDAlloc();


// Inline functions definition
// ***************************

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDInit
 */
inline
sys_error_code_t NullAEDInit(IApplicationErrorDelegate *this, void *pParams) { return SYS_NO_ERROR_CODE; }

/**
 * Default implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDOnStartApplication
 */
inline
sys_error_code_t NullAEDOnStartApplication(IApplicationErrorDelegate *this, ApplicationContext *pxContext) { return SYS_NO_ERROR_CODE; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDProcessEvent
 */
inline
sys_error_code_t NullAEDProcessEvent(IApplicationErrorDelegate *this, ApplicationContext *pxContext, SysEvent xEvent) { return SYS_NO_ERROR_CODE; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDOnNewErrEvent
 */

inline
sys_error_code_t NullAEDOnNewErrEvent(IApplicationErrorDelegate *this, SysEvent xEvent) { return SYS_NO_ERROR_CODE; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDIsLastErrorPending
 */
inline
boolean_t NullAEDIsLastErrorPending(IApplicationErrorDelegate *this) { return FALSE; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDAddFirstResponder
 */
inline
sys_error_code_t NullAEDAddFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder, uint8_t nPriority) { return SYS_NO_ERROR_CODE; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDRemoveFirstResponder
 */
inline
sys_error_code_t NullAEDRemoveFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder) { return SYS_NO_ERROR_CODE; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDGetMaxFirstResponderPriority
 */
inline
uint8_t NullAEDGetMaxFirstResponderPriority(const IApplicationErrorDelegate *this) { return 0; }

/**
 * Null implementation of the IApplicationErrorDelegate. It does no error management.
 * @sa IAEDResetCounter
 */
inline
void NullAEDResetCounter(IApplicationErrorDelegate *this) {};


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_NULLERRORDELEGATE_H_ */
