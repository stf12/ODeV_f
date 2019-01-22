/**
 ******************************************************************************
 * @file    IEventListener.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
 *
 * @brief   Event Listener Interface
 *
 * A tagging interface that all event listener interfaces must extend.
 * Each listener is could be linked to only one owner. The owner is an
 * application specific object.
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
#ifndef INCLUDE_EVENTS_IEVENTLISTENER_H_
#define INCLUDE_EVENTS_IEVENTLISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "IListener.h"
#include "IListenerVtbl.h"

/**
 * Create  type name for _IEventListener.
 */
typedef struct _IEventListener IEventListener;


// Public API declaration
//***********************

/**
 * Set the owner of the listener.
 *
 * @param _this [IN] specifies a pointer to an ::IEventListener object.
 * @param pxOwner [IN] specifies a pointer to an application specific object that become the owner of the listenr.
 */
inline void IEventListenerSetOwner(IEventListener *_this, void *pxOwner);

/**
 * Get the pointer to the listener's owner.
 *
 * @param _this [IN] specifies a pointer to an ::IEventListener object.
 */
inline void *IEventListenerGetOwner(IEventListener *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IEVENTLISTENER_H_ */
