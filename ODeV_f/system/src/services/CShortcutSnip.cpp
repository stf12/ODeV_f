/**
 ******************************************************************************
 * @file    CShortcutSnip.cpp
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
#include <CShortcutSnip.h>

namespace odev {

const uint8_t CShortcutSnip::s_nKeys[] = { KC_127, KC_044, KC_032 };

CShortcutSnip::CShortcutSnip() {
  // TODO Auto-generated constructor stub

}

CShortcutSnip::~CShortcutSnip() {
  // TODO Auto-generated destructor stub
}

uint8_t CShortcutSnip::operator [](uint8_t nIndex) const {
   return nIndex < 3 ? s_nKeys[nIndex] : KC_NUL;
}

} /* namespace odev */
