/**
 ******************************************************************************
 * @file    IDriverVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Sep 7, 2016
 * @brief   Private API for the Driver Interface
 *
 * This header file must be included included in all source files that use the
 * IDriver public API.
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

#ifndef INCLUDE_DRIVERS_IDRIVERVTBL_H_
#define INCLUDE_DRIVERS_IDRIVERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "systp.h"

typedef struct _IDriver_vtbl IDriver_vtbl;


struct _IDriver_vtbl {
	sys_error_code_t (*Init)(IDriver *_this, void *pParams);
	sys_error_code_t (*Start)(IDriver *_this);
	sys_error_code_t (*Stop)(IDriver *_this);
	sys_error_code_t (*DoEnterPowerMode)(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
	sys_error_code_t (*Write)(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);
	sys_error_code_t (*Read)(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);
};

/**
 * IDriver interface internal state. This is the base interface for the the driver subsystem.
 * It declares only the virtual table used to implement the inheritance.
 */
struct _IDriver {
	const IDriver_vtbl *vptr;
};

// Inline function definition.
//

SYS_DEFINE_INLINE
sys_error_code_t IDrvInit(IDriver *_this, void *pParams) {
	return _this->vptr->Init(_this, pParams);
}

SYS_DEFINE_INLINE
sys_error_code_t IDrvStart(IDriver *_this) {
	return _this->vptr->Start(_this);
}

SYS_DEFINE_INLINE
sys_error_code_t IDrvStop(IDriver *_this) {
	return _this->vptr->Stop(_this);
}

SYS_DEFINE_INLINE
sys_error_code_t IDrvDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
	return _this->vptr->DoEnterPowerMode(_this, eActivePowerMode, eNewPowerMode);
}

SYS_DEFINE_INLINE
sys_error_code_t IDrvWrite(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
	return _this->vptr->Write(_this, pDataBuffer, nDataSize, nChannel);
}

SYS_DEFINE_INLINE
sys_error_code_t IDrvRead(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
	return _this->vptr->Read(_this, pDataBuffer, nDataSize, nChannel);
}


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_DRIVERS_IDRIVERVTBL_H_ */
