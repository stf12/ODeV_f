/**
  ******************************************************************************
  * @file    ts.c
  * @author  MCD Application Team
  * @version $VERSION$
  * @date    $DATE$ 
  * @brief   This file provides the kernel menu functions 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright © 2016 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "stm32l4r9i_eval.h"
#include "stm32l4r9i_eval_io.h"
#include "stm32l4r9i_eval_ts.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Private variables ---------------------------------------------------------*/
static TaskHandle_t xTouchTaskHandle;
static xQueueHandle vsync_q = 0;

/* Just a dummy value to insert in the Touch queue. */
static uint8_t dummy = 0x5a;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void TouchThread(void* params);

#define configTOUCH_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 4 )
#define configTOUCH_TASK_STACK_SIZE               ( 150 )

/**
  * @brief  Touch hardware abd thread initialization to manage interrupt
  * @param  None 
  * @retval None
  */
void TouchTaskInit(void)
{
  /* Create Touch Screen task */
  BaseType_t xReturned;
  xReturned = xTaskCreate(TouchThread, (TASKCREATE_NAME_TYPE)"TouchTask",
                configTOUCH_TASK_STACK_SIZE,
                NULL,
                configTOUCH_TASK_PRIORITY,
                &xTouchTaskHandle);
  configASSERT(xReturned == pdPASS);
  
  vsync_q = xQueueGenericCreate(1, 1, 0);
  
}

/**
  * @brief  Touch hardware and thread de-initialization
  * @param  None 
  * @retval None
  */
void TouchDeInit(void)
{
  // Use the handle to delete the task.
  if( xTouchTaskHandle != NULL )
  {
     vTaskDelete( xTouchTaskHandle );
  }
  vQueueDelete( vsync_q );
}

/**
  * @brief  interrupt detection
  * @param  None 
  * @retval None
  */
void TouchDetectInt(void)
{  
  /* Release the semaphore if the thread was created prior to this call */
  portBASE_TYPE px = pdFALSE;
  xQueueSendFromISR(vsync_q, &dummy, &px);
  portEND_SWITCHING_ISR(px);
}

/**
  * @brief  Touch Screen Thread
  * @param  argument: pointer that is passed to the thread function as start argument.
  * @retval None
  */
static void TouchThread(void* params)
{
  for( ;; )
  {
    /* Wait for next Touch to occur. */
    xQueueReceive(vsync_q, &dummy, portMAX_DELAY);
    /* Update TS data */
    BSP_TS_UpdateState();
  }
}
