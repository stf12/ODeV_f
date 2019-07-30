/**
 ******************************************************************************
 * @file    IFNDelegate.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jun 24, 2017
 *
 * @brief   Function Key delegate interface.
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
#ifndef IFNDELEGATE_H_
#define IFNDELEGATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"

/**
 * Creates a type name for _IFNDelegate.
 */
typedef struct _IFNDelegate IFNDelegate;


// Public API declaration
//***********************

/**
 * Called when a function key is pressed.
 *
 * @param _this [IN] specifies a pointer to an object implementing the interface.
 * @param nKeycode [IN] specifies a Fx key code.
 */
inline void IFNDelegateOnKeyPressed(IFNDelegate *_this, uint8_t nKeycode);

/**
 * Called when a function key is released.
 *
 * @param _this [IN] specifies a pointer to an object implementing the interface.
 * @param nKeycode [IN] specifies a Fx key code.
 */
inline void IFNDelegateOnKeyReleased(IFNDelegate *_this, uint8_t nKeycode);

/**
 * Called when the FN key change status (pressed or released). It depends on the FN and FN Lock keys.
 *
 * @param _this [IN] specifies a pointer to an object implementing the interface.
 * @param bFnActive TRUE if FN is active, FALSE otherwise.
 */
inline void IFNDelegateOnFnStatusChanged(IFNDelegate *_this, boolean_t bFnActive);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* IFNDELEGATE_H_ */