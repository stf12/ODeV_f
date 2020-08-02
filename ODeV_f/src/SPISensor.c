/**
 ******************************************************************************
 * @file    SPISensor.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 6, 2020
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

#include "SPISensor.h"

// Private functions declaration
// *****************************

static int32_t SPIBusNullRW(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

// Private variables
// *****************

static SPIBusIF s_xSPINullIF = {
    .m_pfRead = SPIBusNullRW,
    .m_pfWrite = SPIBusNullRW
};

// Public API implementation.
// **************************

sys_error_code_t SPISensorInit(SPISensor *_this, uint8_t nWhoAmI, GPIO_TypeDef* pxSSPinPort, uint16_t nSSPin) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_nWhoAmI = nWhoAmI;
  _this->m_pxSSPinPort = pxSSPinPort;
  _this->m_nSSPin = nSSPin;

  // initialize the software resources
  _this->m_xSyncObj = xSemaphoreCreateBinary();
  if (_this->m_xSyncObj == NULL){
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }
#ifdef DEBUG
  else {
    vQueueAddToRegistry(_this->m_xSyncObj, "SPI_IP_S");
  }
#endif

  return xRes;
}

sys_error_code_t SPISensorWaitIOComplete(SPISensor *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (_this->m_xSyncObj != NULL){
    if (pdTRUE != xSemaphoreTake(_this->m_xSyncObj, portMAX_DELAY)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
      xRes = SYS_UNDEFINED_ERROR_CODE;
    }
  }
  else {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
    xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  return xRes;
}

sys_error_code_t SPISensorNotifyIOComplete(SPISensor *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (_this->m_xSyncObj != NULL){
    if (pdTRUE != xSemaphoreGive(_this->m_xSyncObj)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
      xRes = SYS_UNDEFINED_ERROR_CODE;
    }
  }
  else {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
    xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
  }

  return xRes;
}

SPIBusIF *SPISensorGetNullIF() {
  return  &s_xSPINullIF;
}


// Private functions definition
// ****************************

int32_t SPIBusNullRW(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize) {
  UNUSED(pxSensor);
  UNUSED(nRegAddr);
  UNUSED(pnData);
  UNUSED(nSize);

  return 0;
}
