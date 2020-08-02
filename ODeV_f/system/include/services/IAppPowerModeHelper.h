/**
 ******************************************************************************
 * @file    IAppPowerModeHelper.h
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
#ifndef INCLUDE_SERVICES_IAPPPOWERMODEHELPER_H_
#define INCLUDE_SERVICES_IAPPPOWERMODEHELPER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "syslowpower.h"


/**
 * Create  type name for _IAppPowerModeHelper.
 */
typedef struct _IAppPowerModeHelper IAppPowerModeHelper;


// Public API declaration
//***********************

/**
 * Initialize the interface IAppPowerModeHelper. It should be called after the object allocation and before using the object.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IapmhInit(IAppPowerModeHelper *_this);

/**
 * Compute the new power mode depending on the input event.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param xEvent [IN] an power mode input event.
 * @return the power mode that the system should enter due to the event.
 */
inline EPowerMode IapmhComputeNewPowerMode(IAppPowerModeHelper *_this, const SysEvent xEvent);

/**
 * Used mainly for debug purpose. It checks if a request power mode transaction is valid.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param eActivePowerMode [IN] species the actual power mode.
 * @param eNewPowerMode [IN] specifies a new power mode.
 * @return `TRUE` if the transaction is valid. If the transaction is not valid the program execution is stopped.
 */
inline boolean_t IapmhCheckPowerModeTransaction(IAppPowerModeHelper *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * Used by the system to enter the new power mode. It is called after all application task are ready for the new power mode.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param ePowerMode [IN] specifies a new power mode.
 */
inline sys_error_code_t IapmhDidEnterPowerMode(IAppPowerModeHelper *_this, EPowerMode ePowerMode);

/**
 * Get the active power mode.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return the active power mode.
 */
inline EPowerMode IapmhGetActivePowerMode(IAppPowerModeHelper *_this);

/**
 * Get a copy of the actual power status.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return a copy of the actual power status.
 */
inline SysPowerStatus IapmhGetPowerStatus(IAppPowerModeHelper *_this);

/**
 * Check if a power mode is a low power mode that put the MCU in STOPx mode or higher power efficient mode.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @param ePowerMode [IN] species a power mode.
 * @return `TRUE` if ePowerMode is a low power mode, `FALSE' otherwise.
 */
inline boolean_t IapmhIsLowPowerMode(IAppPowerModeHelper *_this, const EPowerMode ePowerMode);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_IAPPPOWERMODEHELPER_H_ */
