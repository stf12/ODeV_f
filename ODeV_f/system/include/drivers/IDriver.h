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
 * @param _this [IN] specifies a pointer to a IDriver object.
 * @param pParams [IN] specifies a pointer to a initialization parameters defined by a subclass.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvInit(IDriver *_this, void *pParams);

/**
 * Start the driver. This method enable the driver normal processing. For example it enables the related IRQ.
 * It should be called after the driver initialization.
 *
 * @param _this s[IN] specifies a pointer to a IDriver object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvStart(IDriver *_this);

/**
 * Stop a driver. This method disable the driver normal operation. For example it disables the IRQ. It doesn't
 * de-initialize the driver.
 *
 * @param _this [IN] specifies a pointer to a IDriver object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvStop(IDriver *_this);

/**
 * This function is called by the framework when the system is changing the power mode. The driver
 * must reconfigure itself according the new power mode.
 *
 * @param _this [IN] specifies a pointer to a IDriver object.
 * @param eActivePowerMode [IN] specifies the actual power mode.
 * @param eNewPowerMode [IN] specifies the new power mode.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IDrvDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * Reset the peripherals owned by the driver. This method should operate at hardware level, for example
 * by using the HAL_XXX_DeInit() and HAL_XXX_Init() functions.
 *
 * @param _this [IN] specifies a pointer to a IDriver object.
 * @param pParams [IN] specifies a pointer to a parameters defined by a subclass.
 * @return
 */
inline sys_error_code_t IDrvReset(IDriver *_this, void *pParams);


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_DRIVERS_IDRIVER_H_ */
