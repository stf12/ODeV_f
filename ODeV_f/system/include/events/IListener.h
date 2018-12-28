/**
 ******************************************************************************
 * @file    IListener.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 6, 2017
 *
 * @brief   Generic Listener interface
 *
 * This interface is the base class for the Listener hierarchy. A Listener
 * is an object that can be notified about a status change.
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

#ifndef INCLUDE_EVENTS_ILISTENER_H_
#define INCLUDE_EVENTS_ILISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systypes.h"
#include "syserror.h"

typedef struct _IListener IListener;


// Public API declaration
//***********************


inline sys_error_code_t IListenerOnStatusChange(IListener *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_EVENTS_ILISTENER_H_ */
