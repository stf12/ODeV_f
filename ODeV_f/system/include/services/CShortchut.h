/**
 ******************************************************************************
 * @file    CShortchut.h
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
#ifndef INCLUDE_SERVICES_CSHORTCHUT_H_
#define INCLUDE_SERVICES_CSHORTCHUT_H_

#include <AShortcut.h>
#include "syserror.h"

namespace odev {

/**
 * Generic shortcut. The number of keys and the sequence is passed to the constructor.
 * The memory is allocated from the FreeRTOS heap.
 *
 * For example this is equivalent to odev::CShortcutCopy "CRTL + C":
 *
 *     uint8_t nBuff[] = { KC_044, KC_048 };
 *     CShortcut copy(2, nBuff);
 *
 *  A CShortcut object can be instantiated without any initialization using the default constructor,
 *  but the application has to call the CShortchut::Init() before using the object.
 */
class CShortchut: public AShortcut {
public:
  /**
   * Default constructor. It create an empity shortcut that must be initialized using the CShortchut::Init() method.
   */
  CShortchut();

  /**
   * Create and initialize shortcut with nKeysNumber keys in the given sequence specified by pnKeyKode.
   *
   * @param nKeysNumber [IN] specifies the number of keys of the shortcut.
   * @param pnKeyCode [IN] specifies the array of key code. pnKeyCode[0] is the first key of the shortcut sequence.
   */
  CShortchut(uint8_t nKeysNumber, uint8_t *pnKeyCode);

  /**
   * Default destructor. It does nothing.
   */
  virtual ~CShortchut();

  /**
   * Initialize the shortcut. It allocates the memory from the FreeRTOS heap.
   *
   * @param nKeysNumber [IN] specifies the number of keys of the shortcut.
   * @param pnKeyCode [IN] specifies the array of key code. pnKeyCode[0] is the first key of the shortcut sequence.
   * @return
   */
  sys_error_code_t Init(uint8_t nKeysNumber, uint8_t *pnKeyCode);
private:
  /**
   * number of keys of the shortcut.
   */
  uint8_t m_nKeysNumber;

  /**
   * Array of key code. pnKeyCode[0] is the first key of the shortcut sequence.
   */
  uint8_t *m_pnKeyCode;
};

} /* namespace odev */

#endif /* INCLUDE_SERVICES_CSHORTCHUT_H_ */
