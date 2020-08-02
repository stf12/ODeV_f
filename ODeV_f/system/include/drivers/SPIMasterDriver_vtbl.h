/**
 ******************************************************************************
 * @file    SPIMasterDriver_vtbl.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 3, 2020
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
#ifndef INCLUDE_DRIVERS_SPIMASTERDRIVER_VTBL_H_
#define INCLUDE_DRIVERS_SPIMASTERDRIVER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @sa IDrvInit
 */
sys_error_code_t SPIMasterDriver_vtblInit(IDriver *_this, void *pParams);

/**
 * @sa IDrvStart
 */
sys_error_code_t SPIMasterDriver_vtblStart(IDriver *_this);

/**
 * @sa IDrvStop
 */
sys_error_code_t SPIMasterDriver_vtblStop(IDriver *_this);

/**
 *
 * @sa IDrvDoEnterPowerMode
 */
sys_error_code_t SPIMasterDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode);

/**
 * @sa IDrvReset
 */
sys_error_code_t SPIMasterDriver_vtblReset(IDriver *_this, void *pParams);

/**
 * @sa IIODrvWrite
 */
sys_error_code_t SPIMasterDriver_vtblWrite(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

/**
 * @sa IIODrvRead
 */
sys_error_code_t SPIMasterDriver_vtblRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_SPIMASTERDRIVER_VTBL_H_ */
