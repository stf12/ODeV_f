/**
 ******************************************************************************
 * @file    IUsbEventListenerVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 13, 2017
 *
 * @brief   IUsbEventListener virtual table definition.
 *
 * This file declares the virtual table for the IUsbEventListener
 * interface.
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
#ifndef INCLUDE_EVENTS_IUSBEVENTLISTENERVTBL_H_
#define INCLUDE_EVENTS_IUSBEVENTLISTENERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"


typedef struct _IUsbEventListener_vtbl IUsbEventListener_vtbl;

/**
 * IUsbEventListener virtual table. This table define all the functions
 * that a subclass must overload.
 */
struct _IUsbEventListener_vtbl {
	sys_error_code_t (*OnStatusChange)(IListener *_this);                                            ///< @sa IListenerOnStatusChange
	void (*SetOwner)(IEventListener *_this, void *pxOwner);                                          ///< @sa IEventListenerSetOwner
	void *(*GetOwner)(IEventListener *_this);                                                        ///< @sa IEventListenerGetOwner
	sys_error_code_t (*OnGetReport)(IEventListener *_this, const UsbEvent *pxEvent, void *pParams);  ///< @sa IUsbEventListenerOnGetReport
	sys_error_code_t (*OnSetReport)(IEventListener *_this, const UsbEvent *pxEvent, void *pParams);  ///< @sa IUsbEventListenerOnSetReport
};

/**
 * IUsbEventListener interface definition.
 */
struct _IUsbEventListener {
	/**
	 * Pointer to the class virtual table.
	 */
	const IUsbEventListener_vtbl *vptr;
};

// Public API declaration
//***********************


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IUsbEventListenerOnGetReport(IEventListener *_this, const UsbEvent *pxEvent, void *pParams) {
	IUsbEventListener *pObj = (IUsbEventListener*)_this;
	return pObj->vptr->OnGetReport(_this, pxEvent, pParams);
}

SYS_DEFINE_INLINE
sys_error_code_t IUsbEventListenerOnSetReport(IEventListener *_this, const UsbEvent *pxEvent, void *pParams) {
	IUsbEventListener *pObj = (IUsbEventListener*)_this;
	return pObj->vptr->OnSetReport(_this, pxEvent, pParams);
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IUSBEVENTLISTENERVTBL_H_ */
