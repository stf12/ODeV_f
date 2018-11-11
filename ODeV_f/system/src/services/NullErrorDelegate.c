/**
 ******************************************************************************
 * @file    NullErrorDelegate.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 14, 2017
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

#include "NullErrorDelegate.h"

// Private member function declaration
// ***********************************


// Inline function farward declaration
// ***********************************

#if defined (__GNUC__)
extern sys_error_code_t NullAEDInit(IApplicationErrorDelegate *this, void *pParams);
extern sys_error_code_t NullAEDOnStartApplication(IApplicationErrorDelegate *this, ApplicationContext *pxContext);
extern sys_error_code_t NullAEDProcessEvent(IApplicationErrorDelegate *this, ApplicationContext *pxContext, SysEvent xEvent);
extern sys_error_code_t NullAEDOnNewErrEvent(IApplicationErrorDelegate *this, SysEvent xEvent);
extern boolean_t NullAEDIsLastErrorPending(IApplicationErrorDelegate *this);
extern sys_error_code_t NullAEDAddFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder, uint8_t nPriority);
extern sys_error_code_t NullAEDRemoveFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder);
extern uint8_t NullAEDGetMaxFirstResponderPriority(const IApplicationErrorDelegate *this);
extern void NullAEDResetCounter(IApplicationErrorDelegate *this);
#endif


// Public API definition
// *********************

IApplicationErrorDelegate *NullAEDAlloc() {
  static IApplicationErrorDelegate_vtbl s_xNullAED_vtbl = {
      NullAEDInit,
      NullAEDOnStartApplication,
      NullAEDProcessEvent,
      NullAEDOnNewErrEvent,
      NullAEDIsLastErrorPending,
      NullAEDAddFirstResponder,
      NullAEDRemoveFirstResponder,
      NullAEDGetMaxFirstResponderPriority,
      NullAEDResetCounter
  };

  static NullErrorDelegate s_xNullAED;
  s_xNullAED.super.vptr = &s_xNullAED_vtbl;

  return (IApplicationErrorDelegate*) &s_xNullAED;
}

// Private function definition
// ***************************
