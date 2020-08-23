/**
 ******************************************************************************
 * @file    UtilityDriver_vtbl.h
 * @author  Stefano Oliveri
 * @version 1.0.0
 * @date    Aug 23, 2020
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
#ifndef INCLUDE_DRIVERS_UTILITYDRIVER_VTBL_H_
#define INCLUDE_DRIVERS_UTILITYDRIVER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @sa IDrvInit
 */
sys_error_code_t UtilityDriver_vtblInit(IDriver *_this, void *pParams);

/**
 * @sa IDrvStart
 */
sys_error_code_t UtilityDriver_vtblStart(IDriver *_this);

/**
 * @sa IDrvStop
 */
sys_error_code_t UtilityDriver_vtblStop(IDriver *_this);

/**
 *
 * @sa IDrvDoEnterPowerMode
 */
sys_error_code_t UtilityDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * @sa IDrvReset
 */
sys_error_code_t UtilityDriver_vtblReset(IDriver *_this, void *pParams);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_UTILITYDRIVER_VTBL_H_ */
