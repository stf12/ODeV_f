/**
 ******************************************************************************
 * @file    AEventSrc.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 13, 2020
 *
 * @brief   Generic implementation of the ::IEventSrc interface.
 *
 * Generic abstract implementation of the ::IEventSrc interface.
 * The ::IEventListener objects are managed with an array of fixed size.
 * This class must be extended in order to define the
 * IEventSrcSendEvent() method.
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
#ifndef INCLUDE_EVENTS_AEVENTSRC_H_
#define INCLUDE_EVENTS_AEVENTSRC_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IEventSrc.h"
#include "IEventSrcVtbl.h"
#include "systp.h"


#ifndef AEVENT_SRC_CFG_MAX_LISTENERS
#define AEVENT_SRC_CFG_MAX_LISTENERS         2
#endif


/**
 * Create  type name for _AEventSrc.
 */
typedef struct _AEventSrc AEventSrc;

/**
 * AEventSrc internal state.
 */
struct _AEventSrc {
  IEventSrc super;

  /**
   * Set of IEventListener object.
   */
  IEventListener *m_pxListeners[AEVENT_SRC_CFG_MAX_LISTENERS];

  /**
   * Specifies the owner of the object.
   */
  void *m_pxOwner;
};


// Public API declaration
//***********************

/**
 * Set the owner of the event source object.
 * @param _this [IN] specifies a pointer to an ::AEventSrc object.
 * @param pxOwner [IN] specifies a pointer to an application specific object that become the owner of the event source.
 *
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t AEvtSrcSetOwner(IEventSrc *_this, void *pxOwner);

/**
 * Get the pointer to the owner of the event source.
 * @param _this [IN] specifies a pointer to an ::AEventSrc object.
 */
void *AEvtSrcGetOwner(IEventSrc *_this);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_AEVENTSRC_H_ */
