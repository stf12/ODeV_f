/**
 ******************************************************************************
 * @file    syslowpower.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jun 2, 2017
 *
 * @brief This file declares the public API related to the power management.
 *
 * This header file declares the public API and the data structures used by the
 * application tasks in order to:
 * - Inform the system INIT task about the the status of the events that can
 * trigger a power mode change.
 * - Request to switch to a power mode.
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
#ifndef SYSLOWPOWER_H_
#define SYSLOWPOWER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"
#include "syserror.h"
#include "sysevent.h"


#ifndef SYS_CFG_USE_DEFAULT_PM_HELPER
#error Missing definition: SYS_CFG_USE_DEFAULT_PM_HELPER must be defined in sysconfig.h
#endif

#if (SYS_CFG_USE_DEFAULT_PM_HELPER==1)
#include "sysdeflowpower.h"
#else
#include "applowpower.h"
#endif

/**
 * Macro to make system power mode event.
 *
 * @param src [IN] specifies the source of the event
 * @param params [IN] specifies a parameter. Its value depend on the event source.
 */
#define SYS_PM_MAKE_EVENT(src, params)      ((((src) & 0X7U) | (((params)<<3) & 0xF8U)) & 0x000000FF)

/**
 * Check if the current code is inside an ISR or not.
 */
#define SYS_IS_CALLED_FROM_ISR() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ? TRUE : FALSE)

#ifndef SysPostEvent
#define SysPostPowerModeEvent SysPostEvent
#endif


// Public API declaration
//***********************

/**
 * Get a copy of the system status related to the power management.
 *
 * @return copy of the system status related to the power management.
 */
SysPowerStatus SysGetPowerStatus();

/**
 * Get the current system power mode.
 *
 * @return the current system power mode.
 */
EPowerMode SysGetPowerMode();

/**
 * Notify the system about an event related to the power mode management.
 * This function can be called also from an ISR.
 *
 * @param xEvent [IN] specifies a power mode event.
 * @return SYS_NO_ERROR_CODE if the event has been posted in the system queue with success,
 *         an error code otherwise.
 */
sys_error_code_t SysPostPowerModeEvent(SysEvent xEvent);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SYSLOWPOWER_H_ */
