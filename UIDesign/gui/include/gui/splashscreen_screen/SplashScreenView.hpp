#ifndef SPLASHSCREEN_VIEW_HPP
#define SPLASHSCREEN_VIEW_HPP

#include <gui_generated/splashscreen_screen/SplashScreenViewBase.hpp>
#include <gui/splashscreen_screen/SplashScreenPresenter.hpp>

class SplashScreenView : public SplashScreenViewBase
{
public:
    SplashScreenView();
    virtual ~SplashScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SPLASHSCREEN_VIEW_HPP
