#include <gui/shortcutsscreen_screen/ShortcutsScreenView.hpp>

ShortcutsScreenView::ShortcutsScreenView()
{

}

void ShortcutsScreenView::setupScreen()
{
    ShortcutsScreenViewBase::setupScreen();
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

void ShortcutsScreenView::OnButtonMouseClicked() {
  presenter->OnButtonCursorClicked();
}
