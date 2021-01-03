/**
 ******************************************************************************
 * @file    SPIBusTask.h
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
#ifndef SPIBUSTASK_H_
#define SPIBUSTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "IIODriver.h"
#include "IIODriver_vtbl.h"
#include "SPIBusIF.h"
#include "IBus.h"
#include "IBus_vtbl.h"
#include "queue.h"


/**
 * Create  type name for _SPIBusTask.
 */
typedef struct _SPIBusTask SPIBusTask;

/**
 *  SPIBusTask internal structure.
 */
struct _SPIBusTask {
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
   * Bus interface used to connect and disconnect devices to this object.
   */
  IBus *m_pBusIF;

  /**
   * Task message queue. Read and write request are wrapped into message posted in this queue.
   */
  QueueHandle_t m_xInQueue;

  /**
   * Count the number of devices connected to the bus. It can be used in further version to
   * de-initialize the SPI IP in some of the PM state.
   */
  uint8_t m_nConnectedDevices;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of SPIBusTask.
 *
 * @return a pointer to the generic object ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *SPIBusTaskAlloc();

/**
 * Connect a device to the bus using its interface.
 *
 * @param _this [IN] specifies a task object.
 * @param pxBusIF [IN] specifies the device interface to connect.
 * @return SYS_NO_ERROR_CODE is success, SYS_INVALID_PARAMETER_ERROR_CODE if pxBuff is NULL.
 */
sys_error_code_t SPIBusTaskConnectDevice(SPIBusTask *_this, SPIBusIF *pxBusIF);

/**
 * Disconnect a device from the bus using its interface.
 *
 * @param _this [IN] specifies a task object.
 * @param pxBusIF [IN] specifies the device interface to connect.
 * @return SYS_NO_ERROR_CODE is success, SYS_INVALID_PARAMETER_ERROR_CODE if pxBuff is NULL.
 */
sys_error_code_t SPIBusTaskDisconnectDevice(SPIBusTask *_this, SPIBusIF *pxBusIF);

/**
 * Get the ::IBus interface of teh task.
 *
 * @param _this [IN] specifies a task object.
 * @return the ::IBus interface of the task.
 */
IBus *SPIBusTaskGetBusIF(SPIBusTask *_this);



// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SPIBUSTASK_H_ */
