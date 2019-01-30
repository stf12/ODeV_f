#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#ifdef ODEV_F
#include "syserror.h"
#include "hid_report_parser.h"
#include "string.h"
#include <CShortcutCopy.h>
#include <CShortcutCut.h>
#include <CShortcutPaste.h>
#endif

Model::Model() : modelListener(0)
{
  m_nCounter = 0;

#ifdef ODEV_F
  m_pxShortcutsTask = NULL;
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

void Model::sendCopyShortcut() {
  static odev::CShortcutCopy s_xShortcut;

  if (m_pxShortcutsTask != NULL) {
    ShortcutsDemonTaskPostShortcuts(m_pxShortcutsTask, &s_xShortcut);
  }
}

void Model::sendCutShotcut() {
  static odev::CShortcutCut s_xShortcut;

  if (m_pxShortcutsTask != NULL) {
    ShortcutsDemonTaskPostShortcuts(m_pxShortcutsTask, &s_xShortcut);
  }
}

void Model::sendPasteShortcut() {
  static odev::CShortcutPaste s_xShortcut;

  if (m_pxShortcutsTask != NULL) {
    ShortcutsDemonTaskPostShortcuts(m_pxShortcutsTask, &s_xShortcut);
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

void Model::SetShortcutsDemonTask(ShortcutsDemonTask *pxTask) {
  m_pxShortcutsTask = pxTask;
}

#endif
