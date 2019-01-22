/**
  ******************************************************************************
  * @file    usbd_hid.h
  * @author  MCD Application Team
  * @brief   Header file for the usbd_hid_core.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_HID_H
#define __USB_HID_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"
#include "HID-USBHelper.h"
#include "hid_report_parser.h"

/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */

/** @defgroup USBD_HID
  * @brief This file is the Header file for usbd_hid.c
  * @{
  */


/** @defgroup USBD_HID_Exported_Defines
  * @{
  */
#define HID_MOUSE_EPIN_ADDR                  0x81U
#define HID_MOUSE_EPIN_SIZE                  0x04U

#define HID_KEYBOARD_EPIN_ADDR               0x82U
#define HID_KEYBOARD_EPIN_SIZE               0x08U

#define HID_CUSTOM_EPIN_ADDR                 0x83U
#define HID_CUSTOM_EPIN_SIZE                 0x08U

#define HID_CUSTOM_EPOUT_ADDR                0x03U
#define HID_CUSTOM_EPOUT_SIZE                0x08U

#define USB_HID_CONFIG_DESC_SIZ              91U
#define USB_HID_DESC_SIZ                     9U
#define HID_MOUSE_REPORT_DESC_SIZE           50U
#define HID_MOUSE_REPORT_DESC_SIZE_MBS       0U
#define HID_MOUSE_REPORT_DESC_SIZE_LBS       50U
#define HID_KEYBOARD_REPORT_DESC_SIZE        59U
#define HID_CUSTOM_REPORT_DESC_SIZE          85U
#define HID_MOUSE_IF_IDX                     0U
#define HID_KEYBOARD_IF_IDX                  1U
#define HID_CUSTOM_IF_IDX                    2U

#define HID_DESCRIPTOR_TYPE                  0x21U
#define HID_REPORT_DESC                      0x22U

#ifndef HID_HS_BINTERVAL
  #define HID_HS_BINTERVAL                   0x07U
#endif /* HID_HS_BINTERVAL */

#ifndef HID_FS_BINTERVAL
  #define HID_FS_BINTERVAL                   0x0AU
#endif /* HID_FS_BINTERVAL */

#define HID_REQ_SET_PROTOCOL                 0x0BU
#define HID_REQ_GET_PROTOCOL                 0x03U

#define HID_REQ_SET_IDLE                     0x0AU
#define HID_REQ_GET_IDLE                     0x02U

#define HID_REQ_SET_REPORT                   0x09U
#define HID_REQ_GET_REPORT                   0x01U

#define HID_MOUSE_INTERFACE                  0x00U
#define HID_KEYBOARD_INTERFACE               0x01U
#define HID_CUSTOM_INTERFACE                 0x02U
/**
  * @}
  */


/** @defgroup USBD_CORE_Exported_TypesDefinitions
  * @{
  */
typedef enum
{
  HID_IDLE = 0,
  HID_BUSY,
}
HID_StateTypeDef;

/**
 * A convenient way to operate the device report.
 */
typedef union _HIDReportBuffU {
  HIDReport xReport;
  uint8_t pcReportBuffer[sizeof(HIDReport)];
} HIDReportBuffU;

typedef struct
{
  HIDReportBuffU       Report;
  uint8_t              Interface;          ///< 1 when the output report is for the USB IF 1 (Keyboard)
  uint32_t             IsReportAvailable;
  uint32_t             Protocol;
  uint32_t             IdleState;
  uint32_t             AltSetting;
  HID_StateTypeDef     state[USBD_MAX_NUM_INTERFACES];
}
USBD_HID_HandleTypeDef;
/**
  * @}
  */



/** @defgroup USBD_CORE_Exported_Macros
  * @{
  */

/**
  * @}
  */

/** @defgroup USBD_CORE_Exported_Variables
  * @{
  */

extern const USBD_ClassTypeDef  USBD_HID;
#define USBD_HID_CLASS    &USBD_HID
/**
  * @}
  */

/** @defgroup USB_CORE_Exported_Functions
  * @{
  */
uint8_t USBD_HID_MouseSendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);

uint8_t USBD_HID_KeyboardSendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);

uint8_t USBD_HID_CustomSendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len);

uint32_t USBD_HID_GetPollingInterval (USBD_HandleTypeDef *pdev);

#if ((USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0 == 1) || (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0 == 1))
USBD_StatusTypeDef USBD_VendorRequest(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req);
#endif

#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0 == 1)
extern const MS_OS_STR_DESC_V2_0 MsOsStrDescriptor_V2_0;
#endif


/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif  /* __USB_HID_H */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
