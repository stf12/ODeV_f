/**
  ******************************************************************************
  * @file    stm32l4r9i_eval_rgb_lcd.h
  * @author  MCD Application Team
  * @brief   This file contains the common defines and functions prototypes for
  *          the stm32l4r9i_eval_rgb_lcd.c driver.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32L4R9I_EVAL_RGB_LCD_H
#define __STM32L4R9I_EVAL_RGB_LCD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32l4r9i_eval.h"
/* Include LCD component Driver */
/* LCD RK043FN48H-CT672B 4,3" 480x272 pixels */
#include "../Components/rk043fn48h/rk043fn48h.h"
 
  
/** @addtogroup BSP
  * @{
  */

/** @addtogroup STM32L4R9I_EVAL
  * @{
  */
    
/** @addtogroup STM32L4R9I_EVAL_RGB_LCD
  * @{
  */ 

/** @defgroup STM32L4R9I_EVAL_RGB_LCD_Exported_Constants STM32L4R9I_EVAL_RGB_LCD Exported Constants
  * @{
  */ 
/** 
  * @brief  LCD FB_StartAddress  
  */
#define LCD_FB_START_ADDRESS     (uint32_t)PhysFrameBuffer  /* Internal SRAM used for LCD Frame buffer */

/** 
  * @brief  LCD Reload Types  
  */
#define LCD_RELOAD_IMMEDIATE               ((uint32_t)LTDC_SRCR_IMR)
#define LCD_RELOAD_VERTICAL_BLANKING       ((uint32_t)LTDC_SRCR_VBR) 

/**
  * @}
  */ 

/** @addtogroup STM32L4R9I_EVAL_RGB_LCD_Exported_Variables 
  * @{
  */
extern LTDC_HandleTypeDef         hltdc_eval;
extern uint32_t                   PhysFrameBuffer[];

/**
  * @}
  */

/** @addtogroup STM32L4R9I_EVAL_RGB_LCD_Exported_Functions
  * @{
  */
uint8_t  BSP_RGB_LCD_Init(void);
uint8_t  BSP_RGB_LCD_DeInit(void);
uint32_t BSP_RGB_LCD_GetXSize(void);
uint32_t BSP_RGB_LCD_GetYSize(void);
void     BSP_RGB_LCD_SetXSize(uint32_t imageWidthPixels);
void     BSP_RGB_LCD_SetYSize(uint32_t imageHeightPixels);

/* Functions using the LTDC controller */
void     BSP_RGB_LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FrameBuffer);
void     BSP_RGB_LCD_LayerRgb565Init(uint16_t LayerIndex, uint32_t FB_Address);
void     BSP_RGB_LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency);
void     BSP_RGB_LCD_SetTransparency_NoReload(uint32_t LayerIndex, uint8_t Transparency);
void     BSP_RGB_LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address);
void     BSP_RGB_LCD_SetLayerAddress_NoReload(uint32_t LayerIndex, uint32_t Address);
void     BSP_RGB_LCD_SetColorKeying(uint32_t LayerIndex, uint32_t RGBValue);
void     BSP_RGB_LCD_SetColorKeying_NoReload(uint32_t LayerIndex, uint32_t RGBValue);
void     BSP_RGB_LCD_ResetColorKeying(uint32_t LayerIndex);
void     BSP_RGB_LCD_ResetColorKeying_NoReload(uint32_t LayerIndex);
void     BSP_RGB_LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     BSP_RGB_LCD_SetLayerWindow_NoReload(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     BSP_RGB_LCD_SelectLayer(uint32_t LayerIndex);
void     BSP_RGB_LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State);
void     BSP_RGB_LCD_SetLayerVisible_NoReload(uint32_t LayerIndex, FunctionalState State);
void     BSP_RGB_LCD_Reload(uint32_t ReloadType);

void     BSP_RGB_LCD_SetTextColor(uint32_t Color);
uint32_t BSP_RGB_LCD_GetTextColor(void);
void     BSP_RGB_LCD_SetBackColor(uint32_t Color);
uint32_t BSP_RGB_LCD_GetBackColor(void);
void     BSP_RGB_LCD_SetFont(sFONT *fonts);
sFONT    *BSP_RGB_LCD_GetFont(void);

uint32_t BSP_RGB_LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void     BSP_RGB_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t pixel);
void     BSP_RGB_LCD_Clear(uint32_t Color);
void     BSP_RGB_LCD_ClearStringLine(uint32_t Line);
void     BSP_RGB_LCD_DisplayStringAtLine(uint16_t Line, uint8_t *ptr);
void     BSP_RGB_LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text, Text_AlignModeTypdef Mode);
void     BSP_RGB_LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);

void     BSP_RGB_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     BSP_RGB_LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void     BSP_RGB_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void     BSP_RGB_LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     BSP_RGB_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void     BSP_RGB_LCD_DrawPolygon(pPoint Points, uint16_t PointCount);
void     BSP_RGB_LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
void     BSP_RGB_LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp);

void     BSP_RGB_LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height);
void     BSP_RGB_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void     BSP_RGB_LCD_FillPolygon(pPoint Points, uint16_t PointCount);
void     BSP_RGB_LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);

void     BSP_RGB_LCD_DisplayOff(void);
void     BSP_RGB_LCD_DisplayOn(void);

/* These functions can be modified in case the current settings
   need to be changed for specific application needs */
void     BSP_RGB_LCD_MspInit(LTDC_HandleTypeDef *hltdc, void *Params);
void     BSP_RGB_LCD_MspDeInit(LTDC_HandleTypeDef *hltdc, void *Params);
void     BSP_RGB_LCD_ClockConfig(LTDC_HandleTypeDef *hltdc, void *Params);
void     BSP_RGB_LCD_LTDC_IRQHandler(void);
void     BSP_RGB_LCD_LTDC_ER_IRQHandler(void);
void     BSP_RGB_LCD_DMA2D_IRQHandler(void);
/**
  * @}
  */ 

/**
  * @}
  */ 
  
/**
  * @}
  */
 
/**
  * @}
  */ 

#ifdef __cplusplus
}
#endif

#endif /* __STM32L4R9I_EVAL_RGB_LCD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/