/**
 ******************************************************************************
 * @file    IIODriver.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Aug 6, 2019
 *
 * @brief   I/O Driver interface.
 *
 * I/O driver interface extends the basic ::IDriver interface with read and
 * write operation.
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
#ifndef INCLUDE_DRIVERS_IIODRIVER_H_
#define INCLUDE_DRIVERS_IIODRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IDriver.h"
//#include "IDriver_vtbl.h"


/**
 * Create  type name for _IIODriver.
 */
typedef struct _IIODriver IIODriver;


// Public API declaration
//***********************

/**
 *
 * @param _this [IN] specifies a pointer to a IIODriver object.
 * @param pDataBuffer [IN] specifies the buffer with the data to be written by the driver.
 * @param nDataSize [IN] specified the size in byte of the buffer.
 * @param nChannel [IN] specifies the channel where to write the data.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IIODrvWrite(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);

/**
 *
 * @param _this [IN] specifies a pointer to a IIODriver object.
 * @param pDataBuffer [OUT] specifies the buffer used to store the received data.
 * @param nDataSize [IN] specified the size in byte of the buffer.
 * @param nChannel [IN] specifies the channel from where to read the data.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
inline sys_error_code_t IIODrvRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_IIODRIVER_H_ */
