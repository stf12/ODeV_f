/**
 ******************************************************************************
 * @file    IListenerVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 6, 2017
 *
 * @brief   IListener virtual table definition.
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

#ifndef INCLUDE_EVENTS_ILISTENERVTBL_H_
#define INCLUDE_EVENTS_ILISTENERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systypes.h"
#include "syserror.h"
#include "systp.h"

typedef struct _IListener_vtbl IListener_vtbl;


struct _IListener_vtbl {
	sys_error_code_t (*OnStatusChange)(IListener *_this);
};

struct _IListener {
	const IListener_vtbl *vptr;
};

// Public API declaration
//***********************



// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IListenerOnStatusChange(IListener *_this) {
	return _this->vptr->OnStatusChange(_this);
}

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_EVENTS_ILISTENERVTBL_H_ */
