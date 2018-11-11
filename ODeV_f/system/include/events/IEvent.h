/**
 ******************************************************************************
 * @file    IEvent.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 17, 2017
 *
 * @brief The root class from which all event state objects shall be derived.
 *
 * An event is an object that contains information about something that
 * happened in the system at a given moment. An event object is constructed
 * with a link to the EventSrc that has generated it.
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
#ifndef INCLUDE_EVENTS_IEVENT_H_
#define INCLUDE_EVENTS_IEVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"

typedef struct _IEventSrc IEventSrc;

typedef struct _IEvent IEvent;

/**
 * Specifies a generic Event. Each event has a pointer to the ::IEventSrc object
 * that has generated the event.
 */
struct _IEvent {
	/**
	 * Specifies a pointer to the Event Source that generated the event.
	 */
	const IEventSrc *pSource;
};


// Public API declaration
//***********************

/**
 * Initialize an event. An is initialized when it is linked with the ::IEventSrc that has generated the event.
 *
 * @param this [IN] specifies an ::IEvent object
 * @param pSource [IN] specifies the ::IEventSrc object that has generated teh event.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IEventInit(IEvent *this, const IEventSrc *pSource);

// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IEventInit(IEvent *this, const IEventSrc *pSource) {
	this->pSource = pSource;

	return SYS_NO_ERROR_CODE;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IEVENT_H_ */
