/**
 ******************************************************************************
 * @file    IEventListenerVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
 *
 * @brief   IEventListener virtual table definition.
 *
 * This file declares the virtual table for the IEventListener interface.
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
#ifndef INCLUDE_EVENTS_IEVENTLISTENERVTBL_H_
#define INCLUDE_EVENTS_IEVENTLISTENERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IListenerVtbl.h"
#include "systp.h"

typedef struct _IEventListener_vtbl IEventListener_vtbl;

/**
 * IEventListener virtual table. This table define all the functions
 * that a subclass must overload.
 */
struct _IEventListener_vtbl {
	sys_error_code_t (*OnStatusChange)(IListener *this);    ///< @sa IListenerOnStatusChange
	void (*SetOwner)(IEventListener *this, void *pxOwner);  ///< @sa IEventListenerSetOwner
	void *(*GetOwner)(IEventListener *this);                ///< @sa IEventListenerGetOwner
};

/**
 * IEventListener interface definition.
 */
struct _IEventListener {
	/**
	 * Pointer to the class virtual table.
	 */
	const IEventListener_vtbl *vptr;
};


// Public API declaration
//***********************


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
void IEventListenerSetOwner(IEventListener *this, void *pxOwner) {
	this->vptr->SetOwner(this, pxOwner);
}

SYS_DEFINE_INLINE
void *IEventListenerGetOwner(IEventListener *this) {
	return this->vptr->GetOwner(this);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IEVENTLISTENERVTBL_H_ */
