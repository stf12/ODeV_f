#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#ifdef ODEV_F
#include "syserror.h"
#include "hid_report_parser.h"
#include "string.h"
#endif

Model::Model() : modelListener(0)
{
  m_nCounter = 0;

#ifdef ODEV_F
  m_xOutputQueue = NULL;
  m_xInputQueue = xQueueCreate(10, 1);
  if (m_xInputQueue == NULL) {
    sys_error_handler();
  }
#ifdef DEBUG
  else {
    vQueueAddToRegistry(m_xInputQueue, "UI_MQ");
  }
#endif
#endif
}

void Model::tick()
{
#ifdef ODEV_F
  uint8_t nIncrement = 0;

  if (xQueueReceive(m_xInputQueue, &nIncrement, 0) == pdTRUE) {
    m_nCounter = (m_nCounter + nIncrement) % 100;
    modelListener->onCounterChanged(m_nCounter);
  }
#endif
}

void Model::setCounter(uint8_t nNewCounter) {
  m_nCounter = nNewCounter % 100;
}

void Model::incrementCounter(uint8_t nIncrement) {
#ifdef ODEV_F
  xQueueSendToBack(m_xInputQueue, &nIncrement, pdMS_TO_TICKS(50));
#endif
}

#ifdef ODEV_F

void Model::sendChar() {
  static HIDReport xReport10;

  if (m_xOutputQueue != NULL) {
    memset(&xReport10, 0, sizeof(HIDReport));

    xReport10.reportID = HID_REPORT_ID_KEYBOARD;
    xReport10.inputReport10.nKeyCodeArray[0] = 0x16; // s for STF :)
    xQueueSendToBack(m_xOutputQueue, &xReport10, pdMS_TO_TICKS(50));

    xReport10.inputReport10.nKeyCodeArray[0] = 0; // s for STF :)
    xQueueSendToBack(m_xOutputQueue, &xReport10, pdMS_TO_TICKS(50));
  }
}

void Model::moveCursor() {
  static HIDReport xReport02;

  if (m_xOutputQueue != NULL) {
    memset(&xReport02, 0, sizeof(HIDReport));

    xReport02.reportID = HID_REPORT_ID_MOUSE;
    xReport02.inputReport02.nDX = 0x25;

    xQueueSendToBack(m_xOutputQueue, &xReport02, pdMS_TO_TICKS(50));
  }
}

void Model::SetOutputQueue(QueueHandle_t xQueue) {
  m_xOutputQueue = xQueue;
}

#endif
