/**
  ******************************************************************************
  * @file    usbd_hid.c
  * @author  MCD Application Team
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                HID Class  Description
  *          ===================================================================
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
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

  /* BSPDependencies
  - "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
  - "stm32xxxxx_{eval}{discovery}_io.c"
  EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "usbd_hid.h"
#include "usbd_ctlreq.h"
#include "sysdebug.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_HID_USB_DEVICE_SERVICES, level, message)



/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_HID_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_HID_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_HID_Private_Macros
  * @{
  */
/**
  * @}
  */




/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */


static uint8_t  USBD_HID_Init (USBD_HandleTypeDef *pdev,
                               uint8_t cfgidx);

static uint8_t  USBD_HID_DeInit (USBD_HandleTypeDef *pdev,
                                 uint8_t cfgidx);

static uint8_t  USBD_HID_Setup (USBD_HandleTypeDef *pdev,
                                USBD_SetupReqTypedef *req);

static uint8_t  *USBD_HID_GetFSCfgDesc (uint16_t *length);

static uint8_t  *USBD_HID_GetHSCfgDesc (uint16_t *length);

static uint8_t  *USBD_HID_GetOtherSpeedCfgDesc (uint16_t *length);

static uint8_t  *USBD_HID_GetDeviceQualifierDesc (uint16_t *length);

static uint8_t  USBD_HID_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t USBD_HID_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  USBD_HID_MouseIF_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  USBD_HID_KeyboardIF_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t  USBD_HID_CustomIF_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);

#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0 == 1)
static uint8_t* USBD_HID_OsStrDescriptor_V1_0(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length);
static uint8_t* USBD_HID_ExtCompatIdDescriptor(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length);
static uint8_t* USBD_HID_ExtPropDescriptor(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length);
#endif

#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0 == 1)
static uint8_t* USBD_HID_OsStrDescriptor_V2_0(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length);
#endif

/**
  * @}
  */

/** @defgroup USBD_HID_Private_Variables
  * @{
  */

static USBD_HID_HandleTypeDef s_xHidClassData;

const USBD_ClassTypeDef  USBD_HID =
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/
  USBD_HID_EP0_RxReady, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  NULL, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_HID_GetHSCfgDesc,
  USBD_HID_GetFSCfgDesc,
  USBD_HID_GetOtherSpeedCfgDesc,
  USBD_HID_GetDeviceQualifierDesc,
#if (USBD_SUPPORT_USER_STRING == 1)
  NULL,
#endif
#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0 == 1)
  USBD_HID_OsStrDescriptor_V1_0,
  USBD_HID_ExtCompatIdDescriptor,
  USBD_HID_ExtPropDescriptor,
#endif
#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0 == 1)
  USBD_HID_OsStrDescriptor_V2_0,
#endif
};

/* USB HID device FS Configuration Descriptor */
__ALIGN_BEGIN static const uint8_t USBD_HID_CfgFSDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  0x09,                           /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,    /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,        /* wTotalLength (LBS): Bytes returned */
  0x00,                           /* wTotalLength (MBS): Bytes returned */
  0x03,                           /* bNumInterfaces: 3 interface*/
  0x01,                           /* bConfigurationValue: Configuration value*/
  0x00,                           /* iConfiguration: Index of string descriptor describing the configuration*/
  0xA0,                           /* bmAttributes: bus powered (D6) and Support Remote Wake-up (D5) */
  0x64,                           /* MaxPower 200 mA: this current is used for detecting Vbus*/
  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,                           /* bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,        /* bDescriptorType: Interface descriptor type*/
  HID_MOUSE_INTERFACE,            /* bInterfaceNumber: 0 Number of Interface*/
  0x00,                           /* bAlternateSetting: Alternate setting*/
  0x01,                           /* bNumEndpoints*/
  0x03,                           /* bInterfaceClass: HID*/
  0x00,                           /* bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x02,                           /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0x00,                           /* iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,                           /* bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE,            /* bDescriptorType: HID*/
  0x11,                           /* bcdHID (LBS): HID Class Spec release number*/
  0x01,                           /* bcdHID (MBS): HID Class Spec release number*/
  0x00,                           /* bCountryCode: Hardware target country*/
  0x01,                           /* bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,                           /* bDescriptorType: REPORT*/
  HID_MOUSE_REPORT_DESC_SIZE_LBS,
  HID_MOUSE_REPORT_DESC_SIZE_MBS,
  /******************** Descriptor of Mouse endpoint (IN)****************/
  /* 27 */
  0x07,                           /* bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT,         /* bDescriptorType:*/
  HID_MOUSE_EPIN_ADDR,            /* bEndpointAddress: Endpoint Address (IN)*/
  0x03,                           /* bmAttributes: Interrupt endpoint*/
  HID_MOUSE_EPIN_SIZE,            /* wMaxPacketSize (LBS): 4 Byte max */
  0x00,                           /* wMaxPacketSize (MBS) */
  HID_FS_BINTERVAL,               /* bInterval: Polling Interval (1 ms)*/
  /************** Descriptor of Keyboard interface ****************/
  /* 34 */
  0x09,                           //   bLength
  USB_DESC_TYPE_INTERFACE,        //   bDescriptorType (Interface)
  HID_KEYBOARD_INTERFACE,         //   bInterfaceNumber 1
  0x00,                           //   bAlternateSetting
  0x01,                           //   bNumEndpoints 1
  0x03,                           //   bInterfaceClass
  0x01,                           //   bInterfaceSubClass (0): 1=BOOT, 0=no boot
  0x01,                           //   bInterfaceProtocol: 0=none, 1=keyboard, 2=mouse
  0x00,                           //   iInterface (String Index)
  /******************** Descriptor of Keyboard HID ********************/
  // 43
  0x09,                           //   bLength
  HID_DESCRIPTOR_TYPE,            //   bDescriptorType (HID)
  0x11, 0x01,                     //   bcdHID 1.11(1.12)
  0x00,                           //   bCountryCode
  0x01,                           //   bNumDescriptors
  0x22,                           //   bDescriptorType[0] (HID)
  HID_KEYBOARD_REPORT_DESC_SIZE,  //   wItemLength (LBS): 59 Total length of Report descriptor
  0x00,                           //   wItemLength (MBS)
  /******************** Descriptor of Keyboard endpoint (IN) ********************/
  // 52
  0x07,                           //   bLength
  USB_DESC_TYPE_ENDPOINT,         //   bDescriptorType (Endpoint)
  HID_KEYBOARD_EPIN_ADDR,         //   bEndpointAddress (IN/D2H)
  0x03,                           //   bmAttributes (Interrupt)
  HID_KEYBOARD_EPIN_SIZE,         //   wMaxPacketSize (LBS): 4 Byte max
  0x00,                           //   wMaxPacketSize (MBS)
  0x02,                            //   bInterval (2) 10 (unit depends on device speed)
  /************** Descriptor of Custom interface ****************/
  // 59
  0x09,                           //   bLength
  0x04,                           //   bDescriptorType (Interface)
  HID_CUSTOM_INTERFACE,           //   bInterfaceNumber 2
  0x00,                           //   bAlternateSetting
  0x02,                           //   bNumEndpoints 2
  0x03,                           //   bInterfaceClass
  0x00,                           //   bInterfaceSubClass
  0x00,                           //   bInterfaceProtocol
  0x00,                           //   iInterface (String Index)
  /******************** Descriptor of Custom HID ********************/
  // 68
  0x09,                           //   bLength
  0x21,                           //   bDescriptorType (HID)
  0x11, 0x01,                     //   bcdHID 1.12
  0x00,                           //   bCountryCode
  0x01,                           //   bNumDescriptors
  0x22,                           //   bDescriptorType[0] (HID)
  HID_CUSTOM_REPORT_DESC_SIZE,    //   wItemLength (LBS): 85 Total length of Report descriptor
  0x00,                           //   wItemLength (MBS)
  /******************** Descriptor of Custom endpoint (IN) ********************/
  // 77
  0x07,                           //   bLength
  USB_DESC_TYPE_ENDPOINT,         //   bDescriptorType (Endpoint)
  HID_CUSTOM_EPIN_ADDR,           //   bEndpointAddress (OUT/H2D)
  0x03,                           //   bmAttributes (Interrupt)
  HID_CUSTOM_EPIN_SIZE,           //   wMaxPacketSize (LBS): 4 Byte max
  0x00,                           //   wMaxPacketSize (MBS)
  0x04,                           //   bInterval 4 (unit depends on device speed)
  /******************** Descriptor of Custom endpoint (OUT) ********************/
  // 84
  0x07,                           //   bLength
  USB_DESC_TYPE_ENDPOINT,         //   bDescriptorType (Endpoint)
  HID_CUSTOM_EPOUT_ADDR,          //   bEndpointAddress (OUT/H2D)
  0x03,                           //   bmAttributes (Interrupt)
  HID_CUSTOM_EPOUT_SIZE,          //   wMaxPacketSize (LBS): 4 Byte max
  0x00,                           //   wMaxPacketSize (MBS)
  0x04                            //   bInterval 4 (unit depends on device speed)
  // 91
};

///* USB HID device HS Configuration Descriptor */
//__ALIGN_BEGIN static uint8_t USBD_HID_CfgHSDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
//{
//  0x09, /* bLength: Configuration Descriptor size */
//  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
//  USB_HID_CONFIG_DESC_SIZ,
//  /* wTotalLength: Bytes returned */
//  0x00,
//  0x01,         /*bNumInterfaces: 1 interface*/
//  0x01,         /*bConfigurationValue: Configuration value*/
//  0x00,         /*iConfiguration: Index of string descriptor describing
//  the configuration*/
//  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
//  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
//
//  /************** Descriptor of Joystick Mouse interface ****************/
//  /* 09 */
//  0x09,         /*bLength: Interface Descriptor size*/
//  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
//  0x00,         /*bInterfaceNumber: Number of Interface*/
//  0x00,         /*bAlternateSetting: Alternate setting*/
//  0x01,         /*bNumEndpoints*/
//  0x03,         /*bInterfaceClass: HID*/
//  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
//  0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
//  0,            /*iInterface: Index of string descriptor*/
//  /******************** Descriptor of Joystick Mouse HID ********************/
//  /* 18 */
//  0x09,         /*bLength: HID Descriptor size*/
//  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//  0x11,         /*bcdHID: HID Class Spec release number*/
//  0x01,
//  0x00,         /*bCountryCode: Hardware target country*/
//  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//  0x22,         /*bDescriptorType*/
//  HID_MOUSE_REPORT_DESC_SIZE_LBS,/*wItemLength: Total length of Report descriptor*/
//  HID_MOUSE_REPORT_DESC_SIZE_MBS,
//  /******************** Descriptor of Mouse endpoint ********************/
//  /* 27 */
//  0x07,          /*bLength: Endpoint Descriptor size*/
//  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
//
//  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
//  0x03,          /*bmAttributes: Interrupt endpoint*/
//  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
//  0x00,
//  HID_HS_BINTERVAL,          /*bInterval: Polling Interval */
//  /* 34 */
//};

///* USB HID device Other Speed Configuration Descriptor */
//__ALIGN_BEGIN static uint8_t USBD_HID_OtherSpeedCfgDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
//{
//  0x09, /* bLength: Configuration Descriptor size */
//  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
//  USB_HID_CONFIG_DESC_SIZ,
//  /* wTotalLength: Bytes returned */
//  0x00,
//  0x01,         /*bNumInterfaces: 1 interface*/
//  0x01,         /*bConfigurationValue: Configuration value*/
//  0x00,         /*iConfiguration: Index of string descriptor describing
//  the configuration*/
//  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
//  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/
//
//  /************** Descriptor of Joystick Mouse interface ****************/
//  /* 09 */
//  0x09,         /*bLength: Interface Descriptor size*/
//  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
//  0x00,         /*bInterfaceNumber: Number of Interface*/
//  0x00,         /*bAlternateSetting: Alternate setting*/
//  0x01,         /*bNumEndpoints*/
//  0x03,         /*bInterfaceClass: HID*/
//  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
//  0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
//  0,            /*iInterface: Index of string descriptor*/
//  /******************** Descriptor of Joystick Mouse HID ********************/
//  /* 18 */
//  0x09,         /*bLength: HID Descriptor size*/
//  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//  0x11,         /*bcdHID: HID Class Spec release number*/
//  0x01,
//  0x00,         /*bCountryCode: Hardware target country*/
//  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//  0x22,         /*bDescriptorType*/
//  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
//  0x00,
//  /******************** Descriptor of Mouse endpoint ********************/
//  /* 27 */
//  0x07,          /*bLength: Endpoint Descriptor size*/
//  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
//
//  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
//  0x03,          /*bmAttributes: Interrupt endpoint*/
//  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
//  0x00,
//  HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
//  /* 34 */
//};


///* USB HID device Configuration Descriptor */
//__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ]  __ALIGN_END  =
//{
//  /* 18 */
//  0x09,         /*bLength: HID Descriptor size*/
//  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//  0x11,         /*bcdHID: HID Class Spec release number*/
//  0x01,
//  0x00,         /*bCountryCode: Hardware target country*/
//  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//  0x22,         /*bDescriptorType*/
//  HID_MOUSE_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
//  0x00,
//};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static const uint8_t USBD_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,        // bLength
  USB_DESC_TYPE_DEVICE_QUALIFIER,    // bDescriptorType (Device Qualifier)
  0x01,                              // bcdUSB 2.01
  0x02,                              // bcdUSB 2.01
  0x00,                              // bDeviceClass (Use class information in the Interface Descriptors)
  0x00,                              // bDeviceSubClass
  0x00,                              // bDeviceProtocol
  0x40,                              // bMaxPacketSize0 64
  0x01,                              // bNumConfigurations 1
  0x00,                              // bReserved
};

__ALIGN_BEGIN static uint8_t HID_MOUSE_ReportDesc[HID_MOUSE_REPORT_DESC_SIZE]  __ALIGN_END =
{
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x02,        // Usage (Mouse)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x02,        //   Report ID (2)
  0x09, 0x01,        //   Usage (Pointer)
  0xA1, 0x00,        //   Collection (Physical)
  0x05, 0x09,        //     Usage Page (Button)
  0x19, 0x01,        //     Usage Minimum (0x01)
  0x29, 0x02,        //     Usage Maximum (0x02)
  0x15, 0x00,        //     Logical Minimum (0)
  0x25, 0x01,        //     Logical Maximum (1)
  0x75, 0x01,        //     Report Size (1)
  0x95, 0x02,        //     Report Count (2)
  0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x95, 0x06,        //     Report Count (6)
  0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
  0x09, 0x30,        //     Usage (X)
  0x09, 0x31,        //     Usage (Y)
  0x15, 0x81,        //     Logical Minimum (129)
  0x25, 0x7F,        //     Logical Maximum (127)
  0x75, 0x08,        //     Report Size (8)
  0x95, 0x02,        //     Report Count (2)
  0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
  0xC0,              //   End Collection
  0xC0,              // End Collection
};

__ALIGN_BEGIN static const uint8_t HID_KEYBOARD_ReportDesc[HID_KEYBOARD_REPORT_DESC_SIZE]  __ALIGN_END =
{
  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x06,        // Usage (Keyboard)
  0xA1, 0x01,        // Collection (Application)
  0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
  0x19, 0xE0,        //   Usage Minimum (Keyboard LeftControl)
  0x29, 0xE7,        //   Usage Maximum (Keyboard Right GUI)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x08,        //   Report Count (8)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x19, 0x00,        //   Usage Minimum (UNDEFINED)
  0x29, 0x73,        //   Usage Maximum (0x73)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x73,        //   Logical Maximum (115)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x06,        //   Report Count (6)
  0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x08,        //   Usage Page (LEDs)
  0x19, 0x01,        //   Usage Minimum (Num Lock)
  0x29, 0x05,        //   Usage Maximum (Kana)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x05,        //   Report Count (5)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x95, 0x03,        //   Report Count (3)
  0x91, 0x01,        //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection
};

__ALIGN_BEGIN static const uint8_t HID_CUSTOM_ReportDesc[HID_CUSTOM_REPORT_DESC_SIZE]  __ALIGN_END =
{
  0x05, 0x0C,        // Usage Page(Consumer ) 
  0x09, 0x01,        // Usage(Consumer Control)
  0xA1, 0x01,        // Collection(Application )
  0x85, 0x12,        //   Report ID(0x12)
  0x05, 0x0C,        //   Usage Page(Consumer )
  0x15, 0x00,        //   Logical Minimum(0x0 )
  0x25, 0x01,        //   Logical Maximum(0x1 )
  0x75, 0x01,        //   Report Size(0x1 )
  0x95, 0x05,        //   Report Count(0x5 )
  0x09, 0x6F,        //   Usage(Unassigned)
  0x09, 0x70,        //   Usage(Unassigned)
  0x09, 0xE2,        //   Usage(Mute)
  0x09, 0xE9,        //   Usage(Volume Increment)
  0x09, 0xEA,        //   Usage(Volume Decrement)
  0x81, 0x02,        //   Input(Data, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
  0x95, 0x03,        //   Report Count(0x3 )
  0x81, 0x01,        //   Input(Constant, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
  0xC0,              // End Collection
  0x05, 0x01,        // Usage Page(Generic Desktop Controls )
  0x09, 0x0C,        // Usage(Undefined)
  0xA1, 0x01,        // Collection(Application )
  0x85, 0x14,        //   Report ID(0x14 )
  0x15, 0x00,        //   Logical Minimum(0x0 )
  0x25, 0x01,        //   Logical Maximum(0x1 )
  0x09, 0xC6,        //   Usage(Undefined)
  0x95, 0x01,        //   Report Count(0x1 )
  0x75, 0x01,        //   Report Size(0x1 )
  0x81, 0x06,        //   Input(Data, Variable, Relative, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
  0x75, 0x07,        //   Report Size(0x7 )
  0x81, 0x03,        //   Input(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
  0xC0,              // End Collection
  0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
  0x09, 0x00,        // USAGE (Undefined)
  0xa1, 0x01,        // COLLECTION (Application)
  0x85, 0x17,        //   REPORT_ID (23)
  0x15, 0x00,        //   LOGICAL_MINIMUM (0)
  0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
  0x09, 0x00,        //   USAGE (Undefined)
  0x95, 0x01,        //   REPORT_COUNT (1)
  0x75, 0x01,        //   REPORT_SIZE (1)
  0x91, 0x02,        //   OUTPUT (Data,Var,Abs)
  0x75, 0x07,        //   REPORT_SIZE (7)
  0x91, 0x03,        //   OUTPUT (Cnst,Var,Abs)
  0xc0               // END_COLLECTION
};


/**
  * @}
  */

/** @defgroup USBD_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  // Open Mouse EP IN
  USBD_LL_OpenEP(pdev, HID_MOUSE_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_MOUSE_EPIN_SIZE);
  pdev->ep_in[HID_MOUSE_EPIN_ADDR & 0xFU].is_used = 1U;

  // Open Keyboard EP IN
  USBD_LL_OpenEP(pdev, HID_KEYBOARD_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_KEYBOARD_EPIN_SIZE);
  pdev->ep_in[HID_KEYBOARD_EPIN_ADDR & 0xFU].is_used = 1U;

  // Open Custom EP IN
  USBD_LL_OpenEP(pdev, HID_CUSTOM_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_CUSTOM_EPIN_SIZE);
  pdev->ep_in[HID_CUSTOM_EPIN_ADDR & 0xFU].is_used = 1U;

  // Open Custom EP OUT
  USBD_LL_OpenEP(pdev, HID_CUSTOM_EPOUT_ADDR, USBD_EP_TYPE_INTR, HID_CUSTOM_EPOUT_SIZE);
  pdev->ep_out[HID_CUSTOM_EPOUT_ADDR & 0xFU].is_used = 1U;


//  pdev->pClassData = USBD_malloc(sizeof (USBD_HID_HandleTypeDef));
  s_xHidClassData.Interface = 0;
  s_xHidClassData.IsReportAvailable = 0;
  s_xHidClassData.AltSetting = 0;
  s_xHidClassData.IdleState = 0;
  s_xHidClassData.Protocol = 0;
  for (uint8_t i=0; i<USBD_MAX_NUM_INTERFACES; ++i) {
    s_xHidClassData.state[i] = HID_IDLE;
  }

  pdev->pClassData = &s_xHidClassData;

  if (pdev->pClassData == NULL)
  {
    return USBD_FAIL;
  }

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  USBD_HID_DeInit (USBD_HandleTypeDef *pdev,  uint8_t cfgidx)
{
  // Close Mouse EP IN
  USBD_LL_CloseEP(pdev, HID_MOUSE_EPIN_ADDR);
  pdev->ep_in[HID_MOUSE_EPIN_ADDR & 0xFU].is_used = 0U;

  // Close Keyboard EP IN
  USBD_LL_CloseEP(pdev, HID_KEYBOARD_EPIN_ADDR);
  pdev->ep_in[HID_KEYBOARD_EPIN_ADDR & 0xFU].is_used = 0U;

  // Close Custom EP IN
  USBD_LL_CloseEP(pdev, HID_CUSTOM_EPIN_ADDR);
  pdev->ep_in[HID_CUSTOM_EPIN_ADDR & 0xFU].is_used = 0U;

  // Close Custom EP OUT
  USBD_LL_CloseEP(pdev, HID_CUSTOM_EPOUT_ADDR);
  pdev->ep_out[HID_CUSTOM_EPOUT_ADDR & 0xFU].is_used = 0U;

  /* FRee allocated memory */
  if(pdev->pClassData != NULL)
  {
//    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
  switch (req->bmRequest & USB_REQ_RECIPIENT_MASK) {
  case USB_REQ_RECIPIENT_INTERFACE:
    if (req->wIndex == HID_MOUSE_INTERFACE) {
      return (USBD_HID_MouseIF_Setup(pdev, req));
    }
    else if(req->wIndex == HID_KEYBOARD_INTERFACE) {
      return (USBD_HID_KeyboardIF_Setup(pdev, req));
    }
    else if(req->wIndex == HID_CUSTOM_INTERFACE) {
      return (USBD_HID_CustomIF_Setup(pdev, req));
    }
    else {
      SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("HID_USB_D_S: unknown IF:%i.\r\n", req->wIndex));
    }
    break;
  case USB_REQ_RECIPIENT_ENDPOINT:
    if ((req->wIndex == HID_MOUSE_EPIN_ADDR)) {
      return (USBD_HID_MouseIF_Setup (pdev, req));
    }
    else if ((req->wIndex == HID_KEYBOARD_EPIN_ADDR)) {
      return (USBD_HID_KeyboardIF_Setup(pdev, req));
    }
    else if(req->wIndex == HID_CUSTOM_EPIN_ADDR) {
      return (USBD_HID_CustomIF_Setup(pdev, req));
    }
    else {
      SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("HID_USB_D_S: unknown EP:%i.\r\n", req->wIndex));
    }
    break;
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("HID_USB_D_S: unknown:0x%x.\r\n", req->bRequest));

  return USBD_OK;
}

/**
  * @brief  USBD_HID_MouselIF_Setup
  *         Handle the HID specific requests for the mouse IF
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  USBD_HID_MouseIF_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
{
  uint16_t len = 0;
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*) pdev->pClassData;
  HIDUSBHelper *pHelper = (HIDUSBHelper*)pdev->pUserData;
  UsbEvent xEvent;

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HID_USB_D_S: MouselIF_Setup() ---> bRequest = 0x%x bmRequest = 0x%x\r\n", req->bRequest, req->bmRequest));

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {

  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest) {
    case HID_REQ_SET_PROTOCOL:
      hhid->Protocol = (uint8_t)(req->wValue);
      break;
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->Protocol, 1);
      break;
    case HID_REQ_SET_IDLE:
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->IdleState, 1);
      break;
    case HID_REQ_SET_REPORT:
      hhid->IsReportAvailable = 1;
      USBD_CtlPrepareRx (pdev, hhid->Report.pcReportBuffer, (uint8_t)(req->wLength));
      break;
    case HID_REQ_GET_REPORT:
      SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HID_USB_D_S: MouseIF_Setup() ---> reportID = %x \r\n", LOBYTE(req->wValue)));
      UsbEventInit(&xEvent, (IEventSrc*)pHelper, E_USB_GET_REPORT, LOBYTE(req->wValue), NULL);
      IEventSrcSendEvent((IEventSrc*)pHelper, (IEvent*)&xEvent, NULL);
      break;
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest) {
    case USB_REQ_GET_DESCRIPTOR:
      if( req->wValue >> 8 == HID_REPORT_DESC) {
        len = MIN(HID_MOUSE_REPORT_DESC_SIZE , req->wLength);
        // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.(uint8_t*)
        USBD_CtlSendData (pdev, (uint8_t*)HID_MOUSE_ReportDesc, len);
      }
      break;
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->AltSetting, 1);
      break;
    case USB_REQ_SET_INTERFACE :
      hhid->AltSetting = (uint8_t)(req->wValue);
      break;
    }
  }

  return USBD_OK;
}

uint8_t  USBD_HID_KeyboardIF_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
  uint16_t len = 0;
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {

  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest) {
    case HID_REQ_SET_PROTOCOL:
      hhid->Protocol = (uint8_t)(req->wValue);
      break;
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->Protocol, 1);
      break;
    case HID_REQ_SET_IDLE:
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->IdleState, 1);
      break;
    case HID_REQ_SET_REPORT:
      hhid->IsReportAvailable = 1;
      hhid->Interface = 1;
      USBD_CtlPrepareRx (pdev, hhid->Report.pcReportBuffer, (uint8_t)(req->wLength));
      break;
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest) {
    case USB_REQ_GET_DESCRIPTOR:
      if( req->wValue >> 8 == HID_REPORT_DESC) {
        len = MIN(HID_KEYBOARD_REPORT_DESC_SIZE , req->wLength);
        // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.
        USBD_CtlSendData (pdev, (uint8_t*)HID_KEYBOARD_ReportDesc, len);
      }
      break;
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->AltSetting, 1);
      break;
    case USB_REQ_SET_INTERFACE :
      hhid->AltSetting = (uint8_t)(req->wValue);
      break;
    }
  }

  return USBD_OK;
}

/**
  * @brief  USBD_HID_CustomIF_Setup
  *         Handle the HID specific requests for the custom IF
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
uint8_t  USBD_HID_CustomIF_Setup (USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req) {
  uint16_t len = 0;
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;

  switch (req->bmRequest & USB_REQ_TYPE_MASK) {

  case USB_REQ_TYPE_CLASS :
    switch (req->bRequest) {
    case HID_REQ_SET_PROTOCOL:
      hhid->Protocol = (uint8_t)(req->wValue);
      break;
    case HID_REQ_GET_PROTOCOL:
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->Protocol, 1);
      break;
    case HID_REQ_SET_IDLE:
      hhid->IdleState = (uint8_t)(req->wValue >> 8);
      break;
    case HID_REQ_GET_IDLE:
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->IdleState, 1);
      break;
    case HID_REQ_SET_REPORT:
      hhid->IsReportAvailable = 1;
      USBD_CtlPrepareRx(pdev, (uint8_t *)&hhid->Report.pcReportBuffer, req->wLength);
      break;
    default:
      USBD_CtlError (pdev, req);
      return USBD_FAIL;
    }
    break;

  case USB_REQ_TYPE_STANDARD:
    switch (req->bRequest) {
    case USB_REQ_GET_DESCRIPTOR:
      if( req->wValue >> 8 == HID_REPORT_DESC) {
        len = MIN(HID_CUSTOM_REPORT_DESC_SIZE , req->wLength);
        // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.
        USBD_CtlSendData (pdev, (uint8_t*)HID_CUSTOM_ReportDesc, len);
      }
      break;
    case USB_REQ_GET_INTERFACE :
      USBD_CtlSendData (pdev, (uint8_t *)&hhid->AltSetting, 1);
      break;
    case USB_REQ_SET_INTERFACE :
      hhid->AltSetting = (uint8_t)(req->wValue);
      break;
    }
  }

  return USBD_OK;
}

/**
  * @brief  USBD_HID_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_MouseSendReport(USBD_HandleTypeDef  *pdev, uint8_t *report, uint16_t len) {
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;

  if (pdev->dev_state == USBD_STATE_CONFIGURED ) {
    if(hhid->state[HID_MOUSE_IF_IDX] == HID_IDLE) {
      hhid->state[HID_MOUSE_IF_IDX] = HID_BUSY;
      USBD_LL_Transmit (pdev, HID_MOUSE_EPIN_ADDR, report, len);
    }
    else {
      return USBD_BUSY;
    }
  }

  return USBD_OK;
}

uint8_t USBD_HID_KeyboardSendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len) {
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;

  if (pdev->dev_state == USBD_STATE_CONFIGURED ) {
    if(hhid->state[HID_KEYBOARD_IF_IDX] == HID_IDLE) {
      hhid->state[HID_KEYBOARD_IF_IDX] = HID_BUSY;
      USBD_LL_Transmit (pdev, HID_KEYBOARD_EPIN_ADDR, report, len);
    }
    else {
      return USBD_BUSY;
    }
  }

  return USBD_OK;
}

uint8_t USBD_HID_CustomSendReport(USBD_HandleTypeDef *pdev, uint8_t *report, uint16_t len) {
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;

  if (pdev->dev_state == USBD_STATE_CONFIGURED ) {
    if(hhid->state[HID_CUSTOM_IF_IDX] == HID_IDLE) {
      hhid->state[HID_CUSTOM_IF_IDX] = HID_BUSY;
      USBD_LL_Transmit (pdev, HID_CUSTOM_EPIN_ADDR, report, len);
    }
    else {
      return USBD_BUSY;
    }
  }

  return USBD_OK;
}

/**
  * @brief  USBD_HID_GetPollingInterval
  *         return polling interval from endpoint descriptor
  * @param  pdev: device instance
  * @retval polling interval
  */
uint32_t USBD_HID_GetPollingInterval (USBD_HandleTypeDef *pdev)
{
  uint32_t polling_interval = 0U;

  /* HIGH-speed endpoints */
  if(pdev->dev_speed == USBD_SPEED_HIGH)
  {
   /* Sets the data transfer polling interval for high speed transfers.
    Values between 1..16 are allowed. Values correspond to interval
    of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1U <<(HID_HS_BINTERVAL - 1U))) / 8U);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;
  }

  return ((uint32_t)(polling_interval));
}

/**
  * @brief  USBD_HID_GetCfgFSDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetFSCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_HID_CfgFSDesc);
  // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.
  return (uint8_t*)USBD_HID_CfgFSDesc;
}

/**
  * @brief  USBD_HID_GetCfgHSDesc
  *         return HS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetHSCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_HID_CfgFSDesc);
  // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.
  return (uint8_t*)USBD_HID_CfgFSDesc;
}

/**
  * @brief  USBD_HID_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *USBD_HID_GetOtherSpeedCfgDesc (uint16_t *length)
{
  *length = sizeof (USBD_HID_CfgFSDesc);
  // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.
  return (uint8_t*)USBD_HID_CfgFSDesc;
}

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
static uint8_t  USBD_HID_DataIn (USBD_HandleTypeDef *pdev,
                              uint8_t epnum)
{

  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_HID_HandleTypeDef *)pdev->pClassData)->state[epnum-1] = HID_IDLE;
  return USBD_OK;
}

/**
  * Handles control request data.
  *
  * @param  pdev: device instance
  * @retval status
  */
uint8_t USBD_HID_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef*)pdev->pClassData;
  HIDUSBHelper *pHelper = (HIDUSBHelper*)pdev->pUserData;
  UsbEvent xEvent;

  if (hhid->IsReportAvailable == 1) {
    if (hhid->Interface == 1) {
      // Special case!!! The only output report without a report ID in the report descriptor.
      // It is the report 0x11 that specifies the status of the LEDs.
      // Normalize the content of the buffer so also this report has an ID.
      hhid->Report.pcReportBuffer[1] = hhid->Report.pcReportBuffer[0];
      hhid->Report.pcReportBuffer[0] = HID_REPORT_ID_LEDS;
      hhid->Interface = 0;
    }

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("HID_USB_D_S: EP0_RxReady() ---> reportID = %x \r\n", hhid->Report.xReport.reportID));

    UsbEventInit(&xEvent, (IEventSrc*)pHelper, E_USB_SET_REPORT, hhid->Report.xReport.reportID, hhid->Report.pcReportBuffer);
    IEventSrcSendEvent((IEventSrc*)pHelper, (IEvent*)&xEvent, NULL);
    hhid->IsReportAvailable = 0;
  }

  return USBD_OK;
}


/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
static uint8_t  *USBD_HID_GetDeviceQualifierDesc (uint16_t *length)
{
  *length = sizeof (USBD_HID_DeviceQualifierDesc);
  // To silence annoying compiler warnings due to a poor API design I cast the descriptor buffer.
  return (uint8_t*)USBD_HID_DeviceQualifierDesc;
}


// Support to the Windows USB Selective suspend
// ********************************************

#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0 == 1)

const MS_OS_STR_DESC_V1_0 MsOsStrDescriptor_V1_0 =
{
  sizeof(MsOsStrDescriptor_V1_0),           //bLength
  USB_DESC_TYPE_STRING,                     //bDescriptorType
  {'M','S','F','T','1','0','0'},            //qwSignature
  GET_MS_DESCRIPTOR_FEATURE,                //bMS_VendorCode
  0x00                                      //bPad
};

const MS_COMPAT_ID_FEATURE_DESC_V1_0 ExtCompatIDFeatureDescriptor_V1_0 =
{
  //header
  sizeof(ExtCompatIDFeatureDescriptor_V1_0),//dwLength
  0x0100,                                   //bcdVersion
  MS_FEATURE_DESC_EXTENDED_COMPAT_ID,       //wIndex
  MAX_USB_FUNC_NUM,                         //bCount
  {0,0,0,0,0,0,0},                          //reserved[7]
  //fuction
  .func[0].bFirstInterfaceNumber = HID_MOUSE_INTERFACE,
  .func[0].reserved_1 = 1,
  .func[0].compatID = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  .func[0].subCompatID = {0,0,0,0,0,0,0,0},
  .func[0].reserved_2 = {0,0,0,0,0,0},
  //fuction
  .func[1].bFirstInterfaceNumber = HID_KEYBOARD_INTERFACE,
  .func[1].reserved_1 = 1,
  .func[1].compatID = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  .func[1].subCompatID = {0,0,0,0,0,0,0,0},
  .func[1].reserved_2 = {0,0,0,0,0,0},
#if (MAX_USB_FUNC_NUM == 3)
  //fuction
  .func[2].bFirstInterfaceNumber = HID_CUSTOM_INTERFACE,
  .func[2].reserved_1 = 1,
  .func[2].compatID = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
  .func[2].subCompatID = {0,0,0,0,0,0,0,0},
  .func[2].reserved_2 = {0,0,0,0,0,0},
#endif
};

const MS_PROPERTY_FEATURE_DESC ExtPropertyFeatureDescriptor_V1_0 =
{
  //header
  sizeof(ExtPropertyFeatureDescriptor_V1_0),//dwLength
  0x0100,                                   //bcdVersion
  MS_FEATURE_DESC_EXTENDED_PROPERTIES,      //wIndex
  MAX_USB_PROPERTY_NUM,                     //wCount
  //Property
  .prop[0].dwSize = sizeof(ExtPropertyFeatureDescriptor_V1_0.prop[0]),
  .prop[0].dwPropertyDataType = MS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN,
  .prop[0].wPropertyNameLength = sizeof(ExtPropertyFeatureDescriptor_V1_0.prop[0].bPropertyName),
  .prop[0].bPropertyName = {'S','e','l','e','c','t','i','v','e','S','u','s','p','e','n','d','E','n','a','b','l','e','d',0},
  .prop[0].dwPropertyDataLength = sizeof(ExtPropertyFeatureDescriptor_V1_0.prop[0].bPropertyData),
  .prop[0].bPropertyData = USBD_CFG_ENABLE_MS_SELECTIVE_SUSPEND,
};

static uint8_t* USBD_HID_OsStrDescriptor_V1_0(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length)
{
  *length = sizeof(MsOsStrDescriptor_V1_0);
  return (uint8_t *)&MsOsStrDescriptor_V1_0;
}

static uint8_t* USBD_HID_ExtCompatIdDescriptor(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length)
{
  *length = sizeof(ExtCompatIDFeatureDescriptor_V1_0);
  return (uint8_t *)&ExtCompatIDFeatureDescriptor_V1_0;
}

static uint8_t* USBD_HID_ExtPropDescriptor(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length)
{
//uint8_t page_num = (wValue>>8)&0xff;
uint8_t intf_num = (wValue)&0xff;

  if((intf_num == HID_MOUSE_INTERFACE) || (intf_num == HID_KEYBOARD_INTERFACE) || (intf_num == HID_CUSTOM_INTERFACE)){
    *length = sizeof(ExtPropertyFeatureDescriptor_V1_0);
    return (uint8_t *)&ExtPropertyFeatureDescriptor_V1_0;
  }
  return NULL;
}
#endif


#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0 == 1)

const MS_OS_STR_DESC_V2_0 MsOsStrDescriptor_V2_0 =
{
  //header
  .hdr.wLength = sizeof(MsOsStrDescriptor_V2_0.hdr),
  .hdr.wDescriptorType = MS_OS_20_SET_HEADER_DESCRIPTOR,
  .hdr.dwWindowsVersion = WINDOWS_VERSION_8_1,
  .hdr.wTotalLength = sizeof(MsOsStrDescriptor_V2_0),
  //registry
  .reg1.wLength = sizeof(MsOsStrDescriptor_V2_0.reg1),
  .reg1.wDescriptorType = MS_OS_20_FEATURE_REG_PROPERTY,
  .reg1.wPropertyDataType = MS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN,
  .reg1.wPropertyNameLength = sizeof(MsOsStrDescriptor_V2_0.reg1.bPropertyName),
  .reg1.bPropertyName = {'S','e','l','e','c','t','i','v','e','S','u','s','p','e','n','d','E','n','a','b','l','e','d',0},
  .reg1.wPropertyDataLength = sizeof(MsOsStrDescriptor_V2_0.reg1.PropertyData),
  .reg1.PropertyData = USBD_CFG_ENABLE_MS_SELECTIVE_SUSPEND,
  //config header
  .conf.hdr.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.hdr),
  .conf.hdr.wDescriptorType = MS_OS_20_SUBSET_HEADER_CONFIGURATION,
  .conf.hdr.bConfigurationValue = 0x00,   //it's not bConfigurationValue from configuration descriptor but iConfiguration(index)
  .conf.hdr.bReserved = 0x00,
  .conf.hdr.wTotalLength = sizeof(MsOsStrDescriptor_V2_0.conf),
  //func header
  .conf.funcs[0].hdr.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[0].hdr),
  .conf.funcs[0].hdr.wDescriptorType = MS_OS_20_SUBSET_HEADER_FUNCTION,
  .conf.funcs[0].hdr.bFirstInterface = HID_MOUSE_INTERFACE,
  .conf.funcs[0].hdr.bReserved = 0x00,
  .conf.funcs[0].hdr.wSubsetLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[0].hdr)+sizeof(MsOsStrDescriptor_V2_0.conf.funcs[0].reg),
  //registry
  .conf.funcs[0].reg.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[0].reg),
  .conf.funcs[0].reg.wDescriptorType = MS_OS_20_FEATURE_REG_PROPERTY,
  .conf.funcs[0].reg.wPropertyDataType = MS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN,
  .conf.funcs[0].reg.wPropertyNameLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[0].reg.bPropertyName),
  .conf.funcs[0].reg.bPropertyName = {'S','e','l','e','c','t','i','v','e','S','u','s','p','e','n','d','E','n','a','b','l','e','d',0},
  .conf.funcs[0].reg.wPropertyDataLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[0].reg.PropertyData),
  .conf.funcs[0].reg.PropertyData = USBD_CFG_ENABLE_MS_SELECTIVE_SUSPEND,
  //func header
  .conf.funcs[1].hdr.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[1].hdr),
  .conf.funcs[1].hdr.wDescriptorType = MS_OS_20_SUBSET_HEADER_FUNCTION,
  .conf.funcs[1].hdr.bFirstInterface = HID_KEYBOARD_INTERFACE,
  .conf.funcs[1].hdr.bReserved = 0x00,
  .conf.funcs[1].hdr.wSubsetLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[1].hdr)+sizeof(MsOsStrDescriptor_V2_0.conf.funcs[1].reg),
  //registry
  .conf.funcs[1].reg.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[1].reg),
  .conf.funcs[1].reg.wDescriptorType = MS_OS_20_FEATURE_REG_PROPERTY,
  .conf.funcs[1].reg.wPropertyDataType = MS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN,
  .conf.funcs[1].reg.wPropertyNameLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[1].reg.bPropertyName),
  .conf.funcs[1].reg.bPropertyName = {'S','e','l','e','c','t','i','v','e','S','u','s','p','e','n','d','E','n','a','b','l','e','d',0},
  .conf.funcs[1].reg.wPropertyDataLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[1].reg.PropertyData),
  .conf.funcs[1].reg.PropertyData = USBD_CFG_ENABLE_MS_SELECTIVE_SUSPEND,
#if (MAX_USB_FUNC_NUM == 3)
  //func header
  .conf.funcs[2].hdr.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[2].hdr),
  .conf.funcs[2].hdr.wDescriptorType = MS_OS_20_SUBSET_HEADER_FUNCTION,
  .conf.funcs[2].hdr.bFirstInterface = HID_CUSTOM_INTERFACE,
  .conf.funcs[2].hdr.bReserved = 0x00,
  .conf.funcs[2].hdr.wSubsetLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[2].hdr)+sizeof(MsOsStrDescriptor_V2_0.conf.funcs[2].reg),
  //registry
  .conf.funcs[2].reg.wLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[2].reg),
  .conf.funcs[2].reg.wDescriptorType = MS_OS_20_FEATURE_REG_PROPERTY,
  .conf.funcs[2].reg.wPropertyDataType = MS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN,
  .conf.funcs[2].reg.wPropertyNameLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[2].reg.bPropertyName),
  .conf.funcs[2].reg.bPropertyName = {'S','e','l','e','c','t','i','v','e','S','u','s','p','e','n','d','E','n','a','b','l','e','d',0},
  .conf.funcs[2].reg.wPropertyDataLength = sizeof(MsOsStrDescriptor_V2_0.conf.funcs[2].reg.PropertyData),
  .conf.funcs[2].reg.PropertyData = USBD_CFG_ENABLE_MS_SELECTIVE_SUSPEND,
#endif
};


static uint8_t* USBD_HID_OsStrDescriptor_V2_0(USBD_HandleTypeDef *pdev, uint16_t wValue, uint16_t *length)
{
  *length = sizeof(MsOsStrDescriptor_V2_0);
  return (uint8_t *)&MsOsStrDescriptor_V2_0;
}
#endif


#if ((USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0 == 1) || (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0 == 1))
USBD_StatusTypeDef USBD_VendorRequest(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req)
{
uint16_t len = 0;
uint8_t *pbuf = NULL;

  if((req->bRequest == GET_MS_DESCRIPTOR_FEATURE) &&
    ((req->bmRequest & 0x60) == USB_REQ_TYPE_VENDOR))
  {
    switch(req->bmRequest & 0x1F)
    {
      case USB_REQ_RECIPIENT_DEVICE:
#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0==1)
        if(req->wIndex == MS_FEATURE_DESC_EXTENDED_COMPAT_ID){
          pbuf = pdev->pClass->GetOsExtCompatIdDescriptor(pdev, (req->wValue) , &len);
        }
#endif
        //break;  //Windows 7 drivers query extended properties to device?? not interface???
      case USB_REQ_RECIPIENT_INTERFACE:
#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V1_0==1)
        if(req->wIndex == MS_FEATURE_DESC_EXTENDED_PROPERTIES){
          pbuf = pdev->pClass->GetOsExtPropDescriptor(pdev, (req->wValue) , &len);
        }
#endif
#if (USBD_CFG_ENABLE_MS_OS_DESCRIPTOR_V2_0==1)
        if(req->wIndex == USBD_IDX_MS_OS_STR_V2_0){
          pbuf = pdev->pClass->GetOsStrDescriptor_V2_0(pdev, (req->wValue) , &len);
        }
#endif
        break;
    }

    if((len != 0) && (req->wLength != 0) && (pbuf != NULL)){
      len = MIN(len , req->wLength);
      return USBD_CtlSendData (pdev, pbuf, len);
    }

  }
  return USBD_FAIL;
}
#endif


/**
  * @}
  */


/**
  * @}
  */


/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
