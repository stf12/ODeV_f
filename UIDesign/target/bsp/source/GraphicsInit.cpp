/**
  ******************************************************************************
  * @file    GraphicsInit.cpp
  * @author  MCD Application Team
  * @version $VERSION$
  * @date    $DATE$
  * @brief   Graphics resources initialization for STM32L4R9I_EVAL - MB1313 Eval
  *          board with the MB1314 GVO_390x390 LCD board
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
#include <bsp/ts.h>

#ifndef USE_GVO_390x390
#error This Graphics Initialization is for GVO 390x390 LCD round screen! Please check your app configuration file!
#endif

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern "C"
{
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_gfxmmu.h"
#include "stm32l4xx_hal_ltdc.h"
#include "stm32l4xx_hal_dsi.h"

    /* Eval includes component */
#include "stm32l4r9i_eval.h"
#include "stm32l4r9i_eval_io.h"
#include "stm32l4r9i_eval_ts.h"
#include "stm32l4r9i_eval_ospi_nor.h"

    LTDC_HandleTypeDef                   hltdc = { 0 };
    GFXMMU_HandleTypeDef                 hgfxmmu = { 0 };
    DSI_HandleTypeDef                    hdsi = { 0 };
    DMA2D_HandleTypeDef                  hdma2d = { 0 };
    void OS_SysTick(void);
}

/* Private function prototypes -----------------------------------------------*/
static void LCD_Reset(void);
static void LCD_Init();
static void Error_Handler(void);

/* Private functions ---------------------------------------------------------*/
using namespace touchgfx;

LOCATION_FRAMEBUFFER_PRAGMA
uint8_t _frameBuf0[GUI_FRAME_BUFFER_SIZE] LOCATION_FRAMEBUFFER_ATTRIBUTE;

uint8_t* frameBuf0 = (uint8_t*)GFXMMU_VIRTUAL_BUFFER0_BASE;

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
static void Error_Handler(void)
{
    //configASSERT(0);
    while (1);
}

/**
  * @brief  LCD Reset
  *         Hw reset the LCD DSI activating its XRES signal (active low for some time)
  *         and desactivating it later.
  */
static void LCD_Reset(void)
{
    /* Reset the LCD by activation of XRES (active low) */
    BSP_IO_Init();

    /* Configure the GPIO connected to XRES signal */
    BSP_IO_ConfigPin(IO_PIN_9, IO_MODE_OUTPUT);

    /* Activate XRES (active low) */
    BSP_IO_WritePin(IO_PIN_9, GPIO_PIN_RESET);

    /* Wait at least 1 ms (reset low pulse width) */
    HAL_Delay(2);

    /* Desactivate XRES */
    BSP_IO_WritePin(IO_PIN_9, GPIO_PIN_SET);

    /* Wait reset complete time (maximum time is 5ms when LCD in sleep mode and 120ms when LCD is not in sleep mode) */
    HAL_Delay(120);
}

static void LCD_Init()
{
    uint8_t ScanLineParams[2];
    uint16_t scanline = (GFXMMU_LCD_SIZE - 10);

    DSI_PLLInitTypeDef      dsiPllInit;
    DSI_PHY_TimerTypeDef    PhyTimings;
    DSI_HOST_TimeoutTypeDef HostTimeouts;
    DSI_LPCmdTypeDef        LPCmd;
    DSI_CmdCfgTypeDef       CmdCfg;
    LTDC_LayerCfgTypeDef    LayerCfg;

    /* Toggle Hardware Reset of the DSI LCD using its XRES signal (active low) */
    LCD_Reset();

    /************************/
    /* GFXMMU CONFIGURATION */
    /************************/
    hgfxmmu.Instance = GFXMMU;
    __HAL_GFXMMU_RESET_HANDLE_STATE(&hgfxmmu);
    hgfxmmu.Init.BlocksPerLine = GFXMMU_192BLOCKS;
    hgfxmmu.Init.DefaultValue = 0xFFFFFFFF;
    hgfxmmu.Init.Buffers.Buf0Address = (uint32_t)_frameBuf0;
#if (GUI_NUM_FRAME_BUFFERS > 1)
    hgfxmmu.Init.Buffers.Buf1Address = (uint32_t)(_frameBuf0 + (1 * GUI_FRAME_BUFFER_SIZE));
#endif /* GUI_NUM_FRAME_BUFFERS > 1 */
#if (GUI_NUM_FRAME_BUFFERS > 2)
    hgfxmmu.Init.Buffers.Buf2Address = (uint32_t)(_frameBuf0 + (2 * GUI_FRAME_BUFFER_SIZE));
#endif /* GUI_NUM_FRAME_BUFFERS > 2 */
#if (GUI_NUM_FRAME_BUFFERS > 3)
    hgfxmmu.Init.Buffers.Buf3Address = (uint32_t)(_frameBuf0 + (3 * GUI_FRAME_BUFFER_SIZE));
#endif /* GUI_NUM_FRAME_BUFFERS > 3 */
    hgfxmmu.Init.Interrupts.Activation = DISABLE;
    hgfxmmu.Init.Interrupts.UsedInterrupts = GFXMMU_AHB_MASTER_ERROR_IT; /* NU */
    if (HAL_OK != HAL_GFXMMU_Init(&hgfxmmu))
    {
        Error_Handler();
    }

    /* Initialize LUT */
    if (HAL_OK != HAL_GFXMMU_ConfigLut(&hgfxmmu, 0, GFXMMU_LCD_SIZE, (uint32_t)gfxmmu_lut_config))
    {
        Error_Handler();
    }

    /**********************/
    /* LTDC CONFIGURATION */
    /**********************/

    /* LTDC initialization */
    hltdc.Instance = LTDC;
    __HAL_LTDC_RESET_HANDLE_STATE(&hltdc);
    hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
    hltdc.Init.HorizontalSync = 0;   /* HSYNC width - 1 */
    hltdc.Init.VerticalSync = 0;   /* VSYNC width - 1 */
    hltdc.Init.AccumulatedHBP = 1;   /* HSYNC width + HBP - 1 */
    hltdc.Init.AccumulatedVBP = 1;   /* VSYNC width + VBP - 1 */
    hltdc.Init.AccumulatedActiveW = GFXMMU_LCD_SIZE + 1; /* HSYNC width + HBP + Active width - 1 */
    hltdc.Init.AccumulatedActiveH = GFXMMU_LCD_SIZE + 1; /* VSYNC width + VBP + Active height - 1 */
    hltdc.Init.TotalWidth = GFXMMU_LCD_SIZE + 2; /* HSYNC width + HBP + Active width + HFP - 1 */
    hltdc.Init.TotalHeigh = GFXMMU_LCD_SIZE + 2; /* VSYNC width + VBP + Active height + VFP - 1 */
    hltdc.Init.Backcolor.Red = 0; /* NU default value */
    hltdc.Init.Backcolor.Green = 0; /* NU default value */
    hltdc.Init.Backcolor.Blue = 0; /* NU default value */
    hltdc.Init.Backcolor.Reserved = 0xFF;
    if (HAL_LTDC_Init(&hltdc) != HAL_OK)
    {
        Error_Handler();
    }

    /* LTDC layers configuration */
    LayerCfg.WindowX0 = 0;
    LayerCfg.WindowX1 = GFXMMU_LCD_SIZE;
    LayerCfg.WindowY0 = 0;
    LayerCfg.WindowY1 = GFXMMU_LCD_SIZE;
    LayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB888;
    LayerCfg.Alpha = 0xFF; /* NU default value */
    LayerCfg.Alpha0 = 0; /* NU default value */
    LayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA; /* NU default value */
    LayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA; /* NU default value */
    LayerCfg.FBStartAdress = GFXMMU_VIRTUAL_BUFFER0_BASE;
    LayerCfg.ImageWidth = BSP_LCD_IMAGE_WIDTH; /* virtual frame buffer contains 768 pixels per line for 32bpp */
    LayerCfg.ImageHeight = GFXMMU_LCD_SIZE;
    LayerCfg.Backcolor.Red = 0; /* NU default value */
    LayerCfg.Backcolor.Green = 0; /* NU default value */
    LayerCfg.Backcolor.Blue = 0; /* NU default value */
    LayerCfg.Backcolor.Reserved = 0xFF;
    if (HAL_LTDC_ConfigLayer(&hltdc, &LayerCfg, 0) != HAL_OK)
    {
        Error_Handler();
    }

    /*********************/
    /* DSI CONFIGURATION */
    /*********************/

    /* DSI initialization */
    hdsi.Instance = DSI;
    /* PATCH PNA */
    //  __HAL_DSI_RESET_HANDLE_STATE(&hdsi);
    hdsi.State = HAL_DSI_STATE_RESET;
    /* END PATCH PNA */
    hdsi.Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
    /* We have 1 data lane at 500Mbps => lane byte clock at 500/8 = 62,5 MHZ */
    /* We want TX escape clock at arround 20MHz and under 20MHz so clock division is set to 4 */
    hdsi.Init.TXEscapeCkdiv = 4;
    hdsi.Init.NumberOfLanes = DSI_ONE_DATA_LANE;
    /* We have HSE value at 25MHz and we want data lane at 500Mbps */
    dsiPllInit.PLLNDIV = 100;
    dsiPllInit.PLLIDF = DSI_PLL_IN_DIV5;
    dsiPllInit.PLLODF = DSI_PLL_OUT_DIV1;
    if (HAL_DSI_Init(&hdsi, &dsiPllInit) != HAL_OK)
    {
        Error_Handler();
    }

    PhyTimings.ClockLaneHS2LPTime = 33; /* Tclk-post + Tclk-trail + Ths-exit = [(60ns + 52xUI) + (60ns) + (300ns)]/16ns */
    PhyTimings.ClockLaneLP2HSTime = 30; /* Tlpx + (Tclk-prepare + Tclk-zero) + Tclk-pre = [150ns + 300ns + 8xUI]/16ns */
    PhyTimings.DataLaneHS2LPTime = 11; /* Ths-trail + Ths-exit = [(60ns + 4xUI) + 100ns]/16ns */
    PhyTimings.DataLaneLP2HSTime = 21; /* Tlpx + (Ths-prepare + Ths-zero) + Ths-sync = [150ns + (145ns + 10xUI) + 8xUI]/16ns */
    PhyTimings.DataLaneMaxReadTime = 0;
    PhyTimings.StopWaitTime = 7;
    if (HAL_DSI_ConfigPhyTimer(&hdsi, &PhyTimings) != HAL_OK)
    {
        Error_Handler();
    }

    HostTimeouts.TimeoutCkdiv = 1;
    HostTimeouts.HighSpeedTransmissionTimeout = 0;
    HostTimeouts.LowPowerReceptionTimeout = 0;
    HostTimeouts.HighSpeedReadTimeout = 0;
    HostTimeouts.LowPowerReadTimeout = 0;
    HostTimeouts.HighSpeedWriteTimeout = 0;
    HostTimeouts.HighSpeedWritePrespMode = 0;
    HostTimeouts.LowPowerWriteTimeout = 0;
    HostTimeouts.BTATimeout = 0;
    if (HAL_DSI_ConfigHostTimeouts(&hdsi, &HostTimeouts) != HAL_OK)
    {
        Error_Handler();
    }

    LPCmd.LPGenShortWriteNoP = DSI_LP_GSW0P_ENABLE;
    LPCmd.LPGenShortWriteOneP = DSI_LP_GSW1P_ENABLE;
    LPCmd.LPGenShortWriteTwoP = DSI_LP_GSW2P_ENABLE;
    LPCmd.LPGenShortReadNoP = DSI_LP_GSR0P_ENABLE;
    LPCmd.LPGenShortReadOneP = DSI_LP_GSR1P_ENABLE;
    LPCmd.LPGenShortReadTwoP = DSI_LP_GSR2P_ENABLE;
    LPCmd.LPGenLongWrite = DSI_LP_GLW_DISABLE;
    LPCmd.LPDcsShortWriteNoP = DSI_LP_DSW0P_ENABLE;
    LPCmd.LPDcsShortWriteOneP = DSI_LP_DSW1P_ENABLE;
    LPCmd.LPDcsShortReadNoP = DSI_LP_DSR0P_ENABLE;
    LPCmd.LPDcsLongWrite = DSI_LP_DLW_DISABLE;
    LPCmd.LPMaxReadPacket = DSI_LP_MRDP_DISABLE;
    LPCmd.AcknowledgeRequest = DSI_ACKNOWLEDGE_DISABLE;
    if (HAL_DSI_ConfigCommand(&hdsi, &LPCmd) != HAL_OK)
    {
        Error_Handler();
    }

    CmdCfg.VirtualChannelID = 0;
    CmdCfg.ColorCoding = DSI_RGB888;
    CmdCfg.CommandSize = GFXMMU_LCD_SIZE;
    CmdCfg.TearingEffectSource = DSI_TE_EXTERNAL;
    CmdCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
    CmdCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
    CmdCfg.AutomaticRefresh = DSI_AR_DISABLE;
    CmdCfg.TearingEffectPolarity = DSI_TE_FALLING_EDGE;
    CmdCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
    CmdCfg.VSyncPol = DSI_VSYNC_FALLING;
    CmdCfg.TEAcknowledgeRequest = DSI_TE_ACKNOWLEDGE_ENABLE;
    if (HAL_DSI_ConfigAdaptedCommandMode(&hdsi, &CmdCfg) != HAL_OK)
    {
        Error_Handler();
    }

    //  /* Disable the Tearing Effect interrupt activated by default on previous function */
    //  __HAL_DSI_DISABLE_IT(&hdsi, DSI_IT_TE);

    if (HAL_DSI_ConfigFlowControl(&hdsi, DSI_FLOW_CONTROL_BTA) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable DSI */
    __HAL_DSI_ENABLE(&hdsi);

    /*************************/
    /* LCD POWER ON SEQUENCE */
    /*************************/
    /* Step 1 */
    /* Go to command 2 */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x01);
    /* IC Frame rate control, set power, sw mapping, mux swithc timing command */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x06, 0x62);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0E, 0x80);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0F, 0x80);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x10, 0x71);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x13, 0x81);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x14, 0x81);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x15, 0x82);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x16, 0x82);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x18, 0x88);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x19, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1A, 0x10);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1C, 0x99);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1D, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1E, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1F, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x20, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x25, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x26, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2A, 0x03);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2B, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x36, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x37, 0x10);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3A, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3B, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3D, 0x20);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x3F, 0x3A);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x40, 0x30);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x41, 0x1A);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x42, 0x33);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x43, 0x22);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x44, 0x11);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x45, 0x66);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x46, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x47, 0x44);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4C, 0x33);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4D, 0x22);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4E, 0x11);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x4F, 0x66);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x50, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x51, 0x44);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x57, 0x33);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x6B, 0x1B);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x70, 0x55);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x74, 0x0C);

    /* Go to command 3 */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x02);
    /* Set the VGMP/VGSP coltage control */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9B, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9C, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9D, 0x20);

    /* Go to command 4 */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x03);
    /* Set the VGMP/VGSP coltage control */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9B, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9C, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x9D, 0x20);

    /* Go to command 5 */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x04);
    /* VSR command */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x5D, 0x10);
    /* VSR1 timing set */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x00, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x01, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x02, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x03, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x04, 0x10);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x05, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x06, 0xA7);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x07, 0x20);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x08, 0x00);
    /* VSR2 timing set */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x09, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0A, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0B, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0C, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0D, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0E, 0x06);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x0F, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x10, 0xA7);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x11, 0x00);
    /* VSR3 timing set */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x12, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x13, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x14, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x15, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x16, 0x40);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x17, 0x07);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x18, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x19, 0xA7);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1A, 0x00);
    /* VSR4 timing set */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1B, 0x82);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1C, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1D, 0xFF);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1E, 0x05);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x1F, 0x60);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x20, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x21, 0x01);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x22, 0x7C);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x23, 0x00);
    /* VSR5 timing set */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x24, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x25, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x26, 0x04);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x27, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x28, 0x70);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x29, 0x05);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2A, 0x74);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2B, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2D, 0x00);
    /* VSR6 timing set */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2F, 0xC2);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x30, 0x00);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x31, 0x04);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x32, 0x02);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x33, 0x70);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x34, 0x07);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x35, 0x74);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x36, 0x8D);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x37, 0x00);
    /* VSR marping command */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x5E, 0x20);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x5F, 0x31);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x60, 0x54);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x61, 0x76);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x52, 0x98);

    /* Go to command 6 */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x05);
    /* Set the ELVSS voltage */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x05, 0x07);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x2A, 0x04);
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0x91, 0x00);

    /* Go back in standard commands */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xFE, 0x00);

    /* Set tear off */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, DSI_SET_TEAR_OFF, 0x0);

    /* Set DSI mode to internal timing added vs ORIGINAL for Command mode */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, 0xC2, 0x0);

    /* Set memory address MODIFIED vs ORIGINAL */
    uint8_t InitParam1[4] = { 0x00, 0x04, 0x01, 0x89 }; // MODIF OFe: adjusted w/ real image
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_COLUMN_ADDRESS, InitParam1);
    uint8_t InitParam2[4] = { 0x00, 0x00, 0x01, 0x85 };
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_PAGE_ADDRESS, InitParam2);

    /* Sleep out */
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P0, DSI_EXIT_SLEEP_MODE, 0x0);

    HAL_Delay(120);

    /* Set display on */
    if (HAL_DSI_ShortWrite(&hdsi,
                           0,
                           DSI_DCS_SHORT_PKT_WRITE_P0,
                           DSI_SET_DISPLAY_ON,
                           0x0) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable DSI Wrapper */
    __HAL_DSI_WRAPPER_ENABLE(&hdsi);

    ScanLineParams[0] = scanline >> 8;
    ScanLineParams[1] = scanline & 0x00FF;

    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 2, DSI_SET_TEAR_SCANLINE, ScanLineParams);
}

static void LCD_DeInit()
{
    /* Power off the LCD */
    LCD_Reset();

    if (HAL_DSI_DeInit(&hdsi) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_LTDC_DeInit(&hltdc) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_GFXMMU_DeInit(&hgfxmmu) != HAL_OK)
    {
        Error_Handler();
    }
}

/* Exported C functions ------------------------------------------------------*/
/* Graphics hardware and software resources initialization */
void GRAPHICS_Init()
{
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

    /* Initialize the Touch Screen Controller */
    if (BSP_TS_Init(GUI_DISPLAY_WIDTH, GUI_DISPLAY_HEIGHT) == TS_OK)
    {
        /* Enable TS interrupt */
        if (BSP_TS_ITConfig() != TS_OK)
        {
            Error_Handler();
        }
        TouchTaskInit();
    }
    else
    {
        Error_Handler();
    }
}

void GRAPHICS_DeInit()
{
    /* DeInitialize the LCD Display */
    LCD_DeInit();

    /* Stop and then deInitialize the Touch Screen Controller */
    //BSP_TS_ITClear();
    BSP_TS_DeInit();

    /* DeInitialize the NOR flash */
    BSP_OSPI_NOR_DeInit();
    BSP_OSPI_NOR_EnableMemoryMappedMode();
}

extern "C"
void BSP_ErrorHandler(void)
{
}

/**
 * Request TE at scanline.
 */
extern "C"
void LCD_ReqTear(void)
{
    static uint8_t ScanLineParams[2];

    uint16_t scanline = (GFXMMU_LCD_SIZE - 10);
    ScanLineParams[0] = scanline >> 8;
    ScanLineParams[1] = scanline & 0x00FF;

    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 2, DSI_SET_TEAR_SCANLINE, ScanLineParams);
    // set_tear_on
    HAL_DSI_ShortWrite(&hdsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, DSI_SET_TEAR_ON, 0x00);
}

// Configure display to update specified region.
extern "C"
void LCD_SetUpdateRegion(int idx)
{
    //    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_COLUMN_ADDRESS, pCols[idx]);
    uint8_t InitParam1[4] = { 0x00, 0x04, 0x01, 0x89 }; // MODIF OFe: adjusted w/ real image
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_COLUMN_ADDRESS, InitParam1);
}

#if 0 // TODO : split display into 2 or 4 regions in case of tearing
extern "C"
void LCD_SetUpdateRegionLeft()
{
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_COLUMN_ADDRESS, pColLeft);
}

extern "C"
void LCD_SetUpdateRegionRight()
{
    HAL_DSI_LongWrite(&hdsi, 0, DSI_DCS_LONG_PKT_WRITE, 4, DSI_SET_COLUMN_ADDRESS, pColRight);
}
#endif

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
  * @brief  Initialize the GFXMMU MSP.
  * @param  hgfxmmu : GFXMMU handle.
  * @retval None.
  */
extern "C"
void HAL_GFXMMU_MspInit(GFXMMU_HandleTypeDef* hgfxmmu)
{
    /* Enable the GFXMMU clock */
    __HAL_RCC_GFXMMU_CLK_ENABLE();

    /* Reset of GFXMMU IP */
    __HAL_RCC_GFXMMU_FORCE_RESET();
    __HAL_RCC_GFXMMU_RELEASE_RESET();
}

/**
  * @brief  De-initialize the GFXMMU MSP.
  * @param  hgfxmmu : GFXMMU handle.
  * @retval None.
  */
extern "C"
void HAL_GFXMMU_MspDeInit(GFXMMU_HandleTypeDef* hgfxmmu)
{
    /* Enable the GFXMMU clock */
    __HAL_RCC_GFXMMU_CLK_ENABLE();

    /* Reset of GFXMMU IP */
    __HAL_RCC_GFXMMU_FORCE_RESET();
    __HAL_RCC_GFXMMU_RELEASE_RESET();

    __HAL_RCC_GFXMMU_CLK_DISABLE();
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
    /* Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();

    /* Reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();

    /* Configure the clock for the LTDC */
    /* We want DSI PHI at 500MHz */
    /* We have only one line => 500Mbps */
    /* With 24bits per pixel, equivalent PCLK is 500/24 = 20.8MHz */
    /* We will set PCLK at 15MHz */ /* FIXED BY OFe to 21MHz */
    /* Following values are OK with MSI = 4MHz */
    /* (4*60)/(1*4*4) = 15MHz */
    RCC_PeriphCLKInitTypeDef  PeriphClkInit;
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
    PeriphClkInit.PLLSAI2.PLLSAI2Source = RCC_PLLSOURCE_MSI;
    PeriphClkInit.PLLSAI2.PLLSAI2M = 1;
    PeriphClkInit.PLLSAI2.PLLSAI2N = 84; /* OFe :84 /60 for 15 MHz (80 MHZ for MCU) */
    PeriphClkInit.PLLSAI2.PLLSAI2R = RCC_PLLR_DIV4;
    PeriphClkInit.LtdcClockSelection = RCC_LTDCCLKSOURCE_PLLSAI2_DIV4;
    PeriphClkInit.PLLSAI2.PLLSAI2ClockOut = RCC_PLLSAI2_LTDCCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
        while (1);
    }

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

/**
  * @brief  Initializes the DSI MSP.
  * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
  *               the configuration information for the DSI.
  * @retval None
  */
extern "C"
void HAL_DSI_MspInit(DSI_HandleTypeDef* hdsi)
{
    GPIO_InitTypeDef  gpio_init_structure;

    /* Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();

    /* Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();

    /* Enable HSE used for DSI PLL */
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
    if (RCC_OscInitStruct.HSEState == RCC_HSE_OFF)
    {
        /* Workaround for long HSE startup time (set PH0 to ouput PP low) */
        GPIO_InitTypeDef  GPIO_InitStruct;
        __HAL_RCC_GPIOH_CLK_ENABLE();
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Pin = GPIO_PIN_0;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);
        HAL_GPIO_WritePin(GPIOH, GPIO_PIN_0, GPIO_PIN_RESET);

        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            while (1);
        }
    }

    /* Configure PG6 in AF11 DSIHOST_TE */
    __HAL_RCC_GPIOF_CLK_ENABLE();
    gpio_init_structure.Mode = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull = GPIO_NOPULL;
    gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF11_DSI;
    gpio_init_structure.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOF, &gpio_init_structure);

    /* NVIC configuration for DSI interrupt that is now enabled */
    HAL_NVIC_SetPriority(DSI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DSI_IRQn);
}

/**
  * @brief  De-initializes the DSI MSP.
  * @param  hdsi: pointer to a DSI_HandleTypeDef structure that contains
  *               the configuration information for the DSI.
  * @retval None
  */
extern "C"
void HAL_DSI_MspDeInit(DSI_HandleTypeDef* hdsi)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    GPIO_InitTypeDef  gpio_init_structure;

    /* Disable IRQ of DSI IP */
    HAL_NVIC_DisableIRQ(DSI_IRQn);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        while (1);
    }

    /* GPIOF deactivation */
    gpio_init_structure.Pin = GPIO_PIN_11;
    HAL_GPIO_DeInit(GPIOF, gpio_init_structure.Pin);

    /* Enable DSI reset state */
    __HAL_RCC_DSI_FORCE_RESET();

    /* ##-2- Disable DSI Clock ##################################################*/
    __HAL_RCC_DSI_CLK_DISABLE();
}

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
    case TS_DSI_INT_PIN:
        TouchDetectInt();
        break;

    /*      case MFX_INT_PIN:
          osMessagePut (MfxEvent, MFX_INTERRUPT_EVENT, 0); //For SD card detection
          break;*/

    default:
        break;
    }

    return;
}

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
