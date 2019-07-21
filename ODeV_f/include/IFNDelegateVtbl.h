/**
 ******************************************************************************
 * @file    IFNDelegateVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jun 24, 2017
 *
 * @brief   Definition of the IFNDelegate virtual functions
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
#ifndef IFNDELEGATEVTBL_H_
#define IFNDELEGATEVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"

/**
 * Creates a type name for _IFNDelegate_vtbl.
 */
typedef struct _IFNDelegate_vtbl IFNDelegate_vtbl;

/**
 * IFNDelegate_vtbl virtual table. This table define all the functions
 * that a subclass must overload.
 */
struct _IFNDelegate_vtbl {
	void (*OnKeyPressed)(IFNDelegate *_this, uint8_t nKeycode);           ///< @sa IFNDelegateOnKeyPressed
	void (*OnKeyReleased)(IFNDelegate *_this, uint8_t nKeycode);          ///< @sa IFNDelegateOnKeyReleased
	void (*OnFnStatusChanged)(IFNDelegate *_this, boolean_t bFnActive);   ///< @sa IFNDelegateOnFnStatusChanged
};

/**
 * IFNDelegate interface definition.
 */
struct _IFNDelegate {
	/**
	 * Pointer to the class virtual table.
	 */
	const IFNDelegate_vtbl *vptr;
};

// Public API declaration
//***********************


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
void IFNDelegateOnKeyPressed(IFNDelegate *_this, uint8_t nKeycode) {
	_this->vptr->OnKeyPressed(_this, nKeycode);
}

SYS_DEFINE_INLINE
void IFNDelegateOnKeyReleased(IFNDelegate *_this, uint8_t nKeycode) {
	_this->vptr->OnKeyReleased(_this, nKeycode);
}

SYS_DEFINE_INLINE
void IFNDelegateOnFnStatusChanged(IFNDelegate *_this, boolean_t bFnActive) {
	_this->vptr->OnFnStatusChanged(_this, bFnActive);
}


#ifdef __cplusplus
}
#endif

#endif /* IFNDELEGATEVTBL_H_ */
