/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/main_screen/mainViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"
#include <texts/TextKeysAndLanguages.hpp>

mainViewBase::mainViewBase() :
    buttonCallback(this, &mainViewBase::buttonCallbackHandler)
{
    background.setPosition(0, 0, 390, 390);
    background.setColor(touchgfx::Color::getColorFrom24BitRGB(0, 0, 0));

    imageBack.setXY(254, 174);
    imageBack.setBitmap(Bitmap(BITMAP_LEFT_ARROW_SMALL_ID));

    buttonBack.setXY(110, 297);
    buttonBack.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), Bitmap(BITMAP_DARK_ICONS_BACK_ARROW_32_ID), Bitmap(BITMAP_DARK_ICONS_BACK_ARROW_32_ID));
    buttonBack.setIconXY(76, 14);
    buttonBack.setAction(buttonCallback);

    buttonCount.setXY(110, 60);
    buttonCount.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID));
    buttonCount.setLabelText(TypedText(T_SINGLEUSEID3));
    buttonCount.setLabelColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonCount.setLabelColorPressed(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    buttonCount.setAction(buttonCallback);

    textArea1.setXY(74, 183);
    textArea1.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));
    textArea1.setLinespacing(0);
    Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%s", TypedText(T_SINGLEUSEID4).getText());
    textArea1.setWildcard(textArea1Buffer);
    textArea1.resizeToCurrentText();
    textArea1.setTypedText(TypedText(T_SINGLEUSEID2));

    add(background);
    add(imageBack);
    add(buttonBack);
    add(buttonCount);
    add(textArea1);
}

void mainViewBase::setupScreen()
{

}

void mainViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &buttonBack)
    {
        //InteractionGoToSplashScreen
        //When buttonBack clicked change screen to splashScreen
        //Go to splashScreen with screen transition towards West
        application().gotosplashScreenScreenSlideTransitionWest();
    }
    else if (&src == &buttonCount)
    {
        //Interaction1
        //When buttonCount clicked call virtual function
        //Call OnButtonCountClicked
        OnButtonCountClicked();
    }
}
