/**
 ******************************************************************************
 * @file    KeyMatrixDriver.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Dec 13, 2016
 *
 * @brief   This driver implements the keys matrix scanner algorithm.
 *
 * This driver implements the keys matrix scanner algorithm. The keys matrix
 * is a matrix of [8 x KM_DRV_CFG_COLUMNS_NUMBER] bit.
 * ![Keys matrix](key_scanner_timing.png "Key matrix Title")
 * Each bit is 1 when the corresponding key is pressed, and 0 otherwise. The
 * driver scans the keys matrix and when it detects a change, it notifies a
 * registered listener (IKeyMatrixDrvListener).
 * Moreover this driver implements also a debounce filter that, when it is active,
 * it oversamples the keys matrix in order to filter spurious value changes
 * due to the mechanical key.
 * This driver subclass the generic IDriver interface.
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

#ifndef INCLUDE_DRIVERS_KEYMATRIXDRIVER_H_
#define INCLUDE_DRIVERS_KEYMATRIXDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "IDriver.h"
#include "IDriverVtbl.h"
#include "IKeyMatrixDriverListener.h"

/**
 * Specifies the number of column of the keys matrix.
 */
#define KMDRV_CFG_COLUMNS_NUMBER              14

#ifndef KMDRV_CFG_DEBOUNCE_FILTER
/**
 * Initial configuration value for the debounce filter.
 * \sa KeyMatrixDriver::m_nDebounceCount
 */
#define KMDRV_CFG_DEBOUNCE_FILTER             2
#endif

#ifndef KMDRV_CFG_DEBOUNCE_FILTER_WITH_EFT
/**
 * Configuration vale to be used in case of IFT noise.
 */
#define KMDRV_CFG_DEBOUNCE_FILTER_WITH_EFT    8
#endif

/**
 * Create  type name for _KeyMatrixDriver.
 */
typedef struct _KeyMatrixDriver KeyMatrixDriver;

/**
 * Specifies the internal state of the Keys Scanner Driver.
 */
struct _KeyMatrixDriver {
	/**
	 * Base class object.
	 */
	IDriver super;

	/**
	 * Handle to the HAL resource.
	 */
	TIM_HandleTypeDef m_xTim;

	/**
	 * Specifies the internal representation of the keys matrix.
   * Each byte specifies the data read from KEY_IN[0..7] PINs for each column of the key matrix:
   * m_pnKeyMatrixDataBuff1[i] = KEY_IN[0..7] when KEY_OUT(i) == 1
   */
  uint8_t m_pnKeyMatrixDataBuff1[KMDRV_CFG_COLUMNS_NUMBER];

  /**
   * Specifies the internal representation of the keys matrix.
   * Each byte specifies the data read from KEY_IN[0..7] PINs for each column of the key matrix:
   * m_pnKeyMatrixDataBuff2[i] = KEY_IN[0..7] when KEY_OUT(i) == 1
   */
  uint8_t m_pnKeyMatrixDataBuff2[KMDRV_CFG_COLUMNS_NUMBER];

  /**
   * Specifies the internal representation of the keys matrix.
   * Each byte specifies the data read from KEY_IN[0..7] PINs for each column of the key matrix:
   * m_pnKeyMatrixDataBuff2[i] = KEY_IN[0..7] when KEY_OUT(i) == 1
   */
  uint8_t m_pnKeyMatrixDataBuff3[KMDRV_CFG_COLUMNS_NUMBER];

  /**
   * Specifies a pointer to the active buffer. It is the buffer that store the last stable keys matrix data.
   * The size of the array is specified by the constant KM_DRV_CFG_COLUMNS_NUMBER.
	 */
  uint8_t *m_pnKeyMatrixData;

	/**
	 * Listener object. It is notified when the keys matrix change.
	 */
	IListener *m_pListener;

	/**
	 * Used by the debounce algorithm. It specifies the number of samples of the keys matrix
	 * value used to validate the data. The initial value is specified by the configuration
	 * constant KMDRV_CFG_DEBOUNCE_FILTER. It can be modified by the function
   * ::KeyMatrixDrvSetDebounceFilter.
	 */
  uint8_t m_nDebounceFilter;
};


// Public API declaration
//***********************

/**
 * Instantiate a class object. Before using the object it has to be initialized by calling IDrvInit function.
 *
 * @return a pointer to a class object.
 */
IDriver *KeyMatrixDrvAlloc();

/**
 * Set the listener object. The lister is notified when the keys matrix vale changes.
 * To remove a listener use NULL as second parameter.
 *
 * @param _this [IN] specifies a pointer to a KeyMatrixDriver object.
 * @param pListener [IN] specifies a pointer to a IKeyMatrixDrvListener object.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t KeyMatrixDrvSetListener(IDriver *_this, IListener *pListener);

/**
 * Set the debounce count. It specifies how many time the keys matrix is read to validate a value change.
 * When nDebounceCount is equal to 0 the debounce algorithm is disabled.
 *
 * @param _this [IN] specifies a pointer to a KeyMatrixDriver object.
 * @param nDebounceCount [IN] specifies new value for the debounce count.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t KeyMatrixDrvSetDebounceFilter(IDriver *_this, uint8_t nDebounceCount);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_DRIVERS_KEYMATRIXDRIVER_H_ */
