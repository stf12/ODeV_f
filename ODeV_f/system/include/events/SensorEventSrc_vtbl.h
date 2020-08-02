/**
 ******************************************************************************
 * @file    SensorEventSrc_vtbl.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 13, 2020
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
#ifndef INCLUDE_EVENTS_SENSOREVENTSRC_VTBL_H_
#define INCLUDE_EVENTS_SENSOREVENTSRC_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


// Inline functions definition
// ***************************

// IEventSrc virtual functions
sys_error_code_t SensorEventSrc_vtblSendEvent(const IEventSrc *this, const IEvent *pxEvent, void *pvParams);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_SENSOREVENTSRC_VTBL_H_ */
