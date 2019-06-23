/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/infoscreen_screen/InfoScreenViewBase.hpp>
#include "BitmapDatabase.hpp"

InfoScreenViewBase::InfoScreenViewBase() :
    waitAndGoCounter(0)
{
    splashImage.setXY(0, 0);
    splashImage.setBitmap(Bitmap(BITMAP_VISUAL_SHORTCUTS_SPLASH_ID));

    add(splashImage);
}

void InfoScreenViewBase::setupScreen()
{

}

//Handles delays
void InfoScreenViewBase::handleTickEvent()
{
    View::handleTickEvent();
    if(waitAndGoCounter > 0)
    {
        waitAndGoCounter--;
        if(waitAndGoCounter == 0)
        {
            //Interaction2
            //When waitAndGo completed change screen to ShortcutsScreen
            //Go to ShortcutsScreen with screen transition towards East
            application().gotoShortcutsScreenScreenSlideTransitionEast();
        }
    }
}

//Called when the screen is done with transition/load
void InfoScreenViewBase::afterTransition()
{
    //waitAndGo
    //When screen is entered delay
    //Delay for 2000 ms (120 Ticks)
    waitAndGoCounter = WAITANDGO_DURATION;
}
