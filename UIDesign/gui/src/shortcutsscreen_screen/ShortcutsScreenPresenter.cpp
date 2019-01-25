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
  model->sendChar();
#endif
}

void ShortcutsScreenPresenter::OnButtonCutClicked() {
#ifdef ODEV_F
  model->moveCursor();
#endif
}
