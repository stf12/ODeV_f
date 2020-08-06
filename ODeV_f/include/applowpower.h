/**
 ******************************************************************************
 * @file    applowpower.h
 * @author  STF12
 * @version 1.0.0
 * @date    Aug 10, 2020
 *
 * @brief   Declare the Power Mode State Machine events
 *
 * This file declares the events that define the transactions in the Power Mode
 * State Machine.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 STF12 - Stefano Oliveri</center></h2>
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
#ifndef APPLOWPOWER_H_
#define APPLOWPOWER_H_

#ifdef __cplusplus
extern "C" {
#endif


#define SYS_PM_EVT_SRC_PB                   0x1U  ///< Event generated from the PushButton.
// PB parameters
#define SYS_PM_EVT_PARAM_PRESSED            0x1U  ///< Event parameter.
// Generic event parameters
#define SYS_PM_EVT_PARAM_ENTER_LP           0x1U  ///< Event parameter: enter low power mode command.
#define SYS_PM_EVT_PARAM_EXIT_LP            0x2U  ///< Event parameter: exit low power mode command.


/**
 * Specifies the possible power states of the system.
 */
typedef enum {
  /**
   * The system is active waiting for input.
   */
  E_POWER_MODE_RUN = 0,

  /**
   * The system is active and the data logger is started.
   */
  E_POWER_MODE_DATALOG = 1,

  /**
   * The system is active and the AI algorithm is started.
   */
  E_POWER_MODE_AI = 2,

  /**
   * The system is running and both data logging and AI algorithm are started.
   */
  E_POWER_MODE_DATALOG_AI = 3,

  /**
   * Specifies the lowest power mode for the system: the MCU is in STOP.
   */
  E_POWER_MODE_SLEEP_1 = 4,

}EPowerMode;

/**
 * Specifies some information about the current power mode of the system.
 */
typedef struct _SysPowerStatus {

  /**
   * Specifies the current power mode of the system. An application task can read this value.
   */
  EPowerMode m_eActivePowerMode;
}SysPowerStatus;


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* APPLOWPOWER_H_ */
