/**
 ******************************************************************************
 * @file    SPIBusIF.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 6, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
#ifndef SPIBUSIF_H_
#define SPIBUSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "systypes.h"
#include "syserror.h"
#include "FreeRTOS.h"
#include "semphr.h"


/**
 * Create a type name for _SPIBusIF.
 */
typedef struct _SPIBusIF SPIBusIF;

/**
 * Create a type name for the function to write in the SPI bus.
 *
 * @param pxSensor [IN] specifies a pointer to a device.
 * @param nRegAddr [IN] specifies a register address.
 * @param pnData [IN] specifies a buffer with the data to write.
 * @param nSize [IN] specifies the size in byte of the data to write.
 * @return 0 if success, an error code otherwise.
 */
typedef int32_t (*SPIBusWriteF)(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

/**
 * Create a type name for the function to read in the SPI bus.
 *
 * @param pxSensor [IN] specifies a pointer to a device.
 * @param nRegAddr [IN] specifies a register address.
 * @param pnData [OUT] specifies a buffer for the data to read.
 * @param nSize [IN] specifies the size in byte of the data to read.
 * @return 0 if success, an error code otherwise.
 */
typedef int32_t (*SPIBusReadF) (void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

/**
 * This struct is a clone of ST stmdev_ctx_t that is defined in each sensor header file.
 * But in the bus there can be connected other devices than the ST sensors.
 */
typedef struct _SPIBusConnector {
  /**
   * Function to write in the bus. It is a component mandatory fields.
   */
  SPIBusWriteF  pfWriteReg;

  /**
   * Function to read from the bus. It is a component mandatory fields.
   */
  SPIBusReadF   pfReadReg;

  /**
   * Customizable optional pointer.
   */
  void *pxHandle;
} SPIBusConnector;


/**
 * Specifies the SPI interface for a generic sensor.
 */
struct _SPIBusIF {
  /**
   * The bus connector encapsulates the function pointer to read and write in the bus,
   * and it is compatible with the the ST universal sensor driver.
   */
  SPIBusConnector m_xConnector;

  /**
   * Sensor ID.
   */
  uint8_t m_nWhoAmI;

  /**
   * Slave select GPIO Port
   */
  GPIO_TypeDef* m_pxSSPinPort;

  /**
   * Slave select GPIO Pin.
   */
  uint16_t m_nSSPin;

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
 * @param pxSSPinPort [IN] specifies the port PIN of the Slave Select line.
 * @param nSSPin [IN] specifies the pin number of the Slave Select line.
 * @return SYS_NO_EROR_CODE if success, an error code otherwise.
 */
sys_error_code_t SPIBusIFInit(SPIBusIF *_this, uint8_t nWhoAmI, GPIO_TypeDef* pxSSPinPort, uint16_t nSSPin);

sys_error_code_t SPIBusIFWaitIOComplete(SPIBusIF *_this);
sys_error_code_t SPIBusIFNotifyIOComplete(SPIBusIF *_this);

inline sys_error_code_t SPIBusIFSetHandle(SPIBusIF *_this, void *pxHandle);
inline void *SPIBusIFGetHandle(const SPIBusIF *_this);

inline sys_error_code_t SPIBusIFSetWhoAmI(SPIBusIF *_this, uint8_t nWhoAmI);
inline uint8_t SPIBusIFGetWhoAmI(const SPIBusIF *_this);

int32_t SPIBusNullRW(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

// Inline function definition
// **************************

SYS_DEFINE_INLINE
sys_error_code_t SPIBusIFSetHandle(SPIBusIF *_this, void *pxHandle) {
  assert_param(_this);

  _this->m_xConnector.pxHandle = pxHandle;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
void *SPIBusIFGetHandle(const SPIBusIF *_this) {
  assert_param(_this);

  return _this->m_xConnector.pxHandle;
}

SYS_DEFINE_INLINE
inline sys_error_code_t SPIBusIFSetWhoAmI(SPIBusIF *_this, uint8_t nWhoAmI) {
  assert_param(_this);

  _this->m_nWhoAmI = nWhoAmI;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
inline uint8_t SPIBusIFGetWhoAmI(const SPIBusIF *_this) {
  assert_param(_this);

  return _this->m_nWhoAmI;
}

#ifdef __cplusplus
}
#endif

#endif /* SPIBUSIF_H_ */
