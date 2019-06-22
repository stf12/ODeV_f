/**
 ******************************************************************************
 * @file    CShortchut.cpp
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Feb 20, 2019
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
#include <CShortchut.h>
#include "FreeRTOS.h"

namespace odev {

CShortchut::CShortchut() {
  m_nKeysNumber = 0;
  m_pnKeyCode = NULL;
}

CShortchut::CShortchut(uint8_t nKeysNumber, uint8_t *pnKeyKode) {
  m_nKeysNumber = 0;
  m_pnKeyCode = NULL;

  Init(nKeysNumber, pnKeyKode);
}

CShortchut::~CShortchut() {
  // TODO Auto-generated destructor stub
}

sys_error_code_t CShortchut::Init(uint8_t nKeysNumber, uint8_t *pnKeyCode) {
  assert_param(pnKeyCode);
  assert_param(nKeysNumber);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (m_nKeysNumber != 0) {
    // the shortcut has been already initialized. You cannot initialize it twice, because
    // the memory from the FreeRTOS heap (heap_1.c) cannot be released.
    xRes = SYS_INVALID_FUNC_CALL_ERROR_CODE;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
  }
  else {
    m_pnKeyCode = (uint8_t*)pvPortMalloc(nKeysNumber);
    if (m_pnKeyCode == NULL) {
      xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
      SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
    }
    else {
      m_nKeysNumber = nKeysNumber;
      for (int i=0; i<nKeysNumber; ++i) {
        m_pnKeyCode[i] = pnKeyCode[i];
      }
    }
  }

  return xRes;
}

} /* namespace odev */


