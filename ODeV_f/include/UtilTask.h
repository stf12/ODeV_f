/**
 ******************************************************************************
 * @file    UtilTask.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jul 23, 2020
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
#ifndef UTILTASK_H_
#define UTILTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"


/**
 * Create  type name for _UtilTask.
 */
typedef struct _UtilTask UtilTask;


// Public API declaration
//***********************

/**
 * Allocate an instance of UtilTask.
 *
 * @return a pointer to the generic obejct ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *UtilTaskAlloc();

/**
 * Get the time stamp. It is defined only when the system is in E_POWER_MODE_DATALOG
 *
 * @param _this [IN] specifies a pointer to a task object.
 * @return the system time stamp in tick.
 */
uint32_t UtilTaskGetTimeStamp(UtilTask *_this);

/**
 * The same as UtilTaskGetTimeStamp() but without a task object. It implicitly uses the only
 * the only instance of ::UtilTask.
 *
 * @return the system time stamp in tick.
 */
uint32_t UtilGetTimeStamp();


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* UTILTASK_H_ */
