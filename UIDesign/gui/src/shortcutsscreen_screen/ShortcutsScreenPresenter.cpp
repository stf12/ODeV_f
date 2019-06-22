#include <gui/shortcutsscreen_screen/ShortcutsScreenView.hpp>
#include <gui/shortcutsscreen_screen/ShortcutsScreenPresenter.hpp>

ShortcutsScreenPresenter::ShortcutsScreenPresenter(ShortcutsScreenView& v)
    : view(v)
{
}

void ShortcutsScreenPresenter::activate()
{

}

void ShortcutsScreenPresenter::deactivate()
{

}

void ShortcutsScreenPresenter::OnButtonCopyClicked() {
#ifdef ODEV_F
  model->sendCopyShortcut();
#endif
}

void ShortcutsScreenPresenter::OnButtonCutClicked() {
#ifdef ODEV_F
  model->sendCutShotcut();
#endif
}

void ShortcutsScreenPresenter::OnButtonPasteClicked() {
#ifdef ODEV_F
  model->sendPasteShortcut();
#endif
}

void ShortcutsScreenPresenter::OnButtonSnipClicked() {
#ifdef ODEV_F
  model->sendSnipShortcut();
#endif
}

void ShortcutsScreenPresenter::OnButtonCursorClicked() {
#ifdef ODEV_F
  model->moveCursor();
#endif
}

void ShortcutsScreenPresenter::onShortcutsPageChanged(uint8_t nSelectedPage) {
  view.SetPage(nSelectedPage);
}
