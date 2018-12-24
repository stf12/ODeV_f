/******************************************************************************
 *
 * @brief     This file is part of the TouchGFX 4.8.0 evaluation distribution.
 *
 * @author    Draupner Graphics A/S <http://www.touchgfx.com>
 *
 ******************************************************************************
 *
 * @section Copyright
 *
 * Copyright (C) 2014-2016 Draupner Graphics A/S <http://www.touchgfx.com>.
 * All rights reserved.
 *
 * TouchGFX is protected by international copyright laws and the knowledge of
 * this source code may not be used to write a similar product. This file may
 * only be used in accordance with a license and should not be re-
 * distributed in any way without the prior permission of Draupner Graphics.
 *
 * This is licensed software for evaluation use, any use must strictly comply
 * with the evaluation license agreement provided with delivery of the
 * TouchGFX software.
 *
 * The evaluation license agreement can be seen on www.touchgfx.com
 *
 * @section Disclaimer
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Draupner Graphics A/S has
 * no obligation to support this software. Draupner Graphics A/S is providing
 * the software "AS IS", with no express or implied warranties of any kind,
 * including, but not limited to, any implied warranties of merchantability
 * or fitness for any particular purpose or warranties against infringement
 * of any proprietary rights of a third party.
 *
 * Draupner Graphics A/S can not be held liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this software.
 *
 *****************************************************************************/
#include <touchgfx/hal/GPIO.hpp>
#include <common/TouchGFXInit.hpp>
#include <touchgfx/hal/NoDMA.hpp>
#include <touchgfx/hal/OSWrappers.hpp>
#include <platform/driver/touch/NoTouchController.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>
#include <platform/driver/lcd/LCD24bpp.hpp>

#include <GraphicsInit.h> // Graphics initialization header file

extern "C"
{
#include "stm32l4xx_hal.h"

    /* Eval includes component */
#include "stm32l4r9i_eval.h"
#include "stm32l4r9i_eval_io.h"
}

#if !defined(GUI_DISPLAY_BPP) || (GUI_DISPLAY_BPP==16)
#include <platform/driver/lcd/LCD16bpp.hpp>
#elif (GUI_DISPLAY_BPP==24)
#include <platform/driver/lcd/LCD24bpp.hpp>
#elif (GUI_DISPLAY_BPP==4)
#include <platform/driver/lcd/LCD4bpp.hpp>
#elif (GUI_DISPLAY_BPP==2)
#include <platform/driver/lcd/LCD2bpp.hpp>
#elif (GUI_DISPLAY_BPP==1)
#include <platform/driver/lcd/LCD1bpp.hpp>
#else
#error Unknown GUI_DISPLAY_BPP
#endif

// Include STM32 MCU - HAL - DMA - TC header files
#include <STM32HAL_DPI.hpp>
#include <STM32HAL_DSI.hpp>
#include <STM32DMA.hpp>
#include <STM32TouchController.hpp>
#include <STM32MCUInstrumentation.hpp>

// HAL class type definition
#if !defined(GUI_DISPLAY_INTERFACE) || (GUI_DISPLAY_INTERFACE==DISPLAY_BUS_INTERFACE)
#error DBI display interface type NOT SUPPORTED!!!
#elif (GUI_DISPLAY_INTERFACE==DISPLAY_PARALLEL_INTERFACE)
#define HAL_CLASS_TYPE    STM32HAL_DPI
#elif (GUI_DISPLAY_INTERFACE==DISPLAY_SERIAL_INTERFACE)
#define HAL_CLASS_TYPE    STM32HAL_DSI
#endif

// DMA class type definition
#if (GUI_SUPPORT_HW_ACCEL != 0)
#define DMA_CLASS_TYPE    STM32DMA
#else
#define DMA_CLASS_TYPE    NoDMA
#endif

// TouchScreen Controller class type definition
#if (GUI_SUPPORT_TOUCH != 0)
#define TSC_CLASS_TYPE    STM32TouchController
#else
#define TSC_CLASS_TYPE    NoTouchController
#endif

// MCU Instruentation class type definition
#if (GUI_SUPPORT_MCU_INSTRUMENTATION != 0)
#define MCU_INS_CLASS_TYPE    STM32MCUInstrumentation
#else
#define MCU_INS_CLASS_TYPE    CortexMMCUInstrumentation
#endif

// Framebuffer memory placement rules
#ifndef LOCATION_FRAMEBUFFER_PRAGMA
#define LOCATION_FRAMEBUFFER_PRAGMA
#endif
#ifndef LOCATION_FRAMEBUFFER_ATTRIBUTE
#define LOCATION_FRAMEBUFFER_ATTRIBUTE
#endif

#ifndef GUI_NUM_FRAME_BUFFERS
#define GUI_NUM_FRAME_BUFFERS 1
#endif

#ifndef GUI_FRAME_BUFFER_WIDTH
#define GUI_FRAME_BUFFER_WIDTH 480
#endif

#ifndef GUI_FRAME_BUFFER_HEIGHT
#define GUI_FRAME_BUFFER_HEIGHT 272
#endif

#ifndef GUI_DISPLAY_WIDTH
#define GUI_DISPLAY_WIDTH 480
#endif

#ifndef GUI_DISPLAY_HEIGHT
#define GUI_DISPLAY_HEIGHT 272
#endif

static void SystemClock_Config(void);

/***********************************************************
 ******   Single buffer in internal RAM              *******
 ***********************************************************
 * On this platform, TouchGFX is able to run using a single
 * frame buffer in internal SRAM, thereby avoiding the need
 * for external SDRAM.
 * This feature was introduced in TouchGFX 4.7.0. To enable it,
 * uncomment the define below. The function touchgfx_init()
 * later in this file will check for this define and configure
 * TouchGFX accordingly.
 * For details on the single buffer strategy, please refer to
 * the knowledge base article "Single vs double buffering in TouchGFX"
 * on our support site.
 */
#if !defined(GUI_DISPLAY_BPP) || GUI_DISPLAY_BPP==16
LCD16bpp display;
#elif GUI_DISPLAY_BPP==24
LCD24bpp display;
#elif GUI_DISPLAY_BPP==4
LCD4bpp display;
#elif GUI_DISPLAY_BPP==2
LCD2bpp display;
#else
LCD1bpp display;
#endif

MCU_INS_CLASS_TYPE mcuInstr;
TSC_CLASS_TYPE tc;
DMA_CLASS_TYPE dma;

/***********************************************************
 ******         24 Bits Per Pixel Support            *******
 ***********************************************************
 *
 * The default bit depth of the framebuffer is 16bpp. If you want 24bpp support, define the symbol "GUI_DISPLAY_BPP" with a value
 * of "24", e.g. "GUI_DISPLAY_BPP=24". This symbol affects the following:
 *
 * 1. Type of TouchGFX LCD (16bpp vs 24bpp)
 * 2. Bit depth of the framebuffer(s)
 * 3. TFT controller configuration.
 *
 * WARNING: Remember to modify your image formats accordingly in app/config/. Please see the following knowledgebase article
 * for further details on how to choose and configure the appropriate image formats for your application:
 *
 * https://touchgfx.zendesk.com/hc/en-us/articles/206725849
 */
#if 0 // No Dynamic Bitmaps support right now
#ifdef __GNUC__
#ifdef __CODE_RED
#include <cr_section_macros.h>
#define LOCATION_DYNP_PRAGMA
#define LOCATION_DYNP_ATTRIBUTE __NOLOAD(SRAM)
#else
#define LOCATION_DYNP_PRAGMA
#define LOCATION_DYNP_ATTRIBUTE __attribute__ ((section ("DynBitmapsSection"))) __attribute__ ((aligned(8)))
#endif
#elif defined __ICCARM__
#define LOCATION_DYNP_PRAGMA _Pragma("location=\"DynBitmapsSection\"") _Pragma("data_alignment=8")
#define LOCATION_DYNP_ATTRIBUTE
#elif defined(__ARMCC_VERSION)
#define LOCATION_DYNP_PRAGMA
#define LOCATION_DYNP_ATTRIBUTE __attribute__ ((section ("DynBitmapsSection"))) __attribute__ ((aligned(8)))
#endif

static const uint8_t MAX_DYN_COVERS = 0;
static const uint32_t DYN_SIZE = 221 * 221 * 4 * MAX_DYN_COVERS + 221 + 1023; /* 192 Kbytes */
LOCATION_DYNP_PRAGMA
uint8_t dynBitmapsBuffer[DYN_SIZE] LOCATION_DYNP_ATTRIBUTE;
#endif

namespace touchgfx
{
void hw_init()
{
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Enable RTC back-up registers access */
    HAL_PWR_EnableBkUpAccess();

    /* STM32L4xx HAL library initialization:
    - Configure the Flash prefetch, Instruction cache, Data cache
    - Systick timer is configured by default as source of time base, but user
      can eventually implement his proper time base source (a general purpose
      timer for example or other time source), keeping in mind that Time base
      duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
      handled in milliseconds basis.
    - Set NVIC Group Priority to 4
    - Low Level Initialization */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    //Enable CRC engine for STM32 Lock check
    __HAL_RCC_CRC_CLK_ENABLE();

    BSP_IO_Init();

    /* Configure LED1, LED2, LED3 and LED4 */
    BSP_LED_Init(LED1);
    BSP_LED_Init(LED2);
    BSP_LED_Init(LED3);
    BSP_LED_Init(LED4);

    /* Configure the GPIOs for MFX */
    MFX_IO_Init();

    /* Initialize Graphics hardware resources */
    GRAPHICS_Init();
    GPIO::init();
}

void touchgfx_init()
{
    HAL& hal = touchgfx_generic_init<HAL_CLASS_TYPE>(dma, display, tc, GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT, 0, 0, 0);

#if (GUI_SUPPORT_DOUBLE_BUFFERING == 1)
    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0, GUI_DISPLAY_BPP, true, false);
#else // Single buffer configuration
    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0, GUI_DISPLAY_BPP, false, false);
#endif

    hal.setTouchSampleRate(2);
    hal.setFingerSize(3);

    // By default frame rate compensation is off.
    // Enable frame rate compensation to smooth out animations in case there is periodic slow frame rates.
    hal.setFrameRateCompensation(false);

    // This platform can handle simultaneous DMA and TFT accesses to SDRAM, so disable lock to increase performance.
    hal.lockDMAToFrontPorch(false);

    mcuInstr.init();

    //Set MCU instrumentation and Load calculation
    hal.setMCUInstrumentation(&mcuInstr);
    hal.enableMCULoadCalculation(true);
}
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *
  *         If define USB_USE_LSE_MSI_CLOCK enabled:
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 12
  *            PLL_N                          = 60
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 4
  *
  *         If define USB_USE_HSI48_CLOCK enabled:
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 120000000
  *            HCLK(Hz)                       = 120000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 60
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 7
  *            Flash Latency(WS)              = 4
  *
  * @param  None
  * @retval None
  */
#define USB_USE_HSI48_CLOCK
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

#if defined (USB_USE_HSI48_CLOCK)
    static RCC_CRSInitTypeDef RCC_CRSInitStruct;
#endif

    /* Enable voltage range 1 boost mode for frequency above 80 Mhz */
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
    __HAL_RCC_PWR_CLK_DISABLE();

#if defined (USB_USE_HSI48_CLOCK)

    /* Enable the LSE Oscilator */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
    RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Enable MSI Oscillator and activate PLL with MSI as source   */
    /* (Default MSI Oscillator enabled at system reset remains ON) */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 60;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* Enable MSI Auto-calibration through LSE */
    HAL_RCCEx_EnableMSIPLLMode();

    /* Select HSI84 output as USB clock source */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
    /* with AHB prescaler divider 2 as first step */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* AHB prescaler divider at 1 as second step */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /*Configure the clock recovery system (CRS)**********************************/

    /* Enable CRS Clock */
    __HAL_RCC_CRS_CLK_ENABLE();

    /* Default Synchro Signal division factor (not divided) */
    RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

    /* Set the SYNCSRC[1:0] bits according to CRS_Source value */
    RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

    /* HSI48 is synchronized with USB SOF at 1KHz rate */
    RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
    RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

    /* Set the TRIM[5:0] to the default value*/
    RCC_CRSInitStruct.HSI48CalibrationValue = 0x20;

    /* Start automatic synchronization */
    HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

#elif defined (USB_USE_LSE_MSI_CLOCK)

    /* Enable MSI Oscillator and activate PLL with MSI as source   */
    /* (Default MSI Oscillator enabled at system reset remains ON) */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11; /*!< MSI = 48 MHz. Do not modify this value used as SAI Source, MMC  */
    RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 12;
    RCC_OscInitStruct.PLL.PLLN = 60;
    RCC_OscInitStruct.PLL.PLLR = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLP = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* Enable MSI Auto-calibration through LSE */
    HAL_RCCEx_EnableMSIPLLMode();

    /* Select MSI output as USB clock source */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_MSI;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
    /* with AHB prescaler divider 2 as first step */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* AHB prescaler divider at 1 as second step */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

#endif /* USB_USE_HSI48_CLOCK */
}
