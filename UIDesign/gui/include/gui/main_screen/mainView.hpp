#ifndef MAIN_VIEW_HPP
#define MAIN_VIEW_HPP

#include <gui_generated/main_screen/mainViewBase.hpp>
#include <gui/main_screen/mainPresenter.hpp>

class mainView : public mainViewBase
{
public:
    mainView();
    virtual ~mainView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();

    // Declaring callback handler for image
    void ImageClickHandler(const Image& i, const ClickEvent& e);

    void OnButtonCountClicked();

    void SetCounter(uint8_t nNewCounter);
    uint8_t GetCounter() const { return m_nCounter; }

protected:
    // Declaring callback type of box and clickEvent
    Callback<mainView, const Image&, const ClickEvent&> ImageClickedCallback;

private:
    uint8_t m_nCounter;
};

#endif // MAIN_VIEW_HPP