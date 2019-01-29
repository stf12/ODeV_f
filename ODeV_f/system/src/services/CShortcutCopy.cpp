/**
 ******************************************************************************
 * @file    CShortcutCopy.cpp
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
#include <CShortcutCopy.h>

namespace odev {

CShortcutCopy::CShortcutCopy(): m_nKeys{KC_058, KC_048}
{
  // TODO Auto-generated constructor stub
}

CShortcutCopy::~CShortcutCopy() {
  // TODO Auto-generated destructor stub
}

uint8_t CShortcutCopy::operator [](uint8_t nIndex) const {
   return nIndex < 2 ? m_nKeys[nIndex] : KC_NUL;
}

} /* namespace odev */