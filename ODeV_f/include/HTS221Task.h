/**
 ******************************************************************************
 * @file    HTS221Task.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Aug 29, 2020
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
#ifndef HTS221TASK_H_
#define HTS221TASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "I2CBusIF.h"
#include "SensorsCommonParam.h"
#include "hts221_reg.h"
#include "SensorEventSrc.h"
#include "SensorEventSrc_vtbl.h"
#include "queue.h"

//TODO: STF.Begin - where should these be defined ?
#define HTS221_GY_MAX_SAMPLES_PER_IT     (256)
#define HTS221_GY_SAMPLES_PER_IT         (HTS221_GY_MAX_SAMPLES_PER_IT)
#define HTS221_AX_SAMPLES_PER_IT         (0)
#define HTS221_WTM_LEVEL                 (HTS221_GY_SAMPLES_PER_IT)
//STF.End

#define HTS221_CFG_MAX_LISTENERS         2

/**
 * Create a type name for _HTS221Task.
 */
typedef struct _HTS221Task HTS221Task;


// Public API declaration
//***********************

/**
 * Allocate an instance of HTS221Task.
 *
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *HTS221TaskAlloc();

/**
 * Get the SPI interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the SPI interface of the sensor.
 */
I2CBusIF *HTS221TaskGetSensorIF(HTS221Task *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc *HTS221TaskGetEventSrcIF(HTS221Task *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* HTS221TASK_H_ */
