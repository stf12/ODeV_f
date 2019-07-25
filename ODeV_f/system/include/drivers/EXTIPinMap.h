/**
 ******************************************************************************
 * @file    EXTIPinMap.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 8, 2016
 * @brief   External Interrupt callback declaration.
 *
 * This file declares a set of macro that the application use to define the
 * pin to callback map for the external interrupt.
 *
 * Each entry has two values that are:
 * - GPIO pin.
 * - Callback function.
 *
 * To access the map from an application source file:
 * - Include the EXTIPinMap.h
 * - use the EXTI_DECLARE_PIN2F_MAP() macro to declare the map.
 * - use the EXTI_GET_PIN2F_MAP() to get a pointer to the first element
 *   of the map.
 *
 * The application has the responsibility to define the map according to the
 * PIN used as external interrupt source.
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

#ifndef INCLUDE_DRIVERS_EXTIPINMAP_H_
#define INCLUDE_DRIVERS_EXTIPINMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "systp.h"

#define EXTI_DECLARE_PIN2F_MAP() \
		extern const EXTIMapEntry g_xExtiPin2FMap[];

#define EXTI_GET_P2F_MAP() g_xExtiPin2FMap

#define EXTI_BEGIN_P2F_MAP() \
  const EXTIMapEntry g_xExtiPin2FMap[] = {

#define EXTI_P2F_MAP_ENTRY(pin, callbackF) \
    { (pin), (callbackF) },

#define EXTI_END_P2F_MAP() \
    { 0, 0 }\
  };

typedef void ExtiCallbackF(uint16_t nPin);

struct _EXTIMapEntry {
	/**
	 * a GPIO_Pin. Valid value are GPIO_PINx with x in [0, 15].
	 */
	uint16_t nPin;

	/**
	 * Pointer to the callback called when the IRQ for the PIN nPin is triggered.
	 */
	ExtiCallbackF *pfCallback;
};

typedef struct _EXTIMapEntry EXTIMapEntry;

typedef const EXTIMapEntry* EXTIPin2CallbckMap;


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_DRIVERS_EXTIPINMAP_H_ */
