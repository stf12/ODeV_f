/**
 ******************************************************************************
 * @file    ApplicationContext.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 13, 2017
 *
 * @brief   Define the Application Context public API.
 *
 * TODO - insert here the file description
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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

#ifndef INCLUDE_SERVICES_APPLICATIONCONTEXT_H_

#define INCLUDE_SERVICES_APPLICATIONCONTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "AManagedTask.h"
#include "AManagedTaskEx.h"
#include "AManagedTaskEx_vtbl.h"

/**
 * An application context is a linked list of Managed tasks.
 */
typedef struct _ApplicationContext {
	/**
	 * Specifies the pointer to the first task.
	 */
	AManagedTask *m_pHead;

	/**
	 * Specifies the number of item (Managed Task object) in the list.
	 */
	uint8_t m_nListSize;
}ApplicationContext;

// Public API declaration
//***********************

/**
 * Initialize this application context.
 *
 * @param this specifies a pointer to the application context object.
 * @return SYS_NO_ERROR_CODE.
 */
sys_error_code_t ACInit(ApplicationContext *this);

/**
 * Add a managed task to this context. If the task is already in this application context it is not added twice.
 *
 * @param this specifies a pointer to the application context object.
 * @param pTask specifies a pointer to a managed task object to be added to this context.
 * @return SYS_NO_ERROR_CODE if the the task has been added to the application context, or SYS_AC_TASK_ALREADY_ADDED_ERROR_CODE
 *         if the task is already in this application context.
 */
sys_error_code_t ACAddTask(ApplicationContext *this, AManagedTask *pTask);

/**
 * Remove a managed task from this context.
 *
 * @param this specifies a pointer to the application context object.
 * @param pTask specifies a pointer to a managed task object to be removed from this context.
 * @return SYS_NO_ERROR_CODE.
 */
sys_error_code_t ACRemoveTask(ApplicationContext *this, AManagedTask *pTask);

/**
 * Get the number of managed task in this context.
 *
 * @param this specifies a pointer to the application context object.
 * @return the number of managed task in this context.
 */
inline uint8_t ACGetTaskCount(ApplicationContext *this);

/**
 * Get a pointer to the first task object in this application context.
 *
 * @param this specifies a pointer to the application context object.
 * @return a pointer to the first task object in this application context, or NULL if the this context is empty.
 */
inline AManagedTask *ACGetFirstTask(ApplicationContext *this);

/**
 * Get a pointer to the next task object after pTask in this application context.
 *
 * @param this specifies a pointer to the application context object.
 * @param pTasks specifies a pointer a managed task object in this context.
 * @return a pointer to the next task object after pTask in this application context.
 */
inline AManagedTask *ACGetNextTask(ApplicationContext *this, const AManagedTask *pTask);


// Inline functions definition
// ***************************

SYS_DEFINE_INLINE
uint8_t ACGetTaskCount(ApplicationContext *this) {
	assert_param(this);

	return this->m_nListSize;
}

SYS_DEFINE_INLINE
AManagedTask *ACGetFirstTask(ApplicationContext *this) {
	assert_param(this);

	return this->m_pHead;
}

SYS_DEFINE_INLINE
AManagedTask *ACGetNextTask(ApplicationContext *this, const AManagedTask *pTask) {
	assert_param(this);
	assert_param(pTask);
	UNUSED(this);

	return pTask->m_pNext;
}

#ifdef __cplusplus
}
#endif


#endif /* INCLUDE_SERVICES_APPLICATIONCONTEXT_H_ */
