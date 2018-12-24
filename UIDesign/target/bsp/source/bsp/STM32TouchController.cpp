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

#include <STM32TouchController.hpp>
#include "stm32l4r9i_eval_ts.h"

// TS C Interface for STM32F746G Discovery board (actually based
// on BSP API implementation).
static int hal_ts_get_state(int32_t* x_coord, int32_t* y_coord)
{
    int touchDetected = 0;
    TS_StateTypeDef state = { 0 };

    BSP_TS_GetState(&state);
    if (state.touchDetected)
    {
        *x_coord = state.touchX[0];
        *y_coord = state.touchY[0];

        touchDetected = 1;
    }

    return touchDetected;
}

using namespace touchgfx;

bool STM32TouchController::sampleTouch(int32_t& x, int32_t& y)
{
    bool touched = static_cast<bool>(hal_ts_get_state(&x, &y));
    return touched;
}
