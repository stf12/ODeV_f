#ifndef SPALSHSCREEN_PRESENTER_HPP
#define SPALSHSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class spalshScreenView;

class spalshScreenPresenter : public Presenter, public ModelListener
{
public:
    spalshScreenPresenter(spalshScreenView& v);

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

    virtual ~spalshScreenPresenter() {};

private:
    spalshScreenPresenter();

    spalshScreenView& view;
};


#endif // SPALSHSCREEN_PRESENTER_HPP
