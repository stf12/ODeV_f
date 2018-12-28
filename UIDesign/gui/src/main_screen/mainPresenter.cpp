#include <gui/main_screen/mainView.hpp>
#include <gui/main_screen/mainPresenter.hpp>

mainPresenter::mainPresenter(mainView& v)
    : view(v)
{
}

void mainPresenter::activate()
{
  uint8_t nCounter = model->getCounter();
  view.SetCounter(nCounter);
}

void mainPresenter::deactivate()
{
  uint8_t nCounter = view.GetCounter();
  model->setCounter(nCounter);
}

void mainPresenter::onCounterChanged(uint8_t nNewCounter) {
  view.SetCounter(nNewCounter);
}
