/**
#include <sensor_db.h>
  ******************************************************************************
  * @file    sensor_db.c
  * @author  SRA - Central Labs
  * @version v2.0.0
  * @date    15-Nov-19
  * @brief   This file provides a set of functions to handle the COM structure
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"
#include "sensor_db.h"
#include "sysmem.h"
#include "stdlib.h"
#include "string.h"

static COM_Device_t s_xSDBObj = {0};


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief Get Device Struct
* @param None
* @retval whole device Structure
*/
COM_Device_t * SDB_GetIstance(void)
{
  return &s_xSDBObj;
}

/**
* @brief Add Sensor to Db
* @param None
* @retval Sensor unique sID
*/
int32_t SDB_AddSensor(COM_Device_t *pxSDB)
{
  assert_param(pxSDB);

  uint32_t nSensorID = pxSDB->deviceDescriptor.nSensor;

  pxSDB->sensors[nSensorID] = (COM_Sensor_t*)SysAlloc(sizeof(COM_Sensor_t));

  if(pxSDB->sensors[nSensorID] == NULL)
    return -1;

  memset( pxSDB->sensors[nSensorID], 0, sizeof(COM_Sensor_t));

  pxSDB->sensors[nSensorID]->sensorDescriptor.id = nSensorID;
  pxSDB->deviceDescriptor.nSensor++;

  return nSensorID;
}

/**
* @brief Get Device Descriptor
* @param None
* @retval Device Descriptor
*/
COM_DeviceDescriptor_t * SDB_GetDeviceDescriptor(const COM_Device_t *pxSDB)
{
  assert_param(pxSDB);

  return (COM_DeviceDescriptor_t *)&(pxSDB->deviceDescriptor);
}

/**
* @brief Get Sensor
* @param None
* @retval Sensor unique sID
*/
COM_Sensor_t * SDB_GetSensor(const COM_Device_t *pxSDB, uint8_t sID)
{
  assert_param(pxSDB);

  if (sID < pxSDB->deviceDescriptor.nSensor) {
    return pxSDB->sensors[sID];
  }
  else {
    return NULL;
  }
}

/**
* @brief Get Sensor Descriptor
* @param None
* @retval Sensor unique sID
*/
COM_SensorDescriptor_t * SDB_GetSensorDescriptor(const COM_Device_t *pxSDB, uint8_t sID)
{
  assert_param(pxSDB);

  if (sID < pxSDB->deviceDescriptor.nSensor) {
    return &(pxSDB->sensors[sID]->sensorDescriptor);
  }
  else {
    return NULL;
  }
}

/**
* @brief Get Sensor Status
* @param None
* @retval Sensor unique sID
*/
COM_SensorStatus_t * SDB_GetSensorStatus(const COM_Device_t *pxSDB, uint8_t sID)
{
  assert_param(pxSDB);

  if (sID < pxSDB->deviceDescriptor.nSensor) {
    return &(pxSDB->sensors[sID]->sensorStatus);
  }
  else {
    return NULL;
  }
}


/**
* @brief Set Sensor Config
* @param None
* @retval Sensor unique ID
*/
void SDB_SetSensorStatus(const COM_Device_t *pxSDB, uint8_t sID, const COM_SensorStatus_t * source)
{
  assert_param(pxSDB);

  if (sID < pxSDB->deviceDescriptor.nSensor) {
    memcpy(&(pxSDB->sensors[sID]->sensorStatus), source, sizeof(COM_SensorStatus_t));
  }
}

/**
* @brief Get Sensor Descriptor
* @param None
* @retval Sensor unique sID
*/
COM_SubSensorDescriptor_t * SDB_GetSubSensorDescriptor(const COM_Device_t *pxSDB, uint8_t sID, uint8_t ssID)
{
  assert_param(pxSDB);

  if ((sID < pxSDB->deviceDescriptor.nSensor) && (ssID < N_MAX_SESNSOR_COMBO)) {
    return &(pxSDB->sensors[sID]->sensorDescriptor.subSensorDescriptor[ssID]);
  }
  else {
    return NULL;
  }
}

/**
* @brief Get Sensor Status
* @param None
* @retval Sensor unique sID
*/
COM_SubSensorStatus_t * SDB_GetSubSensorStatus(const COM_Device_t *pxSDB, uint8_t sID, uint8_t ssID)
{
  assert_param(pxSDB);

  if ((sID < pxSDB->deviceDescriptor.nSensor) && (ssID < N_MAX_SESNSOR_COMBO)) {
    return &(pxSDB->sensors[sID]->sensorStatus.subSensorStatus[ssID]);
  }
  else {
    return NULL;
  }
}


/**
* @brief Get Sensor Status
* @param Sensor unique sID
* @retval number of subsensors
*/
uint8_t SDB_GetSubSensorNumber(const COM_Device_t *pxSDB, uint8_t sID)
{
  assert_param(pxSDB);

  if (sID < pxSDB->deviceDescriptor.nSensor) {
    return pxSDB->sensors[sID]->sensorDescriptor.nSubSensors;
  }
  else {
    return 0;
  }
}

uint8_t SDB_IsFsLegal(const COM_Device_t *pxSDB, float value, uint8_t sID, uint8_t ssID)
{
  assert_param(pxSDB);

  uint16_t i = 0;
  uint8_t ret = 0;

  if ((sID < pxSDB->deviceDescriptor.nSensor) && (ssID < N_MAX_SESNSOR_COMBO)) {
    float * list = pxSDB->sensors[sID]->sensorDescriptor.subSensorDescriptor[ssID].FS;
    while(list[i] != COM_END_OF_LIST_FLOAT)
    {
      if(list[i] == value) {
        ret = 1;
        break;
      }
      i++;
    }
  }

  return ret;
}

uint8_t SDB_IsOdrLegal(const COM_Device_t *pxSDB, float value, uint8_t sID)
{
  assert_param(pxSDB);

  uint16_t i = 0;
  uint8_t ret = 0;
  if (sID < pxSDB->deviceDescriptor.nSensor) {
    float * list = pxSDB->sensors[sID]->sensorDescriptor.ODR;
    while(list[i] != COM_END_OF_LIST_FLOAT)
    {
      if(list[i] == value)
        ret = 1;
      i++;
    }
  }

  return ret;
}

 /**
 * @brief Set default device description
 * @param None
 * @retval None
 */
//__weak void set_default_description(void)
// {
//   COM_Sensor_t * tempSensor;
//   COM_DeviceDescriptor_t * tempDeviceDescriptor;
//   tempDeviceDescriptor = SDB_GetDeviceDescriptor();
//
//   get_unique_id(tempDeviceDescriptor->serialNumber);
//   strcpy(tempDeviceDescriptor->alias, "STWIN_001");
//
//   /***** IIS3DWB *****/
//   iis3dwb_com_id = SDB_AddSensor();
//
//   tempSensor = SDB_GetSensor(iis3dwb_com_id);
//
//   /* SENSOR DESCRIPTOR */
//   strcpy(tempSensor->sensorDescriptor.name, "IIS3DWB");
//   tempSensor->sensorDescriptor.dataType = DATA_TYPE_INT16;
//   tempSensor->sensorDescriptor.ODR[0] = 26667.0f;
//   tempSensor->sensorDescriptor.ODR[1] = COM_END_OF_LIST_FLOAT;  /* Terminate list */
//   tempSensor->sensorDescriptor.samplesPerTimestamp[0] = 0;
//   tempSensor->sensorDescriptor.samplesPerTimestamp[1] = 1000;
//   tempSensor->sensorDescriptor.nSubSensors = 1;
//
//   /* SENSOR STATUS */
//   tempSensor->sensorStatus.ODR = 26667.0f;
//   tempSensor->sensorStatus.measuredODR = 0.0f;
//   tempSensor->sensorStatus.initialOffset = 0.0f;
//   tempSensor->sensorStatus.samplesPerTimestamp = 1000;
//   tempSensor->sensorStatus.isActive = 0;
//   tempSensor->sensorStatus.usbDataPacketSize = 3000;
//   tempSensor->sensorStatus.sdWriteBufferSize = WRITE_BUFFER_SIZE_IIS3DWB;
//   tempSensor->sensorStatus.comChannelNumber = -1;
//
//   /* SUBSENSOR 0 DESCRIPTOR */
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].id = 0;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].sensorType = COM_TYPE_ACC;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].dataPerSample = 3;
//   strcpy(tempSensor->sensorDescriptor.subSensorDescriptor[0].unit, "g");
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[0] = 2.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[1] = 4.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[2] = 8.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[3] = 16.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[4] = COM_END_OF_LIST_FLOAT;
//
//   /* SUBSENSOR 0 STATUS */
//   tempSensor->sensorStatus.subSensorStatus[0].FS = 16.0f;
//   tempSensor->sensorStatus.subSensorStatus[0].isActive = 0;
//   tempSensor->sensorStatus.subSensorStatus[0].sensitivity = 0.061f *  tempSensor->sensorStatus.subSensorStatus[0].FS/2;
//
//   IIS3DWB_Init_Param.ODR = tempSensor->sensorStatus.ODR;
//   IIS3DWB_Init_Param.FS[0] = tempSensor->sensorStatus.subSensorStatus[0].FS;
//   IIS3DWB_Init_Param.pbSubSensorActive[0] = tempSensor->sensorStatus.subSensorStatus[0].isActive;
//
//   maxWriteTimeSensor[iis3dwb_com_id] = 1000 * WRITE_BUFFER_SIZE_IIS3DWB / (uint32_t)(IIS3DWB_Init_Param.ODR * 6);
//
//   /*****                                                                 *****/
//
//   /* ISM330DHCX */
//   ism330dhcx_com_id = SDB_AddSensor();
//
//     tempSensor = SDB_GetSensor(ism330dhcx_com_id);
//
//   /* SENSOR DESCRIPTOR */
//   strcpy(tempSensor->sensorDescriptor.name, "ISM330DHCX");
//   tempSensor->sensorDescriptor.dataType = DATA_TYPE_INT16;
//   tempSensor->sensorDescriptor.ODR[0] = 12.5f;
//   tempSensor->sensorDescriptor.ODR[1] = 26.0f;
//   tempSensor->sensorDescriptor.ODR[2] = 52.0f;
//   tempSensor->sensorDescriptor.ODR[3] = 104.0f;
//   tempSensor->sensorDescriptor.ODR[4] = 208.0f;
//   tempSensor->sensorDescriptor.ODR[5] = 417.0f;
//   tempSensor->sensorDescriptor.ODR[6] = 833.0f;
//   tempSensor->sensorDescriptor.ODR[7] = 1667.0f;
//   tempSensor->sensorDescriptor.ODR[8] = 3333.0f;
//   tempSensor->sensorDescriptor.ODR[9] = 6667.0f;
//   tempSensor->sensorDescriptor.ODR[10] = COM_END_OF_LIST_FLOAT;
//   tempSensor->sensorDescriptor.samplesPerTimestamp[0] = 0;
//   tempSensor->sensorDescriptor.samplesPerTimestamp[1] = 1000;
//   tempSensor->sensorDescriptor.nSubSensors = 2;
//
//   /* SENSOR STATUS */
//   tempSensor->sensorStatus.ODR = 1667.0f;
//   tempSensor->sensorStatus.measuredODR = 0.0f;
//   tempSensor->sensorStatus.initialOffset = 0.0f;
//   tempSensor->sensorStatus.samplesPerTimestamp = 0;
//   tempSensor->sensorStatus.isActive = 0;
//   tempSensor->sensorStatus.usbDataPacketSize = 2048;
//   tempSensor->sensorStatus.sdWriteBufferSize = WRITE_BUFFER_SIZE_ISM330DHCX;
//   tempSensor->sensorStatus.comChannelNumber = -1;
//
//   /* SUBSENSOR 0 DESCRIPTOR */
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].id = 0;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].sensorType = COM_TYPE_ACC;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].dataPerSample = 3;
//   strcpy(tempSensor->sensorDescriptor.subSensorDescriptor[0].unit, "g");
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[0] = 2.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[1] = 4.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[2] = 8.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[3] = 16.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[0].FS[4] = COM_END_OF_LIST_FLOAT;
//
//   /* SUBSENSOR 0 STATUS */
//   tempSensor->sensorStatus.subSensorStatus[0].FS = 16.0f;
//   tempSensor->sensorStatus.subSensorStatus[0].isActive = 1;
//   tempSensor->sensorStatus.subSensorStatus[0].sensitivity = 0.061f * tempSensor->sensorStatus.subSensorStatus[0].FS/2.0f;
//
//     /* SUBSENSOR 1 DESCRIPTOR */
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].id = 1;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].sensorType = COM_TYPE_GYRO;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].dataPerSample = 3;
//   strcpy(tempSensor->sensorDescriptor.subSensorDescriptor[1].unit, "mdps");
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[0] = 125.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[1] = 250.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[2] = 500.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[3] = 1000.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[4] = 2000.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[5] = 4000.0f;
//   tempSensor->sensorDescriptor.subSensorDescriptor[1].FS[6] = COM_END_OF_LIST_FLOAT;
//
//   /* SUBSENSOR 1 STATUS */
//   tempSensor->sensorStatus.subSensorStatus[1].FS = 4000.0f;
//   tempSensor->sensorStatus.subSensorStatus[1].isActive = 0;
//   tempSensor->sensorStatus.subSensorStatus[1].sensitivity = 4.375f * tempSensor->sensorStatus.subSensorStatus[1].FS/125.0f;
//
//   ISM330DHCX_Init_Param.ODR = tempSensor->sensorStatus.ODR;
//   ISM330DHCX_Init_Param.FS[0] = tempSensor->sensorStatus.subSensorStatus[0].FS;
//   ISM330DHCX_Init_Param.FS[1] = tempSensor->sensorStatus.subSensorStatus[1].FS;
//   ISM330DHCX_Init_Param.pbSubSensorActive[0] = tempSensor->sensorStatus.subSensorStatus[0].isActive;
//   ISM330DHCX_Init_Param.pbSubSensorActive[1] = tempSensor->sensorStatus.subSensorStatus[1].isActive;
//
//   maxWriteTimeSensor[ism330dhcx_com_id] = 1000 * WRITE_BUFFER_SIZE_ISM330DHCX / (uint32_t)(ISM330DHCX_Init_Param.ODR * 12);
//
//   /**********/
//
// }


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
