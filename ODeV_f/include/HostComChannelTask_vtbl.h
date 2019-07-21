/**
 ******************************************************************************
 * @file    HostComChannelTask_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Mar 8, 2018
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
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
#ifndef HOSTCOMCHANNELTASK_VTBL_H_
#define HOSTCOMCHANNELTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// IManagedTask virtual functions
sys_error_code_t HostComChannelTask_vtblHardwareInit(AManagedTask *this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t HostComChannelTask_vtblOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority); ///< @sa AMTOnCreateTask
sys_error_code_t HostComChannelTask_vtblDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t HostComChannelTask_vtblHandleError(AManagedTask *this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t HostComChannelTask_vtblForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode); ///< @sa AMTForceExecuteStepRun

#ifdef __cplusplus
}
#endif

#endif /* HOSTCOMCHANNELTASK_VTBL_H_ */
