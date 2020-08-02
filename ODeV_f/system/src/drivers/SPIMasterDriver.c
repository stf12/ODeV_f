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
#include "FreeRTOS.h"
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
  SemaphoreHandle_t m_xSyncObj;
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
  IIODriver *pNewObj = (IIODriver*)pvPortMalloc(sizeof(SPIMasterDriver));

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
  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  MX_DMA_Init();
  MX_SPI3_Init();

  // Register SPI DMA complete Callback
  if (HAL_OK != HAL_SPI_RegisterCallback(&hspi3, HAL_SPI_TX_RX_COMPLETE_CB_ID, SPIMasterDriverTxRxCpltCallback)) {
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_UNDEFINED_ERROR_CODE);
    xRes = SYS_UNDEFINED_ERROR_CODE;
  }
  else {
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
    vQueueAddToRegistry(pObj->m_xSyncObj, "SPI3Drv");
  }
#endif

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
//  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;


//  HAL_SPI_Transmit(&hsm_spi, &msg->regAddr, 1, 1000);
  if (HAL_OK != HAL_SPI_Transmit(&hspi3, pDataBuffer, nDataSize, nChannel)) {
    xRes = SYS_SPI_M_WRITE_ERROR_CODE;
    SYS_SET_LOW_LEVEL_ERROR_CODE(SYS_SPI_M_WRITE_ERROR_CODE);
    // block the application
    sys_error_handler();
  }

  return xRes;
}

sys_error_code_t SPIMasterDriver_vtblRead(IIODriver *_this, uint8_t *pDataBuffer, uint16_t nDataSize, uint16_t nChannel) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
//  SPIMasterDriver *pObj = (SPIMasterDriver*)_this;

  SYS_DEBUGF(SYS_DBG_LEVEL_WARNING, ("SPI_M_DRV: warning! Read not implemented.\r\n"));

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
  xSemaphoreTake(pObj->m_xSyncObj, portMAX_DELAY);

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

  if (s_pxHwResouces[0].m_xSyncObj) {
    xSemaphoreGiveFromISR(s_pxHwResouces[0].m_xSyncObj, NULL);
  }
}
