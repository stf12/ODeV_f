/**
 ******************************************************************************
 * @file    ApplicationContext.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Jan 13, 2017
 * @brief
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

#include "ApplicationContext.h"

// Private member function declaration
// ***********************************

// Inline function farward declaration
// ***********************************

#if defined (__GNUC__)
extern uint8_t ACGetTaskCount(ApplicationContext *this);
extern AManagedTask *ACGetFirstTask(ApplicationContext *this);
extern AManagedTask *ACGetNextTask(ApplicationContext *this, const AManagedTask *pTask);
#endif

// Public API definition
// *********************

sys_error_code_t ACInit(ApplicationContext *this) {
	assert_param(this);

	this->m_nListSize = 0;
	this->m_pHead = NULL;

	return SYS_NO_ERROR_CODE;
}

sys_error_code_t ACAddTask(ApplicationContext *this, AManagedTask *pTask) {
	assert_param(this);
	sys_error_code_t xRes = SYS_NO_ERROR_CODE;

	if (pTask != NULL) {
		// check if pTask is already in the list
		const AManagedTask *pTaskTemp = ACGetFirstTask(this);
		while ((pTaskTemp != NULL) && (pTaskTemp != pTask)) {
			pTaskTemp = ACGetNextTask(this, pTaskTemp);
		}

		if ((pTaskTemp == pTask)) {
			// the task is already in this context.
			xRes = SYS_AC_TASK_ALREADY_ADDED_ERROR_CODE;
			SYS_SET_SERVICE_LEVEL_ERROR_CODE(xRes);
		}
		else {
			// add the task in the context.
			pTask->m_pNext = this->m_pHead;
			this->m_pHead = pTask;
			this->m_nListSize++;
		}
	}

	return xRes;
}

sys_error_code_t ACRemoveTask(ApplicationContext *this, AManagedTask *pTask) {
	assert_param(this);
	sys_error_code_t xRes = SYS_NO_ERROR_CODE;

	// TODO: STF - TO BE IMPLEMENTED

	sys_error_handler();

	return xRes;
}
