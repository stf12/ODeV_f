/**
 ******************************************************************************
 * @file    IListener.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Mar 20, 2017
 *
 * @brief   External definition of the IListener inline methods.
 *
 * This file is used by GCC only to to provide a compilation unit where to
 * put the methods implementation when the compiler can't expand them inline.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

#include "IListener.h"
#include "IListenerVtbl.h"

#if defined (__GNUC__)

extern sys_error_code_t IListenerOnStatusChange(IListener *this);

#endif
