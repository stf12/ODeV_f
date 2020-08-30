/**
 ******************************************************************************
 * @file    I2CBusTask.h
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
 *
 ******************************************************************************
 */
#ifndef I2CBUSTASK_H_
#define I2CBUSTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "IIODriver.h"
#include "IIODriver_vtbl.h"
#include "I2CBusIF.h"
#include "queue.h"


/**
 * Create  type name for _I2CBusTask.
 */
typedef struct _I2CBusTask I2CBusTask;

/**
 *  I2CBusTask internal structure.
 */
struct _I2CBusTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * Driver object.
   */
  IIODriver *m_pxDriver;

  /**
   * Task message queue. Read and write request are wrapped into message posted in this queue.
   */
  QueueHandle_t m_xInQueue;

  /**
   * Count the number of devices connected to the bus. It can be used in furter version to
   * de-initialize the SPI IP in some of the PM state.
   */
  uint8_t m_nConnectedDevices;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of I2CBusTask.
 *
 * @return a pointer to the generic obejct ::AManagedTask if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *I2CBusTaskAlloc();

/**
 * Connect a device to the bus using its interface.
 *
 * @param _this [IN] specifies a task object.
 * @param pxBusIF [IN] specifies the device interface to connect.
 * @return SYS_NO_ERROR_CODE is success, SYS_INVALID_PARAMETER_ERROR_CODE if pxBuff is NULL.
 */
sys_error_code_t I2CBusTaskConnectDevice(I2CBusTask *_this, I2CBusIF *pxBusIF);

/**
 * Disconnect a device from the bus using its interface.
 *
 * @param _this [IN] specifies a task object.
 * @param pxBusIF [IN] specifies the device interface to connect.
 * @return SYS_NO_ERROR_CODE is success, SYS_INVALID_PARAMETER_ERROR_CODE if pxBuff is NULL.
 */
sys_error_code_t I2CBusTaskDisconnectDevice(I2CBusTask *_this, I2CBusIF *pxBusIF);


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* I2CBUSTASK_H_ */
