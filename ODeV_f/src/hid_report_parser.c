/**
 ******************************************************************************
 * @file    hid_report_parser.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Jan 22, 2019
 *
 * @brief   Definition of the HID REport PArser API.
 *
 * Definition of the HID REport PArser API.
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

#include "hid_report_parser.h"

uint16_t HidReportGetSize(uint8_t nReportID) {
  uint16_t nSize;
  switch (nReportID) {
  case HID_REPORT_ID_MOUSE:
    nSize = sizeof(struct inputReport02_t);
    break;
  case HID_REPORT_ID_KEYBOARD:
    nSize = sizeof(struct inputReport10_t);
    break;
  case HID_REPORT_ID_LEDS:
    nSize = sizeof(struct outputReport11_t);
    break;
  case HID_REPORT_ID_FN_KEYS_1:
    nSize = sizeof(struct inputReport12_t);
    break;
  case HID_REPORT_ID_FN_KEYS_2:
    nSize = sizeof(struct inputReport14_t);
    break;
  case HID_REPORT_ID_FORCE_STEP:
    nSize = sizeof(struct internalReportFE_t);
    break;

  default:
    nSize = HID_REPORT_ID_FORCE_STEP;
  }

  return nSize;
}
