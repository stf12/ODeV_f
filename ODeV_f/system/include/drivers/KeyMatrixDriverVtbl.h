/**
 ******************************************************************************
 * @file    KeyMatrixDriverVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 13, 2016
 *
 * @brief Declaration of the virtual method overloaded by the KeyMatrixDriver.
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

#ifndef INCLUDE_DRIVERS_KEYMATRIXDRIVERVTBL_H_
#define INCLUDE_DRIVERS_KEYMATRIXDRIVERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @sa IDrvInit
 */
sys_error_code_t KeyMatrixDrvInit(IDriver *this, void *pParams);

/**
 * @sa IDrvStart
 */
sys_error_code_t KeyMatrixDrvStart(IDriver *this);

/**
 * @sa IDrvStop
 */
sys_error_code_t KeyMatrixDrvStop(IDriver *this);

/**
 * @sa IDrvEnablePowerMode
 */
sys_error_code_t KeyMatrixDrvDoEnterPowerMode(IDriver *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * Not implemented.
 *
 * @sa IDrvWrite
 */
sys_error_code_t KeyMatrixDrvWrite(IDriver *this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

/**
 * Not implemented.
 *
 * @sa IDrvRead
 */
sys_error_code_t KeyMatrixDrvRead(IDriver *this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_DRIVERS_KEYMATRIXDRIVERVTBL_H_ */
