/**
 ******************************************************************************
 * @file    ABusIF.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    23 dic 2020
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
#ifndef ABUSIF_H_
#define ABUSIF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "systypes.h"
#include "syserror.h"


typedef enum _EBusCtrlCmd {
	E_BUS_CTRL_DEV_NOTIFY_POWER_MODE,
} EBusCtrlCmd;

/**
 * forward declaration
 */
struct _IBus;

/**
 * Create a type name for _ABusIF.
 */
typedef struct _ABusIF ABusIF;

/**
 * Create a type name for the function to write in the SPI bus.
 *
 * @param pxSensor [IN] specifies a pointer to a device.
 * @param nRegAddr [IN] specifies a register address.
 * @param pnData [IN] specifies a buffer with the data to write.
 * @param nSize [IN] specifies the size in byte of the data to write.
 * @return 0 if success, an error code otherwise.
 */
typedef int32_t (*ABusWriteF)(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

/**
 * Create a type name for the function to read in the SPI bus.
 *
 * @param pxSensor [IN] specifies a pointer to a device.
 * @param nRegAddr [IN] specifies a register address.
 * @param pnData [OUT] specifies a buffer for the data to read.
 * @param nSize [IN] specifies the size in byte of the data to read.
 * @return 0 if success, an error code otherwise.
 */
typedef int32_t (*ABusReadF) (void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);

/**
 * This struct is a clone of ST stmdev_ctx_t that is defined in each sensor header file.
 * But in the bus there can be connected other devices than the ST sensors.
 */
typedef struct _ABusConnector {
  /**
   * Function to write in the bus. It is a component mandatory fields.
   */
  ABusWriteF  pfWriteReg;

  /**
   * Function to read from the bus. It is a component mandatory fields.
   */
  ABusReadF   pfReadReg;

  /**
   * Customizable optional pointer.
   */
  void *pxHandle;
} ABusConnector;


/**
 * Specifies the SPI interface for a generic sensor.
 */
struct _ABusIF {
  /**
   * The bus connector encapsulates the function pointer to read and write in the bus,
   * and it is compatible with the the ST universal sensor driver.
   */
  ABusConnector m_xConnector;

  struct _IBus *m_pxBus;

  /**
   * Bus control function. When the pointer is not NULL it can be used to perform bus specific control requests.
   *
   * @param [IN] _this specifies a pointer to the bus object.
   * @param [IN] eCtrlCmd specifies the type of control command to send to the bus.
   * @param [IN] nParams specifies a command parameter.
   */
  sys_error_code_t (*m_pfBusCtrl)(ABusIF *_this, EBusCtrlCmd eCtrlCmd, uint32_t nParams);
};


// Public API declaration
// **********************

inline sys_error_code_t ABusIFInit(ABusIF *_this);

inline sys_error_code_t ABusIFSetHandle(ABusIF *_this, void *pxHandle);
inline void *ABusIFGetHandle(const ABusIF *_this);

int32_t ABusIFNullRW(void *pxSensor, uint8_t nRegAddr, uint8_t* pnData, uint16_t nSize);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
sys_error_code_t ABusIFSetHandle(ABusIF *_this, void *pxHandle) {
  assert_param(_this);

  _this->m_xConnector.pxHandle = pxHandle;

  return SYS_NO_ERROR_CODE;
}

SYS_DEFINE_INLINE
void *ABusIFGetHandle(const ABusIF *_this) {
  assert_param(_this);

  return _this->m_xConnector.pxHandle;
}

SYS_DEFINE_INLINE
sys_error_code_t ABusIFInit(ABusIF *_this) {
  assert_param(_this);

  _this->m_pfBusCtrl = NULL;
  _this->m_xConnector.pfReadReg = ABusIFNullRW;
  _this->m_xConnector.pfWriteReg = ABusIFNullRW;
  _this->m_xConnector.pxHandle = NULL;

  return SYS_NO_ERROR_CODE;
}


#ifdef __cplusplus
}
#endif

#endif /* ABUSIF_H_ */
