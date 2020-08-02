/**
 ******************************************************************************
 * @file    syserror.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Sep 5, 2016
 * @brief   Implement the global error management API
 *
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

#include "systp.h"
#include "syserror.h"
#include "FreeRTOS.h"
#include "task.h"

sys_error_t g_nSysError = {0};

#define COUNTOF(A)        (sizeof(A)/sizeof(*A))

#define FREERTOS_CONFIG_ASSERT_MUST_BLOCK

#ifndef SYS_IS_CALLED_FROM_ISR
#define SYS_IS_CALLED_FROM_ISR() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ? TRUE : FALSE)
#endif


void sys_error_handler(void)
{
#if defined(DEBUG)
	__asm volatile ("bkpt 0");
#else
	if (SYS_IS_CALLED_FROM_ISR()) {
		taskENTER_CRITICAL_FROM_ISR();
	}
	else {
		taskENTER_CRITICAL();
	}

	while(1) {
		__asm volatile( "NOP" );

#if (SYS_TRACE > 1)
		for (unsigned int i=0; i<4000000; i++)
			__asm volatile( "NOP" );
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
		for (unsigned int j=0; j<4000000; j++)
			__asm volatile( "NOP" );
//		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
#endif

	}
#endif
}

#if (SYS_TRACE > 1)
void sys_check_error_code(sys_error_code_t xError) {
	// first check if it is a general error code
	switch (xError) {
	case SYS_UNDEFINED_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_OUT_OF_MEMORY_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_INVALID_PARAMETER_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_INVALID_FUNC_CALL_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	}

	// check the low level error code
	switch (xError) {
	case SYS_I2C_READ_ERROR_CODE :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_I2C_WRITE_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_I2C_INIT_ERROR_CODE :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HIDI2C_INIT_ERROR_CODE                      :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HIDI2C_TIMEOUT_ERROR_CODE                   :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HIDI2C_EVENT_TASK_SWITCH_ERROR_CODE         :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HIDI2C_EVENT_STOP_ERROR_CODE                :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HIDI2C_EVENT_TASK_SWITCH_AND_STOP_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_KMDRV_INVALID_CONFIG_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_KMDRV_TASK_SWITCH_ERROR_CODE   :
		__asm volatile( "bkpt 0" );
		break;
	}

	// check the service level error code
	switch (xError) {
	case SYS_HID_I2C_P_UNEXPECTED_BCD_VERSION_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HID_I2C_P_WEIRD_HID_DESCR_SIZE_ERROR_CODE  :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HID_I2C_P_REQUEST_INVALID_OPCODE           :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HID_I2C_P_REQUEST_RESET_ERROR_CODE         :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HID_I2C_P_DEVICE_RESET_ERROR_CODE          :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HID_I2C_P_DEVICE_MISSING_ERROR_CODE        :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_HID_I2C_P_REQUEST_NOT_SUPPORTED_OPCODE     :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_KS_TIMEOUT_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_AC_TASK_ALREADY_ADDED_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_TASK_HEAP_OUT_OF_MEMORY_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_TASK_INVALID_CALL_ERROR_CODE      :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_TASK_INVALID_PARAM_ERROR_CODE     :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_TASK_QUEUE_FULL_ERROR_CODE        :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_INIT_TASK_FAILURE_ERROR_CODE              :
		__asm volatile( "bkpt 0" );
		break;
	case SYS_INIT_TASK_POWER_MODE_NOT_ENABLE_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	case SYS_USBD_TASK_REPORT_LOST_ERROR_CODE:
		__asm volatile( "bkpt 0" );
		break;
	}
}
#endif
