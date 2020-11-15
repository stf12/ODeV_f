/**
 ******************************************************************************
 * @file    I2CBusIF.c
 * @author  Stefano Oliveri
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
 * <h2><center>&copy; COPYRIGHT 2020 Stefano Oliveri</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#include "I2CBusIF.h"

// Private functions declaration
// *****************************


// Private variables
// *****************


// Public API implementation.
// **************************

sys_error_code_t I2CBusIFInit(I2CBusIF *_this, uint8_t nWhoAmI, uint8_t nAddress) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_nWhoAmI = nWhoAmI;
  _this->m_nAddress = nAddress;

  // initialize the software resources
  if (TX_SUCCESS != tx_semaphore_create(&_this->m_xSyncObj, "I2C_IF_S", 0)){
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
  }

  _this->m_xConnector.pfReadReg = I2CBusNullRW;
  _this->m_xConnector.pfWriteReg = I2CBusNullRW;
  _this->m_xConnector.pxHandle = NULL;

  return xRes;
}

sys_error_code_t I2CBusIFWaitIOComplete(I2CBusIF *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

//  if (_this->m_xSyncObj != NULL){ //TODO: STF.Port - how to know if a sem is ready?
    if (TX_SUCCESS != tx_semaphore_get(&_this->m_xSyncObj, TX_WAIT_FOREVER)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
      xRes = SYS_UNDEFINED_ERROR_CODE;
//    }
  }
//  else {
//    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
//    xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
//  }

  return xRes;
}

sys_error_code_t I2CBusIFNotifyIOComplete(I2CBusIF *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

//  if (_this->m_xSyncObj != NULL){ //TODO: STF.Port - how to know if a sem is ready?
    if (TX_SUCCESS != tx_semaphore_put(&_this->m_xSyncObj)) {
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
      xRes = SYS_UNDEFINED_ERROR_CODE;
    }
//  }
//  else {
//    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_FUNC_CALL_ERROR_CODE);
//    xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
//  }

  return xRes;
}


// Private functions definition
// ****************************

int32_t I2CBusNullRW(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize) {
  UNUSED(pxSensor);
  UNUSED(nRegAddr);
  UNUSED(pnData);
  UNUSED(nSize);

  return 0;
}
