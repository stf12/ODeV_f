/**
 ******************************************************************************
 * @file    HID-USBHelper.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
 *
 * @brief   Helper object to integrate the ST-USB stack into the system.
 *
 * This helper object provides a generic way to broadcast the HID related
 * USB messages to the system. It uses the _event source_ <-> _event listener_
 * design pattern, and it implements the IEventSrc interface.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2019 STMicroelectronics</center></h2>
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
#ifndef INCLUDE_SERVICES_HID_USBHELPER_H_
#define INCLUDE_SERVICES_HID_USBHELPER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx_hal.h"
#include "IEventSrc.h"
#include "IEventSrcVtbl.h"
#include "IUsbEventListener.h"
#include "IUsbEventListenerVtbl.h"
#include "UsbEvent.h"

#ifndef HIDUSBH_CONGIG_MAX_LISTENERS
/**
 * Specifies the maximum number of listeners that is possible to register with this event source.
 */
#define HIDUSBH_CONFIG_MAX_LISTENERS    4
#endif

/**
 * Create  type name for _HIDUSBHelper.
 */
typedef struct _HIDUSBHelper HIDUSBHelper;

/**
 * This helper object implements the IEventSrc interface.
 */
struct _HIDUSBHelper {
	/**
	 * Base interface.
	 */
	IEventSrc super;

	/**
	 * Set of IEventListener object.
	 */
	IEventListener *m_pxListeners[HIDUSBH_CONFIG_MAX_LISTENERS];
};


// Public API declaration
//***********************

/**
 * Instantiate a class object. Before using the object it has to be initialized by calling IEventSrcInit function.
 *
 * @return a pointer to a class object.
 */
IEventSrc *HIDUSBHelperAlloc();


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_HID_USBHELPER_H_ */
