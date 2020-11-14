/**
 ******************************************************************************
 * @file    IIS3DWBTask.h
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
#ifndef IIS3DWBTASK_H_
#define IIS3DWBTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "SPIBusIF.h"
#include "SensorsCommonParam.h"
#include "iis3dwb_reg.h"
#include "SensorEventSrc.h"
#include "SensorEventSrc_vtbl.h"

//TODO: STF.Begin - where should these be defined ?
#define IIS3DWB_SAMPLES_PER_IT              (128)
#define IIS3DWB_WTM_LEVEL                   (IIS3DWB_SAMPLES_PER_IT)
#define FFT_LEN_AXL                         (uint32_t)(256)
#define OVLP_AXL                            (float)(0.25)
#define N_AVERAGE_AXL                       (int)(4)
//STF.End

#define IIS3DWB_CFG_MAX_LISTENERS           2

/**
 * Create  type name for _IIS3DWBTask.
 */
typedef struct _IIS3DWBTask IIS3DWBTask;


// Public API declaration
//***********************

/**
 * Allocate an instance of IIS3DWBTask.
 *
 * @return a pointer to the generic obejct ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *IIS3DWBTaskAlloc();

/**
 * Get the SPI interface for the sensor task.
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the SPI interface of the sensor.
 */
SPIBusIF *IIS3DWBTaskGetSensorIF(IIS3DWBTask *_this);

/**
 * Get the ::IEventSrc interface for the sensor task.
 * @param _this [IN] specifies a pointer to a task object.
 * @return a pointer to the ::IEventSrc interface of the sensor.
 */
IEventSrc *IIS3DWBTaskGetEventSrcIF(IIS3DWBTask *_this);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* IIS3DWBTASK_H_ */
