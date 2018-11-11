/**
 ******************************************************************************
 * @file    IApplicationErrorDelegateVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Aug 4, 2017
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
#ifndef INCLUDE_SERVICES_IAPPLICATIONERRORDELEGATEVTBL_H_
#define INCLUDE_SERVICES_IAPPLICATIONERRORDELEGATEVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"

/**
 * Create  type name for _IApplicationErrorDelegate_vtbl.
 */
typedef struct _IApplicationErrorDelegate_vtbl IApplicationErrorDelegate_vtbl;

/**
 * Virtual table for the _IApplicationErrorDelegate.
 */
struct _IApplicationErrorDelegate_vtbl {
	sys_error_code_t (*Init)(IApplicationErrorDelegate *this, void *pParams);  ///< @sa IAEDInit
	sys_error_code_t (*OnStartApplication)(IApplicationErrorDelegate *this, ApplicationContext *pxContext);  ///< @sa IAEDOnStartApplication
	sys_error_code_t (*ProcessEvent)(IApplicationErrorDelegate *this, ApplicationContext *pxContext, SysEvent xEvent);  ///< @sa IAEDProcessEvent
	sys_error_code_t (*OnNewErrEvent)(IApplicationErrorDelegate *this, SysEvent xEvent);  ///< @sa IAEDOnNewErrEvent
	boolean_t (*IsLastErrorPending)(IApplicationErrorDelegate *this); ///< @sa IAEDIsLastErrorPending
	sys_error_code_t (*AddFirstResponder)(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder, uint8_t nPriority);  ///< @sa IAEDAddFirstResponder
	sys_error_code_t (*RemoveFirstResponder)(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder);  ///< @sa IAEDRemoveFirstResponder
	uint8_t (*GetMaxFirstResponderPriority)(const IApplicationErrorDelegate *this);  ///< @sa IAEDGetMaxFirstResponderPriority
	void (*ResetCounter)(IApplicationErrorDelegate *this); ///< @sa IAEDResetAEDCounter
};

/**
 * IApplicationErrorDelegate interface internal state. This is the base interface for the the error management.
 * It declares only the virtual table used to implement the inheritance. A subclass implements
 * application specific error management policy.
 */
struct _IApplicationErrorDelegate {

	/**
	 * Specifies a pointer to the actual class virtual table.
	 */
	const IApplicationErrorDelegate_vtbl *vptr;
};

// Public API declaration
//***********************


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IAEDInit(IApplicationErrorDelegate *this, void *pParams) {
	return this->vptr->Init(this, pParams);
}

SYS_DEFINE_INLINE
sys_error_code_t IAEDOnStartApplication(IApplicationErrorDelegate *this, ApplicationContext *pxContext) {
	return this->vptr->OnStartApplication(this, pxContext);
}

SYS_DEFINE_INLINE
sys_error_code_t IAEDProcessEvent(IApplicationErrorDelegate *this, ApplicationContext *pxContext, SysEvent xEvent) {
	return this->vptr->ProcessEvent(this, pxContext, xEvent);
}

SYS_DEFINE_INLINE
sys_error_code_t IAEDOnNewErrEvent(IApplicationErrorDelegate *this, SysEvent xEvent) {
	return this->vptr->OnNewErrEvent(this, xEvent);
}

SYS_DEFINE_INLINE
inline boolean_t IAEDIsLastErrorPending(IApplicationErrorDelegate *this) {
	return this->vptr->IsLastErrorPending(this);
}

SYS_DEFINE_INLINE
sys_error_code_t IAEDAddFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder, uint8_t nPriority) {
	return this->vptr->AddFirstResponder(this, pFirstResponder, nPriority);
}

SYS_DEFINE_INLINE
sys_error_code_t IAEDRemoveFirstResponder(IApplicationErrorDelegate *this, IErrFirstResponder *pFirstResponder) {
	return this->vptr->RemoveFirstResponder(this, pFirstResponder);
}

SYS_DEFINE_INLINE
uint8_t IAEDGetMaxFirstResponderPriority(const IApplicationErrorDelegate *this) {
	return this->vptr->GetMaxFirstResponderPriority(this);
}

SYS_DEFINE_INLINE
void IAEDResetCounter(IApplicationErrorDelegate *this) {
	this->vptr->ResetCounter(this);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IAPPLICATIONERRORDELEGATEVTBL_H_ */
