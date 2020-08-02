/**
 ******************************************************************************
 * @file    IErrorFirstResponderVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Aug 11, 2017
 *
 * @brief
 *
 * <DESCRIPTIOM>
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
#ifndef INCLUDE_SERVICES_IERRORFIRSTRESPONDERVTBL_H_
#define INCLUDE_SERVICES_IERRORFIRSTRESPONDERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IErrorFirstResponder.h"
#include "systp.h"

/**
 * Create  type name for _IErrFirstResponder.
 */
typedef struct _IErrFirstResponder_vtbl IErrFirstResponder_vtbl;

/**
 * IErrFirstResponder virtual table. This table define all the functions
 * that a subclass must overload.
 */
struct _IErrFirstResponder_vtbl {
	void (*SetOwner)(IErrFirstResponder *_this, void *pxOwner);  ///< @sa IErrFirstResponderSetOwner
	void *(*GetOwner)(IErrFirstResponder *_this);  ///< @sa IErrFirstResponderGetOwner
	sys_error_code_t (*NewError)(IErrFirstResponder *_this, SysEvent xError, boolean_t bIsCalledFromISR);  ///< @sa IErrFirstResponderNewError
};

/**
 * IErrFirstResponder interface definition.
 */
struct _IErrFirstResponder {
	/**
	 * Pointer to the class virtual table.
	 */
	const IErrFirstResponder_vtbl *vptr;
};


// Public API declaration
//***********************


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
void IErrFirstResponderSetOwner(IErrFirstResponder *_this, void *pxOwner) {
	_this->vptr->SetOwner(_this, pxOwner);
}

SYS_DEFINE_INLINE
void *IErrFirstResponderGetOwner(IErrFirstResponder *_this) {
	return _this->vptr->GetOwner(_this);
}

SYS_DEFINE_INLINE
sys_error_code_t IErrorFirstResponderNewError(IErrFirstResponder *_this, SysEvent xError, boolean_t bIsCalledFromISR) {
	return _this->vptr->NewError(_this, xError, bIsCalledFromISR);
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IERRORFIRSTRESPONDERVTBL_H_ */
