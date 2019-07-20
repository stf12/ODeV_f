/**
 ******************************************************************************
 * @file    systp.h
 * @author  STMicroelectronics - ST-Korea - MCD Team
 * @version 1.3.0
 * @date    July 20, 2019
 * @brief   Target platform definition.
 *
 * This file include definitions depending on the target platform.
 * A target platform is a tuple Hardware + Compiler.
 * It support the following MCU family:
 * - STM32L4
 * - STM32L0
 * - STM32F0
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
#ifndef SYSTARGETPLATFORM_H_
#define SYSTARGETPLATFORM_H_

// MCU specific include
// ********************

#ifdef SYS_TP_MCU_STM32L4
#include "stm32l4xx.h"
#elif defined (SYS_TP_MCU_STM32L0)
#include "stm32l0xx.h"
#elif defined (SYS_TP_MCU_STM32F0)
#include "stm32f0xx.h"
#endif

// Compiler specific define
// ************************

#if defined (__ICCARM__)

#define SYS_DEFINE_INLINE #pragma inline

#elif defined (__GNUC__)

#define SYS_DEFINE_INLINE inline

#endif

// This section defines some symbol specific to the STM32L4 memory map.
// see bugtabs4 #5265
#ifdef SYS_TP_MCU_STM32L4
#ifdef STM32L431xx

#define SKP_PRWR_SCR_CWUF_1_5    0x1FU
#define SKP_FLASH_SR_OPTVERR     (0x1U << (15U))

#endif
#endif

#endif /* SYSTARGETPLATFORM_H_ */
