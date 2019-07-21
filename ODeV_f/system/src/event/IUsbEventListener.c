/**
 ******************************************************************************
 * @file    IUsbEventListener.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 13, 2017
 *
 * @brief   External definition of the IUsbEventListener inline methods.
 *
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
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

#include "IUsbEventListener.h"
#include "IUsbEventListenerVtbl.h"

#if defined (__GNUC__)

extern sys_error_code_t IUsbEventListenerOnGetReport(IEventListener *this, const UsbEvent *pxEvent, void *pParams);
extern sys_error_code_t IUsbEventListenerOnSetReport(IEventListener *this, const UsbEvent *pxEvent, void *pParams);

#endif
