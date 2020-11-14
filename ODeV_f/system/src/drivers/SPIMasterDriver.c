/**
 ******************************************************************************
 * @file    SPIMasterDriver.c
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

#include "SPIMasterDriver.h"
#include "SPIMasterDriver_vtbl.h"
#include "dma.h"
#include "spi.h"
#include "sysdebug.h"

#define SYS_DEBUGF(level, message)              SYS_DEBUGF3(SYS_DBG_DRIVERS, level, message)

#define SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT   1


/**
 * SPIMasterDriver Driver virtual table.
 */
static const IIODriver_vtbl s_xSPIMasterDriver_vtbl = {
    SPIMasterDriver_vtblInit,
    SPIMasterDriver_vtblStart,
    SPIMasterDriver_vtblStop,
    SPIMasterDriver_vtblDoEnterPowerMode,
    SPIMasterDriver_vtblReset,
    SPIMasterDriver_vtblWrite,
    SPIMasterDriver_vtblRead
};

/**
 * Data associated to the hardware peripheral.
 */
typedef struct _SPIPeripheralResources {
  /**
   * Synchronization object used by the driver to synchronize the I2C ISR with the task using the driver;
   */
  TX_SEMAPHORE *m_pxSyncObj;
}SPIPeripheralResources;

/**
 *
 */
static SPIPeripheralResources s_pxHwResouces[SPIDRV_CFG_HARDWARE_PERIPHERALS_COUNT] = {
  {NULL}
};


// Private member function declaration
// ***********************************

static void SPIMasterDriverTxRxCpltCallback(SPI_HandleTypeDef *hspi);

// Public API definition
// *********************

IIODriver *SPIMasterDriverAlloc() {
  IIODriver *pNewObj = (IIODriver*)SysAlloc(sizeof(SPIMasterDriver));

  if (pNewObj == NULL) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
    SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPIMasterDriver - alloc failed.\r\n"));
  }
  else {
    pNewObj->vptr = &s_xSPIMasterDriver_vtbl;
  }

  return pNewObj;
}

sys_error_code_t SPIMasterDriver_vtblInit(IDriver *_this, void *pParams) {
  assert_param(_this);
  UNUSED(pParams);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  UINT nRes = TX_SUCCESS;
  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  MX_DMA_Init();
  MX_SPI3_Init();

  // Register SPI DMA complete Callback
  if (HAL_OK != HAL_SPI_RegisterCallback(&hspi3, HAL_SPI_RX_COMPLETE_CB_ID, SPIMasterDriverTxRxCpltCallback)) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    xRes = SYS_UNDEFINED_ERROR_CODE;
  }
  else if (HAL_OK != HAL_SPI_RegisterCallback(&hspi3, HAL_SPI_TX_COMPLETE_CB_ID, SPIMasterDriverTxRxCpltCallback)) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    xRes = SYS_UNDEFINED_ERROR_CODE;
  }
  else {
    // initialize the software resources
    nRes = tx_semaphore_create(&pObj->m_xSyncObj, "SPIM_S", 0);
    if (nRes != TX_SUCCESS){
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
      xRes = SYS_OUT_OF_MEMORY_ERROR_CODE;
    }

    s_pxHwResouces[0].m_pxSyncObj = &pObj->m_xSyncObj;
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("SPIMasterDriver: initialization done.\r\n"));

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblStart(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  // enable the IRQ
//  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
//  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblStop(IDriver *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  // disable the IRQ
//  HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
//  HAL_NVIC_DisableIRQ(DMA1_Channel2_IRQn);

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblDoEnterPowerMode(IDriver *_this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblReset(IDriver *_this, void *pParams) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblWrite(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  xRes = SPIMasterDriverTransmitRegAddr(pObj, nChannel, 500);
  if (!SYS_IS_ERROR_CODE(xRes)) {
    while (HAL_SPI_Transmit_DMA(&hspi3, pDataBuffer, nDataSize) != HAL_OK) {
      if (HAL_SPI_GetError(&hspi3) != HAL_BUSY) {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_READ_ERROR_CODE);
        sys_error_handler();
      }
    }
    // Suspend the calling task until the operation is completed.
    tx_semaphore_get(&pObj->m_xSyncObj, TX_WAIT_FOREVER);
  }

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  xRes = SPIMasterDriverTransmitRegAddr(pObj, nChannel, 500);
  if (!SYS_IS_ERROR_CODE(xRes)) {
    while (HAL_SPI_Receive_DMA(&hspi3, pDataBuffer, nDataSize) != HAL_OK) {
      if (HAL_SPI_GetError(&hspi3) != HAL_BUSY) {
        SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_READ_ERROR_CODE);
        sys_error_handler();
      }
    }
    // Suspend the calling task until the operation is completed.
    tx_semaphore_get(&pObj->m_xSyncObj, TX_WAIT_FOREVER);
  }


  return xRes;
}

sys_error_code_t SPIMasterDriverTransmitRegAddr(SPIMasterDriver *_this, uint8_t nRegAddr, uint32_t nTimeoutMS) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  if (HAL_OK != HAL_SPI_Transmit(&hspi3, &nRegAddr, 1, nTimeoutMS)) {
    xRes = SYS_SPI_M_WRITE_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_ERROR_CODE);
    // block the application
    sys_error_handler();
  }

  return xRes;
}

sys_error_code_t SPIMasterDriverWriteRead(SPIMasterDriver *_this, uint8_t *pnTxDataBuffer, uint8_t *pnRxDataBuffer, uint16_t nDataSize) {
  assert_param(_this);
  assert_param(pnTxDataBuffer);
  assert_param(pnTxDataBuffer);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  while (HAL_SPI_TransmitReceive_DMA(&hspi3, pnTxDataBuffer, pnRxDataBuffer, nDataSize) != HAL_OK) {
    if (HAL_SPI_GetError(&hspi3) != HAL_BUSY) {
      SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_READ_ERROR_CODE);
      sys_error_handler();
    }
  }

  // Suspend the calling task until the operation is completed.
  tx_semaphore_get(&pObj->m_xSyncObj, TX_WAIT_FOREVER);

  return xRes;
}

sys_error_code_t SPIMasterDriverSelectDevice(SPIMasterDriver *_this, GPIO_TypeDef* xDeviceGPIOPort, uint16_t nDeviceGPIOPin) {
  // this is a class method so pointer _this is not used
  UNUSED(_this);

  HAL_GPIO_WritePin(xDeviceGPIOPort, nDeviceGPIOPin, GPIO_PIN_RESET);

  return SYS_NO_ERROR_CODE;
}

sys_error_code_t SPIMasterDriverDeselectDevice(SPIMasterDriver *_this, GPIO_TypeDef* xDeviceGPIOPort, uint16_t nDeviceGPIOPin) {
  // this is a class method so pointer _this is not used
  UNUSED(_this);

  HAL_GPIO_WritePin(xDeviceGPIOPort, nDeviceGPIOPin, GPIO_PIN_SET);

  return SYS_NO_ERROR_CODE;
}


// Private function definition
// ***************************


// CubeMX integration
// ******************

static void SPIMasterDriverTxRxCpltCallback(SPI_HandleTypeDef *hspi) {
  UNUSED(hspi);

  if (s_pxHwResouces[0].m_pxSyncObj) {
    tx_semaphore_put(s_pxHwResouces[0].m_pxSyncObj);
  }
}
