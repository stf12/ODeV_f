/**
 ******************************************************************************
 * @file    ISM330DHCXTask_vtbl.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 7, 2020
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
#ifndef ISM330DHCXTASK_VTBL_H_
#define ISM330DHCXTASK_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// AManagedTaskEx virtual functions

/**
 * Initialize the hardware resource for the task.
 * This task doesn't need a driver extending the ::IDriver interface because:
 * - it manages two GPIO pins, that are the CS connected to the sensor SPI IF and the EXTI line.
 * - it uses the common sensor driver provided by the ST Sensor Solutions Software Team .
 *
 * @param _this [IN] specifies a task object.
 * @param pParams [IN] specifies task specific parameters. Not used
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 * @sa AMTHardwareInit
 */
sys_error_code_t ISM330DHCXTask_vtblHardwareInit(AManagedTask *_this, void *pParams);
sys_error_code_t ISM330DHCXTask_vtblOnCreateTask(AManagedTask *_this, TaskFunction_t *pvTaskCode, const char **pcName, unsigned short *pnStackDepth, void **pParams, UBaseType_t *pxPriority); ///< @sa AMTOnCreateTask
sys_error_code_t ISM330DHCXTask_vtblDoEnterPowerMode(AManagedTask *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode); ///< @sa AMTDoEnterPowerMode
sys_error_code_t ISM330DHCXTask_vtblHandleError(AManagedTask *_this, SysEvent xError); ///< @sa AMTHandleError
sys_error_code_t ISM330DHCXTask_vtblForceExecuteStep(AManagedTaskEx *_this, EPowerMode eActivePowerMode); ///< @sa AMTExForceExecuteStep


#ifdef __cplusplus
}
#endif

#endif /* ISM330DHCXTASK_VTBL_H_ */
