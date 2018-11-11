/**
 ******************************************************************************
 * @file    IDriver.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Sep 6, 2016
 *
 * @brief   Public API for the Driver Interface.
 *
 * IDriver is the base interface for the Driver subsystem. Each driver
 * implements this interface.
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

#ifndef INCLUDE_DRIVERS_IDRIVER_H_
#define INCLUDE_DRIVERS_IDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "syslowpower.h"

/**
 * Creates a type name for _IDriver.
 */
typedef struct _IDriver IDriver;

/**
 * Initialize the driver. This method should be used by a task object during
 * the hardware initialization process.
 *
 * @param this [IN] specifies a pointer to a IDriver object.
 * @param pParams specifies a pointer to a subclass defined initialization parameters.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvInit(IDriver *this, void *pParams);

/**
 *
 * @param this s[IN] specifies a pointer to a IDriver object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvStart(IDriver *this);

/**
 *
 * @param this [IN] specifies a pointer to a IDriver object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvStop(IDriver *this);

/**
 * This function is called by the framework when the system changing the power mode.
 *
 * @param this [IN] specifies a pointer to a IDriver object.
 * @param eActivePowerMode [IN] specifies the actual power mode.
 * @param eNewPowerMode [IN] specifies the new power mode.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvDoEnterPowerMode(IDriver *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 *
 * @param this [IN] specifies a pointer to a IDriver object.
 * @param pDataBuffer [IN] specifies the buffer used to store the received data.
 * @param nDataSize [IN] specified the size in byte of the buffer.
 * @param nChannel [IN] not used by
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvWrite(IDriver *this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

/**
 *
 * @param this [IN] specifies a pointer to a IDriver object.
 * @param pDataBuffer
 * @param nDataSize
 * @param nChannel
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvRead(IDriver *this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_DRIVERS_IDRIVER_H_ */
