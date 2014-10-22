#ifndef _XPStandardWidgets_h_
#define _XPStandardWidgets_h_

/*
 * Copyright 2005-2012 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 2.1.1                                                  
 *
 */

/*
 * XPStandardWidgets - THEORY OF OPERATION 
 * 
 * The standard widgets are widgets built into the widgets library.  While you 
 * can gain access to the widget function that drives them, you generally use 
 * them by calling XPCreateWidget and then listening for special messages, 
 * etc. 
 * 
 * The standard widgets often send mesages to themselves when the user 
 * performs an event; these messages are sent up the widget hierarchy until 
 * they are handled.  So you can add a widget proc directly to a push button 
 * (for example) to intercept the message when it is clicked, or you can put 
 * one widget proc on a window for all of the push buttons in the window.  
 * Most of these messages contain the original widget ID as a parameter so you 
 * can know which widget is messaging no matter who it is sent to.             
 *
 */

#include "XPWidgetDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * MAIN WINDOW
 ***************************************************************************/
/*
 * The main window widget class provides a "window" as the user knows it.  
 * These windows are dragable and can be selected.  Use them to create 
 * floating windows and non-modal dialogs.                                     
 *
 */



#define xpWidgetClass_MainWindow 1

/*
 * Main Window Type Values
 * 
 * These type values are used to control the appearance of a main window.      
 *
 */
enum {
     /* The standard main window; pin stripes on XP7, metal frame on XP 6.          */
     xpMainWindowStyle_MainWindow             = 0

     /* A translucent dark gray window, like the one ATC messages appear in.        */
    ,xpMainWindowStyle_Translucent            = 1


};

/*
 * Main Window Properties
 * 
 *
 */
enum {
     /* This property specifies the type of window.  Set to one of the main window  *
      * types above.                                                                */
     xpProperty_MainWindowType                = 1100

     /* This property specifies whether the main window has close boxes in its      *
      * corners.                                                                    */
    ,xpProperty_MainWindowHasCloseBoxes       = 1200


};

/*
 * MainWindow Messages
 * 
 *
 */
enum {
     /* This message is sent when the close buttons are pressed for your window.    */
     xpMessage_CloseButtonPushed              = 1200


};

/***************************************************************************
 * SUB WINDOW
 ***************************************************************************/
/*
 * X-plane dialogs are divided into separate areas; the sub window widgets 
 * allow you to make these areas. Create one main window and place several 
 * subwindows inside it.  Then place your controls inside the subwindows.      
 *
 */



#define xpWidgetClass_SubWindow 2

/*
 * SubWindow Type Values
 * 
 * These values control the appearance of the subwindow.                       
 *
 */
enum {
     /* A panel that sits inside a main window.                                     */
     xpSubWindowStyle_SubWindow               = 0

     /* A screen that sits inside a panel for showing text information.             */
    ,xpSubWindowStyle_Screen                  = 2

     /* A list view for scrolling lists.                                            */
    ,xpSubWindowStyle_ListView                = 3


};

/*
 * SubWindow Properties
 * 
 *
 */
enum {
     /* This property specifies the type of window.  Set to one of the subwindow    *
      * types above.                                                                */
     xpProperty_SubWindowType                 = 1200


};

/***************************************************************************
 * BUTTON
 ***************************************************************************/
/*
 * The button class provides a number of different button styles and 
 * behaviors, including push buttons, radio buttons, check boxes, etc.  The 
 * button label appears on or next to the button depending on the button's 
 * appearance, or type.   
 * 
 * The button's behavior is a separate property that dictates who it hilights 
 * and what kinds of messages it sends.  Since behavior and type are 
 * different, you can do strange things like make check boxes that act as push 
 * buttons or push buttons with radio button behavior. 
 * 
 * In X-Plane 6 there were no check box graphics.  The result is the following 
 * behavior: in x-plane 6 all check box and radio buttons are round 
 * (radio-button style) buttons; in X-Plane 7 they are all square (check-box 
 * style) buttons.  In a future version of x-plane, the xpButtonBehavior enums 
 * will provide the correct graphic (check box or radio button) giving the 
 * expected result.                                                            
 *
 */



#define xpWidgetClass_Button 3

/*
 * Button Types
 * 
 * These define the visual appearance of buttons but not how they respond to 
 * the mouse.                                                                  
 *
 */
enum {
     /* This is a standard push button, like an "OK" or "Cancel" button in a dialog *
      * box.                                                                        */
     xpPushButton                             = 0

     /* A check box or radio button.  Use this and the button behaviors below to    *
      * get the desired behavior.                                                   */
    ,xpRadioButton                            = 1

     /* A window close box.                                                         */
    ,xpWindowCloseBox                         = 3

     /* A small down arrow.                                                         */
    ,xpLittleDownArrow                        = 5

     /* A small up arrow.                                                           */
    ,xpLittleUpArrow                          = 6


};

/*
 * Button Behavior Values
 * 
 * These define how the button responds to mouse clicks.                       
 *
 */
enum {
     /* Standard push button behavior.  The button hilites while the mouse is       *
      * clicked over it and unhilites when the mouse is moved outside of it or      *
      * released. If the mouse is released over the button, the                     *
      * xpMsg_PushButtonPressed message is sent.                                    */
     xpButtonBehaviorPushButton               = 0

     /* Check box behavior.  The button immediately toggles its value when the      *
      * mouse is clicked and sends out a xpMsg_ButtonStateChanged message.          */
    ,xpButtonBehaviorCheckBox                 = 1

     /* Radio button behavior.  The button immediately sets its state to one and    *
      * sends out a xpMsg_ButtonStateChanged message if it was not already set to   *
      * one.  You must turn off other radio buttons in a group in your code.        */
    ,xpButtonBehaviorRadioButton              = 2


};

/*
 * Button Properties
 * 
 *
 */
enum {
     /* This property sets the visual type of button.  Use one of the button types  *
      * above.                                                                      */
     xpProperty_ButtonType                    = 1300

     /* This property sets the button's behavior.  Use one of the button behaviors  *
      * above.                                                                      */
    ,xpProperty_ButtonBehavior                = 1301

     /* This property tells whether a check box or radio button is "checked" or     *
      * not.  Not used for push buttons.                                            */
    ,xpProperty_ButtonState                   = 1302


};

/*
 * Button Messages
 * 
 * These messages are sent by the button to itself and then up the widget 
 * chain when the button is clicked.  (You may intercept them by providing a 
 * widget handler for the button itself or by providing a handler in a parent 
 * widget.)                                                                    
 *
 */
enum {
     /* This message is sent when the user completes a click and release in a       *
      * button with  push button behavior.  Parameter one of the message is the     *
      * widget ID of the button.  This message is dispatched up the  widget         *
      * hierarchy.                                                                  */
     xpMsg_PushButtonPressed                  = 1300

     /* This message is sent when a button is clicked that has radio button or      *
      * check box behavior and its value changes.  (Note that if the value changes  *
      * by setting a property you do not receive this message!)  Parameter one is   *
      * the widget ID of the button, parameter 2 is the new state value, either     *
      * zero or one.  This message is dispatched up the widget hierarchy.           */
    ,xpMsg_ButtonStateChanged                 = 1301


};

/***************************************************************************
 * TEXT FIELD
 ***************************************************************************/
/*
 * The text field widget provides an editable text field including mouse 
 * selection and keyboard navigation.   The contents of the text field are its 
 * descriptor.  (The descriptor changes as the user types.) 
 * 
 * The text field can have a number of types, that effect the visual layout of 
 * the text field. The text field sends messages to itself so you may control 
 * its behavior. 
 * 
 * If you need to filter keystrokes, add a new handler and intercept the key 
 * press message.  Since key presses are passed by pointer, you can modify the 
 * keystroke and pass it through to the text field widget. 
 * 
 * WARNING: in x-plane before 7.10 (including 6.70) null characters could 
 * crash x-plane.  To prevent this, wrap this object with a filter function 
 * (more instructions can be found on the SDK website).                        
 *
 */



#define xpWidgetClass_TextField 4

/*
 * Text Field Type Values
 * 
 * These control the look of the text field.                                   
 *
 */
enum {
     /* A field for text entry.                                                     */
     xpTextEntryField                         = 0

     /* A transparent text field.  The user can type and the text is drawn, but no  *
      * background is drawn. You can draw your own background by adding a widget    *
      * handler and prehandling the draw message.                                   */
    ,xpTextTransparent                        = 3

     /* A translucent edit field, dark gray.                                        */
    ,xpTextTranslucent                        = 4


};

/*
 * Text Field Properties
 * 
 *
 */
enum {
     /* This is the character position the selection starts at, zero based. If it   *
      * is the same as the end insertion point, the insertion point  is not a       *
      * selection.                                                                  */
     xpProperty_EditFieldSelStart             = 1400

     /* This is the character position of the end of the selection.                 */
    ,xpProperty_EditFieldSelEnd               = 1401

     /* This is the character position a drag was started at if the user is         *
      * dragging to select text, or -1 if a drag is not in progress.                */
    ,xpProperty_EditFieldSelDragStart         = 1402

     /* This is the type of text field to display, from the above list.             */
    ,xpProperty_TextFieldType                 = 1403

     /* Set this property to 1 to password protect the field.  Characters will be   *
      * drawn as *s even though the descriptor will contain plain-text.             */
    ,xpProperty_PasswordMode                  = 1404

     /* The max number of characters you can enter, if limited.  Zero means         *
      * unlimited.                                                                  */
    ,xpProperty_MaxCharacters                 = 1405

     /* The first visible character on the left.  This effectively scrolls the text *
      * field.                                                                      */
    ,xpProperty_ScrollPosition                = 1406

     /* The font to draw the field's text with.  (An XPLMFontID.)                   */
    ,xpProperty_Font                          = 1407

     /* This is the active side of the insert selection.  (Internal)                */
    ,xpProperty_ActiveEditSide                = 1408


};

/*
 * Text Field Messages
 * 
 *
 */
enum {
     /* Text Field Messages                                                         *
      *                                                                             *
      * The text field sends this message to itself when its text changes.  It      *
      * sends the message up the call chain; param1 is the text field's widget ID.  */
     xpMsg_TextFieldChanged                   = 1400


};

/***************************************************************************
 * SCROLL BAR
 ***************************************************************************/
/*
 * A standard scroll bar or slider control.  The scroll bar has a minimum, 
 * maximum and current value that is updated when the user drags it.  The 
 * scroll bar sends continuous messages as it is dragged.                      
 *
 */



#define xpWidgetClass_ScrollBar 5

/*
 * Scroll Bar Type Values
 * 
 * This defines how the scroll bar looks.                                      
 *
 */
enum {
     /* Scroll bar types.                                                           *
      *                                                                             *
      * A standard x-plane scroll bar (with arrows on the ends).                    */
     xpScrollBarTypeScrollBar                 = 0

     /* A slider, no arrows.                                                        */
    ,xpScrollBarTypeSlider                    = 1


};

/*
 * Scroll Bar Properties
 * 
 *
 */
enum {
     /* The current position of the thumb (in between the min and max, inclusive)   */
     xpProperty_ScrollBarSliderPosition       = 1500

     /* The value the scroll bar has when the thumb is in the lowest position.      */
    ,xpProperty_ScrollBarMin                  = 1501

     /* The value the scroll bar has when the thumb is in the highest position.     */
    ,xpProperty_ScrollBarMax                  = 1502

     /* How many units to moev the scroll bar when clicking next to the thumb. The  *
      * scroll bar always moves one unit when the arrows are clicked.               */
    ,xpProperty_ScrollBarPageAmount           = 1503

     /* The type of scrollbar from the enums above.                                 */
    ,xpProperty_ScrollBarType                 = 1504

     /* Used internally.                                                            */
    ,xpProperty_ScrollBarSlop                 = 1505


};

/*
 * Scroll Bar Messages
 * 
 *
 */
enum {
     /* The Scroll Bar sends this message when the slider position changes.  It     *
      * sends the message up the call chain; param1 is the Scroll Bar widget ID.    */
     xpMsg_ScrollBarSliderPositionChanged     = 1500


};

/***************************************************************************
 * CAPTION
 ***************************************************************************/
/*
 * A caption is a simple widget that shows its descriptor as a string, useful 
 * for labeling parts of a window.  It always shows its descriptor as its 
 * string and is otherwise transparent.                                        
 *
 */



#define xpWidgetClass_Caption 6

/*
 * Caption Properties
 * 
 *
 */
enum {
     /* This property specifies whether the caption is lit; use lit captions        *
      * against screens.                                                            */
     xpProperty_CaptionLit                    = 1600


};

/***************************************************************************
 * GENERAL GRAPHICS
 ***************************************************************************/
/*
 * The general graphics widget can show one of many icons available from 
 * x-plane.                                                                    
 *
 */



#define xpWidgetClass_GeneralGraphics 7

/*
 * General Graphics Types Values
 * 
 * These define the icon for the general graphics.                             
 *
 */
enum {
     xpShip                                   = 4

    ,xpILSGlideScope                          = 5

    ,xpMarkerLeft                             = 6

    ,xp_Airport                               = 7

    ,xpNDB                                    = 8

    ,xpVOR                                    = 9

    ,xpRadioTower                             = 10

    ,xpAircraftCarrier                        = 11

    ,xpFire                                   = 12

    ,xpMarkerRight                            = 13

    ,xpCustomObject                           = 14

    ,xpCoolingTower                           = 15

    ,xpSmokeStack                             = 16

    ,xpBuilding                               = 17

    ,xpPowerLine                              = 18

    ,xpVORWithCompassRose                     = 19

    ,xpOilPlatform                            = 21

    ,xpOilPlatformSmall                       = 22

    ,xpWayPoint                               = 23


};

/*
 * General Graphics Properties
 * 
 *
 */
enum {
     /* This property controls the type of icon that is drawn.                      */
     xpProperty_GeneralGraphicsType           = 1700


};

/***************************************************************************
 * PROGRESS INDICATOR
 ***************************************************************************/
/*
 * This widget implements a progress indicator as seen when x-plane starts up. 
 *
 */



#define xpWidgetClass_Progress 8

/*
 * Progress Indicator Properties
 * 
 *
 */
enum {
     /* This is the current value of the progress indicator.                        */
     xpProperty_ProgressPosition              = 1800

     /* This is the minimum value, equivalent to 0% filled.                         */
    ,xpProperty_ProgressMin                   = 1801

     /* This is the maximum value, equivalent to 100% filled.                       */
    ,xpProperty_ProgressMax                   = 1802


};

#ifdef __cplusplus
}
#endif

#endif
