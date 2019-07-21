/**
 ******************************************************************************
 * @file    HID-USBHelper.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
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

#include "HID-USBHelper.h"
#include "HID-USBHelper_vtbl.h"
#include "FreeRTOS.h"
#include "sysdebug.h"

#define SYS_DEBUGF(level, message) 			SYS_DEBUGF3(SYS_DBG_HID_USB_DEVICE_SERVICES, level, message)

static const IEventSrc_vtbl s_xHIDUSBHelper_vtbl = {
		HIDUSBHelper_vtblInit,
		HIDUSBHelper_vtblAddEventListener,
		HIDUSBHelper_vtblRemoveEventListener,
		HIDUSBHelper_vtblGetMaxListenerCount,
		HIDUSBHelper_vtblSendEvent
};


// Private member function declaration
// ***********************************


// Public API definition
// *********************

IEventSrc *HIDUSBHelperAlloc() {
	IEventSrc *pNewObj = (IEventSrc*)pvPortMalloc(sizeof(HIDUSBHelper));

	if (pNewObj == NULL) {
		SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
		SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("HIDUSBHelper - alloc failed.\r\n"));
	}

	pNewObj->vptr = &s_xHIDUSBHelper_vtbl;

	return pNewObj;
}

sys_error_code_t HIDUSBHelper_vtblInit(IEventSrc *this) {
	assert_param(this);
	sys_error_code_t xRes = SYS_NO_ERROR_CODE;
	HIDUSBHelper *pObj = (HIDUSBHelper*)this;

	for(int i=0; i<HIDUSBH_CONFIG_MAX_LISTENERS; ++i) {
		pObj->m_pxListeners[i] = NULL;
	}

	return xRes;
}

sys_error_code_t HIDUSBHelper_vtblAddEventListener(IEventSrc *this, IEventListener *pListener) {
	assert_param(this);
	assert_param(pListener);
	sys_error_code_t xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
	HIDUSBHelper *pObj = (HIDUSBHelper*)this;

	for(int i=0; i<HIDUSBH_CONFIG_MAX_LISTENERS; ++i) {
		if (pObj->m_pxListeners[i] == NULL) {
			pObj->m_pxListeners[i] = pListener;
			xRes = SYS_NO_ERROR_CODE;
			break;
		}
	}

	if (SYS_IS_ERROR_CODE(xRes)) {
		SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
	}

	return xRes;
}

sys_error_code_t HIDUSBHelper_vtblRemoveEventListener(IEventSrc *this, IEventListener *pListener) {
	assert_param(this);
	assert_param(pListener);
	HIDUSBHelper *pObj = (HIDUSBHelper*)this;

	for(int i=0; i<HIDUSBH_CONFIG_MAX_LISTENERS; ++i) {
		if (pObj->m_pxListeners[i] == pListener) {
			pObj->m_pxListeners[i] = NULL;
			break;
		}
	}

	return SYS_NO_ERROR_CODE;
}

uint32_t HIDUSBHelper_vtblGetMaxListenerCount(const IEventSrc *this) {
	UNUSED(this);

	return HIDUSBH_CONFIG_MAX_LISTENERS;
}

sys_error_code_t HIDUSBHelper_vtblSendEvent(const IEventSrc *this, const IEvent *pxEvent, void *pvParams) {
	assert_param(this);
	HIDUSBHelper *pObj = (HIDUSBHelper*)this;
	UsbEvent *pxUsbEvent = (UsbEvent*)pxEvent;

	UNUSED(pvParams);

	IEventListener *pxListener = NULL;
	for (int i=0; i<HIDUSBH_CONFIG_MAX_LISTENERS; ++i) {
		pxListener = pObj->m_pxListeners[i];
		if (pxListener == NULL) {
			break;
		}
		//TODO: STF - for the moment don't take into account the return value.
		switch (pxUsbEvent->eType) {
		case E_USB_GET_REPORT:
			IUsbEventListenerOnGetReport(pxListener, pxUsbEvent, pvParams);
			break;
		case E_USB_SET_REPORT:
			IUsbEventListenerOnSetReport(pxListener, pxUsbEvent, pvParams);
		}

	}

	return SYS_NO_ERROR_CODE;
}

// Private function definition
// ***************************
