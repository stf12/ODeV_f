/**
 ******************************************************************************
 * @file    KeyScannerVtbl.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 6, 2017
 * @brief
 *
 * TODO - insert here the file description
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

#ifndef INCLUDE_SERVICES_KEYSCANNERVTBL_H_
#define INCLUDE_SERVICES_KEYSCANNERVTBL_H_

#ifdef __cplusplus
extern "C" {
#endif

sys_error_code_t KeyScannerOnStatusChange(IListener *this);

/**
 * @sa IKeyMatrixDrvListenerOnKeyMatrixChange
 */
sys_error_code_t KeyScannerOnKeyMatrixChange(IListener *this, uint8_t *pnKeyMatrixData, uint8_t nColumns, uint16_t nModifiedColumnBitMask);


// Public API declaration
//***********************



// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICES_KEYSCANNERVTBL_H_ */
