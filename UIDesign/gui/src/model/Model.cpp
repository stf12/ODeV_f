#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#ifdef ODEV_F
#include "syserror.h"
#endif

Model::Model() : modelListener(0)
{
  m_nCounter = 0;

#ifdef ODEV_F
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
