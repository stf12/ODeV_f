/**
 ******************************************************************************
 * @file    SDCardTask.h
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
#ifndef SDCARDTASK_H_
#define SDCARDTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "ISensorEventListener.h"
#include "ISensorEventListener_vtbl.h"

// Command ID. These are all commands supported by a sensor task.
#define SDT_CMD_ID_START                    ((uint16_t)0x0001)              ///< START command ID.
#define SDT_CMD_ID_STOP                     ((uint16_t)0x0002)              ///< STOP command ID.
#define SDT_CMD_ID_FIRST_HALF_DATA_READY    ((uint16_t)0x0003)              ///< FIRST HALF DATA_READY command ID.
#define SDT_CMD_ID_SECOND_HALF_DATA_READY   ((uint16_t)0x0004)              ///< SECOND HALF DATA_READY command ID.


/**
 * Create  type name for _SDCardTask.
 */
typedef struct _SDCardTask SDCardTask;


// Public API declaration
//***********************

/**
 * Allocate an instance of SDCardTask.
 *
 * @return a pointer to the generic obejct ::AManagedTaskEx if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *SDCardTaskAlloc();

/**
 * Get the ::ISensorEventListener interface of the task object.
 *
 * @param _this  [IN] specifies a pointer to the task object.
 * @return a pointer to the ::ISensorEventListener of the task object.
 */
IEventListener *SDCardTaskGetEventListenrIF(const SDCardTask *_this);

// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* SDCARDTASK_H_ */
