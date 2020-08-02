/**
 ******************************************************************************
 * @file    IIODriver.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Aug 6, 2019
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

#include "IIODriver.h"
#include "IIODriver_vtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
//extern sys_error_code_t IDrvInit(IDriver *this, void *pParams);
//extern sys_error_code_t IDrvStart(IDriver *this);
//extern sys_error_code_t IDrvStop(IDriver *this);
//extern sys_error_code_t IDrvDoEnterPowerMode(IDriver *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
extern sys_error_code_t IIODrvWrite(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);
extern sys_error_code_t IIODrvRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);
#endif

