/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/shortcutsscreen_screen/ShortcutsScreenViewBase.hpp>
#include <touchgfx/Color.hpp>
#include "BitmapDatabase.hpp"

ShortcutsScreenViewBase::ShortcutsScreenViewBase() :
    buttonCallback(this, &ShortcutsScreenViewBase::buttonCallbackHandler)
{
    Background.setPosition(0, 0, 480, 272);
    Background.setColor(touchgfx::Color::getColorFrom24BitRGB(255, 255, 255));

    swipeContainer1.setXY(0, 137);

    swipeContainer1Page1.setPosition(0, 0, 480, 135);

    buttonCut.setXY(0, -1);
    buttonCut.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), Bitmap(BITMAP_DARK_ICONS_EXPAND_48_ID), Bitmap(BITMAP_DARK_ICONS_EXPAND_48_ID));
    buttonCut.setIconXY(7, 7);
    buttonCut.setAction(buttonCallback);
    swipeContainer1Page1.add(buttonCut);

    buttonCopy.setXY(60, -1);
    buttonCopy.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), Bitmap(BITMAP_DARK_ICONS_DOCUMENT_32_ID), Bitmap(BITMAP_DARK_ICONS_DOCUMENT_32_ID));
    buttonCopy.setIconXY(16, 14);
    buttonCopy.setAction(buttonCallback);
    swipeContainer1Page1.add(buttonCopy);
    swipeContainer1.add(swipeContainer1Page1);

    swipeContainer1Page2.setPosition(0, 0, 480, 135);

    buttonPaste.setXY(117, 7);
    buttonPaste.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), Bitmap(BITMAP_DARK_ICONS_DONE_32_ID), Bitmap(BITMAP_DARK_ICONS_DONE_32_ID));
    buttonPaste.setIconXY(14, 14);
    swipeContainer1Page2.add(buttonPaste);

    buttonPaste_1.setXY(191, 7);
    buttonPaste_1.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_ICON_BUTTON_PRESSED_ID), Bitmap(BITMAP_DARK_ICONS_DONE_32_ID), Bitmap(BITMAP_DARK_ICONS_DONE_32_ID));
    buttonPaste_1.setIconXY(14, 14);
    swipeContainer1Page2.add(buttonPaste_1);
    swipeContainer1.add(swipeContainer1Page2);

    swipeContainer1.setPageIndicatorBitmaps(Bitmap(BITMAP_DARK_PAGEINDICATOR_DOT_INDICATOR_SMALL_NORMAL_ID), Bitmap(BITMAP_DARK_PAGEINDICATOR_DOT_INDICATOR_SMALL_HIGHLIGHT_ID));
    swipeContainer1.setPageIndicatorXY(225, 112);
    swipeContainer1.setSwipeCutoff(200);
    swipeContainer1.setEndSwipeElasticWidth(50);
    swipeContainer1.setSelectedPage(1);

    buttonBack.setXY(39, 32);
    buttonBack.setBitmaps(Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_ID), Bitmap(BITMAP_DARK_BUTTONS_ROUND_EDGE_SMALL_PRESSED_ID), Bitmap(BITMAP_DARK_ICONS_GO_BACK_32_ID), Bitmap(BITMAP_DARK_ICONS_GO_BACK_32_ID));
    buttonBack.setIconXY(69, 15);
    buttonBack.setAction(buttonCallback);

    add(Background);
    add(swipeContainer1);
    add(buttonBack);
}

void ShortcutsScreenViewBase::setupScreen()
{

}

void ShortcutsScreenViewBase::buttonCallbackHandler(const touchgfx::AbstractButton& src)
{
    if (&src == &buttonCut)
    {
        //Interaction3
        //When buttonCut clicked call virtual function
        //Call OnButtonCutClicked
        OnButtonCutClicked();
    }
    else if (&src == &buttonCopy)
    {
        //Interaction2
        //When buttonCopy clicked call virtual function
        //Call OnButtonCopyClicked
        OnButtonCopyClicked();
    }
    else if (&src == &buttonPaste)
    {

    }
    else if (&src == &buttonPaste_1)
    {

    }
    else if (&src == &buttonBack)
    {
        //Interaction1
        //When buttonBack clicked change screen to main
        //Go to main with screen transition towards West
        application().gotomainScreenSlideTransitionWest();
    }
}
