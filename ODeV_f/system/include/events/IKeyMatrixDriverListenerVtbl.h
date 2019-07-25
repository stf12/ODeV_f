/**
 ******************************************************************************
 * @file    IKeyMatrixDriverListenerVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 6, 2017
 *
 * @brief   Declaration of the virtual method overloaded by the
 * IKeyMatrixDrvListener
 *
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

#ifndef INCLUDE_EVENTS_IKEYMATRIXDRIVERLISTENERVTBL_H_
#define INCLUDE_EVENTS_IKEYMATRIXDRIVERLISTENERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IListenerVtbl.h"
#include "systp.h"

typedef struct __IKeyMatrixDrvListener_vtbl IKeyMatrixDrvListener_vtbl;


struct __IKeyMatrixDrvListener_vtbl {
	sys_error_code_t (*OnStatusChange)(IListener *_this);
	sys_error_code_t (*OnKeyMatrixChange)(IListener *_this, uint8_t *pnKeyMatrixData, uint8_t nColumns, uint16_t nModifiedColumnBitMask);
};

/**
 * @class IKeyMatrixDrvListener.
 *
 * This interface extend the IListener interface.
 */
struct _IKeyMatrixDrvListener {
	/**
	 * Base interface.
	 */
	const IKeyMatrixDrvListener_vtbl *vptr;
};



// Public API declaration
//***********************



// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IKeyMatrixDrvListenerOnKeyMatrixChange(IListener *_this, uint8_t *pnKeyMatrixData, uint8_t nColumns, uint16_t nModifiedColumnBitMask) {
	struct _IKeyMatrixDrvListener *pObj = (struct _IKeyMatrixDrvListener*)_this;
	return pObj->vptr->OnKeyMatrixChange(_this, pnKeyMatrixData, nColumns, nModifiedColumnBitMask);
}


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_EVENTS_IKEYMATRIXDRIVERLISTENERVTBL_H_ */
