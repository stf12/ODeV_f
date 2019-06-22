/**
 ******************************************************************************
 * @file    CShortcutsSet.h
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
#ifndef INCLUDE_SERVICES_CSHORTCUTSSET_H_
#define INCLUDE_SERVICES_CSHORTCUTSSET_H_

#include <IShortcut.h>
#include "syserror.h"

#ifndef SC_CFG_SET_SIZE
#define SC_CFG_SET_SIZE    100 ///< Specifies the maximum number of shortcuts managed by the Shortcuts Set
#endif

namespace odev {

class CShortcutsSet {
public:
  CShortcutsSet();
  virtual ~CShortcutsSet();

  sys_error_code_t RegisterShortcut(IShortcut *pxShortcut, uint16_t &nShortcutID);

  IShortcut *GetShortcut(uint16_t nShortcutID) const;

private:
  IShortcut *m_pSet[SC_CFG_SET_SIZE];
};

} /* namespace odev */

#endif /* INCLUDE_SERVICES_CSHORTCUTSSET_H_ */
