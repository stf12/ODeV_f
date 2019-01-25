/**
 ******************************************************************************
 * @file    sysdebug_config.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Oct 10, 2016
 * @brief   Configure the debug log functionality
 *
 * Each logic module of the application should define a DEBUG control byte
 * used to turn on/off the log for the module.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

#ifndef SYSDEBUG_CONFIG_H_
#define SYSDEBUG_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

// Example
//#define SYS_DBG_MODULE1     SYS_DBG_ON|GTS_DBG_HALT  ///< Module 1 debug control byte
//#define SYS_DBG_MODULE2     SYS_DBG_ON               ///< Module 2 debug control byte


#define SYS_DBG_INIT                       SYS_DBG_ON                 ///< Init task debug control byte
#define SYS_DBG_DRIVERS                    SYS_DBG_OFF                ///< Drivers debug control byte
#define SYS_DBG_APP                        SYS_DBG_OFF                ///< Generic Application debug control byte
#define SYS_DBG_APMH                       SYS_DBG_ON                 ///< Application Power Mode Helper debug control byte
#define SYS_DBG_HW                         SYS_DBG_OFF                ///< Hello World task debug control byte
#define SYS_DBG_PB                         SYS_DBG_ON                 ///< Push button task debug control byte
#define SYS_DBG_GUI                        SYS_DBG_ON                 ///< GUI task (TouchGFX) debug control byte
#define SYS_DBG_TSC                        SYS_DBG_ON                 ///< Touch Screen Controller (TouchGFX) debug control byte
#define SYS_DBG_HID_USB_DEVICE_SERVICES    SYS_DBG_ON                ///< USB debug control byte
#define SYS_DBG_HCC_TASK                   SYS_DBG_OFF                ///< Hst Communication Channel task debug control byte


#ifdef __cplusplus
}
#endif


#endif /* SYSDEBUG_CONFIG_H_ */
