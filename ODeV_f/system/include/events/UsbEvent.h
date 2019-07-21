/**
 ******************************************************************************
 * @file    UsbEvent.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Apr 17, 2017
 *
 * @brief   USB Event.
 *
 * This class extends the IEvent interface. It represent an event coming from
 * the USB subsystem.
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
#ifndef INCLUDE_EVENTS_USBEVENT_H_
#define INCLUDE_EVENTS_USBEVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IEvent.h"
#include "stddef.h"

/**
 * Specifies the maximum size (in byte) of a feature or output report coming from from the HOST via USB.
 */
#define USB_EVENT_CFG_MAX_REPORT_SIZE  0x02

/**
 * Enumeration of the the USB event type.
 */
typedef enum _EUsbEventType {
	E_USB_GET_REPORT,//!< E_USB_GET_REPORT
	E_USB_SET_REPORT //!< E_USB_SET_REPORT
} EUsbEventType;

/**
 * USB Event.
 */
typedef struct _UsbEvent {
	/**
	 * Base class.
	 */
	IEvent super;

	/**
	 * Specifies the type of the USB event.
	 */
	EUsbEventType eType;

	/**
	 * Specifies the ID of the report associated with the event.
	 * @sa hid_i2c_sk_report_parser.h
	 */
	uint8_t nReportID;

	/**
	 * Specifies the report data associated with the event.
	 */
	uint8_t pcReportBuff[USB_EVENT_CFG_MAX_REPORT_SIZE];
}UsbEvent;



// Public API declaration
//***********************

/**
 * Initialize an USB event.
 *
 * @param _this [IN] specifies a pointer to an UebEvent.
 * @param pSource [IN] specifies a pointer to the IEventSrc object that has generated this event.
 * @param eType [IN} specifies the type of this UsbEvent.
 * @param nReportID specifies the report ID for this UsbEvent.
 * @param pcReportBuff specifies the report data for this UsbEvent.
 * @return SYS_NO_ERROR_CODE
 */
inline sys_error_code_t UsbEventInit(UsbEvent *_this, const IEventSrc *pSource, EUsbEventType eType, uint8_t nReportID, uint8_t *pcReportBuff);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t UsbEventInit(UsbEvent *_this, const IEventSrc *pSource, EUsbEventType eType, uint8_t nReportID, uint8_t *pcReportBuff) {
	// parameters validation.
	if ((_this == NULL) || (pSource == NULL)) {
		SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_INVALID_PARAMETER_ERROR_CODE);
		return SYS_INVALID_PARAMETER_ERROR_CODE;
	}

	_this->super.pSource = pSource;
	_this->eType = eType;
	_this->nReportID = nReportID;
	if (pcReportBuff != NULL) {
		for (int i=0; i<USB_EVENT_CFG_MAX_REPORT_SIZE; ++i) {
			_this->pcReportBuff[i] = pcReportBuff[i];
		}
	}

	return SYS_NO_ERROR_CODE;
}

#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_EVENTS_USBEVENT_H_ */
