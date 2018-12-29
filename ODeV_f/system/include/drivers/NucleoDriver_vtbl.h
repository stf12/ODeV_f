/**
 ******************************************************************************
 * @file    NucleoDriver_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 2, 2018
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
#ifndef INCLUDE_DRIVERS_NUCLEODRIVER_VTBL_H_
#define INCLUDE_DRIVERS_NUCLEODRIVER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @sa IDrvInit
 */
sys_error_code_t NucleoDriver_vtblInit(IDriver *_this, void *pParams);

/**
 * @sa IDrvStart
 */
sys_error_code_t NucleoDriver_vtblStart(IDriver *_this);

/**
 * @sa IDrvStop
 */
sys_error_code_t NucleoDriver_vtblStop(IDriver *_this);

/**
 *
 * @sa IDrvDoEnterPowerMode
 */
sys_error_code_t NucleoDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * @sa IDrvWrite
 */
sys_error_code_t NucleoDriver_vtblWrite(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

/**
 * @sa IDrvRead
 */
sys_error_code_t NucleoDriver_vtblRead(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_NUCLEODRIVER_VTBL_H_ */
