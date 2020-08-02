/**
 ******************************************************************************
 * @file    AManagedTaskEx.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Oct 25, 2018
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

#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"


// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
extern sys_error_code_t AMTExForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode);
extern sys_error_code_t AMTExSetInactiveState(AManagedTaskEx *this, boolean_t bBlockedSuspended);
extern sys_error_code_t AMTInitEx(AManagedTaskEx *this);
extern boolean_t AMTExIsTaskInactive(AManagedTaskEx *this);
#endif

