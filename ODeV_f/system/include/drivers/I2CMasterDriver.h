/**
 ******************************************************************************
 * @file    I2CMasterDriver.h
 * @author  Stefano Oliveri
 * @version 1.0.0
 * @date    Aug 26, 2020
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
#ifndef INCLUDE_DRIVERS_I2CMASTERDRIVER_H_
#define INCLUDE_DRIVERS_I2CMASTERDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IIODriver.h"
#include "IDriver_vtbl.h"
#include "IIODriver_vtbl.h"
#include "FreeRTOS.h"
#include "semphr.h"


/**
 * Create  type name for _I2CMasterDriver.
 */
typedef struct _I2CMasterDriver I2CMasterDriver;

/**
 *  I2CMasterDriver internal structure.
 */
struct _I2CMasterDriver {
  /**
   * Base class object.
   */
  IIODriver super;

  // Driver variables should be added here.

  /**
   * Synchronization object used to synchronize the low lever driver with the task.
   */
  SemaphoreHandle_t m_xSyncObj;

  /**
   * Specifies the address of the target device. All read and write operation are done with the device specified by this address.
   */
  uint16_t m_nTargetDeviceAddr;

  /**
   * Count the number of errors reported by the hardware IP.
   */
  uint16_t m_nIPErrors;
};

/**
 * Initialization parameters for the driver.
 */
typedef struct _I2CMasterDriverParams {
  uint32_t nParam;
}I2CMasterDriverParams;


// Public API declaration
//***********************

/**
 * Allocate an instance of I2CMasterDriver. The driver is allocated
 * in the FreeRTOS heap.
 *
 * @return a pointer to the generic interface ::IDriver if success,
 * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
 */
IIODriver *I2CMasterDriverAlloc();

/**
 * Set the address of a slave device in the I2C bus. All read and write operation are done with
 * the device specified by this address.
 *
 * @param _this [IN] specifies a pointer to a SPIMasterDriver object.
 * @param nAddress [IN] specifies address of the slave device in the I2C bus.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t I2CMasterDriverSetDeviceAddr(I2CMasterDriver *_this, uint16_t nAddress);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_I2CMASTERDRIVER_H_ */
