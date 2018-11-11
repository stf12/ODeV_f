/**
 ******************************************************************************
 * @file    IAppPowerModeHelper_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Oct 30, 2018
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2018 STMicroelectronics</center></h2>
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
#ifndef INCLUDE_SERVICES_IAPPPOWERMODEHELPER_VTBL_H_
#define INCLUDE_SERVICES_IAPPPOWERMODEHELPER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "systypes.h"
#include "syserror.h"


/**
 * Create  type name for _IAppPowerModeHelper_vtbl.
 */
typedef struct _IAppPowerModeHelper_vtbl IAppPowerModeHelper_vtbl;

/**
 * Specifies the virtual table for the  class.
 */
struct _IAppPowerModeHelper_vtbl {
  sys_error_code_t (*Init)(IAppPowerModeHelper *this);
  EPowerMode (*ComputeNewPowerMode)(IAppPowerModeHelper *this, const SysEvent xEvent);
  boolean_t (*CheckPowerModeTransaction)(IAppPowerModeHelper *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
  sys_error_code_t (*DidEnterPowerMode)(IAppPowerModeHelper *this, EPowerMode ePowerMode);
  EPowerMode (*GetActivePowerMode)(IAppPowerModeHelper *this);
  SysPowerStatus (*GetPowerStatus)(IAppPowerModeHelper *this);
  boolean_t (*IsLowPowerMode)(IAppPowerModeHelper *this, const EPowerMode ePowerMode);
};

/**
 * IF_NAME interface internal state.
 * It declares only the virtual table used to implement the inheritance.
 */
struct _IAppPowerModeHelper {
  /**
   * Pointer to the virtual table for the class.
   */
  const IAppPowerModeHelper_vtbl *vptr;
};


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t IapmhInit(IAppPowerModeHelper *this) {
  return this->vptr->Init(this);
}

SYS_DEFINE_INLINE
EPowerMode IapmhComputeNewPowerMode(IAppPowerModeHelper *this, const SysEvent xEvent) {
  return this->vptr->ComputeNewPowerMode(this, xEvent);
}

SYS_DEFINE_INLINE
boolean_t IapmhCheckPowerModeTransaction(IAppPowerModeHelper *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  return this->vptr->CheckPowerModeTransaction(this, eActivePowerMode, eNewPowerMode);
}

SYS_DEFINE_INLINE
sys_error_code_t IapmhDidEnterPowerMode(IAppPowerModeHelper *this, EPowerMode ePowerMode) {
  return this->vptr->DidEnterPowerMode(this, ePowerMode);
}

SYS_DEFINE_INLINE
EPowerMode IapmhGetActivePowerMode(IAppPowerModeHelper *this) {
  return this->vptr->GetActivePowerMode(this);
}

SYS_DEFINE_INLINE
SysPowerStatus IapmhGetPowerStatus(IAppPowerModeHelper *this) {
  return this->vptr->GetPowerStatus(this);
}

SYS_DEFINE_INLINE
boolean_t IapmhIsLowPowerMode(IAppPowerModeHelper *this, const EPowerMode ePowerMode) {
  return this->vptr->IsLowPowerMode(this, ePowerMode);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IAPPPOWERMODEHELPER_VTBL_H_ */
