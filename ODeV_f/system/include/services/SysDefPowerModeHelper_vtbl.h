/**
 ******************************************************************************
 * @file    SysDefPowerModeHelper_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Oct 31, 2018
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
#ifndef INCLUDE_SERVICES_SYSDEFPOWERMODEHELPER_VTBL_H_
#define INCLUDE_SERVICES_SYSDEFPOWERMODEHELPER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


sys_error_code_t SysDefPowerModeHelper_vtblInit(IAppPowerModeHelper *this); ///< @sa IapmhInit
EPowerMode SysDefPowerModeHelper_vtblComputeNewPowerMode(IAppPowerModeHelper *this, const SysEvent xEvent); ///< @sa IapmhComputeNewPowerMode
boolean_t SysDefPowerModeHelper_vtblCheckPowerModeTransaction(IAppPowerModeHelper *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa IapmhCheckPowerModeTransaction
sys_error_code_t SysDefPowerModeHelper_vtblDidEnterPowerMode(IAppPowerModeHelper *this, EPowerMode ePowerMode); ///< @sa IapmhDidEnterPowerMode
EPowerMode SysDefPowerModeHelper_vtblGetActivePowerMode(IAppPowerModeHelper *this); ///< @sa IapmhGetActivePowerMode
SysPowerStatus SysDefPowerModeHelper_vtblGetPowerStatus(IAppPowerModeHelper *this); ///< @sa IapmhGetPowerStatus
boolean_t SysDefPowerModeHelper_vtblIsLowPowerMode(IAppPowerModeHelper *this, const EPowerMode ePowerMode); ///< @sa IapmhIsLowPowerMode


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_SYSDEFPOWERMODEHELPER_VTBL_H_ */
