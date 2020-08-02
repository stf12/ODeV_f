/**
 ******************************************************************************
 * @file    ISensorEventListener_vtbl.h
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
#ifndef INCLUDE_EVENTS_ISENSOREVENTLISTENER_VTBL_H_
#define INCLUDE_EVENTS_ISENSOREVENTLISTENER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IEventListenerVtbl.h"


/**
 * Create  type name for _ISensorEventListener_vtbl.
 */
typedef struct _ISensorEventListener_vtbl ISensorEventListener_vtbl;


/**
 * Specifies the virtual table for the  class.
 */
struct _ISensorEventListener_vtbl {
  sys_error_code_t (*OnStatusChange)(IListener *this);                                   ///< @sa IListenerOnStatusChange
  void (*SetOwner)(IEventListener *this, void *pxOwner);                                 ///< @sa IEventListenerSetOwner
  void *(*GetOwner)(IEventListener *this);                                               ///< @sa IEventListenerGetOwner
  sys_error_code_t (*OnNewDataReady)(IEventListener *_this, const SensorEvent *pxEvt);   ///< @sa ISensorEventListenerOnNewDataReady
};

/**
 * _ISensorEventListener interface internal state.
 * It declares only the virtual table used to implement the inheritance.
 */
struct _ISensorEventListener {
  /**
   * Pointer to the virtual table for the class.
   */
  const ISensorEventListener_vtbl *vptr;
};


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t ISensorEventListenerOnNewDataReady(IEventListener *_this, const SensorEvent *pxEvt) {
  assert_param(_this);

  return ((ISensorEventListener*)_this)->vptr->OnNewDataReady(_this, pxEvt);
}


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_ISENSOREVENTLISTENER_VTBL_H_ */
