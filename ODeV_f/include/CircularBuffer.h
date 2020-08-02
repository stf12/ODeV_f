/**
 ******************************************************************************
 * @file    CircularBuffer.h
 * @author  STMicroelectronics - AIS - MCD Team
 * @version 1.0.0
 * @date    Jun 10, 2020
 *
 * @brief  Circular buffer implementation specialized for the producer /
 * consumer design pattern.
 *
 * This class allocates and manage a set of user defined type items
 * (::CBItemData) in a circular way. The user get a new free item from the
 * head of the buffer (to produce its content), and a he get a ready item from
 * the tail (to consume its content).
 *
 * This class is specialized for the producer /consumer design pattern.
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
#ifndef USER_INC_CIRCULARBUFFER_H_
#define USER_INC_CIRCULARBUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "systp.h"
#include "systypes.h"
#include "syserror.h"
#include "CircularBufferConfig.h"


/**
 * Create a type name for _CircularBuffer
 */
typedef struct _CircularBuffer CircularBuffer;

/**
 * Create a type name for _CBItem
 */
typedef struct _CBItem CBItem;


// Public API declaration
// **********************

/**
 * Allocate an object of ::CircularBuffer type. This allocator implement the singleton design pattern,
 * so there is only one instance of circular buffer.
 * A new allocated object must be initialized before use it.
 *
 * @return a pointer to new allocated circular buffer object.
 */
CircularBuffer *CBAlloc();

/**
 * Initialize a circular buffer object.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @return SYS_NO_ERROR_CODE
 */
sys_error_code_t CBInit(CircularBuffer *_this);

/**
 * Check if the circular buffer is empty.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @return `TRUE` if the circular buffer is empty, `FALSE` otherwise
 */
boolean_t CBIsEmpty(CircularBuffer *_this);

/**
 * Check if the circular buffer is full.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @return `TRUE` if the circular buffer is full, `FALSE` otherwise
 */
boolean_t CBIsFull(CircularBuffer *_this);

/**
 * Get the number of allocated (NEW or READY) items.
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @return the number of items in the buffer that are NEW or READY.
 */
uint32_t CBGetItemsCount(CircularBuffer *_this);

/**
 * Get a free item from the head of the buffer. A free item can be used by the caller to produce its content.
 * When the item is ready the caller must call CBSetItemReady() to mark the item a ready to be consumed.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @param pxItem [OUT] pointer to the new circular buffer item object. If the operation fails the pointer will be set to NULL.
 * @return SYS_NO_ERROR_CODE if success, SYS_CB_FULL_ERROR_CODE if the buffer is full.
 */
sys_error_code_t CBGetFreeItemFromHead(CircularBuffer *_this, CBItem **pxItem);

/**
 * Get a ready item from the tail of the buffer. A ready item can be consumed by the caller.
 * After the item is consumed the caller must call CBReleaseItem() in order to free the item.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @param pxItem [OUT] pointer to the ready circular buffer item object. If the operation fails the pointer will be set to NULL.
 * @return SYS_NO_ERROR_CODE if success, SYS_CB_NO_READY_ITEM_ERROR_CODE if there are not ready item in the tail of the buffer.
 */
sys_error_code_t CBGetReadyItemFromTail(CircularBuffer *_this, CBItem **pxItem);

/**
 * Release an item. After an item has been consumed it must be released so it is marked as free,
 * and avoid the buffer to become full. If the item is new, that means it has been allocate but it is not ready yet,
 * the function fails.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @param pxItem [IN] specifies a pointer to the item to be released.
 * @return SYS_NO_ERROR_CODE if success, SYS_CB_INVALID_ITEM_ERROR_CODE otherwise.
 */
sys_error_code_t CBReleaseItem(CircularBuffer *_this, CBItem *pxItem);

/**
 * Mar an item as ready. When a new item is ready to be consumed it must be marked as ready.
 * If the item has not been allocated, and it is free, the function fails.
 *
 * @param _this [IN] specifies a pointer to a ::CircularBuffer object.
 * @param pxItem [IN] specifies a pointer to the item to be marked as ready.
 * @return SYS_NO_ERROR_CODE if success, SYS_CB_INVALID_ITEM_ERROR_CODE otherwise.
 */
sys_error_code_t CBSetItemReady(CircularBuffer *_this, CBItem *pxItem);

/**
 * Get a pointer to the user data wrapped inside a circular buffer item.
 *
 * @param pxItem [IN] specifies a pointer to a circular buffer item
 * @return a pointer to the user data wrapped inside a circular buffer item.
 */
CBItemData *CBGetItemData(CBItem *pxItem);


#ifdef __cplusplus
}
#endif

#endif /* USER_INC_CIRCULARBUFFER_H_ */
