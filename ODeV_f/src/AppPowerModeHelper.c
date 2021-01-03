/**
 ******************************************************************************
 * @file    AppPowerModeHelper.c
 * @author  STF12
 * @version 1.0.0
 * @date    Aug 10, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 STF12 - Stefano Oliveri</center></h2>
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

#include "AppPowerModeHelper.h"
#include "AppPowerModeHelper_vtbl.h"
#include "FreeRTOS.h"
#include "sysinit.h"
#include "sysdebug.h"


#define SYS_DEBUGF(level, message)      SYS_DEBUGF3(SYS_DBG_APMH, level, message)


/**
 * Application Power Mode Helper virtual table.
 */
static const IAppPowerModeHelper_vtbl s_xAppPowerModeHelper_vtbl = {
    AppPowerModeHelper_vtblInit,
    AppPowerModeHelper_vtblComputeNewPowerMode,
    AppPowerModeHelper_vtblCheckPowerModeTransaction,
    AppPowerModeHelper_vtblDidEnterPowerMode,
    AppPowerModeHelper_vtblGetActivePowerMode,
    AppPowerModeHelper_vtblGetPowerStatus,
    AppPowerModeHelper_vtblIsLowPowerMode
};

/**
 * Internal state of the Application Power Mode Helper.
 */
struct _AppPowerModeHelper {

  /**
   * Base class object.
   */
  IAppPowerModeHelper *super;

  /**
   * Specifies the system power mode status.
   */
  SysPowerStatus m_xStatus;

  /**
   * Used to buffer the the previous RUN state during the transaction from RUN_x to SLEEP_x.
   * The buffered state is use to compute the correct transaction from SLEEP_x to RUN_x.
   */
  EPowerMode m_ePreviousRunState;
};

// Private member function declaration
// ***********************************

extern void SystemClock_Backup(void);

extern void SystemClock_Restore(void);

// Public API definition
// *********************

IAppPowerModeHelper *AppPowerModeHelperAlloc() {
  IAppPowerModeHelper *pNewObj = (IAppPowerModeHelper*)pvPortMalloc(sizeof(AppPowerModeHelper));

  if (pNewObj == NULL) {
    SYS_SET_SERVICE_LEVEL_ERROR_CODE(SYS_OUT_OF_MEMORY_ERROR_CODE);
  }
  else {
    pNewObj->vptr = &s_xAppPowerModeHelper_vtbl;
  }

  return pNewObj;
}


// Virtual functions definition
// ****************************

sys_error_code_t AppPowerModeHelper_vtblInit(IAppPowerModeHelper *this) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  AppPowerModeHelper *pObj = (AppPowerModeHelper*)this;

  pObj->m_xStatus.m_eActivePowerMode = E_POWER_MODE_RUN;
  pObj->m_ePreviousRunState = E_POWER_MODE_RUN;

  return xRes;
}

EPowerMode AppPowerModeHelper_vtblComputeNewPowerMode(IAppPowerModeHelper *this, const SysEvent xEvent) {
  assert_param(this);
  AppPowerModeHelper *pObj = (AppPowerModeHelper*)this;

  EPowerMode ePowerMode = pObj->m_xStatus.m_eActivePowerMode;

  switch (xEvent.xEvent.nSource) {

  case SYS_PM_EVT_SRC_PB:
    if (ePowerMode == E_POWER_MODE_RUN) {
      ePowerMode = E_POWER_MODE_DATALOG;
    }
    else if (ePowerMode == E_POWER_MODE_DATALOG) {
      ePowerMode = E_POWER_MODE_RUN;
    }
    else if (ePowerMode == E_POWER_MODE_SLEEP_1) {
      ePowerMode = E_POWER_MODE_RUN;
    }
    break;

  case SYS_PM_EVT_SRC_LP_TIMER:
    if (ePowerMode == E_POWER_MODE_RUN) {
      ePowerMode = E_POWER_MODE_SLEEP_1;
    }
    break;

  default:

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: wrong SysEvent.\r\n"));

    sys_error_handler();
    break;
  }

#ifdef SYS_DEBUG
  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: new PM:%u-%u.\r\n", pObj->m_xStatus.m_eActivePowerMode, ePowerMode));
#endif

  return ePowerMode;
}

boolean_t AppPowerModeHelper_vtblCheckPowerModeTransaction(IAppPowerModeHelper *this, const EPowerMode eActivePowerMode, const EPowerMode eNewPowerMode) {
  UNUSED(this);
  boolean_t xRes = FALSE;

  switch (eActivePowerMode) {
  case E_POWER_MODE_RUN:
    if ((eNewPowerMode == E_POWER_MODE_DATALOG) || (eNewPowerMode == E_POWER_MODE_SLEEP_1)) {
      xRes = TRUE;
    }
    break;
  case E_POWER_MODE_DATALOG:
    if (eNewPowerMode == E_POWER_MODE_RUN) {
      xRes = TRUE;
    }
    break;
  case E_POWER_MODE_SLEEP_1:
    if (eNewPowerMode == E_POWER_MODE_RUN) {
      xRes = TRUE;
    }
    break;
  default:
    xRes = FALSE;
  }

  if (xRes == FALSE) {

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: ERR PM transaction %u -> %u\r\n", (uint8_t)eActivePowerMode, (uint8_t)eNewPowerMode));

    sys_error_handler();
  }

  SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: PM transaction %u -> %u\r\n", (uint8_t)eActivePowerMode, (uint8_t)eNewPowerMode));

  return xRes;
}

sys_error_code_t AppPowerModeHelper_vtblDidEnterPowerMode(IAppPowerModeHelper *this, EPowerMode ePowerMode) {
  assert_param(this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;
  AppPowerModeHelper *pObj = (AppPowerModeHelper*)this;

  pObj->m_xStatus.m_eActivePowerMode = ePowerMode;

  switch (ePowerMode) {
  case E_POWER_MODE_SLEEP_1:

    // before put the MCU in STOP check if there are event pending in the system queue

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: try SLEEP_1\r\n"));

    // disable the IRQ
    __asm volatile ("cpsid i");

    // reset the WWDG
    SysResetAEDCounter();

    if (!SysEventsPending()) {
      HAL_SuspendTick();
      // there are no other message waiting so I can put the MCU in stop
      // Enable Power Control clock
      __HAL_RCC_PWR_CLK_ENABLE();

      // Enter Stop Mode

//      GPIOB->BRR = GPIO_PIN_7; //TODO: STF.Debug
//      GPIOB->BRR = GPIO_PIN_6; //TODO: STF.Debug
      GPIOD->BRR = GPIO_PIN_0;


      // Disable all used wakeup sources: WKUP pin
      HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN2);

      __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);

      SystemClock_Backup();
      HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

      // The MCU has exited the STOP mode
      // reset the WWDG
      SysResetAEDCounter();

      // Configures system clock after wake-up from STOP
      SystemClock_Restore();
      HAL_ResumeTick();
    }

//    GPIOB->BSRR = GPIO_PIN_7; //TODO: STF.Debug
    GPIOD->BSRR = GPIO_PIN_0;
    for (int i=0; i< 2000000; i++) __NOP();
    GPIOD->BRR = GPIO_PIN_0;
    for (int i=0; i< 2000000; i++) __NOP();
    GPIOD->BSRR = GPIO_PIN_0;
    for (int i=0; i< 2000000; i++) __NOP();
    GPIOD->BRR = GPIO_PIN_0;

    // enable the IRQ
    __asm volatile ("cpsie i");
    break;

  case E_POWER_MODE_RUN:

//    GPIOB->BSRR = GPIO_PIN_6; //TODO: STF.Debug
    GPIOD->BSRR = GPIO_PIN_0;

    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: RUN\r\n"));

#if defined(DEBUG) || defined(SYS_DEBUG)
    {
      size_t nFreeHeapSize = xPortGetFreeHeapSize();
      SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("PMH: free heap = %i.\r\n", nFreeHeapSize));
    }
#endif
    break;

  case E_POWER_MODE_DATALOG:

//    GPIOB->BSRR = GPIO_PIN_6; //TODO: STF.Debug
    GPIOD->BRR = GPIO_PIN_0;
    SYS_DEBUGF(SYS_DBG_LEVEL_VERBOSE, ("PMH: DATALOG\r\n"));

#if defined(DEBUG) || defined(SYS_DEBUG)
    {
      size_t nFreeHeapSize = xPortGetFreeHeapSize();
      SYS_DEBUGF(SYS_DBG_LEVEL_SL, ("PMH: free heap = %i.\r\n", nFreeHeapSize));
    }
#endif
    break;

  default:
    sys_error_handler();
    break;
  }

  return xRes;
}

EPowerMode AppPowerModeHelper_vtblGetActivePowerMode(IAppPowerModeHelper *this) {
  assert_param(this);
  AppPowerModeHelper *pObj = (AppPowerModeHelper*)this;

  return pObj->m_xStatus.m_eActivePowerMode;
}

SysPowerStatus AppPowerModeHelper_vtblGetPowerStatus(IAppPowerModeHelper *this) {
  assert_param(this);
  AppPowerModeHelper *pObj = (AppPowerModeHelper*)this;

  return pObj->m_xStatus;
}

boolean_t AppPowerModeHelper_vtblIsLowPowerMode(IAppPowerModeHelper *this, const EPowerMode ePowerMode) {
  UNUSED(this);

  return (ePowerMode == E_POWER_MODE_SLEEP_1);
}
