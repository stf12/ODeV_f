#ifndef SHORTCUTSSCREEN_VIEW_HPP
#define SHORTCUTSSCREEN_VIEW_HPP

#include <gui_generated/shortcutsscreen_screen/ShortcutsScreenViewBase.hpp>
#include <gui/shortcutsscreen_screen/ShortcutsScreenPresenter.hpp>

class ShortcutsScreenView : public ShortcutsScreenViewBase
{
public:
    ShortcutsScreenView();
    virtual ~ShortcutsScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    virtual void OnButtonCopyClicked();
    virtual void OnButtonCutClicked();
protected:
};

#endif // SHORTCUTSSCREEN_VIEW_HPP
