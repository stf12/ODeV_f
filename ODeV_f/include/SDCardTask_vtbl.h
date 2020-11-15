/**
 ******************************************************************************
 * @file    SDCardTask_vtbl.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 23, 2020
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
#ifndef SDCARDTASK_VTBL_H_
#define SDCARDTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// AManagedTaskEx virtual functions
sys_error_code_t SDCardTask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t SDCardTask_vtblOnCreateTask(AManagedTask *_this, tx_entry_function_t *pvTaskCode, CHAR **pcName,
    VOID **pvStackStart, ULONG *pnStackSize,
    UINT *pnPriority, UINT *pnPreemptThreshold,
    ULONG *pnTimeSlice, ULONG *pnAutoStart,
    ULONG *pnParams); ///< @sa AMTOnCreateTask
sys_error_code_t SDCardTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t SDCardTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t SDCardTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode); ///< @sa AMTExForceExecuteStep

// IIListener virtual functions
sys_error_code_t SDCardTaskSEL_vtblOnStatusChange(IListener *_this);                                   ///< @sa IListenerOnStatusChange

// IEventListener virtual functions
void SDCardTaskSEL_vtblSetOwner(IEventListener *_this, void *pxOwner);                                 ///< @sa IEventListenerSetOwner
void *SDCardTaskSEL_vtblGetOwner(IEventListener *_this);                                               ///< @sa IEventListenerGetOwner

// ISensorEventListener virtual functions
sys_error_code_t SDCardTaskSEL_vtblOnNewDataReady(IEventListener *_this, const SensorEvent *pxEvt);   ///< @sa ISensorEventListenerOnNewDataReady

#ifdef __cplusplus
}
#endif

#endif /* SDCARDTASK_VTBL_H_ */
