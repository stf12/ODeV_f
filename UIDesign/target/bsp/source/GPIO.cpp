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

// GPIO C Interfaces for STM32L4R9I Evaluation board (actually based
// on BSP API implementation).
extern "C"
{
#include "stm32l4r9i_eval.h"

    static void hal_gpio_set_state(int id, int state)
    {
        if (id < LEDn)
        {
            if (state)
            {
                BSP_LED_On((Led_TypeDef)id);
            }
            else
            {
                BSP_LED_Off((Led_TypeDef)id);
            }
        }
    }
}

// This should be respecting the real board LEDs support
#define __LED1  (0)   // RENDER_TIME
#define __LED2  (1)   // VSYNC_FREQ
#define __LED3  (2)   // MCU_ACTIVE

using namespace touchgfx;

static bool gpioState[GPIO::MCU_ACTIVE + 1];

void GPIO::init()
{
    for (int i = 0; i <= MCU_ACTIVE; i++)
    {
        clear(static_cast<GPIO_ID>(i));
    }
}

void GPIO::set(GPIO_ID id)
{
    gpioState[id] = 1;
    if (id == RENDER_TIME)
    {
        hal_gpio_set_state(__LED1, 1);
    }
    else if (id == VSYNC_FREQ)
    {
        hal_gpio_set_state(__LED2, 1);
    }
    else if (id == MCU_ACTIVE)
    {
        hal_gpio_set_state(__LED3, 1);
    }
}

void GPIO::clear(GPIO_ID id)
{
    gpioState[id] = 0;
    if (id == RENDER_TIME)
    {
        hal_gpio_set_state(__LED1, 0);
    }
    else if (id == VSYNC_FREQ)
    {
        hal_gpio_set_state(__LED2, 0);
    }
    else if (id == MCU_ACTIVE)
    {
        hal_gpio_set_state(__LED3, 0);
    }
}

void GPIO::toggle(GPIO_ID id)
{
    if (get(id))
    {
        clear(id);
    }
    else
    {
        set(id);
    }
}

bool GPIO::get(GPIO_ID id)
{
    return gpioState[id];
}
