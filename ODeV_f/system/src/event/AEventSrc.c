/**
 ******************************************************************************
 * @file    AEventSrc.c
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

#include "AEventSrc.h"
#include "AEventSrc_vtbl.h"
#include <string.h>


// Public functions definition
// ***************************

sys_error_code_t AEvtSrcSetOwner(IEventSrc *_this, void *pxOwner) {
  assert_param(_this);
  AEventSrc *pObj = (AEventSrc*)_this;

  pObj->m_pxOwner = pxOwner;

  return SYS_NO_ERROR_CODE;
}

void *AEvtSrcGetOwner(IEventSrc *_this) {
  assert_param(_this);
  AEventSrc *pObj = (AEventSrc*)_this;

  return pObj->m_pxOwner;
}


// IEventSrc virtual functions definition
// **************************************

sys_error_code_t AEventSrv_vtblInit(IEventSrc *_this) {
  assert_param(_this);
  AEventSrc *pObj = (AEventSrc*)_this;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  memset(pObj->m_pxListeners, 0, sizeof(pObj->m_pxListeners));

  return xRes;
}

sys_error_code_t AEventSrv_vtblAddEventListener(IEventSrc *_this, IEventListener *pListener) {
  assert_param(_this);
  AEventSrc *pObj = (AEventSrc*)_this;
  sys_error_code_t xRes = SYS_IEVTSRC_FULL_ERROR_CODE;

  for (uint8_t i=0; i<AEVENT_SRC_CFG_MAX_LISTENERS; ++i) {
    if (pObj->m_pxListeners[i] == NULL) {
      pObj->m_pxListeners[i] = pListener;
      xRes = SYS_NO_ERROR_CODE;
      break;
    }
  }

  return xRes;
}

sys_error_code_t AEventSrv_vtblRemoveEventListener(IEventSrc *_this, IEventListener *pListener) {
  assert_param(_this);
  AEventSrc *pObj = (AEventSrc*)_this;
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  for (uint8_t i=0; i<AEVENT_SRC_CFG_MAX_LISTENERS; ++i) {
    if (pObj->m_pxListeners[i] == pListener) {
      pObj->m_pxListeners[i] = NULL;
      break;
    }
  }

  return xRes;
}

uint32_t AEventSrv_vtblGetMaxListenerCount(const IEventSrc *_this) {
  UNUSED(_this);

  return AEVENT_SRC_CFG_MAX_LISTENERS;
}
