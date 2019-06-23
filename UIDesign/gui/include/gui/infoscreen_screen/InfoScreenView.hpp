#ifndef INFOSCREEN_VIEW_HPP
#define INFOSCREEN_VIEW_HPP

#include <gui_generated/infoscreen_screen/InfoScreenViewBase.hpp>
#include <gui/infoscreen_screen/InfoScreenPresenter.hpp>

class InfoScreenView : public InfoScreenViewBase
{
public:
    InfoScreenView();
    virtual ~InfoScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // INFOSCREEN_VIEW_HPP
