#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model() : modelListener(0)
{
  m_nCounter = 0;
}

void Model::tick()
{
}

void Model::setCounter(uint8_t nNewCounter) {
  m_nCounter = nNewCounter % 100;
}
