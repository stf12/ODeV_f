/**
 ******************************************************************************
 * @file    hid_report_parser.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.3.0
 * @date    Jan 21, 2019
 *
 * @brief   Input Output and Feature report Declaration.
 *
 * This file declare the Input, Output and Feature report for the mouse
 * (interface 0), the keyboard (interface 1) and the
 * custom control (interface 2).
 * The data are the result of a static analysis of the device report descriptor.
 *
 * Report ID list:
 * - 0x02: inputReport   (Device --> Host). Mouse
 * - 0x10: inputReport   (Device --> Host). Keyboard
 * - 0x11: outputReport  (Device <-- Host). Led
 * - 0x12: inputReport   (Device --> Host). Function keys (F2, F3, F6, F7, F8)
 * - 0x14: inputReport   (Device --> Host). Function keys (F12)
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

#ifndef HID_REPORT_PARSER_H_
#define HID_REPORT_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "syserror.h"

#define HID_REPORT_ID_MOUSE               0x02
#define HID_REPORT_ID_KEYBOARD            0x10
#define HID_REPORT_ID_LEDS                0x11
#define HID_REPORT_ID_FN_KEYS_1           0x12
#define HID_REPORT_ID_FN_KEYS_2           0x14
#define HID_REPORT_ID_SHORTCUTS           0x17
#define HID_REPORT_ID_FORCE_STEP          0xFE  /// Special ID used by the INIT task to force the execution of ManagedTaskEx step.

typedef union _HIDReport {
  uint8_t reportID;

  //--------------------------------------------------------------------------------
  //  inputReport 02 (Device --> Host) - Mouse
  //--------------------------------------------------------------------------------

  struct inputReport02_t
  {
    uint8_t  reportId;                                 // Report ID = 0x02 (2)
    uint8_t  nButton1 : 1;                             // Usage 0x00090001: , Value = 0 to 1
    uint8_t  nButton2 : 1;                             // Usage 0x00090002: , Value = 0 to 1
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    int8_t   nDX;                                      // Usage 0x00010030: , Value = -127 to 127
    int8_t   nDY;                                      // Usage 0x00010031: , Value = -127 to 127
  } inputReport02;


  //--------------------------------------------------------------------------------
  //  inputReport (Device --> Host): keyboard
  //--------------------------------------------------------------------------------

  struct inputReport10_t
  {
    uint8_t  reportId;                                 // Report ID = 0x10 (16)
    uint8_t  nLeftCtrl   : 1;                          // Usage 0x000700E0: , Value = 0 to 1
    uint8_t  nLeftShift  : 1;                          // Usage 0x000700E1: , Value = 0 to 1
    uint8_t  nLeftAlt    : 1;                          // Usage 0x000700E2: , Value = 0 to 1
    uint8_t  nLeftGUI    : 1;                          // Usage 0x000700E3: , Value = 0 to 1
    uint8_t  nRightCtrl  : 1;                          // Usage 0x000700E4: , Value = 0 to 1
    uint8_t  nRightShift : 1;                          // Usage 0x000700E5: , Value = 0 to 1
    uint8_t  nRightAlt   : 1;                          // Usage 0x000700E6: , Value = 0 to 1
    uint8_t  nRightGUI   : 1;                          // Usage 0x000700E7: , Value = 0 to 1
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  nKeyCodeArray[6];                            // Value = 0 to 115
  } inputReport10;


  //--------------------------------------------------------------------------------
  //  inputReport 12 (Device --> Host)
  //--------------------------------------------------------------------------------

  struct inputReport12_t
  {
    uint8_t  reportId;                                 // Report ID = 0x12 (18)
    uint8_t  nBrightnessIncrement: 1;                  // Usage 0x000C006F: , Value = 0 to 1
    uint8_t  nBrightnessDecrement: 1;                  // Usage 0x000C0070: , Value = 0 to 1
    uint8_t  nMute : 1;                                // Usage 0x000C00E2: , Value = 0 to 1
    uint8_t  nVolumeIncrement : 1;                     // Usage 0x000C00E9: , Value = 0 to 1
    uint8_t  nVolumeDecrement : 1;                     // Usage 0x000C00EA: , Value = 0 to 1
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
  } inputReport12;


  //--------------------------------------------------------------------------------
  //  inputReport 14 (Device --> Host)
  //--------------------------------------------------------------------------------

  struct inputReport14_t
  {
    uint8_t  reportId;                                 // Report ID = 0x14 (20)
    uint8_t  nAirplaneMode:         1;                 // Usage 0x000C00C6: , Value = 0 to 1
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
  } inputReport14;


  //--------------------------------------------------------------------------------
  //  outputReport (Device <-- Host)
  //--------------------------------------------------------------------------------

  struct outputReport11_t
  {
    uint8_t  reportId;                                 // Report ID = 0x11 (17)
    uint8_t  nNumLock : 1;                             // Usage 0x00080001: , Value = 0 to 1
    uint8_t  nCapsLock : 1;                            // Usage 0x00080002: , Value = 0 to 1
    uint8_t  nScrollLock : 1;                          // Usage 0x00080003: , Value = 0 to 1
    uint8_t  nCompose : 1;                             // Usage 0x00080004: , Value = 0 to 1
    uint8_t  nKana : 1;                                // Usage 0x00080005: , Value = 0 to 1
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
    uint8_t  : 1;                                      // Pad
  } outputRepor11;


  //--------------------------------------------------------------------------------
  // Generic Desktop Page outputReport 17 (Device <-- Host)
  //--------------------------------------------------------------------------------

  struct outputReport17_t
  {
    uint8_t  reportId;                                 // Report ID = 0x17 (23)
                                                       // Collection: Undefined
    uint8_t  nShortcutsPageId : 1;                     // Usage 0x00010000: Undefined, Value = 0 to 1
    uint8_t  nReserved : 7;                            // Pad
  } outputReport17;


  //--------------------------------------------------------------------------------
  //  internalReport (Device)
  //--------------------------------------------------------------------------------

  struct internalReportFE_t
  {
    uint8_t  reportId;                                 // Report ID = 0xFE
    uint8_t  nData;                                    // reserved. It can be ignored
  } internalReportFE;

} HIDReport;


/**
 * Get the size of the report with a specified ID
 *
 * @param nReportID [IN] specifies a report ID
 * @return the size of the report with the specified ID or SYS_INVALID_PARAMETER_ERROR_CODE
 */
uint16_t HidReportGetSize(uint8_t nReportID);


#ifdef __cplusplus
}
#endif


#endif /* HID_REPORT_PARSER_H_ */
