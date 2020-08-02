/**
 ******************************************************************************
 * @file    AManagedTask.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Mar 20, 2017
 * @brief
 *
 * <DESCRIPTIOM>
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

#include "AManagedTask.h"
#include "AManagedTaskVtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
extern sys_error_code_t AMTHardwareInit(AManagedTask *this, void *pParams);
extern sys_error_code_t AMTOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority);
extern sys_error_code_t AMTDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
extern sys_error_code_t AMTHandleError(AManagedTask *this, SysEvent xError);
extern sys_error_code_t AMTInit(AManagedTask *this);
extern sys_error_code_t AMTNotifyIsStillRunning(AManagedTask *this, sys_error_code_t nStepError);
extern void AMTResetAEDCounter(AManagedTask *this);
extern EPowerMode AMTGetSystemPowerMode();
extern boolean_t AMTIsPowerModeSwitchPending(AManagedTask *this);
extern void AMTReportErrOnStepExecution(AManagedTask *this, sys_error_code_t nStepError);
#endif
