/**
 ******************************************************************************
 * @file    IIS3DWBTask_vtbl.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 10, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
#ifndef IIS3DWBTASK_VTBL_H_
#define IIS3DWBTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// AManagedTaskEx virtual functions
sys_error_code_t IIS3DWBTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t IIS3DWBTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
    VOID **pvStackStart, ULONG *pnStackSize,
    UINT *pnPriority, UINT *pnPreemptThreshold,
    ULONG *pnTimeSlice, ULONG *pnAutoStart,
    ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t IIS3DWBTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t IIS3DWBTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t IIS3DWBTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode); ///< @sa AMTExForceExecuteStep


// IEventSrc virtual functions

sys_error_code_t IIS3DWBTaskES_vtblInit(IEventSrc *_this);
sys_error_code_t IIS3DWBTaskES_vtblAddEventListener(IEventSrc *_this, IEventListener *pListener);
sys_error_code_t IIS3DWBTaskES_vtblRemoveEventListener(IEventSrc *_this, IEventListener *pListener);
uint32_t IIS3DWBTaskES_vtblGetMaxListenerCount(const IEventSrc *_this);
sys_error_code_t IIS3DWBTaskES_vtblSendEvent(const IEventSrc *_this, const IEvent *pxEvent, void *pvParams);


#ifdef __cplusplus
}
#endif

#endif /* IIS3DWBTASK_VTBL_H_ */
