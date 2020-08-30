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
  case HID_REPORT_ID_ISM330DHCX:
    nSize = sizeof(struct ism330dhcxReport_t);
    break;

  case HID_REPORT_ID_IIS3DWB:
    nSize = sizeof(struct iis3dwbReport_t);
    break;

  case HID_REPORT_ID_FORCE_STEP:
    nSize = sizeof(struct internalReportFE_t);
    break;

  case HID_REPORT_ID_AI_CMD:
    nSize = sizeof(struct aiReport_t);
    break;

  case HID_REPORT_ID_SD_CMD:
    nSize = sizeof(struct sdReport_t);
    break;

  case HID_REPORT_ID_SENSOR_CMD:
    nSize = sizeof(struct sensorReport_t);
    break;

  case HID_REPORT_ID_SPI_BUS_READ:
  case HID_REPORT_ID_SPI_BUS_WRITE:
    nSize = sizeof(struct spiIOReport_t);
    break;

  case HID_REPORT_ID_I2C_BUS_READ:
  case HID_REPORT_ID_I2C_BUS_WRITE:
    nSize = sizeof(struct i2cIOReport_t);
    break;

  case HID_REPORT_ID_HTS221:
    nSize = sizeof (struct hts221Report_t);
    break;

  default:
    nSize = sizeof(struct internalReportFE_t);
  }

  return nSize;
}
