/**
 ******************************************************************************
 * @file    CircularBufferConfig.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jun 10, 2020
 *
 * @brief Configuration fiele for teh CircularBuffer class.
 *
 * This file defines the ::CircularBuffer configuration.
 * The user must provides:
 * - The definition of the base type of a circular item data
 * - The depth of the circular buffer, that is the max number of items stored
 *   in the circular buffer
 *
 * The memory footprint of the circular buffer is:
 * CB_SIZE = (sizeof(CBItemData) + 1) * CFG_CB_MAX_ITEMS + 4
 *
 * Note that the ::CircularBuffer uses 1 status byte for each item plus
 * 4 byte to track the head and the tail of the buffer.
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
#ifndef USER_INC_CIRCULARBUFFERCONFIG_H_
#define USER_INC_CIRCULARBUFFERCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <AILib.h>


#define CFG_CB_MAX_ITEMS          3 ///<< Specifies the depth of the circular buffer

/**
 * Specifies the struct of the base type of the circular buffer item data.
 */
typedef struct _CBItemData {
  /**
   * Specifies the data buffer to collect data to pass to the neural network.
   * The buffer of accelerometer values must be ordered as follow:
   * [x0 y0 z0 x1 y1 z1 ... x255 y255 z255]
   * where xi, yi and zi are the accelerometer values for x, y and z axis   */
  float pfBuff[AI_DATA_INPUT_USER * AI_AXIS_NUMBER];
} CBItemData;

#ifdef __cplusplus
}
#endif

#endif /* USER_INC_CIRCULARBUFFERCONFIG_H_ */
