/**
 ******************************************************************************
 * @file    I2CMasterDriver.c
 * @author  STMicroelectronics - AIS - MCD Team
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

#include "I2CMasterDriver.h"
#include "I2CMasterDriver_vtbl.h"
#include "i2c.h"
#include "dma.h"
#include "FreeRTOS.h"
#include "sysdebug.h"

#define I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT   1

#define SYS_DEBUGF(level, message)              SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)


/**
 * I2CMasterDriver Driver virtual table.
 */
static const IIODriver_vtbl s_xI2CMasterDriver_vtbl = {
    I2CMasterDriver_vtblInit,
    I2CMasterDriver_vtblStart,
    I2CMasterDriver_vtblStop,
    I2CMasterDriver_vtblDoEnterPowerMode,
    SPIMasterDriver_vtblReset,
    I2CMasterDriver_vtblWrite,
    I2CMasterDriver_vtblRead
};

/**
 * Data associated to the hardware peripheral.
 */
typedef struct _SPIPeripheralResources {
  /**
   * Synchronization object used by the driver to synchronize the I2C ISR with the task using the driver;
   */
  SemaphoreHandle_t m_xSyncObj;
}I2CPeripheralResources;

/**
 *
 */
static I2CPeripheralResources s_pxHwResouces[I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT] = {
  {NULL}
};


// Private member function declaration
// ***********************************


// Public API definition
// *********************

IIODriver *I2CMasterDriverAlloc() {
  IIODriver *pNewObj = (IIODriver*)pvPortMalloc(sizeof(I2CMasterDriver));

  if (pNewObj == NULL) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("I2CMasterDriver - alloc failed.\r\n"));
  }
  else {
    pNewObj->vptr = &s_xI2CMasterDriver_vtbl;
  }

  return pNewObj;
}

sys_error_code_t I2CMasterDriver_vtblInit(IDriver *_this, void *pParams) {
  assert_param(_this);
  assert_param(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  MX_DMA_Init();
  MX_I2C2_Init();

//  else {
  if(1) {
    // initialize the software resources
    pObj->m_xSyncObj = xSemaphoreCreateBinary();
    if (pObj->m_xSyncObj == NULL){
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
      xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    }

    s_pxHwResouces[0].m_xSyncObj = pObj->m_xSyncObj;
  }

#ifdef DEBUG
  if (pObj->m_xSyncObj) {
    vQueueAddToRegistry(pObj->m_xSyncObj, "I2C2Drv");
  }
#endif

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("I2CMasterDriver: initialization done.\r\n"));

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblStart(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblStop(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblWrite(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  return xRes;
}


// Private function definition
// ***************************
