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
#define SYS_DBG_HW                         SYS_DBG_ON                 ///< Hello World task debug control byte
#define SYS_DBG_SPIBUS                     SYS_DBG_ON                 ///< SPIBus task debug control byte
#define SYS_DBG_ISM330DHCX                 SYS_DBG_ON                 ///< ISM330DHCX sensor task debug control byte
#define SYS_DBG_IIS3DWB                    SYS_DBG_ON                 ///< IIS3DWB sensor task debug control byte
#define SYS_DBG_AI                         SYS_DBG_ON                 ///< AI task debug control byte
#define SYS_DBG_UTIL                       SYS_DBG_ON                 ///< Utility task debug control byte
#define SYS_DBG_SDC                        SYS_DBG_ON                 ///< SDCARD task debug control byte

#ifdef __cplusplus
}
#endif


#endif /* SYSDEBUG_CONFIG_H_ */
