/**
 ******************************************************************************
 * @file    IUsbEventListener.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 13, 2017
 *
 * @brief   USB Event Listener Interafce.
 *
 * This interface extends the IEventListener interface and it allows an
 * object to receive events from the USB. An application object that implement
 * this interface can register itself using the method
 * HidUsbDeviceAddEventListener().
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
#ifndef INCLUDE_EVENTS_IUSBEVENTLISTENER_H_
#define INCLUDE_EVENTS_IUSBEVENTLISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IEventListener.h"
#include "IEventListenerVtbl.h"
#include "UsbEvent.h"

/**
 * Create  type name for _IUsbEventListener
 */
typedef struct _IUsbEventListener IUsbEventListener;

// Public API declaration
//***********************

/**
 * Called when the USB subsystem receive a GET_REPORT request.
 * Note the _this method is called from an ISR so it should execute fast.
 *
 * @param _this [IN] specifies a pointer to an IUsbEventListener object.
 * @param pxEvent [IN] specifies a pointer to an UsbEvent.
 * @param pParams specifies a generic pointer that can be used by the application.
 * @return not used.
 */
inline sys_error_code_t IUsbEventListenerOnGetReport(IEventListener *_this, const UsbEvent *pxEvent, void *pParams);

/**
 * Called when the USB subsystem receive a SET_REPORT request.
 * Note the this method is called from an ISR so it should execute fast.
 *
 * @param _this [IN] specifies a pointer to an IUsbEventListener object.
 * @param pxEvent [IN] specifies a pointer to an UsbEvent.
 * @param pParams specifies a generic pointer that can be used by the application.
 * @return not used
 */
inline sys_error_code_t IUsbEventListenerOnSetReport(IEventListener *_this, const UsbEvent *pxEvent, void *pParams);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_IUSBEVENTLISTENER_H_ */
