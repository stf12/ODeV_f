/**
 ******************************************************************************
 * @file    CircularBuffer.c
 * @author  STMicroelectronics - AIS - MCD Team
 * @version $Version$
 * @date    $Date$
 * @brief   definition of the ::CircularBuffer class.
 *
 * For more information look at the CircularBuffer.h file.
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

#include "CircularBuffer.h"
#include <string.h>
//#include "FreeRTOS.h"
//#include "task.h"

#define CB_ITEM_FREE   0x00  ///< Status of a circular buffer item: FREE.
#define CB_ITEM_NEW    0x01  ///< Status of a circular buffer item: NEW
#define CB_ITEM_READY  0x02  ///< Status of a circular buffer item: READY

#define CB_INCREMENT_INDX(idx)        (((idx) + 1) % CFG_CB_MAX_ITEMS)
#define CB_IS_EMPTY(pCB)              (((pCB)->m_nHeadIdx == (pCB)->m_nTailIdx) && ((pCB)->m_pxItems[(pCB)->m_nHeadIdx].m_xStatus.nStatus == CB_ITEM_FREE) ? TRUE : FALSE)
#define CB_IS_FULL(pCB)               (((pCB)->m_nHeadIdx == (pCB)->m_nTailIdx) && ((pCB)->m_pxItems[(pCB)->m_nHeadIdx].m_xStatus.nStatus != CB_ITEM_FREE) ? TRUE : FALSE)

#ifndef SYS_IS_CALLED_FROM_ISR
#define SYS_IS_CALLED_FROM_ISR() ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ? 1 : 0)
#endif

/**
 * Specifies the status of a ::CBItem. An item can be:
 * - FREE: an item is free if it is not allocated and it cannot be used by the application.
 * - NEW: an item is new if it allocated and can be used by the application to produce its content.
 * - READY: an item is ready if the application has produced its content and it can be consumed.
 */
typedef struct _CBItemStatus {
  /**
   * Status bit.
   */
  uint8_t nStatus  : 2;

  /**
   * reserved. Must be set to zero.
   */
  uint8_t reserved : 6;
} CBItemStatus;

/**
 * ::CBItem internal state.
 */
struct _CBItem {
  /**
   * Specifies the user defined data managed by the circular buffer.
   */
  CBItemData m_xData;

  /**
   * Specifies a status flag added to each item. It is used to manage the item.
   */
  CBItemStatus m_xStatus;
};

/**
 * ::CircularBuffer internal state.
 */
struct _CircularBuffer {

  /**
   * Specifies the index of the circular buffer tail.
   */
  uint16_t m_nTailIdx;

  /**
   * Specifies the index of the circular buffer head.
   */
  uint16_t m_nHeadIdx;

  /**
   * Specified the buffer of items managed as a circular buffer.
   */
  CBItem m_pxItems[CFG_CB_MAX_ITEMS];
};


/**
 * Specifies the only instance of CircularBuffer available in this application.
 */
static CircularBuffer s_xTheCircularBuffer;


// Private functions declarations
// ******************************

/**
 * Start a critical section to protect the circular buffer object from multi-task access.
 */
static inline void CBEnterCritical();

/**
 *  End a critical section to allow other tasks to use the circular buffer object.
 */
static inline void CBExitCritical();


// Public API definition
// **********************

CircularBuffer *CBAlloc() {
  // this allocator implement the singleton design pattern.
  return &s_xTheCircularBuffer;
}

sys_error_code_t CBInit(CircularBuffer *_this) {
  assert_param(_this);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  _this->m_nHeadIdx = 0;
  _this->m_nTailIdx = 0;
  memset(_this->m_pxItems, 0, sizeof(_this->m_pxItems));

  return xRes;
}

boolean_t CBIsEmpty(CircularBuffer *_this) {
  assert_param(_this);
  boolean_t bRes = FALSE;

  CBEnterCritical();
  bRes = CB_IS_EMPTY(_this);
  CBExitCritical();

  return bRes;
}

boolean_t CBIsFull(CircularBuffer *_this) {
  assert_param(_this);
  boolean_t bRes = FALSE;

  CBEnterCritical();
  bRes = CB_IS_EMPTY(_this);
  CBExitCritical();

  return bRes;
}

uint32_t CBGetItemsCount(CircularBuffer *_this) {
  assert_param(_this);
  uint32_t nItemsCount = 0;

  CBEnterCritical();
  if (!CB_IS_EMPTY(_this)) {
    if (_this->m_nHeadIdx > _this->m_nTailIdx) {
      nItemsCount = _this->m_nHeadIdx - _this->m_nTailIdx;
    }
    else {
      nItemsCount = CFG_CB_MAX_ITEMS - (_this->m_nTailIdx - _this->m_nHeadIdx);
    }
  }
  CBExitCritical();

  return nItemsCount;
}

sys_error_code_t CBGetFreeItemFromHead(CircularBuffer *_this, CBItem **pxItem) {
  assert_param(_this);
  assert_param(pxItem);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  CBEnterCritical();
  if (_this->m_pxItems[_this->m_nHeadIdx].m_xStatus.nStatus == CB_ITEM_FREE) {
    *pxItem = &_this->m_pxItems[_this->m_nHeadIdx];
    // Mark the item as NEW
    (*pxItem)->m_xStatus.nStatus = CB_ITEM_NEW;
    // Increment the head pointer
    _this->m_nHeadIdx = CB_INCREMENT_INDX(_this->m_nHeadIdx);
  }
  else {
    *pxItem = NULL;
    xRes = SYS_CB_FULL_ERROR_CODE;
  }
  CBEnterCritical();

  return xRes;
}

sys_error_code_t CBGetReadyItemFromTail(CircularBuffer *_this, CBItem **pxItem) {
  assert_param(_this);
  assert_param(pxItem);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  CBEnterCritical();
  if (_this->m_pxItems[_this->m_nTailIdx].m_xStatus.nStatus == CB_ITEM_READY) {
    *pxItem = &_this->m_pxItems[_this->m_nTailIdx];
    // increment the tail pointer
    _this->m_nTailIdx = CB_INCREMENT_INDX(_this->m_nTailIdx);
  }
  else {
    *pxItem = NULL;
    xRes = SYS_CB_NO_READY_ITEM_ERROR_CODE;
  }
  CBExitCritical();


  return xRes;
}

sys_error_code_t CBReleaseItem(CircularBuffer *_this, CBItem *pxItem) {
  assert_param(_this);
  assert_param(pxItem);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  UNUSED(_this);

  CBEnterCritical();
  if (pxItem->m_xStatus.nStatus == CB_ITEM_NEW) {
    // the item is not valid because it has been only allocated but not produced.
    xRes = SYS_CB_INVALID_ITEM_ERROR_CODE;
  }
  else {
    // item is already FREE or READY, so I can release it.
    pxItem->m_xStatus.nStatus = CB_ITEM_FREE;
  }
  CBExitCritical();

  return xRes;
}

sys_error_code_t CBSetItemReady(CircularBuffer *_this, CBItem *pxItem) {
  assert_param(_this);
  assert_param(pxItem);
  sys_error_code_t xRes = SYS_NO_ERROR_CODE;

  UNUSED(_this);

  CBEnterCritical();
  if (pxItem->m_xStatus.nStatus == CB_ITEM_FREE) {
    // the item is not valid because it has not been allocated
    xRes = SYS_CB_INVALID_ITEM_ERROR_CODE;
  }
  else {
    // the item is already READY or NEW, so I can mark as READY.
    pxItem->m_xStatus.nStatus = CB_ITEM_READY;
  }
  CBExitCritical();

  return xRes;
}

CBItemData *CBGetItemData(CBItem *pxItem) {
  assert_param(pxItem);

  return &pxItem->m_xData;
}


// Private functions definition
// ****************************

static inline void CBEnterCritical() {
//  if (SYS_IS_CALLED_FROM_ISR()) {
//    taskENTER_CRITICAL_FROM_ISR();
//  }
//  else {
//    taskENTER_CRITICAL();
//  }

  __disable_irq();
}

static inline void CBExitCritical() {
//  if (SYS_IS_CALLED_FROM_ISR()) {
//    taskEXIT_CRITICAL_FROM_ISR(0);
//  }
//  else {
//    taskEXIT_CRITICAL();
//  }

  __enable_irq();
  __ISB();
}
