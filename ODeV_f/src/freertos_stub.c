/**
 ******************************************************************************
 * @file    freertos.c
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

#include "FreeRTOS.h"

void xPortSysTickHandler( void ) {}
void *pvPortMalloc( size_t xWantedSize ) { return (void*)0; }
size_t xPortGetFreeHeapSize( void ) { return 0; }

void vTaskDelay( const TickType_t xTicksToDelay ) {}
void vTaskSuspend( TaskHandle_t xTaskToSuspend ) {}
void vTaskStartScheduler( void ) {}
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode, const char * const pcName, const uint16_t usStackDepth, void * const pvParameters, UBaseType_t uxPriority, TaskHandle_t * const pxCreatedTask ) { return 0; }
int xTaskGetSchedulerState() { return 0; }
BaseType_t xTaskResumeAll( void ) { return 0; }
void vTaskResume( TaskHandle_t xTaskToResume ) {}
void vTaskSuspendAll( void ) {}

QueueHandle_t xSemaphoreCreateBinary() { return (void*)0; }
QueueHandle_t xSemaphoreCreateMutex() { return (void*)0; }
BaseType_t xSemaphoreTake( QueueHandle_t xQueue, TickType_t xTicksToWait )    { return 0; }
BaseType_t xSemaphoreGiveFromISR( QueueHandle_t xQueue, BaseType_t * const pxHigherPriorityTaskWoken) { return 0; }
BaseType_t xSemaphoreTakeFromISR( QueueHandle_t xQueue, void * const pvBuffer, BaseType_t * const pxHigherPriorityTaskWoken) { return 0; }
BaseType_t xSemaphoreGive( QueueHandle_t xQueue, const void * const pvItemToQueue, TickType_t xTicksToWait, const BaseType_t xCopyPosition) { return 0; }
void vQueueAddToRegistry(QueueHandle_t xQueue, const char *pcQueueName) {}
BaseType_t xQueueSendToBackFromISR(QueueHandle_t xQueue, const void * const pvItemToQueue, BaseType_t * const pxHigherPriorityTaskWoken, const BaseType_t xCopyPosition) { return 0; }
BaseType_t xQueueSendToBack(QueueHandle_t  xQueue, const void *pvItemToQueue, TickType_t xTicksToWait) { return 0; }
UBaseType_t uxQueueMessagesWaiting( const QueueHandle_t xQueue ) { return 0; }
UBaseType_t uxQueueMessagesWaitingFromISR( const QueueHandle_t xQueue ) { return 0; }
QueueHandle_t xQueueCreateMutex( const uint8_t ucQueueType ) { return (void*)0; }
BaseType_t xQueueReceive(QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait) { return 0; }
QueueHandle_t xQueueCreate(UBaseType_t uxQueueLength, UBaseType_t uxItemSize) { return (void*)0; }

