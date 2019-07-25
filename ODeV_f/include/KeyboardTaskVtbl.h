/**
 ******************************************************************************
 * @file    KeyboardTaskVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 13, 2017
 * @brief
 *
 * TODO - insert here the file description
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

#ifndef KEYBOARDTASKVTBL_H_
#define KEYBOARDTASKVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// AManagedTaskEx virtual functions
sys_error_code_t KeyboardHardwareInit(AManagedTask *this, void *pParams);
sys_error_code_t KeyboardOnCreateTask(AManagedTask *this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority);
sys_error_code_t KeyboardDoEnterPowerMode(AManagedTask *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);
sys_error_code_t KeyboardHandleError(AManagedTask *this, SysEvent xError);
sys_error_code_t Keyboard_vtblForceExecuteStep(AManagedTaskEx *this, EPowerMode eActivePowerMode); ///< @sa AMTForceExecuteStepRun


// IErrFirstResponder virtual functions
void KeyboardErrFRSetOwner(IErrFirstResponder *this, void *pxOwner);
void *KeyboardErrFRGetOwner(IErrFirstResponder *this);
sys_error_code_t KeyboardErrFRNewError(IErrFirstResponder *this, SysEvent xError, boolean_t bIsCalledFromISR);


#ifdef __cplusplus
}
#endif


#endif /* KEYBOARDTASKVTBL_H_ */
