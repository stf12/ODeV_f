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
#include <STM32HAL_DSI.hpp>
#include <STM32DMA.hpp>
#include <touchgfx/lcd/LCD.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <string.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

#include "GuiConfig.h"

/**
* About this implementation:
* This class is for use ONLY with the DSI peripheral. If you have a regular RGB interface display, use the STM32F7HAL.cpp class instead.
*
* This implementation assumes that the DSI is configured to be in adapted command mode, with tearing effect set to external pin.
* Display will only be updated when there has actually been changes to the frame buffer.
*/
extern "C"
{
#include "stm32l4xx.h"
#include "stm32l4xx_hal_dsi.h"
#include "stm32l4xx_hal_ltdc.h"
#include "stm32l4xx_hal_gpio.h"

    extern DSI_HandleTypeDef  hdsi;
    extern LTDC_HandleTypeDef hltdc;

    /* Request tear interrupt at specific scanline. Implemented in BoardConfiguration.cpp */
    void LCD_ReqTear();

    /* Configures display to update indicated region of the screen (200pixel wide chunks) - 16bpp mode */
    void LCD_SetUpdateRegion(int idx);

    /* Configures display to update left half of the screen. Implemented in BoardConfiguration.cpp  - 24bpp mode*/
    void LCD_SetUpdateRegionLeft();

    /* Configures display to update right half of the screen. Implemented in BoardConfiguration.cpp - 24bpp mode*/
    void LCD_SetUpdateRegionRight();
}

volatile bool displayRefreshing = false;
volatile bool refreshRequested = true;
static bool doubleBufferingEnabled = false;
static uint16_t* currFbBase = 0;

uint16_t* STM32HAL_DSI::getTFTFrameBuffer() const
{
    return currFbBase;
}

void STM32HAL_DSI::setFrameBufferStartAddress(void* adr, uint16_t depth, bool useDoubleBuffering, bool useAnimationStorage)
{
    uint8_t* buffer = static_cast<uint8_t*>(adr);
    frameBuffer0 = reinterpret_cast<uint16_t*>(buffer);
    if (useDoubleBuffering)
    {
        buffer += GUI_FRAME_BUFFER_SIZE;
        frameBuffer1 = reinterpret_cast<uint16_t*>(buffer);
    }
    else
    {
        frameBuffer1 = 0;
    }
    if (useAnimationStorage)
    {
        buffer += GUI_FRAME_BUFFER_SIZE;
        frameBuffer2 = reinterpret_cast<uint16_t*>(buffer);
    }
    else
    {
        frameBuffer2 = 0;
    }
    USE_DOUBLE_BUFFERING = useDoubleBuffering;
    USE_ANIMATION_STORAGE = useAnimationStorage;
    FRAME_BUFFER_WIDTH = GUI_FRAME_BUFFER_WIDTH;
    FRAME_BUFFER_HEIGHT = GUI_FRAME_BUFFER_HEIGHT;

    // Make note of whether we are using double buffering.
    doubleBufferingEnabled = useDoubleBuffering;
    currFbBase = frameBuffer0;
}

void STM32HAL_DSI::setTFTFrameBuffer(uint16_t* adr)
{
    if (doubleBufferingEnabled)
    {
        HAL_DSI_Stop(&hdsi);

        currFbBase = adr;

        /* Update LTDC layers base address */
        if (HAL_LTDC_SetAddress(&hltdc, (uint32_t)currFbBase, 0) != HAL_OK)
        {
            while (1);
        }
        __HAL_LTDC_LAYER_ENABLE(&hltdc, 0);
        __HAL_LTDC_RELOAD_IMMEDIATE_CONFIG(&hltdc);

        HAL_DSI_Start(&hdsi);
    }
}

void STM32HAL_DSI::configureInterrupts()
{
    // These two priorities MUST be EQUAL, and MUST be functionally lower than RTOS scheduler interrupts.
    NVIC_SetPriority(DMA2D_IRQn, 7);
    NVIC_SetPriority(DSI_IRQn, 7);
}

/* Enable LCD line interrupt, when entering video (active) area */
void STM32HAL_DSI::enableLCDControllerInterrupt()
{
    LCD_ReqTear();
    __HAL_DSI_CLEAR_FLAG(&hdsi, DSI_IT_ER);
    __HAL_DSI_CLEAR_FLAG(&hdsi, DSI_IT_TE);
    __HAL_DSI_ENABLE_IT(&hdsi, DSI_IT_TE);
    __HAL_DSI_ENABLE_IT(&hdsi, DSI_IT_ER);
}

void STM32HAL_DSI::disableInterrupts()
{
    NVIC_DisableIRQ(DMA2D_IRQn);
    NVIC_DisableIRQ(DSI_IRQn);
}

void STM32HAL_DSI::enableInterrupts()
{
    NVIC_EnableIRQ(DMA2D_IRQn);
    NVIC_EnableIRQ(DSI_IRQn);
    NVIC_EnableIRQ(LTDC_ER_IRQn);
}

bool STM32HAL_DSI::beginFrame()
{
    refreshRequested = false;
    return HAL::beginFrame();
}

void STM32HAL_DSI::endFrame()
{
    HAL::endFrame();
    if (frameBufferUpdatedThisFrame)
    {
        refreshRequested = true;
    }
}

extern "C"
__irq void DSI_IRQHandler(void)
{
    if (__HAL_DSI_GET_IT_SOURCE(&hdsi, DSI_IT_TE))
    {
        // Tearing effect interrupt. Occurs periodically (every 15.7 ms on 469 eval/disco boards)
        __HAL_DSI_CLEAR_FLAG(&hdsi, DSI_IT_TE);

        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            GPIO::set(GPIO::VSYNC_FREQ);
            HAL::getInstance()->vSync();
            OSWrappers::signalVSync();
            if (!doubleBufferingEnabled && HAL::getInstance())
            {
                // In single buffering, only require that the system waits for display update to be finished if we
                // actually intend to update the display in this frame.
                HAL::getInstance()->lockDMAToFrontPorch(refreshRequested);
            }
        }

        if (refreshRequested && !displayRefreshing)
        {
            if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
            {
                // We have an update pending.
                if (doubleBufferingEnabled && HAL::getInstance())
                {
                    // Swap frame buffers immediately instead of waiting for the task to be scheduled in.
                    // Note: task will also swap when it wakes up, but that operation is guarded and will not have
                    // any effect if already swapped.
                    HAL::getInstance()->swapFrameBuffers();
                }
                displayRefreshing = true;
            }
            //Set update whole display region.
            LCD_SetUpdateRegion(0);

            // Transfer a quarter screen of pixel data.
            HAL_DSI_Refresh(&hdsi);
        }
        else
        {
            if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
            {
                GPIO::clear(GPIO::VSYNC_FREQ);
            }
        }
    }

    if (__HAL_DSI_GET_IT_SOURCE(&hdsi, DSI_IT_ER))
    {
        // End-of-refresh interrupt. Meaning one of the 4 regions have been transferred.
        __HAL_DSI_CLEAR_FLAG(&hdsi, DSI_IT_ER);

        //        HAL_DSI_Stop(&hdsi);
        //
        //        //LTDC_LAYER(hltdc, 0)->CFBAR = (uint32_t)currFbBase;
        //        //LTDC->SRCR = (uint32_t)LTDC_SRCR_IMR;
        //        LCD_SetUpdateRegion(0);
        //        //DSI->WCR |= DSI_WCR_DSIEN;
        //
        //        HAL_DSI_Start(&hdsi);

        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
        {
            GPIO::clear(GPIO::VSYNC_FREQ);

            displayRefreshing = false;
            if (HAL::getInstance())
            {
                // Signal to the framework that display update has finished.
                HAL::getInstance()->frontPorchEntered();
            }
        }
    }
}
