/**
 ******************************************************************************
 * @file    KeyScanner.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.3.0
 * @date    Dec 15, 2016
 *
 * @brief   Keys Scanner API declaration.
 *
 * The keys scanner analyze the keys in order to generate high level events
 * (KeyEvent).
 * It uses a KeyMatrixDriver and it registers itself as IKeyMatrixDrvListener.
 * Moreover this service implements the ghost detection algorithm. A task can
 * use the ::KeyScannerWaitForKeyevent method to receive KeyEvent.
 *
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

#ifndef INCLUDE_SERVICES_KEYSCANNER_H_
#define INCLUDE_SERVICES_KEYSCANNER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "KeyMatrixDriver.h"
#include "IKeyMatrixDriverListenerVtbl.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define KS_IS_SPECIAL_KEY(keyEvent) (((*(uint32_t*)(&keyEvent))) & 0x000000FF)

/**
 * Creates a type name for _KeyboardTask.
 */
typedef struct _KeyScanner KeyScanner;

/**
 * A key event specifies a single key pressed or released plus the status of the FN function key.
 */
typedef struct _KeyEvent {
  uint8_t nKeyCode;         ///< Specifies a key code. \sa USBKeyboardKeyCodesMap.h
  uint8_t bKeyPressed : 1;  ///< It is 1 for a key pressed event, and 0 for a key released event.
  uint8_t             : 1;
  uint8_t             : 1;
  uint8_t             : 1;
  uint8_t             : 1;
  uint8_t             : 1;
  uint8_t             : 1;
  uint8_t             : 1;
} KeyEvent;

/**
 * Specifies the internal state of a KeyScanner object.
 */
struct _KeyScanner {
  /**
   * Base class object.
   */
  IKeyMatrixDrvListener super;

  /**
   * Low level driver of type KeyMatrixDrv.
   */
  IDriver *m_pxDriver;

  /**
   * Each byte specifies the data read from PA[0..7] PINs for each column of the key matrix:
   * m_pnKeyMatrixData[i] = PA[0..7] when PB(i) == 1
   */
  uint8_t m_pnKeyMatrixData[KMDRV_CFG_COLUMNS_NUMBER];

  /**
   * Specifies if the anti ghost keys algorithm is enabled or not.
   *
   */
  boolean_t bIsAntiGhostEnabled;

  /**
   * Specifies the queue used to synchronize the task and the KeyScanenr service.
   */
  QueueHandle_t m_xKeyEventQueue;
};


// Public API declaration
//***********************

/**
 * Initialize a KeyScanner object. A KeyScanenr uses a KeyMatrixDriver.
 * It is responsibility of the application to instantiate the driver object.
 *
 * @param _this [IN] specifies a KeyScanenr object.
 * @param pDriver [IN] specifies a KeyMatrixDriver object.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t KeyScannerInit(KeyScanner *_this, IDriver *pDriver);

/**
 * Block the calling task waiting for a KeyEvent. The timeout is used to
 * specify the maximum time to wait before the method return.
 *
 * @param _this [IN] specifies a KeyScanenr object.
 * @param pxKeyEvent [OUT] specifies a KeyEvent object
 * @param nTimeoutMs [IN] specifies the timeout in milliseconds. If it is portMAX_DELAY
 *        the method can block indefinitely.
 * @return SYS_NO_ERROR_CODE if a new KeyEvent is received, SYS_KS_TIMEOUT_ERROR_CODE if the timeout expires.
 */
sys_error_code_t KeyScannerWaitForKeyevent(KeyScanner *_this, KeyEvent *pxKeyEvent, uint32_t nTimeoutMs);

/**
 * Add a key event in the task queue.
 *
 * @param _this [IN] specifies a KeyScanenr object.
 * @param pxKeyEvent [IN] specifies a KeyEvent object
 * @return SYS_NO_ERROR_CODE if success, SYS_KS_TIMEOUT_ERROR_CODE otherwise.
 */
sys_error_code_t KeyScannerForceKeyEvent(KeyScanner *_this, KeyEvent *pxKeyEvent);

/**
 * Reset the internal state of the service object without detaching the driver.
 *
 * @param _this [IN] specifies a KeyScanenr object.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t KeyScannerReset(KeyScanner *_this);

sys_error_code_t KeyScannerResetQueue(KeyScanner *_this);

// Inline functions definition
// ***************************

/**
 * Enable or disable the ghost detection algorithm.
 *
 * @param _this [IN] specifies a KeyScanenr object.
 * @param bEnable [IN] TRUE to enable the ghost detection or FALSE to disable it.
 */
SYS_DEFINE_INLINE
void KeyScannerEnableAntiGhost(KeyScanner *_this, boolean_t bEnable) {
  assert_param(_this);

  _this->bIsAntiGhostEnabled = bEnable;
}

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICES_KEYSCANNER_H_ */
