/**
 ******************************************************************************
 * @file    IErrorFirstResponder.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Aug 11, 2017
 *
 * @brief Declare the Error first responder interface.
 *
 * A first responder object (::IErrFirstResponder) can be used by the
 * application in order to provide a quick response to critical errors.
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
#ifndef INCLUDE_SERVICES_IERRORFIRSTRESPONDER_H_
#define INCLUDE_SERVICES_IERRORFIRSTRESPONDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"


/**
 * Create  type name for _IErrFirstResponder.
 */
typedef struct _IErrFirstResponder IErrFirstResponder;


// Public API declaration
//***********************

/**
 * Set the owner of the listener.
 *
 * @param _this [IN] specifies a pointer to an ::IErrFirstResponder object.
 * @param pxOwner [IN] specifies a pointer to an application specific object that become the owner of this error first responder.
 */
inline void IErrFirstResponderSetOwner(IErrFirstResponder *_this, void *pxOwner);

/**
 * Get the pointer to this object's owner.
 *
 * @param _this [IN] specifies a pointer to an ::IErrFirstResponder object.
 */
inline void *IErrFirstResponderGetOwner(IErrFirstResponder *_this);

/**
 * Implement a first response to an error. This method is called by the application error delegate as soon as
 * an error is detected. The implementation must be short and do only a time critical things related to the error.
 * Other error related stuff will be performed in a second time.
 *
 * @param _this [IN] specifies a pointer to an ::IErrFirstResponder object.
 * @param xError [IN] specifies the occurred error.
 * @param bIsCalledFromISR [IN] it is \a TRUE if the method is called from an ISR call stack, \a FALSE otherwise.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IErrorFirstResponderNewError(IErrFirstResponder *_this, SysEvent xError, boolean_t bIsCalledFromISR);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IERRORFIRSTRESPONDER_H_ */
