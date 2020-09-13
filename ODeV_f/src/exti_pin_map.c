/**
 ******************************************************************************
 * @file    exti_pin_map.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 8, 2016
 * @brief   Application level file. It defines the PIN to callback
 *          mapping function for the external interrupt.
 *
 * TODO - insert here the file description
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

#include "EXTIPinMap.h"

// Forward function declaration.
void Def_EXTI_Callback(uint16_t nPin) {};
void ISM330DHCXTask_EXTI_Callback(uint16_t nPin);
void IIS3DWBTask_EXTI_Callback(uint16_t nPin);
void HTS221Task_EXTI_Callback(uint16_t nPin);
void Util_PB_EXTI_Callback(uint16_t nPin);


EXTI_BEGIN_P2F_MAP()
  EXTI_P2F_MAP_ENTRY(GPIO_PIN_0, Util_PB_EXTI_Callback)
//  EXTI_P2F_MAP_ENTRY(GPIO_PIN_6, HTS221Task_EXTI_Callback)
  EXTI_P2F_MAP_ENTRY(GPIO_PIN_8, ISM330DHCXTask_EXTI_Callback)
  EXTI_P2F_MAP_ENTRY(GPIO_PIN_14, IIS3DWBTask_EXTI_Callback)

EXTI_END_P2F_MAP()
