/**
 ******************************************************************************
 * @file    IBus.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    28 dic 2020
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

#include "IBus.h"
#include "IBus_vtbl.h"

// GCC requires one function forward declaration in only one .c source
// in order to manage the inline.
// See also http://stackoverflow.com/questions/26503235/c-inline-function-and-gcc
#if defined (__GNUC__)
extern sys_error_code_t IBusCtrl(IBus *_this, EBusCtrlCmd eCtrlCmd, uint32_t nParams);
extern sys_error_code_t IBusConnectDevice(IBus *_this, ABusIF *pxBusIF);
extern sys_error_code_t IBusDisconnectDevice(IBus *_this, ABusIF *pxBusIF);
#endif
