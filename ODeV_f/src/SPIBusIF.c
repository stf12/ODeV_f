/**
 ******************************************************************************
 * @file    SPIBusIF.c
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

#include <SPIBusIF.h>

// Private functions declaration
// *****************************


// Private variables
// *****************


// Public API implementation.
// **************************

sys_error_code_t SPIBusIFInit(SPIBusIF *_this, uint8_t nWhoAmI, GPIO_TypeDef* pxSSPinPort, uint16_t nSSPin) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_nWhoAmI = nWhoAmI;
  _this->m_pxSSPinPort = pxSSPinPort;
  _this->m_nSSPin = nSSPin;

  // initialize the software resources
  if (tx_semaphore_create(&_this->m_xSyncObj, "SPI_SUS_S", 0) != TX_SUCCESS){
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  _this->m_xConnector.pfReadReg = SPIBusNullRW;
  _this->m_xConnector.pfWriteReg = SPIBusNullRW;
  _this->m_xConnector.pxHandle = NULL;

  return xRes;
}

sys_error_code_t SPIBusIFWaitIOComplete(SPIBusIF *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

//  if (_this->m_xSyncObj != NULL){ //TODO: STF.Port - how to check teh sem is initialized ??
    if (TX_SUCCESS != tx_semaphore_get(&_this->m_xSyncObj, TX_WAIT_FOREVER)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
      xRes = SYS_UNDEFINED_ERROR_CODE;
    }
//  }
//    else {
//      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
//      xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
//    }

  return xRes;
}

sys_error_code_t SPIBusIFNotifyIOComplete(SPIBusIF *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

//  if (_this->m_xSyncObj != NULL){ //TODO: STF.Port - how to check the sem is initialized?
    if (TX_SUCCESS != tx_semaphore_put(&_this->m_xSyncObj)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
      xRes = SYS_UNDEFINED_ERROR_CODE;
    }
//  }
//    else {
//      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
//      xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
//    }

  return xRes;
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
