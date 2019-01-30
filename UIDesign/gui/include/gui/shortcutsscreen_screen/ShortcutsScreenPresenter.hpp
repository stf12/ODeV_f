#ifndef SHORTCUTSSCREEN_PRESENTER_HPP
#define SHORTCUTSSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class ShortcutsScreenView;

class ShortcutsScreenPresenter : public Presenter, public ModelListener
{
public:
    ShortcutsScreenPresenter(ShortcutsScreenView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~ShortcutsScreenPresenter() {};

    void OnButtonCopyClicked();

    void OnButtonCutClicked();

    void OnButtonPasteClicked();

    void OnButtonCursorClicked();

private:
    ShortcutsScreenPresenter();

    ShortcutsScreenView& view;
};


#endif // SHORTCUTSSCREEN_PRESENTER_HPP
