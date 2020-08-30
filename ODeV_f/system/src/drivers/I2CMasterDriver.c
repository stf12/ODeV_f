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

  /**
   * Count the number of errors reported by the hardware IP.
   */
  uint16_t *m_pnIPErrors;
}I2CPeripheralResources;

/**
 *
 */
static I2CPeripheralResources s_pxHwResouces[I2CDRV_CFG_HARDWARE_PERIPHERALS_COUNT] = {
  {NULL}
};


// Private member function declaration
// ***********************************

static void I2CMasterDrvMemRxCpltCallback(I2C_HandleTypeDef *xI2C);
static void I2CMasterDrvMemTxCpltCallback(I2C_HandleTypeDef *xI2C);
static void I2CMasterDrvErrorCallback(I2C_HandleTypeDef *xI2C);


// Public API definition
// *********************

sys_error_code_t I2CMasterDriverSetDeviceAddr(I2CMasterDriver *_this, uint16_t nAddress) {
  assert_param(_this);

  _this->m_nTargetDeviceAddr = nAddress;

  return SYS_NO_ERROR_CODE;
}


// IIODriver virtual function definition
// *************************************

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

  // Register SPI DMA complete Callback
  if (HAL_OK != HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MEM_RX_COMPLETE_CB_ID, I2CMasterDrvMemRxCpltCallback)) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    xRes = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MEM_TX_COMPLETE_CB_ID, I2CMasterDrvMemTxCpltCallback)) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    xRes = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_ERROR_CB_ID, I2CMasterDrvErrorCallback)) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    xRes = SYS_UNDEFINED_ERROR_CODE;
  }
  else {
    // initialize the software resources
    pObj->m_nTargetDeviceAddr = 0;
    pObj->m_nIPErrors = 0;
    pObj->m_xSyncObj = xSemaphoreCreateBinary();
    if (pObj->m_xSyncObj == NULL){
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
      xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    }

    s_pxHwResouces[0].m_xSyncObj = pObj->m_xSyncObj;
    s_pxHwResouces[0].m_pnIPErrors = &pObj->m_nIPErrors;
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

//  /* I2C2 interrupt Init */
//  HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
//  HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
//
//  /* DMA1_Channel3_IRQn interrupt configuration */
//  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
//  /* DMA1_Channel4_IRQn interrupt configuration */
//  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblStop(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

//  /* I2C2 interrupt Init */
//  HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
//  HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
//
//  /* DMA1_Channel3_IRQn interrupt configuration */
//  HAL_NVIC_DisableIRQ(DMA1_Channel3_IRQn);
//  /* DMA1_Channel4_IRQn interrupt configuration */
//  HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);


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
  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  if (!SYS_IS_ERROR_CODE(xRes)) {
    while (HAL_I2C_Mem_Write_DMA(&hi2c2, pObj->m_nTargetDeviceAddr, nChannel, I2C_MEMADD_SIZE_8BIT, pDataBuffer, nDataSize) != HAL_OK) {
      if (HAL_I2C_GetError(&hi2c2) != HAL_BUSY) {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I2C_M_WRITE_ERROR_CODE);
        sys_error_handler();
      }
    }
    // Suspend the calling task until the operation is completed.
    xSemaphoreTake(pObj->m_xSyncObj, portMAX_DELAY);
  }

  return xRes;
}

sys_error_code_t I2CMasterDriver_vtblRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  I2CMasterDriver *pObj = (I2CMasterDriver*)_this;

  if (!SYS_IS_ERROR_CODE(xRes)) {
    while (HAL_I2C_Mem_Read_DMA(&hi2c2, pObj->m_nTargetDeviceAddr, nChannel, I2C_MEMADD_SIZE_8BIT, pDataBuffer, nDataSize) != HAL_OK) {
      if (HAL_I2C_GetError(&hi2c2) != HAL_BUSY) {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_I2C_M_WRITE_ERROR_CODE);
        sys_error_handler();
      }
    }
    // Suspend the calling task until the operation is completed.
    xSemaphoreTake(pObj->m_xSyncObj, portMAX_DELAY);
  }

  return xRes;
}


// Private function definition
// ***************************


// CubeMX integration
// ******************

static void I2CMasterDrvMemRxCpltCallback(I2C_HandleTypeDef *xI2C) {
  UNUSED(xI2C);

  if (s_pxHwResouces[0].m_xSyncObj) {
    xSemaphoreGiveFromISR(s_pxHwResouces[0].m_xSyncObj, NULL);
  }
}

static void I2CMasterDrvMemTxCpltCallback(I2C_HandleTypeDef *xI2C) {
  UNUSED(xI2C);

  if (s_pxHwResouces[0].m_xSyncObj) {
    xSemaphoreGiveFromISR(s_pxHwResouces[0].m_xSyncObj, NULL);
  }
}

static void I2CMasterDrvErrorCallback(I2C_HandleTypeDef *xI2C) {
  UNUSED(xI2C);

  *(s_pxHwResouces[0].m_pnIPErrors) += 1;
}
