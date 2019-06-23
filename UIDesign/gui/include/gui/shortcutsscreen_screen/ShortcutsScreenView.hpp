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
    virtual void OnButtonPasteClicked();
    virtual void OnButtonMouseClicked();
    virtual void OnButtonSnipClicked();

    // Declaring callback handler for imageMouse
     void MouseClickHandler(const Image& b, const ClickEvent& e);

     virtual void handleDragEvent(const DragEvent& evt);

    void SetPage(uint8_t nPages);

protected:

    // Declaring callback type of box and clickEvent
    Callback<ShortcutsScreenView, const Image&, const ClickEvent&> MouseClickedCallback;

private:

    int16_t m_StartX;
};

#endif // SHORTCUTSSCREEN_VIEW_HPP
