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
  sys_error_code_t (*Init)(IAppPowerModeHelper *_this);
  EPowerMode (*ComputeNewPowerMode)(IAppPowerModeHelper *_this, const SysEvent xEvent);
  boolean_t (*CheckPowerModeTransaction)(IAppPowerModeHelper *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
  sys_error_code_t (*DidEnterPowerMode)(IAppPowerModeHelper *_this, EPowerMode ePowerMode);
  EPowerMode (*GetActivePowerMode)(IAppPowerModeHelper *_this);
  SysPowerStatus (*GetPowerStatus)(IAppPowerModeHelper *_this);
  boolean_t (*IsLowPowerMode)(IAppPowerModeHelper *_this, const EPowerMode ePowerMode);
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
sys_error_code_t IapmhInit(IAppPowerModeHelper *_this) {
  return _this->vptr->Init(_this);
}

SYS_DEFINE_INLINE
EPowerMode IapmhComputeNewPowerMode(IAppPowerModeHelper *_this, const SysEvent xEvent) {
  return _this->vptr->ComputeNewPowerMode(_this, xEvent);
}

SYS_DEFINE_INLINE
boolean_t IapmhCheckPowerModeTransaction(IAppPowerModeHelper *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  return _this->vptr->CheckPowerModeTransaction(_this, eActivePowerMode, eNewPowerMode);
}

SYS_DEFINE_INLINE
sys_error_code_t IapmhDidEnterPowerMode(IAppPowerModeHelper *_this, EPowerMode ePowerMode) {
  return _this->vptr->DidEnterPowerMode(_this, ePowerMode);
}

SYS_DEFINE_INLINE
EPowerMode IapmhGetActivePowerMode(IAppPowerModeHelper *_this) {
  return _this->vptr->GetActivePowerMode(_this);
}

SYS_DEFINE_INLINE
SysPowerStatus IapmhGetPowerStatus(IAppPowerModeHelper *_this) {
  return _this->vptr->GetPowerStatus(_this);
}

SYS_DEFINE_INLINE
boolean_t IapmhIsLowPowerMode(IAppPowerModeHelper *_this, const EPowerMode ePowerMode) {
  return _this->vptr->IsLowPowerMode(_this, ePowerMode);
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IAPPPOWERMODEHELPER_VTBL_H_ */
