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

#include <touchgfx/hal/OSWrappers.hpp>
#include <touchgfx/hal/HAL.hpp>
#include <STM32DMA.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <cassert>

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_dma2d.h"

static void TransferError(DMA2D_HandleTypeDef* dma2dHandle);
static void TransferComplete(DMA2D_HandleTypeDef* dma2dHandle);

extern DMA2D_HandleTypeDef     hdma2d;

STM32DMA::STM32DMA()
    : DMA_Interface(dma_queue), dma_queue(queue_storage, sizeof(queue_storage) / sizeof(queue_storage[0])),
      pBlitOp(0), numLines(0), deadTime(0), use32Bit(0)
{}

STM32DMA::~STM32DMA()
{
    HAL_DMA2D_DeInit(&hdma2d);
    NVIC_DisableIRQ(DMA2D_IRQn);
}

void STM32DMA::initialize()
{
    hdma2d.Instance = DMA2D;
    HAL_DMA2D_Init(&hdma2d);

    // DMA2D Callbacks Configuration
    hdma2d.XferCpltCallback = TransferComplete;
    hdma2d.XferErrorCallback = TransferError;
}

volatile bool disableChromArt = false;

BlitOperations STM32DMA::getBlitCaps()
{
    if (disableChromArt)
    {
        return static_cast<BlitOperations>(0);
    }
    else
    {
        return static_cast<BlitOperations>(BLIT_OP_FILL | BLIT_OP_FILL_WITH_ALPHA | BLIT_OP_COPY | BLIT_OP_COPY_WITH_ALPHA | BLIT_OP_COPY_ARGB8888 | BLIT_OP_COPY_ARGB8888_WITH_ALPHA | BLIT_OP_COPY_A4 | BLIT_OP_COPY_A8);
    }
}

void STM32DMA::setupDataCopy(const BlitOp& blitOp)
{
    uint32_t dma2dTransferMode = DMA2D_M2M_BLEND;  //DMA2D transfer mode - Default blending.
    uint32_t dma2dColorMode = 0;                   //DMA2D foreground color mode.

    bool blendingImage = ((blitOp.operation == BLIT_OP_COPY_ARGB8888) || (blitOp.operation == BLIT_OP_COPY_ARGB8888_WITH_ALPHA) || (blitOp.operation == BLIT_OP_COPY_WITH_ALPHA));
    bool blendingText = ((blitOp.operation == BLIT_OP_COPY_A4) || (blitOp.operation == BLIT_OP_COPY_A8));

    uint8_t bitDepth = HAL::lcd().bitDepth();

    //Determine color mode and transfer mode
    switch (blitOp.operation)
    {
    case BLIT_OP_COPY_A4:
        dma2dColorMode = DMA2D_INPUT_A4;
        break;
    case BLIT_OP_COPY_A8:
        dma2dColorMode = DMA2D_INPUT_A8;
        break;
    case BLIT_OP_COPY_WITH_ALPHA:
        dma2dTransferMode = DMA2D_M2M_BLEND;
        dma2dColorMode = (bitDepth == 16) ? DMA2D_INPUT_RGB565 : DMA2D_INPUT_RGB888;
        break;
    case BLIT_OP_COPY_ARGB8888:
    case BLIT_OP_COPY_ARGB8888_WITH_ALPHA:
        dma2dColorMode = DMA2D_INPUT_ARGB8888;
        break;
    default:
        //default is to revert to color mode specified by LCD driver
        dma2dTransferMode = DMA2D_M2M;
        dma2dColorMode = (bitDepth == 16) ? DMA2D_INPUT_RGB565 : DMA2D_INPUT_RGB888;
        break;
    }

    numLines = blitOp.nLoops;

    // Don't DeInit DMA2D as this will completly shutdown the hardware which is
    // not required for each operation!

    hdma2d.Init.Mode = dma2dTransferMode;
    hdma2d.Init.ColorMode = (bitDepth == 16) ? DMA2D_OUTPUT_RGB565 : DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset = blitOp.dstLoopStride - blitOp.nSteps;
    //hdma2d.Init.AlphaInverted = DMA2D_REGULAR_ALPHA;
    //hdma2d.Init.RedBlueSwap = DMA2D_RB_REGULAR;

    hdma2d.LayerCfg[1].InputColorMode = dma2dColorMode;
    hdma2d.LayerCfg[1].InputOffset = blitOp.srcLoopStride - blitOp.nSteps;
    //hdma2d.LayerCfg[1].AlphaInverted = DMA2D_REGULAR_ALPHA;
    //hdma2d.LayerCfg[1].RedBlueSwap = DMA2D_RB_REGULAR;

    HAL_DMA2D_Init(&hdma2d);

    if (blendingImage || blendingText)
    {
        //Adjust blending for configured alpha value
        if (blitOp.alpha < 255)
        {
            hdma2d.LayerCfg[1].AlphaMode = DMA2D_COMBINE_ALPHA;
            hdma2d.LayerCfg[1].InputAlpha = blitOp.alpha;
        }
        else
        {
            hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
        }

        if (blendingText)
        {
            if (bitDepth == 16)
            {
                uint32_t red = (((blitOp.color & 0xF800) >> 11) * 255) / 31;
                uint32_t green = (((blitOp.color & 0x7E0) >> 5) * 255) / 63;
                uint32_t blue = (((blitOp.color & 0x1F)) * 255) / 31;
                uint32_t alpha = blitOp.alpha;
                hdma2d.LayerCfg[1].InputAlpha = (alpha << 24) | (red << 16) | (green << 8) | blue;
            }
            else
            {
                hdma2d.LayerCfg[1].InputAlpha = blitOp.color.getColor32() | (blitOp.alpha << 24);
            }
        }

        //Configure background
        hdma2d.LayerCfg[0].InputOffset = blitOp.dstLoopStride - blitOp.nSteps;
        hdma2d.LayerCfg[0].InputColorMode = (bitDepth == 16) ? DMA2D_INPUT_RGB565 : DMA2D_INPUT_RGB888;
        //hdma2d.LayerCfg[0].AlphaInverted = DMA2D_REGULAR_ALPHA;
        //hdma2d.LayerCfg[0].RedBlueSwap = DMA2D_RB_REGULAR;
        HAL_DMA2D_ConfigLayer(&hdma2d, 0);
    }

    HAL_DMA2D_ConfigLayer(&hdma2d, 1);

    // Ensure that interrupt is enabled.
    NVIC_EnableIRQ(DMA2D_IRQn);

    if (blendingImage || blendingText)
    {
        HAL_DMA2D_BlendingStart_IT(&hdma2d, (unsigned int)blitOp.pSrc, (unsigned int)blitOp.pDst, (unsigned int)blitOp.pDst, blitOp.nSteps, blitOp.nLoops);
    }
    else
    {
        HAL_DMA2D_Start_IT(&hdma2d, (unsigned int)blitOp.pSrc, (unsigned int)blitOp.pDst, blitOp.nSteps, blitOp.nLoops);
    }
}

void STM32DMA::setupDataFill(const BlitOp& blitOp)
{
    uint8_t bitDepth = HAL::lcd().bitDepth();
    uint32_t dma2dTransferMode = DMA2D_R2M;
    uint32_t dma2dColorMode = DMA2D_INPUT_RGB888;
    bool blendingFill = (blitOp.operation == BLIT_OP_FILL_WITH_ALPHA);
    uint32_t color = (blitOp.alpha << 24) | blitOp.color.getColor32();

    //Determine color mode and transfer mode
    switch (blitOp.operation)
    {
    case BLIT_OP_FILL_WITH_ALPHA:
        dma2dTransferMode = DMA2D_M2M_BLEND;
        dma2dColorMode = DMA2D_INPUT_A8;
        break;
    default:
        dma2dTransferMode = DMA2D_R2M;
        dma2dColorMode = (bitDepth == 16) ? DMA2D_INPUT_RGB565 : DMA2D_INPUT_RGB888;
        break;
    };

    numLines = blitOp.nLoops;

    // Don't DeInit DMA2D as this will completly shutdown the hardware which is
    // not required for each operation!

    hdma2d.Init.Mode = dma2dTransferMode;
    hdma2d.Init.ColorMode = (bitDepth == 16) ? DMA2D_OUTPUT_RGB565 : DMA2D_OUTPUT_RGB888;
    hdma2d.Init.OutputOffset = blitOp.dstLoopStride - blitOp.nSteps;

    hdma2d.LayerCfg[1].InputColorMode = dma2dColorMode;
    hdma2d.LayerCfg[1].InputOffset = 0; //Unused in register-to-memory transfers.

    HAL_DMA2D_Init(&hdma2d);

    if (bitDepth == 16)
    {
        uint32_t red = (((blitOp.color & 0xF800) >> 11) * 255) / 31;
        uint32_t green = (((blitOp.color & 0x7E0) >> 5) * 255) / 63;
        uint32_t blue = (((blitOp.color & 0x1F)) * 255) / 31;
        uint32_t alpha = blitOp.alpha;
        color = (alpha << 24) | (red << 16) | (green << 8) | blue;
    }

    if (blendingFill)
    {
        //Adjust blending for configured alpha value
        if (blitOp.alpha < 255)
        {
            hdma2d.LayerCfg[1].AlphaMode = DMA2D_REPLACE_ALPHA;
        }
        else
        {
            hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
        }
        hdma2d.LayerCfg[1].InputAlpha = color;
        hdma2d.LayerCfg[1].InputColorMode = dma2dColorMode;

        //Configure background
        hdma2d.LayerCfg[0].InputOffset = hdma2d.Init.OutputOffset;
        hdma2d.LayerCfg[0].InputColorMode = hdma2d.Init.ColorMode;
        HAL_DMA2D_ConfigLayer(&hdma2d, 0);
    }

    HAL_DMA2D_ConfigLayer(&hdma2d, 1);

    // Ensure that interrupt is enabled.
    NVIC_EnableIRQ(DMA2D_IRQn);

    if (blendingFill)
    {
        HAL_DMA2D_BlendingStart_IT(&hdma2d, (unsigned int)blitOp.pDst, (unsigned int)blitOp.pDst, (unsigned int)blitOp.pDst, blitOp.nSteps, blitOp.nLoops);
    }
    else
    {
        HAL_DMA2D_Start_IT(&hdma2d, color, (unsigned int)blitOp.pDst, blitOp.nSteps, blitOp.nLoops);
    }
}

/**
  * @brief  DMA2D Transfer completed callback
  * @param  hdma2d: DMA2D handle.
  * @note   This example shows a simple way to report end of DMA2D transfer, and
  *         you can add your own implementation.
  * @retval None
  */
void TransferComplete(DMA2D_HandleTypeDef* hdma2d)
{
    /* Signal DMA Interrupt */
    touchgfx::HAL::getInstance()->signalDMAInterrupt();
}

/**
  * @brief  DMA2D error callbacks
  * @param  hdma2d: DMA2D handle
  * @note   This example shows a simple way to report DMA2D transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void TransferError(DMA2D_HandleTypeDef* hdma2d)
{
    assert(0);
}
