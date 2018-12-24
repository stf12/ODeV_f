#ifndef SPALSHSCREEN_VIEW_HPP
#define SPALSHSCREEN_VIEW_HPP

#include <gui_generated/spalshscreen_screen/spalshScreenViewBase.hpp>
#include <gui/spalshscreen_screen/spalshScreenPresenter.hpp>

class spalshScreenView : public spalshScreenViewBase
{
public:
    spalshScreenView();
    virtual ~spalshScreenView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SPALSHSCREEN_VIEW_HPP
