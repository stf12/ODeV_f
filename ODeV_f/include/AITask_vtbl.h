/**
 ******************************************************************************
 * @file    AITask_vtbl.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 22, 2020
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
#ifndef AITASK_VTBL_H_
#define AITASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// AManagedTaskEx virtual functions
sys_error_code_t AITask_vtblHardwareInit(AManagedTask *_this, void *pParams); ///< @sa AMTHardwareInit
sys_error_code_t AITask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority); ///< @sa AMTOnCreateTask
sys_error_code_t AITask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t AITask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t AITask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode); ///< @sa AMTExForceExecuteStep

// IIListener virtual functions
sys_error_code_t AITaskSEL_vtblOnStatusChange(IListener *_this);                                   ///< @sa IListenerOnStatusChange

// IEventListener virtual functions
void AITaskSEL_vtblSetOwner(IEventListener *_this, void *pxOwner);                                 ///< @sa IEventListenerSetOwner
void *AITaskSEL_vtblGetOwner(IEventListener *_this);                                               ///< @sa IEventListenerGetOwner

// ISensorEventListener virtual functions
sys_error_code_t AITaskSEL_vtblOnNewDataReady(IEventListener *_this, const SensorEvent *pxEvt);   ///< @sa ISensorEventListenerOnNewDataReady

#ifdef __cplusplus
}
#endif

#endif /* AITASK_VTBL_H_ */
