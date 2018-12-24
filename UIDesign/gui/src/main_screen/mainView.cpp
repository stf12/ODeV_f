#include <gui/main_screen/mainView.hpp>

mainView::mainView() :
// In constructor for callback, bind to this view object and bind which function to handle the event.
ImageClickedCallback(this, &mainView::ImageClickHandler)
{

}

void mainView::setupScreen()
{
    mainViewBase::setupScreen();

    // Add the callback to HiddenBox
    imageBack.setClickAction(ImageClickedCallback);

}

void mainView::tearDownScreen()
{
    mainViewBase::tearDownScreen();
}

void mainView::ImageClickHandler(const Image& i, const ClickEvent& e) {

  application().gotosplashScreenScreenSlideTransitionWest();
}
