#ifndef INFOSCREEN_PRESENTER_HPP
#define INFOSCREEN_PRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class InfoScreenView;

class InfoScreenPresenter : public Presenter, public ModelListener
{
public:
    InfoScreenPresenter(InfoScreenView& v);

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

    virtual ~InfoScreenPresenter() {};

private:
    InfoScreenPresenter();

    InfoScreenView& view;
};


#endif // INFOSCREEN_PRESENTER_HPP
