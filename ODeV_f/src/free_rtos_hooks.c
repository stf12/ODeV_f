/**
 ******************************************************************************
 * @file    free_rtos_hooks.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.1
 * @date    Nov 13, 2017
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

#include "FreeRTOS.h"
//#include "task.h" //TODO: STF.Port - threadx
#include "sysdebug.h"
#include "stm32f4xx_hal.h"

#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_APP, level, message)


// Private member function declaration
// ***********************************


// Public API definition
// *********************

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
  ( void ) pcTaskName;
  ( void ) pxTask;

  /* Run time stack overflow checking is performed if
  configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
  function is called if a stack overflow is detected. */
  taskDISABLE_INTERRUPTS();
  for( ;; );
}

void vApplicationMallocFailedHook( void )
{
  /* vApplicationMallocFailedHook() will only be called if
  configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
  function that will get called if a call to pvPortMalloc() fails.
  pvPortMalloc() is called internally by the kernel whenever a task, queue,
  timer or semaphore is created.  It is also called by various parts of the
  demo application.  If heap_1.c or heap_2.c are used, then the size of the
  heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
  FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
  to query the size of free heap space that remains (although it does not
  provide information on how the remaining heap might be fragmented). */
  taskDISABLE_INTERRUPTS();

  SYS_DEBUGF(SYS_DBG_LEVEL_SEVERE, ("FreeRTOS: malloc failed\r\n"));

  for( ;; );
}

/**
 * This function block the program execution if an exception occurs in the FreeRTOS kernel code.
 * See bugTabs4 #5265 (WDGID 201289)
 *
 * @param ulLine [IN] specifies the code line that has triggered the exception.
 * @param pcFileName [IN] specifies the full path of the file that has triggered the exception.
 */
void vFreeRTOSAssertCalled( unsigned long ulLine, const char * const pcFileName )
{
  /* Parameters are not used. */
  ( void ) ulLine;
  ( void ) pcFileName;

#ifdef FREERTOS_CONFIG_ASSERT_MUST_BLOCK
#ifdef DEBUG
  __asm volatile ("bkpt 0");
#else
  while(1) {

#if (SYS_TRACE > 1)
    for (unsigned int i=0; i<5000000; i++)
      __asm volatile( "NOP" );
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET);
    for (unsigned int j=0; j<5000000; j++)
      __asm volatile( "NOP" );
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET);
    __asm volatile ("bkpt 0");
#endif

  }
#endif
#else
volatile unsigned long ulSetToNonZeroInDebuggerToContinue = 0;


  taskENTER_CRITICAL();
  {
    while( ulSetToNonZeroInDebuggerToContinue == 0 )
    {
      /* Use the debugger to set ulSetToNonZeroInDebuggerToContinue to a
      non zero value to step out of this function to the point that raised
      this assert(). */
      __asm volatile( "NOP" );
      __asm volatile( "NOP" );
    }
  }
  taskEXIT_CRITICAL();
#endif
}

void vApplicationIdleHook( void ) {

#if !(SYS_DBG_ENABLE_TA4>=1)
  // Enter Sleep Mode.
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
#endif
}

// Private function definition
// ***************************
