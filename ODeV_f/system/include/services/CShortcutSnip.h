/**
 ******************************************************************************
 * @file    CShortcutSnip.h
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
#ifndef INCLUDE_SERVICES_CSHORTCUTSNIP_H_
#define INCLUDE_SERVICES_CSHORTCUTSNIP_H_

#include <AShortcut.h>

namespace odev {

class CShortcutSnip: public AShortcut {
public:
  CShortcutSnip();
  virtual ~CShortcutSnip();

  virtual const uint8_t GetKeyCount() const { return 3; }

  virtual uint8_t operator[](uint8_t nIndex) const;

private:
  static const uint8_t s_nKeys[3];
};

} /* namespace odev */

#endif /* INCLUDE_SERVICES_CSHORTCUTSNIP_H_ */
