/**
 ******************************************************************************
 * @file    PushButtonTask_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Nov 5, 2018
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
#ifndef PUSHBUTTONTASK_VTBL_H_
#define PUSHBUTTONTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// IManagedTask virtual functions
sys_error_code_t PushButtonTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t PushButtonTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
    VOID **pvStackStart, ULONG *pnStackSize,
    UINT *pnPriority, UINT *pnPreemptThreshold,
    ULONG *pnTimeSlice, ULONG *pnAutoStart,
    ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t PushButtonTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t PushButtonTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t PushButtonTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode); ///< @sa AMTExForceExecuteStep


#ifdef __cplusplus
}
#endif

#endif /* PUSHBUTTONTASK_VTBL_H_ */
