/**
 ******************************************************************************
 * @file    SensorEvent.h
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
#ifndef INCLUDE_EVENTS_SENSOREVENT_H_
#define INCLUDE_EVENTS_ISENSOREVENT_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IEvent.h"


/**
 * Create  type name for _SensorEvent.
 */
typedef struct _SensorEvent SensorEvent;

/**
 * Specifies a generic Event. Each event has a pointer to the ::IEventSrc object
 * that has generated the event.
 */
struct _SensorEvent {
    /**
     * Specifies a pointer to the Event Source that generated the event.
     */
    IEvent super;

    /**
     * Buffer with the data from the sensor
     */
    const uint8_t * pData;

    /**
     * Size of the pData buffer.
     */
    uint16_t nDataSize;

    /**
     * Timestamp
     */
    double fTimeStamp;
};


// Public API declaration
//***********************

/**
 * Initialize the interface ISensorEvent. It should be called after the object allocation and before using the object.
 *
 * @param _this [IN] specifies a pointer to the object.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t SensorEventInit(IEvent *_this, const IEventSrc *pSource, const uint8_t * pData, uint16_t nDataSize, double fTimeStamp);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t SensorEventInit(IEvent *_this, const IEventSrc *pSource, const uint8_t * pData, uint16_t nDataSize, double fTimeStamp) {
  assert_param(_this);
  SensorEvent *pObj = (SensorEvent*)_this;

  IEventInit(_this, pSource);
  pObj->pData = pData;
  pObj->nDataSize = nDataSize;
  pObj->fTimeStamp = fTimeStamp;

  return SYS_NO_ERROR_CODE;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_SENSOREVENT_H_ */
