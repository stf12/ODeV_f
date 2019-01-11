/**
  ******************************************************************************
  * @file    GUIConf.h
  * @author  MCD Application Team
  * @version $VERSION$
  * @date    $DATE$
  * @brief   GUI configuration file.
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

/*---------------------------------------------------------------------------/
/ File        : GUIConf.h                                                    /
/ Purpose     : Configures TouchGFX framework.                               /
/---------------------------------------------------------------------------*/

#ifndef GUICONF_H
#define GUICONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32l4r9i_eval_gfxmmu_lut.h"  //TODO: STF

#define DISPLAY_PARALLEL_INTERFACE      (0)
#define DISPLAY_SERIAL_INTERFACE        (1)
#define DISPLAY_BUS_INTERFACE           (2)

/*********************************************************************
*
*       Display configuration
*/
#if defined(USE_GVO_390x390)

#define GUI_DISPLAY_INTERFACE    (DISPLAY_SERIAL_INTERFACE)
#define GUI_DISPLAY_WIDTH        GFXMMU_LCD_SIZE
#define GUI_DISPLAY_HEIGHT       GFXMMU_LCD_SIZE

#elif defined(USE_ROCKTECH_480x272)

#define GUI_DISPLAY_INTERFACE    (DISPLAY_PARALLEL_INTERFACE)
#define GUI_DISPLAY_WIDTH        480
#define GUI_DISPLAY_HEIGHT       272

#else /* Default configuration */

#ifndef   GUI_DISPLAY_INTERFACE
#define GUI_DISPLAY_INTERFACE    (DISPLAY_PARALLEL_INTERFACE)
#endif /* !GUI_DISPLAY_INTERFACE */
#ifndef   GUI_DISPLAY_WIDTH
#define GUI_DISPLAY_WIDTH        480
#endif /* !GUI_DISPLAY_WIDTH */
#ifndef   GUI_DISPLAY_HEIGHT
#define GUI_DISPLAY_HEIGHT       272
#endif /* !GUI_DISPLAY_HEIGHT */

#endif

#ifndef   GUI_DISPLAY_NUM_LAYERS
#define GUI_DISPLAY_NUM_LAYERS     1    // Maximum number of available layers
#endif /* !GUI_DISPLAY_NUM_LAYERS */

#ifndef   GUI_DISPLAY_BPP

#if defined(USE_BPP)
#define GUI_DISPLAY_BPP           USE_BPP    // GUI bit depth set from user
#else
#define GUI_DISPLAY_BPP           16         // Default to 16 bit depth
#endif /* !USE_BPP */

#endif /* !GUI_DISPLAY_BPP */

/*********************************************************************
*
*       Framebuffer dimension configuration
*/
#ifndef   GUI_NUM_FRAME_BUFFERS
#define GUI_NUM_FRAME_BUFFERS      2
#endif /* !GUI_NUM_FRAME_BUFFERS */
#ifndef   GUI_FRAME_BUFFER_WIDTH
#define GUI_FRAME_BUFFER_WIDTH   480
#endif /* !GUI_FRAME_BUFFER_WIDTH */
#ifndef   GUI_FRAME_BUFFER_HEIGHT
#define GUI_FRAME_BUFFER_HEIGHT  272
#endif /* !GUI_FRAME_BUFFER_HEIGHT */

/*********************************************************************
*
*       Framebuffer size configuration
*/
#ifndef   GUI_FRAME_BUFFER_SIZE
#define GUI_FRAME_BUFFER_SIZE      ((GUI_NUM_FRAME_BUFFERS * GUI_FRAME_BUFFER_WIDTH * GUI_FRAME_BUFFER_HEIGHT * GUI_DISPLAY_BPP)/8)
#endif /* !GUI_FRAME_BUFFER_SIZE */

/*********************************************************************
*
*       Framebuffer memory placement configuration
*/
#if !defined(LOCATION_FRAMEBUFFER_PRAGMA) || !defined(LOCATION_FRAMEBUFFER_ATTRIBUTE)
#undef LOCATION_FRAMEBUFFER_PRAGMA
#undef LOCATION_FRAMEBUFFER_ATTRIBUTE
#ifdef __GNUC__
#ifdef __CODE_RED
#include <cr_section_macros.h>
#define LOCATION_FRAMEBUFFER_PRAGMA
#define LOCATION_FRAMEBUFFER_ATTRIBUTE __NOLOAD(SRAM)
#else
#define LOCATION_FRAMEBUFFER_PRAGMA
#define LOCATION_FRAMEBUFFER_ATTRIBUTE __attribute__ ((section ("FrameBufferSection"))) __attribute__ ((aligned(8)))
#endif
#elif defined __ICCARM__
#define LOCATION_FRAMEBUFFER_PRAGMA _Pragma("location=\"FrameBufferSection\"") _Pragma("data_alignment=8")
#define LOCATION_FRAMEBUFFER_ATTRIBUTE
#elif defined(__ARMCC_VERSION)
#define LOCATION_FRAMEBUFFER_PRAGMA
#define LOCATION_FRAMEBUFFER_ATTRIBUTE __attribute__ ((section ("FrameBufferSection"))) __attribute__ ((aligned(8)))
#endif
#endif

extern uint8_t *frameBuf0;

/*********************************************************************
*
*       Framebuffer Double Buffering support
*/
#ifndef   GUI_SUPPORT_DOUBLE_BUFFERING
  #define GUI_SUPPORT_DOUBLE_BUFFERING      (1)  // Support doube buffering
#endif

/*********************************************************************
*
*       Configuration of touch support
*/
#ifndef   GUI_SUPPORT_TOUCH
  #define GUI_SUPPORT_TOUCH                 (1)  // Support touchscreen
#endif

/*********************************************************************
*
*       Configuration of ChromART hardware acceleration
*/
#ifndef   GUI_SUPPORT_HW_ACCEL
  #define GUI_SUPPORT_HW_ACCEL              (1)  // Support hardware acceleration
#endif

/*********************************************************************
*
*       Configuration of MCU Instrumentation support
*/
#ifndef   GUI_SUPPORT_MCU_INSTRUMENTATION
  #define GUI_SUPPORT_MCU_INSTRUMENTATION   (1)  // Support MCU instrumentation
#endif

/*********************************************************************
*
*       Configuration of OS support
*/
#ifndef   GUI_OS_USE_SAME_HAL_TIMEBASE
  #define GUI_OS_USE_SAME_HAL_TIMEBASE      (1)  // HAL driver and OS uses the same timebase source
#endif

#ifdef __cplusplus
}
#endif

#endif  /* Avoid multiple inclusion */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
