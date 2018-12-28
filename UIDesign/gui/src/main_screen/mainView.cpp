#include <gui/main_screen/mainView.hpp>

mainView::mainView() :
// In constructor for callback, bind to this view object and bind which function to handle the event.
ImageClickedCallback(this, &mainView::ImageClickHandler)
{
  m_nCounter = 0;
}

void mainView::setupScreen()
{
    mainViewBase::setupScreen();

    // Add the callback to HiddenBox
    imageBack.setClickAction(ImageClickedCallback);

}

void mainView::tearDownScreen()
{
    mainViewBase::tearDownScreen();
}

void mainView::ImageClickHandler(const Image& i, const ClickEvent& e) {
  m_nCounter = (m_nCounter + 1) % 100;
  Unicode::snprintf(textArea1Buffer, 3, "%d" , m_nCounter);
  textArea1.invalidate();
}

void mainView::OnButtonCountClicked() {
  m_nCounter = (m_nCounter + 1) % 100;
  Unicode::snprintf(textArea1Buffer, 3, "%d" , m_nCounter);
  textArea1.invalidate();
}

void mainView::SetCounter(uint8_t nNewCounter) {
  m_nCounter = nNewCounter % 100;
  Unicode::snprintf(textArea1Buffer, 3, "%d" , m_nCounter);
  textArea1.invalidate();
}
