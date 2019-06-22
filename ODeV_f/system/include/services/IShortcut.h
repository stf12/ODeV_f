/**
 ******************************************************************************
 * @file    AShortcut.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Jan 28, 2019
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
#ifndef INCLUDE_SERVICES_ISHORTCUT_H_
#define INCLUDE_SERVICES_ISHORTCUT_H_

#include "systp.h"
#include "KeyboardKeyCodesMap.h"

namespace odev {

/**
 * @class AShortcut
 *
 * A shortcut is ...
 */
class IShortcut {
public:

  /**
   * Get the number of keys represented by the shortcuts.
   *
   * @return the number of keys represented by the shortcuts.
   */
  virtual const uint8_t GetKeyCount() const =0;

  /**
   *
   *
   * @param nIndex
   * @return
   */
  virtual uint8_t operator[](uint8_t nIndex) const =0;

  virtual bool operator==(const IShortcut &xOther) =0;
};

} /* namespace odev */

#endif /* INCLUDE_SERVICES_ISHORTCUT_H_ */
