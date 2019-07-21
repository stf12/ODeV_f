/**
 ******************************************************************************
 * @file    HID-USBHelper_vtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 6, 2017
 *
 * @brief  IEventSrv virtual table definition for the HIDUSBHelper.
 *
 * This file declares the virtual functions overridden by the HIDUSBHelper
 * class.
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
#ifndef INCLUDE_SERVICES_HID_USBHELPER_VTBL_H_
#define INCLUDE_SERVICES_HID_USBHELPER_VTBL_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @sa IEventSrcInit
 */
sys_error_code_t HIDUSBHelper_vtblInit(IEventSrc *this);

/**
 * @sa IEventSrcAddEventListener
 */
sys_error_code_t HIDUSBHelper_vtblAddEventListener(IEventSrc *this, IEventListener *pListener);

/**
 * @sa IEventSrcRemoveEventListener
 */
sys_error_code_t HIDUSBHelper_vtblRemoveEventListener(IEventSrc *this, IEventListener *pListener);

/**
 * @sa IEventSrcGetMaxListenerCount
 */
uint32_t HIDUSBHelper_vtblGetMaxListenerCount(const IEventSrc *this);

/**
 * @sa IEventSrcSendEvent
 */
sys_error_code_t HIDUSBHelper_vtblSendEvent(const IEventSrc *this, const IEvent *pxEvent, void *pvParams);


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_SERVICES_HID_USBHELPER_VTBL_H_ */
