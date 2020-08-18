/**
 ******************************************************************************
 * @file    ISM330DHCXTask.h
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
#ifndef ISM330DHCXTASK_H_
#define ISM330DHCXTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "SPIBusIF.h"
#include "SensorsCommonParam.h"
#include "ism330dhcx_reg.h"
#include "SensorEventSrc.h"
#include "SensorEventSrc_vtbl.h"
#include "queue.h"

//TODO: STF.Begin - where should these be defined ?
#define ISM330DHCX_GY_MAX_SAMPLES_PER_IT     (256)
#define ISM330DHCX_GY_SAMPLES_PER_IT         (ISM330DHCX_GY_MAX_SAMPLES_PER_IT)
#define ISM330DHCX_AX_SAMPLES_PER_IT         (0)
#define ISM330DHCX_WTM_LEVEL                 (ISM330DHCX_GY_SAMPLES_PER_IT)
//STF.End

#define ISM330DHCX_CFG_MAX_LISTENERS         2

/**
 * Create a type name for _ISM330DHCXTask.
 */
typedef struct _ISM330DHCXTask ISM330DHCXTask;


// Public API declaration
//***********************

/**
 * Allocate an instance of ISM330DHCXTask.
 *
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *ISM330DHCXTaskAlloc();

/**
 * Get the SPI interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the SPI interface of the sensor.
 */
SPIBusIF *ISM330DHCXTaskGetSensorIF(ISM330DHCXTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc *ISM330DHCXTaskGetEventSrcIF(ISM330DHCXTask *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* ISM330DHCXTASK_H_ */
