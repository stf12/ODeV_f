/**
 ******************************************************************************
 * @file    TscTask.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Dec 21, 2018
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2017 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */
#ifndef TSCTASK_H_
#define TSCTASK_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "systp.h"
#include "syserror.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"
#include "queue.h"



/**
 * Create  type name for _TscTask.
 */
typedef struct _TscTask TscTask;

/**
 *  TscTask internal structure.
 */
struct _TscTask {
  /**
   * Base class object.
   */
  AManagedTaskEx super;

  // Task variables should be added here.

  /**
   * Used to synchronize task and ISR.
   * Why a queue?
   */
  QueueHandle_t m_xVsyncQueue;
};


// Public API declaration
//***********************

/**
 * Allocate an instance of TscTask.
 *
 * @return a pointer to the generic obejct ::AManagedTask if success,
 * or NULL if out of memory error occurs.
 */
AManagedTaskEx *TscTaskAlloc();


// Inline functions definition
// ***************************


#ifdef __cplusplus
}
#endif

#endif /* TSCTASK_H_ */
