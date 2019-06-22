/**
 ******************************************************************************
 * @file    CShortcutsSet.cpp
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Feb 19, 2019
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
#include <CShortcutsSet.h>
#include "systp.h"

namespace odev {

CShortcutsSet::CShortcutsSet() {
  for (int i=0; i<SC_CFG_SET_SIZE; ++i) {
    m_pSet[i] = NULL;
  }
}

CShortcutsSet::~CShortcutsSet() {
  // TODO Auto-generated destructor stub
}

sys_error_code_t CShortcutsSet::RegisterShortcut(IShortcut *pxShortcut, uint16_t &nShortcutID) {
  assert_param(pxShortcut);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  // check if the shortcuts is already registered in this set
  for (nShortcutID=0; nShortcutID<SC_CFG_SET_SIZE; ++nShortcutID) {
    if ((m_pSet[nShortcutID] == NULL) || (*m_pSet[nShortcutID] == *pxShortcut)) {
      break;
    }
  }

  if (nShortcutID < SC_CFG_SET_SIZE) {
    if (m_pSet[nShortcutID] == NULL) {
      // register the shortcut in the free position
      m_pSet[nShortcutID] = pxShortcut;
    }
    else {
      // the shortcut is already registered. Notify the caller using the return code.
      xRes = SYS_SC_ALREADY_REGISTERED_ERROR_CODE;
    }
  }
  else {
    xRes = SYS_SC_SET_FULL;
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
  }

  return xRes;
}

IShortcut *CShortcutsSet::GetShortcut(uint16_t nShortcutID) const {
  if (nShortcutID >= SC_CFG_SET_SIZE) {
    // invalid parameter.
    return NULL;
  }

  return m_pSet[nShortcutID];
}

} /* namespace odev */
