/**
  ******************************************************************************
  * @file    GraphicsInit.cpp
  * @author  MCD Application Team
  * @version $VERSION$
  * @date    $DATE$
  * @brief   Graphics resources initialization for STM32L4R9I_EVAL - MB1313 Eval
  *          board with the MB1315 ROCKTECH_480x272 LCD board
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup Application
  * @{
  */

/** @addtogroup JPEG Viewer
  * @{
  */

/** @addtogroup MAIN_GUI
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <touchgfx/hal/OSWrappers.hpp>

#include "GraphicsInit.h"
#include "FreeRTOS.h"
#include "task.h"

 #ifndef USE_ROCKTECH_480x272
 #error This Graphics Initialization is for RGB 480x272 LCD screen! Please check your app configuration file!
 #endif

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern "C"
{
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_ltdc.h"

/* Eval includes component */
#include "stm32l4r9i_eval.h"
#include "stm32l4r9i_eval_io.h"
#include "stm32l4r9i_eval_ts.h"
#include "stm32l4r9i_eval_ospi_nor.h"
#include "mfx.h"

#include "../Components/rk043fn48h/rk043fn48h.h"

    LTDC_HandleTypeDef                   hltdc = { 0 };
    DMA2D_HandleTypeDef                  hdma2d = { 0 };
    void OS_SysTick(void);
}

/* Private function prototypes -----------------------------------------------*/
static void LCD_Init();
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
using namespace touchgfx;

LOCATION_FRAMEBUFFER_PRAGMA
uint8_t _frameBuf0[GUI_FRAME_BUFFER_SIZE] LOCATION_FRAMEBUFFER_ATTRIBUTE;

uint8_t* frameBuf0 = _frameBuf0;

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
    BSP_LED_On(LED_RED);
    configASSERT(0);
}

static void LCD_Init()
{
    LTDC_LayerCfgTypeDef  Layercfg = {0};
    RCC_PeriphCLKInitTypeDef  PeriphClkInit = {0};

    /* Select the used LCD */

    /* The RK043FN48H LCD 480x272 is selected */
    /* Timing Configuration */
    hltdc.Init.HorizontalSync = (RK043FN48H_HSYNC - 1);
    hltdc.Init.VerticalSync = (RK043FN48H_VSYNC - 1);
    hltdc.Init.AccumulatedHBP = (RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hltdc.Init.AccumulatedVBP = (RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hltdc.Init.AccumulatedActiveH = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP - 1);
    hltdc.Init.AccumulatedActiveW = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP - 1);
    hltdc.Init.TotalHeigh = (RK043FN48H_HEIGHT + RK043FN48H_VSYNC + RK043FN48H_VBP + RK043FN48H_VFP - 1);
    hltdc.Init.TotalWidth = (RK043FN48H_WIDTH + RK043FN48H_HSYNC + RK043FN48H_HBP + RK043FN48H_HFP - 1);

    /* LCD clock configuration */
    /* RK043FN48H LCD clock configuration */
    /* PLLSAI2_VCO Input = MSI_VALUE/PLLSAI2_M = 4 Mhz */
    /* PLLSAI2_VCO Output = PLLSAI2_VCO Input * PLLSAI2N = 192 Mhz */
    /* PLLLCDCLK = PLLSAI2_VCO Output/PLLSAIR = 160/4 = 40 Mhz */
    /* LTDC clock frequency = PLLLCDCLK / LTDC_PLLSAI_DIVR_4 = 40/4 = 10Mhz */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInit.PLLSAI2.PLLSAI2Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI2.PLLSAI2M = 1;
    PeriphClkInit.PLLSAI2.PLLSAI2N = 40;
    PeriphClkInit.PLLSAI2.PLLSAI2R = 4;
    PeriphClkInit.PLLSAI2.PLLSAI2ClockOut = RCC_PLLSAI2_LTDCCLK;
    PeriphClkInit.LtdcClockSelection = RCC_LTDCCLKSOURCE_PLLSAI2_DIV4;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      while(1);
    }

    /* Initialize the LCD pixel width and pixel height */
    hltdc.LayerCfg->ImageWidth  = RK043FN48H_WIDTH;
    hltdc.LayerCfg->ImageHeight = RK043FN48H_HEIGHT;

    /* Background value */
    hltdc.Init.Backcolor.Blue = 0;
    hltdc.Init.Backcolor.Green = 0;
    hltdc.Init.Backcolor.Red = 0;

    /* Polarity */
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc.Instance = LTDC;

    HAL_LTDC_Init(&hltdc);

    Layercfg.WindowX0 = 0;
    Layercfg.WindowX1 = GUI_DISPLAY_WIDTH;
    Layercfg.WindowY0 = 0;
    Layercfg.WindowY1 = GUI_DISPLAY_HEIGHT;
    Layercfg.FBStartAdress = (uint32_t)frameBuf0;
    Layercfg.Alpha = 255;
    Layercfg.Alpha0 = 0;
    Layercfg.Backcolor.Blue = 0;
    Layercfg.Backcolor.Green = 0;
    Layercfg.Backcolor.Red = 0;
    Layercfg.ImageWidth = GUI_DISPLAY_WIDTH;
    Layercfg.ImageHeight = GUI_DISPLAY_HEIGHT;

#if !defined(GUI_DISPLAY_BPP) || GUI_DISPLAY_BPP==16
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
#elif GUI_DISPLAY_BPP==24
    //Redefine pixel format from 16bpp to 24bpp
    Layercfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
    Layercfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
    Layercfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
#else
#error Unknown GUI_DISPLAY_BPP
#endif

    HAL_LTDC_ConfigLayer(&hltdc, &Layercfg, 0);

    /* Screen initialization - Display ON */
    __HAL_LTDC_ENABLE(&hltdc);
    BSP_IO_WritePin(IO_PIN_12, IO_PIN_RESET );
    HAL_Delay(2);
    BSP_IO_WritePin(IO_PIN_12, IO_PIN_SET );

    /* Assert display enable LCD_DISP pin via MFX expander */
    BSP_IO_WritePin(IO_PIN_11, IO_PIN_SET);

    __HAL_LTDC_LAYER_ENABLE(&hltdc, 0);
    __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);

    /* Assert backlight LCD_BL_CTRL pin */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

static void LCD_DeInit()
{
    // TODO : Complete this!
}

/* Exported C functions ------------------------------------------------------*/
/* Graphics hardware and software resources initialization */
void GRAPHICS_Init()
{
    int nb_tries = 5;

    /* Initialize the NOR OctoSPI flash */
    if (BSP_OSPI_NOR_Init() != OSPI_NOR_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
    else
    {
        if (BSP_OSPI_NOR_EnableMemoryMappedMode() != OSPI_NOR_OK)
        {
            /* Initialization Error */
            Error_Handler();
        }
    }

    /* Initialize the LCD Display */
    LCD_Init();

    //TODO: STF.Debug - exclude some code when compiled for ODeV.
#ifndef ODEV_F
    /* Initialize the Touch Screen Controller */
    while(nb_tries > 0)
    {
        if(BSP_TS_Init(GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT) == TS_OK)
            break;
        nb_tries--;
    }

    if (nb_tries < 0)
    {
        /* Initialization Error */
        Error_Handler();
    }
#endif
}

void GRAPHICS_DeInit()
{
    /* DeInitialize the LCD Display */
    LCD_DeInit();

    //TODO: STF.Debug - exclude some code when compiled for ODeV.
#ifndef ODEV_F
    /* Stop and then deInitialize the Touch Screen Controller */
    //BSP_TS_ITClear();
    BSP_TS_DeInit();
#endif

    /* DeInitialize the NOR flash */
    BSP_OSPI_NOR_DeInit();
    BSP_OSPI_NOR_EnableMemoryMappedMode();
}

extern "C"
void BSP_ErrorHandler(void)
{
}

//TODO: STF.Debug - exclude some code when compiled for ODeV.
// It is managed by ODeV framework
#ifndef ODEV_F
/* Update Graphics upon SysTick event reception */
void GRAPHICS_ProcessSysTickEvent()
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
        OS_SysTick();
    }
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern "C"
void SysTick_Handler(void)
{
#if (GUI_OS_USE_SAME_HAL_TIMEBASE == 1)
    // Increment HAL tick counter
    HAL_IncTick();
#endif // (GUI_OS_USE_SAME_HAL_TIMEBASE == 1)
    GRAPHICS_ProcessSysTickEvent();
}
#endif

/**
  * @brief  This function handles DMA2D interrupt request.
  * @param  None
  * @retval None
  */
extern "C"
void DMA2D_IRQHandler(void)
{
    HAL_DMA2D_IRQHandler(&hdma2d);
}

/**
  * @brief  This function handles LTDC global interrupt request.
  * @param  None
  * @retval None
  */
extern "C"
void LTDC_IRQHandler(void)
{
    HAL_LTDC_IRQHandler(&hltdc);
}

/**
  * @brief DMA2D MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hdma2d: DMA2D handle pointer
  * @retval None
  */
extern "C"
void HAL_DMA2D_MspInit(DMA2D_HandleTypeDef* hdma2d)
{
    /* Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /* Reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();

    /*##-2- NVIC configuration  ################################################*/
    /* NVIC configuration for DMA2D transfer complete interrupt */
    HAL_NVIC_SetPriority(DMA2D_IRQn, 8, 0);
    HAL_NVIC_EnableIRQ(DMA2D_IRQn);
}

/**
  * @brief DMA2D MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hdma2d: DMA2D handle pointer
  * @retval None
  */
extern "C"
void HAL_DMA2D_MspDeInit(DMA2D_HandleTypeDef* hdma2d)
{
    /* Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /* Enable DMA2D reset state */
    __HAL_RCC_DMA2D_FORCE_RESET();

    /* Release DMA2D from reset state */
    __HAL_RCC_DMA2D_RELEASE_RESET();

    /* ##-2- Disable DMA2D Clock ##################################################*/
    __HAL_RCC_DMA2D_CLK_DISABLE();

    /* ##-3- Disable DMA2D IRQ in NVIC #############################################*/
    HAL_NVIC_DisableIRQ(DMA2D_IRQn);
}

/**
  * @brief LTDC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
extern "C"
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
    GPIO_InitTypeDef GPIO_Init_Structure;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable the LTDC Clock */
    __HAL_RCC_LTDC_CLK_ENABLE();
    /* IO expander init */
    BSP_IO_Init();

    /* Enable the LTDC and DMA2D clocks */
    __HAL_RCC_LTDC_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*** LTDC Pins configuration ***/
    /* GPIOD configuration */
    GPIO_Init_Structure.Pin       = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_3  | GPIO_PIN_8  | GPIO_PIN_9  | \
                                    GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull      = GPIO_NOPULL;
    GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_Init_Structure.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

    /* GPIOE configuration */
    GPIO_Init_Structure.Pin       = GPIO_PIN_0  | GPIO_PIN_1  | GPIO_PIN_2  | GPIO_PIN_3  | GPIO_PIN_4  | \
                                    GPIO_PIN_7  | GPIO_PIN_8  | GPIO_PIN_9  | GPIO_PIN_10 | GPIO_PIN_11 | \
                                    GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull      = GPIO_NOPULL;
    GPIO_Init_Structure.Speed     = GPIO_SPEED_FREQ_MEDIUM;
    GPIO_Init_Structure.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOE, &GPIO_Init_Structure);

    /* GPIOF configuration */
    GPIO_Init_Structure.Pin       = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Init_Structure.Mode      = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

    GPIO_Init_Structure.Pin       = GPIO_PIN_11;
    GPIO_Init_Structure.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOF, &GPIO_Init_Structure);

    /* LCD_DISP & LCD_RST pins configuration (over MFX IO Expander) */
    BSP_IO_ConfigPin(LCD_RGB_DISP_PIN | LCD_RGB_RST_PIN, IO_MODE_OUTPUT);

    /* LCD_BL_CTRL GPIO configuration */
    GPIO_Init_Structure.Pin       = LCD_RGB_BL_CTRL_PIN;  /* LCD_BL_CTRL pin has to be manually controlled */
    GPIO_Init_Structure.Mode      = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(LCD_RGB_BL_CTRL_GPIO_PORT, &GPIO_Init_Structure);

    /* NVIC configuration for LTDC interrupts that are now enabled */
    HAL_NVIC_SetPriority(LTDC_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
    HAL_NVIC_SetPriority(LTDC_ER_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(LTDC_ER_IRQn);
}

/**
  * @brief LTDC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  * @param hltdc: LTDC handle pointer
  * @retval None
  */
extern "C"
void HAL_LTDC_MspDeInit(LTDC_HandleTypeDef* hltdc)
{
    /* Disable IRQ of LTDC IP */
    HAL_NVIC_DisableIRQ(LTDC_IRQn);
    HAL_NVIC_DisableIRQ(LTDC_ER_IRQn);

    /* Disable LTDC block */
    __HAL_LTDC_DISABLE(hltdc);

    /* Reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();

    /* Disable LTDC clock */
    __HAL_RCC_LTDC_CLK_DISABLE();
}

#ifndef ODEV_F //TODO: STF
/**
* @brief  EXTI line detection callback.
* @param  uint16_t GPIO_Pin Specifies the pins connected EXTI line
* @retval None
*/
extern "C"
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    switch (GPIO_Pin)
    {
//        case WAKEUP_BUTTON_PIN :
//        {
//          /* Turn LED RED off */
//          BSP_LED_Off(LED_RED);
//        }
//        break;

        case TS_RGB_INT_PIN :
        {
          /* Nothing to do */
        }
        break;

        case MFX_INT_PIN:
        {
          MfxDetectInt(); //For SD card detection
        }
        break;

        default:
            break;
    }

    return;
}
#endif
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
