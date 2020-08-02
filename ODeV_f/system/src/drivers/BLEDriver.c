/**
 ******************************************************************************
 * @file    BLEDriver.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Jan 13, 2020
 *
 * @brief   
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
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

#include "BLEDriver.h"
#include "BLEDriver_vtbl.h"
#include "FreeRTOS.h"
#include "sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
 * BLEDriver Driver virtual table.
 */
static const IDriver_vtbl s_xBLEDriver_vtbl = {
    BLEDriver_vtblInit,
    BLEDriver_vtblStart,
    BLEDriver_vtblStop,
    BLEDriver_vtblDoEnterPowerMode,
//    BLEDriver_vtblWrite,
//    BLEDriver_vtblRead
};


// Private member function declaration
// ***********************************


// Public API definition
// *********************

IDriver *BLEDriverAlloc() {
  IDriver *pNewObj = (IDriver*)pvPortMalloc(sizeof(BLEDriver));

  if (pNewObj == NULL) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("BLEDriver - alloc failed.\r\n"));
  }
  else {
    pNewObj->vptr = &s_xBLEDriver_vtbl;
  }

  return pNewObj;
}

sys_error_code_t BLEDriver_vtblInit(IDriver *_this, void *pParams) {
  assert_param(_this);
  assert_param(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  BLEDriver *pObj = (BLEDriver*)_this;

  return xRes;
}

sys_error_code_t BLEDriver_vtblStart(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  BLEDriver *pObj = (BLEDriver*)_this;

  return xRes;
}

sys_error_code_t BLEDriver_vtblStop(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  BLEDriver *pObj = (BLEDriver*)_this;

  return xRes;
}

sys_error_code_t BLEDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  BLEDriver *pObj = (BLEDriver*)_this;

  return xRes;
}

sys_error_code_t BLEDriver_vtblWrite(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  BLEDriver *pObj = (BLEDriver*)_this;

  return xRes;
}

sys_error_code_t BLEDriver_vtblRead(IDriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  BLEDriver *pObj = (BLEDriver*)_this;

  return xRes;
}


// Private function definition
// ***************************
