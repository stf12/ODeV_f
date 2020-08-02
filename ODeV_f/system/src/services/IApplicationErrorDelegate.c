/**
 ******************************************************************************
 * @file    IApplicationErrorDelegate.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Aug 4, 2017
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

#include "IApplicationErrorDelegate.h"
#include "IApplicationErrorDelegateVtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
extern sys_error_code_t IAEDInit(IApplicationErrorDelegate *this, void *pParams);
extern sys_error_code_t IAEDOnStartApplication(IApplicationErrorDelegate *this, ApplicationContext *pxContext);
extern sys_error_code_t IAEDProcessEvent(IApplicationErrorDelegate *this, ApplicationContext *pxContext, SysEvent xEvent);
extern sys_error_code_t IAEDOnNewErrEvent(IApplicationErrorDelegate *this, SysEvent xEvent);
extern boolean_t IAEDIsLastErrorPending(IApplicationErrorDelegate *this);
extern sys_error_code_t IAEDAddFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder, uint8_t nPriority);
extern sys_error_code_t IAEDRemoveFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder);
extern uint8_t IAEDGetMaxFirstResponderPriority(const IApplicationErrorDelegate *this);
extern void IAEDResetCounter(IApplicationErrorDelegate *this);
#endif

