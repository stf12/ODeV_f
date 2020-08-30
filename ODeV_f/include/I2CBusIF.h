/**
 ******************************************************************************
 * @file    I2CBusIF.h
 * @author  Stefano Oliveri
 * @version 1.0.0
 * @date    Aug 29, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 Stefano Oliveri</center></h2>
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
#ifndef I2CBUSIF_H_
#define I2CBUSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "systypes.h"
#include "syserror.h"
#include "FreeRTOS.h"
#include "semphr.h"


/**
 * Create a type name for _I2CBusIF.
 */
typedef struct _I2CBusIF I2CBusIF;

/**
 * Create a type name for the function to write in the I2C bus.
 *
 * @param pxSensor [IN] specifies a pointer to a device.
 * @param nRegAddr [IN] specifies a register address.
 * @param pnData [IN] specifies a buffer with the data to write.
 * @param nSize [IN] specifies the size in byte of the data to write.
 * @return 0 if success, an error code otherwise.
 */
typedef int32_t (*I2CBusWriteF)(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

/**
 * Create a type name for the function to read in the I2C bus.
 *
 * @param pxSensor [IN] specifies a pointer to a device.
 * @param nRegAddr [IN] specifies a register address.
 * @param pnData [OUT] specifies a buffer for the data to read.
 * @param nSize [IN] specifies the size in byte of the data to read.
 * @return 0 if success, an error code otherwise.
 */
typedef int32_t (*I2CBusReadF) (void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

/**
 * This struct is a clone of ST stmdev_ctx_t that is defined in each sensor header file.
 * But in the bus there can be connected other devices than the ST sensors.
 */
typedef struct _I2CBusConnector {
  /**
   * Function to write in the bus. It is a component mandatory fields.
   */
  I2CBusWriteF  pfWriteReg;

  /**
   * Function to read from the bus. It is a component mandatory fields.
   */
  I2CBusReadF   pfReadReg;

  /**
   * Customizable optional pointer.
   */
  void *pxHandle;
} I2CBusConnector;


/**
 * Specifies the I2C interface for a generic sensor.
 */
struct _I2CBusIF {
  /**
   * The bus connector encapsulates the function pointer to read and write in the bus,
   * and it is compatible with the the ST universal sensor driver.
   */
  I2CBusConnector m_xConnector;

  /**
   * Sensor ID.
   */
  uint8_t m_nWhoAmI;

  /**
   * Slave address.
   */
  uint8_t m_nAddress;

  /**
   * Synchronization object used to synchronize the sensor with the bus.
   */
  SemaphoreHandle_t m_xSyncObj;
};


// Public API declaration
// **********************

/**
 * Initialize a sensor object. It must be called once before using the sensor.
 *
 * @param _this [IN] specifies a sensor object.
 * @param nWhoAmI [IN] specifies the sensor ID. It can be zero.
 * @param nAddress [IN] specifies the I2C address of teh device.
 * @return SYS_NO_EROR_CODE if success, an error code otherwise.
 */
sys_error_code_t I2CBusIFInit(I2CBusIF *_this, uint8_t nWhoAmI, uint8_t nAddress);

sys_error_code_t I2CBusIFWaitIOComplete(I2CBusIF *_this);
sys_error_code_t I2CBusIFNotifyIOComplete(I2CBusIF *_this);

inline sys_error_code_t I2CBusIFSetHandle(I2CBusIF *_this, void *pxHandle);
inline void *I2CBusIFGetHandle(const I2CBusIF *_this);

inline sys_error_code_t I2CBusIFSetWhoAmI(I2CBusIF *_this, uint8_t nWhoAmI);
inline uint8_t I2CBusIFGetWhoAmI(const I2CBusIF *_this);

int32_t I2CBusNullRW(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

// Inline function definition
// **************************

SYS_DEFINE_INLINE
sys_error_code_t I2CBusIFSetHandle(I2CBusIF *_this, void *pxHandle) {
  assert_param(_this);

  _this->m_xConnector.pxHandle = pxHandle;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
void *I2CBusIFGetHandle(const I2CBusIF *_this) {
  assert_param(_this);

  return _this->m_xConnector.pxHandle;
}

SYS_DEFINE_INLINE
inline sys_error_code_t I2CBusIFSetWhoAmI(I2CBusIF *_this, uint8_t nWhoAmI) {
  assert_param(_this);

  _this->m_nWhoAmI = nWhoAmI;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
inline uint8_t I2CBusIFGetWhoAmI(const I2CBusIF *_this) {
  assert_param(_this);

  return _this->m_nWhoAmI;
}

#ifdef __cplusplus
}
#endif

#endif /* I2CBUSIF_H_ */
