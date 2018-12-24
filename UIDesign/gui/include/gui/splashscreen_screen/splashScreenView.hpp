#ifndef SPLASHSCREEN_VIEW_HPP
#define SPLASHSCREEN_VIEW_HPP

#include <gui_generated/splashscreen_screen/splashScreenViewBase.hpp>
#include <gui/splashscreen_screen/splashScreenPresenter.hpp>

class splashScreenView : public splashScreenViewBase
{
public:
    splashScreenView();
    virtual ~splashScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SPLASHSCREEN_VIEW_HPP
