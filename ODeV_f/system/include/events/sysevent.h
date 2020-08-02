/**
 ******************************************************************************
 * @file    sysevent.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Aug 7, 2017
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
#ifndef SYSEVENT_H_
#define SYSEVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systypes.h"


/**
 * Specifies the structure of a system event that is possible to send to the Init task.
 */
typedef union _SysEvent {
	/**
	 * Specifies the semantics of the bit field of a system event.
	 */
	struct Event {
		/**
		 * Specifies the source of the event.
		 */
		uint32_t nSource: 3;

		/**
		 *  Specifies the event parameter.
		 */
		uint32_t nParam: 5;

		/**
		 * reserved. It must be zero.
		 */
		uint32_t reserved: 23;

		/**
		 * Specifies the type of the system event. For an error event it must be set to 1.
		 */
		uint32_t nEventType: 1;

	}xEvent;

	/**
	 * Convenient field to operate the data type.
	 */
	uint32_t nRawEvent;
} SysEvent;

// Public API declaration
//***********************


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SYSEVENT_H_ */
