/**
  ******************************************************************************
  * @file    HSD_json.h
  * @author  SRA - Central Labs
  * @version v2.0.0
  * @date    15-Nov-19
  * @brief   This file contains all the functions prototypes for the main.c
  *          file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __JSON_PARSER_H

#define __JSON_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <sensor_db.h>
#include "string.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
  
int32_t HSD_init_JSON(void * (*Malloc_Function)(size_t), void(*Free_Function)(void *));
int32_t HSD_free(void * mem);
int32_t HSD_serialize_Device_JSON(COM_Device_t *Device, char **SerializedJSON);
int32_t HSD_serialize_DeviceInfo_JSON(COM_DeviceDescriptor_t *DeviceInfo, char **SerializedJSON);
int32_t HSD_serialize_Sensor_JSON(COM_Sensor_t *Sensor, char **SerializedJSON);
int32_t HSD_serialize_SensorDescriptor_JSON(COM_SensorDescriptor_t *SensorDescriptor, char **SerializedJSON);
int32_t HSD_serialize_SensorStatus_JSON(COM_SensorStatus_t *SensorStatus, char **SerializedJSON);
int32_t HSD_serialize_SubSensorDescriptor_JSON(COM_SubSensorDescriptor_t *SubSensorDescriptor, char **SerializedJSON);
int32_t HSD_serialize_SubSensorStatus_JSON(COM_SubSensorStatus_t *SubSensorStatus, char **SerializedJSON);
  
int32_t HSD_ParseDevice(char *SerializedJSON, COM_Device_t * Device);
int32_t HSD_ParseStatus(char *SerializedJSON, COM_SensorStatus_t * SensorStatus);
int32_t HSD_ParseCommand(char *SerializedJSON, COM_Command_t * Command);


#ifdef __cplusplus
}
#endif

#endif /* __JSON_PARSER_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
