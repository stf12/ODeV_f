/**
 ******************************************************************************
 * @file    FreeRTOS.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.0.0
 * @date    Sep 21, 2020
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
#ifndef FREERTOS_H_
#define FREERTOS_H_

#include "stdint.h"
#include "stddef.h"

#define taskSCHEDULER_SUSPENDED   ( ( BaseType_t ) 0 )
#define portMAX_DELAY ( TickType_t ) 0xffffffffUL
#define configMAX_PRIORITIES                    ( 7 )
#define configMINIMAL_STACK_SIZE                ( ( uint16_t ) 128 )
#define pdTRUE      ( ( BaseType_t ) 1 )
#define pdPASS      ( pdTRUE )
#define errQUEUE_FULL ( ( BaseType_t ) 0 )
#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#define configTICK_RATE_HZ  16000000

#define tskIDLE_PRIORITY      ( ( UBaseType_t ) 0U )

typedef void (*TaskFunction_t)( void * );

typedef unsigned long UBaseType_t;
typedef long BaseType_t;
typedef uint32_t TickType_t;

struct tskTaskControlBlock; /* The old naming convention is used to prevent breaking kernel aware debuggers. */
typedef struct tskTaskControlBlock* TaskHandle_t;

struct QueueDefinition; /* Using old naming convention so as not to break kernel aware debuggers. */
typedef struct QueueDefinition * QueueHandle_t;
typedef QueueHandle_t SemaphoreHandle_t;

// Stub function definitoin

#define taskEXIT_CRITICAL()
#define taskENTER_CRITICAL()
#define vSemaphoreCreateBinary( xSemaphore )

void vTaskDelay( const TickType_t xTicksToDelay );
void vTaskSuspend( TaskHandle_t xTaskToSuspend );
void vTaskStartScheduler( void );
void xPortSysTickHandler( void );
void *pvPortMalloc( size_t xWantedSize );

#endif /* FREERTOS_H_ */
