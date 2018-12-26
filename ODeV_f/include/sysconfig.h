/**
 ******************************************************************************
 * @file    sysconfig.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 5, 2017
 * @brief   Global System configuration file
 *
 * This file include some configuration parameters grouped here for user
 * convenience. This file override the default configuration value, and it is
 * used in the "Preinclude file" section of the "compiler > prepocessor"
 * options.
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

#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_


// Drivers configuration
// *********************

// file NucleoDriver.c
// uncomment the following line to change the drive common parameters
//#define NUCLEO_DRV_CFG_IRQ_PRIORITY              13

// Other hardware configuration
// ****************************


// Services configuration
// **********************

// files syslowpower.h, SysDefPowerModeHelper.c
#define SYS_CFG_USE_DEFAULT_PM_HELPER              1
#define SYS_CFG_DEF_PM_HELPER_STANDBY              0  ///< if defined to 1 then the MCU goes in STANDBY mode when the system enters in SLEEP_1.


// Tasks configuration
// *******************

// file IManagedTask.h
#define MT_ALLOWED_ERROR_COUNT                     0x2

// file sysinit.c
#define INIT_TASK_CFG_ENABLE_BOOT_IF               0
#define INIT_TASK_CFG_STACK_SIZE                   (configMINIMAL_STACK_SIZE * 4)
#define INIT_TASK_CFG_QUEUE_LENGTH                 100

// file HelloWorldTask.c
// uncomment the following lines to change the task common parameters
//#define HW_TASK_CFG_STACK_DEPTH                    120
//#define HW_TASK_CFG_PRIORITY                       (tskIDLE_PRIORITY+1)

// file PushButtonTask.c
// uncomment the following lines to change the task common parameters
//#define PB_TASK_CFG_STACK_DEPTH                    120
//#define PB_TASK_CFG_PRIORITY                       (tskIDLE_PRIORITY+2)


#endif /* SYSCONFIG_H_ */
