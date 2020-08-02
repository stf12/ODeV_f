/**
 ******************************************************************************
 * @file    IBoot.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 22, 2017
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

#include "IBoot.h"
#include "IBootVtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
extern sys_error_code_t IBootInit(IBoot *this);
extern boolean_t IBootCheckDFUTrigger(IBoot *this);
extern uint32_t IBootGetAppAdderss(IBoot *this);
extern sys_error_code_t IBootOnJampToApp(IBoot *this, uint32_t nAppDress);
#endif
