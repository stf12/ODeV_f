#include <gui/shortcutsscreen_screen/ShortcutsScreenView.hpp>

ShortcutsScreenView::ShortcutsScreenView() : MouseClickedCallback(this, &ShortcutsScreenView::MouseClickHandler)
{
  // In constructor for callback, bind to this view object and bind which function to handle the event.
}

void ShortcutsScreenView::setupScreen()
{
    ShortcutsScreenViewBase::setupScreen();
    imageMouse.setClickAction(MouseClickedCallback);
}

void ShortcutsScreenView::tearDownScreen()
{
    ShortcutsScreenViewBase::tearDownScreen();
}

void ShortcutsScreenView::OnButtonCopyClicked() {
  presenter->OnButtonCopyClicked();
}

void ShortcutsScreenView::OnButtonCutClicked() {
  presenter->OnButtonCutClicked();
}

void ShortcutsScreenView::OnButtonPasteClicked() {
  presenter->OnButtonPasteClicked();
}

void ShortcutsScreenView::OnButtonSnipClicked() {
  presenter->OnButtonSnipClicked();
}

void ShortcutsScreenView::OnButtonMouseClicked() {
  presenter->OnButtonCursorClicked();
}

void ShortcutsScreenView::SetPage(uint8_t nPages) {
  swipeContainer1.setSelectedPage(nPages);
}

void ShortcutsScreenView::MouseClickHandler(const Image& b, const ClickEvent& e) {
  m_StartX = e.getX();
  if (e.getType() == e.RELEASED) {
    imageMouse.moveTo(223, 174);
    imageMouse.invalidate();
  }
}

void ShortcutsScreenView::handleDragEvent(const DragEvent& evt) {
  Screen::handleDragEvent(evt);
}
