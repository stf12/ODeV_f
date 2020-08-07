/**
 ******************************************************************************
 * @file    SensorCommands.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Aug 7, 2020
 *
 * @brief
 *
 * <DESCRIPTIOM>
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
#ifndef SENSORCOMMANDS_H_
#define SENSORCOMMANDS_H_

#ifdef __cplusplus
extern "C" {
#endif

// Command ID. These are all commands supported by a sensor task.
#define SENSOR_CMD_ID_START                ((uint16_t)0x0001)              ///< START command ID.
#define SENSOR_CMD_ID_STOP                 ((uint16_t)0x0002)              ///< STOP command ID.

#ifdef __cplusplus
}
#endif

#endif /* SENSORCOMMANDS_H_ */
