/**
 ******************************************************************************
 * @file    IBootVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 22, 2017
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
#ifndef INCLUDE_SERVICES_IBOOTVTBL_H_
#define INCLUDE_SERVICES_IBOOTVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "systypes.h"
#include "syserror.h"

/**
 * Create a type name for _IBoot_vtbl.
 */
typedef struct _IBoot_vtbl IBoot_vtbl;

/**
 * Specifies the virtual table for the ::IBoot class.
 */
struct _IBoot_vtbl {
  sys_error_code_t (*Init)(IBoot *this);
  boolean_t (*CheckDFUTrigger)(IBoot *this);
  uint32_t (*GetAppAdderss)(IBoot *this);
  sys_error_code_t (*OnJampToApp)(IBoot *this, uint32_t nAppDress);
};

/**
 * IBoot interface internal state.
 * It declares only the virtual table used to implement the inheritance.
 */
struct _IBoot {
  /**
   * Pointer to the virtual table for the class.
   */
  const IBoot_vtbl *vptr;
};


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IBootInit(IBoot *this) {
  return this->vptr->Init(this);
}

SYS_DEFINE_INLINE
boolean_t IBootCheckDFUTrigger(IBoot *this) {
  return this->vptr->CheckDFUTrigger(this);
}

SYS_DEFINE_INLINE
uint32_t IBootGetAppAdderss(IBoot *this) {
  return this->vptr->GetAppAdderss(this);
}

SYS_DEFINE_INLINE
sys_error_code_t IBootOnJampToApp(IBoot *this, uint32_t nAppDress) {
  return this->vptr->OnJampToApp(this, nAppDress);
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IBOOTVTBL_H_ */
