/**
 ******************************************************************************
 * @file    SensorEventSrc.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 13, 2020
 *
 * @brief   Extend the abstract ::AEventSrc class.
 *
 * ::IEventSrc class specialized for the ::SensorEvent.
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
#ifndef INCLUDE_EVENTS_SENSOREVENTSRC_H_
#define INCLUDE_EVENTS_SENSOREVENTSRC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "AEventSrc.h"
#include "AEventSrc_vtbl.h"


/**
 * Create  type name for _SensorEventSrc.
 */
typedef struct _SensorEventSrc SensorEventSrc;


// Public API declaration
//***********************

/**
 * Allocate an instance of SensorEventSrc.
 *
 * @return a pointer to the generic object ::IEventSrc if success,
 * or NULL if out of memory error occurs.
 */
IEventSrc *SensorEventSrcAlloc();

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_SENSOREVENTSRC_H_ */
