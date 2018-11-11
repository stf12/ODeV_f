/**
 ******************************************************************************
 * @file    IEventSrc.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
 *
 * @brief   Event Source interface.
 *
 * An Event Source object provides the API to add / remove Event Listener
 * objects, and to notify the registered listeners.
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
#ifndef INCLUDE_EVENTS_IEVENTSRC_H_
#define INCLUDE_EVENTS_IEVENTSRC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systypes.h"
#include "syserror.h"
#include "IEvent.h"
#include "IEventListener.h"
#include "IEventListenerVtbl.h"

/**
 * Create  type name for _IEventSrc.
 */
typedef struct _IEventSrc IEventSrc;


// Public API declaration
//***********************

/**
 * Initialize an event source. This function should be called after the
 *
 * @param this [IN] specifies a pointer to an IEventSrc object.
 * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
 */
inline sys_error_code_t IEventSrcInit(IEventSrc *this);

/**
 * Register a listener with this event souce.
 *
 * @param this [IN] specifies a pointer to an IEventSrc object.
 * @param pListener [IN] specifies a pointer to an IEventListener object.
 * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
 */
inline sys_error_code_t IEventSrcAddEventListener(IEventSrc *this, IEventListener *pListener);

/**
 * Remove a listener from this event source,
 *
 * @param this [IN] specifies a pointer to an IEventSrc object.
 * @param pListener [IN] specifies a pointer to an IEventListener object.
 * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
 */
inline sys_error_code_t IEventSrcRemoveEventListener(IEventSrc *this, IEventListener *pListener);

/**
 * Get the maximum number of IEventListener that can be registered with this event source.
 *
 * @param this [IN] specifies a pointer to an IEventSrc object.
 * @return the maximum number of IEventListener that can be registered with this event source
 */
inline uint32_t IEventSrcGetMaxListenerCount(const IEventSrc *this);

/**
 * Send an IEvent to all the registered IEventListener.
 *
 * @param this [IN] specifies a pointer to an IEventSrc object.
 * @param pxEvent [IN] specifies a pointer to an IEvent object.
 * @param pvParams [IN] specifies a generic pointer that can be used by the application
 *                      extend the semantic  Event design pattern.
 * @return SYS_NO_ERROR_CODE if success, an subclass specific error code otherwise.
 */
inline sys_error_code_t IEventSrcSendEvent(const IEventSrc *this, const IEvent *pxEvent, void *pvParams);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_EVENTS_IEVENTSRC_H_ */
