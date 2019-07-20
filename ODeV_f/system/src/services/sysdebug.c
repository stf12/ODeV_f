/**
 ******************************************************************************
 * @file    sysdebug.c
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.2.0
 * @date    Oct 10, 2016
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

#include "sysdebug.h"
#include "systp.h"
#include "stdio.h"
#include "stdint.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"


/**
 * Select the system wide debug level. Valid value are:
 * - SYS_DBG_LEVEL_ALL
 * - SYS_DBG_LEVEL_VERBOSE
 * - SYS_DBG_LEVEL_LLA
 * - SYS_DBG_LEVEL_SL
 * - SYS_DBG_LEVEL_DEFAULT
 * - SYS_DBG_LEVEL_WARNING
 * - SYS_DBG_LEVEL_SEVERE
 */
uint8_t g_sys_dbg_min_level = SYS_DBG_LEVEL_VERBOSE;


#ifdef SYS_DEBUG

/**
 * Check if the current code is inside an ISR or not.
 */
#define SYS_DBG_IS_CALLED_FROM_ISR() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ? 1 : 0)

static SemaphoreHandle_t s_xMutex = NULL;
static UART_HandleTypeDef s_xUartHandle;
static TIM_HandleTypeDef s_xTim6Handle;

uint32_t g_ulHighFrequencyTimerTicks = 0;

static void SysDebugSetupRunTimeStatsTimer();

void null_lockfn();
void SysDebugLock();
void SysDebugUnlock();

extern void sys_error_handler(void);
extern void MX_USART2_UART_Init(UART_HandleTypeDef* uartHandle);
extern void MX_TIM6_Init(TIM_HandleTypeDef* tim_baseHandle);


xDebugLockUnlockFnType xSysDebugLockFn = null_lockfn;
xDebugLockUnlockFnType xSysDebugUnlockFn = null_lockfn;

DebugPrintfFn xSysDebugPrintfFn = printf;

int SysDebugInit() {
  // hardware initialization
  SysDebugHardwareInit();

  // software initialization.
  s_xMutex = xSemaphoreCreateMutex();

  if (s_xMutex == NULL) {
    return 1;
  }

  xSysDebugUnlockFn = SysDebugUnlock;
  xSysDebugLockFn = SysDebugLock;

#ifdef DEBUG
  vQueueAddToRegistry(s_xMutex, "DBG");
#endif
  return 0;
}

void SysDebugToggleLed(uint8_t nLed) {
  UNUSED(nLed);

  HAL_GPIO_TogglePin(SYS_DBG_TP1_PORT, SYS_DBG_TP1_PIN);
}

void SysDebugLedOn(uint8_t nLed) {
  UNUSED(nLed);

  HAL_GPIO_WritePin(SYS_DBG_TP1_PORT, SYS_DBG_TP1_PIN, GPIO_PIN_SET);
}

void SysDebugLedOff(uint8_t nLed) {
  UNUSED(nLed);

  HAL_GPIO_WritePin(SYS_DBG_TP1_PORT, SYS_DBG_TP1_PIN, GPIO_PIN_RESET);
}

void null_lockfn()
{
  return;
}

void SysDebugLock() {
  if (SYS_DBG_IS_CALLED_FROM_ISR()) {
    xSemaphoreTakeFromISR(s_xMutex, NULL);
  }
  else {
    if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
      xSemaphoreTake(s_xMutex, 0);
    }
    else {
      xSemaphoreTake(s_xMutex, portMAX_DELAY);
    }
  }
}

void SysDebugUnlock() {
  if (SYS_DBG_IS_CALLED_FROM_ISR()) {
    xSemaphoreGiveFromISR(s_xMutex, NULL);
  }
  else {
    xSemaphoreGive(s_xMutex);
  }
}

#if defined ( __ICCARM__ )
__weak
#else
__attribute__((weak))
#endif
int SysDebugHardwareInit() {

  __HAL_RCC_DBGMCU_CLK_ENABLE();
  MX_USART2_UART_Init(&s_xUartHandle);

#ifdef DEBUG
  // Debug TP1 and TP2 configuration
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(SYS_DBG_TP1_PORT, SYS_DBG_TP1_PIN|SYS_DBG_TP2_PIN, GPIO_PIN_RESET);
  GPIO_InitStruct.Pin = SYS_DBG_TP1_PIN|SYS_DBG_TP2_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SYS_DBG_TP1_PORT, &GPIO_InitStruct);

  SysDebugSetupRunTimeStatsTimer();
#endif

  return 0;
}

void SysDebugSetupRunTimeStatsTimer() {
  MX_TIM6_Init(&s_xTim6Handle);
}

void SysDebugStartRunTimeStatsTimer() {
  HAL_NVIC_EnableIRQ(TIM6_IRQn);
  HAL_TIM_Base_Start_IT(&s_xTim6Handle);
}

int SysDebugLowLevelPutchar(int x) {
  if(HAL_UART_Transmit(&s_xUartHandle, (uint8_t*)&x, 1, 5000)!= HAL_OK) {
    return -1;
  }

//  ITM_SendChar(x);

  return x;
}

// CubeMx integration
// ******************

void TIM6_IRQHandler(void) {
    // TIM Update event
  if(__HAL_TIM_GET_FLAG(&s_xTim6Handle, TIM_FLAG_UPDATE) != RESET) {
    if(__HAL_TIM_GET_IT_SOURCE(&s_xTim6Handle, TIM_IT_UPDATE) != RESET) {
      __HAL_TIM_CLEAR_IT(&s_xTim6Handle, TIM_IT_UPDATE);
      // handle the update event.
      g_ulHighFrequencyTimerTicks++;
    }
  }
}

#endif // SYS_DEBUG

