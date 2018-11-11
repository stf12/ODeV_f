/**
 ******************************************************************************
 * @file    IErrorFirstResponder.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Aug 12, 2017
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

#include "IErrorFirstResponder.h"
#include "IErrorFirstResponderVtbl.h"

#if defined (__GNUC__)

extern void IErrFirstResponderSetOwner(IErrFirstResponder *this, void *pxOwner);
extern void *IErrFirstResponderGetOwner(IErrFirstResponder *this);
sys_error_code_t IErrorFirstResponderNewError(IErrFirstResponder *this, SysEvent xError, boolean_t bIsCalledFromISR);

#endif

// Private member function declaration
// ***********************************


// Public API definition
// *********************


// Private function definition
// ***************************
