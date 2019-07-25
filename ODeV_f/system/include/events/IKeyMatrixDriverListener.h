/**
 ******************************************************************************
 * @file    IKeyMatrixDriverListener.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 6, 2017
 *
 * @brief Keys matrix driver listener interface.
 *
 * An object implements this interface in order to receive notification
 * when the keys matrix changes.
 * This interface subclass the generic IListener interface.
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

#ifndef INCLUDE_EVENTS_IKEYMATRIXDRIVERLISTENER_H_

#define INCLUDE_EVENTS_IKEYMATRIXDRIVERLISTENER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "IListener.h"
#include "IListenerVtbl.h"

/**
 * Create  type name for _IKeyMatrixDrvListener.
 */
typedef struct _IKeyMatrixDrvListener IKeyMatrixDrvListener;


// Public API declaration
//***********************

/**
 * This method is called when the keys matrix change.
 *
 * @param _this [IN] specifies a pointer to a IKeyMatrixDrvListener object.
 * @param pnKeyMatrixData [IN] specifies a pointer to the keys matrix.
 * @param nColumns [IN] specifies the number of column of the keys matrix.
 * @param nModifiedColumnBitMask [IN] specifies the column in the keys matrix with modified value:
 *        the bit nModifiedColumnBitMask[i] == 1 if pnKeyMatrixData[i] has a modified value.
 * @return not used.
 */
inline sys_error_code_t IKeyMatrixDrvListenerOnKeyMatrixChange(IListener *_this, uint8_t *pnKeyMatrixData, uint8_t nColumns, uint16_t nModifiedColumnBitMask);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_EVENTS_IKEYMATRIXDRIVERLISTENER_H_ */
