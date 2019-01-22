/**
 ******************************************************************************
 * @file    IEventSrcVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
 *
 * @brief   Definition of the IEventSrc virtual functions.
 *
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
#ifndef INCLUDE_EVENTS_IEVENTSRCVTBL_H_
#define INCLUDE_EVENTS_IEVENTSRCVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"

typedef struct _IEventSrc_vtbl IEventSrc_vtbl;

/**
 * IEventSrc virtual table. This table define all the functions
 * that a subclass must overload.
 */
struct _IEventSrc_vtbl {
	sys_error_code_t (*Init)(IEventSrc *_this);                                                      ///< @sa IEventSrcInit
	sys_error_code_t (*AddEventListener)(IEventSrc *_this, IEventListener *pListener);               ///< @sa IEventSrcAddEventListener
	sys_error_code_t (*RemoveEventListener)(IEventSrc *_this, IEventListener *pListener);            ///< @sa IEventSrcRemoveEventListener
	uint32_t (*GetMaxListenerCount)(const IEventSrc *_this);                                         ///< @sa IEventSrcGetMaxListenerCount
	sys_error_code_t (*SendEvent)(const IEventSrc *_this, const IEvent *pxEvent, void *pvParams);    ///< @sa IEventSrcSendEvent
};

/**
 * IEventSrc type definition.
 */
struct _IEventSrc {
	/**
	 * Pointer to the class virtual table.
	 */
	const IEventSrc_vtbl *vptr;
};


// Public API declaration
//***********************


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IEventSrcInit(IEventSrc *_this) {
	return _this->vptr->Init(_this);
}

SYS_DEFINE_INLINE
sys_error_code_t IEventSrcAddEventListener(IEventSrc *_this, IEventListener *pListener) {
	return _this->vptr->AddEventListener(_this, pListener);
}

SYS_DEFINE_INLINE
sys_error_code_t IEventSrcRemoveEventListener(IEventSrc *_this, IEventListener *pListener) {
	return _this->vptr->RemoveEventListener(_this, pListener);
}

SYS_DEFINE_INLINE
uint32_t IEventSrcGetMaxListenerCount(const IEventSrc *_this) {
	return _this->vptr->GetMaxListenerCount(_this);
}

SYS_DEFINE_INLINE
sys_error_code_t IEventSrcSendEvent(const IEventSrc *_this, const IEvent *pxEvent, void *pvParams) {
	return _this->vptr->SendEvent(_this, pxEvent, pvParams);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IEVENTSRCVTBL_H_ */
