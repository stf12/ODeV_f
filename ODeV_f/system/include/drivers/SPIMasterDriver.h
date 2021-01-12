/**
 ******************************************************************************
 * @file    SPIMasterDriver.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 3, 2020
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
#ifndef INCLUDE_DRIVERS_SPIMASTERDRIVER_H_
#define INCLUDE_DRIVERS_SPIMASTERDRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "IIODriver.h"
#include "IDriver_vtbl.h"
#include "IIODriver_vtbl.h"
#include "FreeRTOS.h"
#include "semphr.h"



/**
 * Create  type name for _SPIMasterDriver.
 */
typedef struct _SPIMasterDriver SPIMasterDriver;

/**
 *  SPIMasterDriver internal structure.
 */
struct _SPIMasterDriver {
  /**
   * Base class object.
   */
  IIODriver super;

  // Driver variables should be added here.

  /**
   * Synchronization object used to synchronize the low lever driver with the task.
   */
  SemaphoreHandle_t m_xSyncObj;
};

/**
 * Initialization parameters for the driver.
 */
typedef struct _SPIMasterDriverParams {
  uint32_t nParam;
}SPIMasterDriverParams;


// Public API declaration
//***********************

/**
 * Allocate an instance of SPIMasterDriver. The driver is allocated
 * in the FreeRTOS heap.
 *
 * @return a pointer to the generic interface ::IDriver if success,
 * or SYS_OUT_OF_MEMORY_ERROR_CODE otherwise.
 */
IIODriver *SPIMasterDriverAlloc();

/**
 * Make a bidirectional communication in teh SPI bus.
 *
 * @param _this [IN] specifies a pointer to a ::SPIMasterDriver object.
 * @param pnTxDataBuffer [OUT] specifies the buffer with the data to send.
 * @param pnRxDataBuffer [IN] specifies the buffer used to store the received data.
 * @param nDataSize [IN] specified the size in byte of the buffer.
 * @return SYS_NO_ERROR_CODE if success, an error code otherwise.
 */
sys_error_code_t SPIMasterDriverWriteRead(SPIMasterDriver *_this, uint8_t *pnTxDataBuffer, uint8_t *pnRxDataBuffer, uint16_t nDataSize);

/**
 * Send one byte register address in the SPI bus in blocking mode. This method is used
 * to implement the communication protocol adopted by the ST MEMS standard driver.
 *
 * @param _this [IN] specifies a pointer to a SPIMasterDriver object.
 * @param nRegAddr [IN] specifies a register address in the target device.
 * @param nTimeoutMS [IN] specifies a timeout in ms. If the IO operation doesn't end before teh timeout expire, then an error is generated.
 * @return SYS_NO_ERROR_CODE if success, an eror code otherwise.
 */
sys_error_code_t SPIMasterDriverTransmitRegAddr(SPIMasterDriver *_this, uint8_t nRegAddr, uint32_t nTimeoutMS);

/**
 * Select a device on the bus for the communications.
 *
 * @param _this [IN] specifies a pointer to a SPIMasterDriver object.
 * @param xDeviceGPIOPort [IN] specifies the GPIO Port of the device to select for the communications.
 * @param nDeviceGPIOPin [IN] specifies the Port Pin of the device to select for the communications.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t SPIMasterDriverSelectDevice(SPIMasterDriver *_this, GPIO_TypeDef* xDeviceGPIOPort, uint16_t nDeviceGPIOPin);

/**
 *
 * @param _this [IN] specifies a pointer to a SPIMasterDriver object.
 * @param xDeviceGPIOPort [IN] specifies the GPIO Port of the device to deselect for the communications.
 * @param nDeviceGPIOPin [IN] specifies the Port Pin of the device to deselect for the communications.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t SPIMasterDriverDeselectDevice(SPIMasterDriver *_this, GPIO_TypeDef* xDeviceGPIOPort, uint16_t nDeviceGPIOPin);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* INCLUDE_DRIVERS_SPIMASTERDRIVER_H_ */
