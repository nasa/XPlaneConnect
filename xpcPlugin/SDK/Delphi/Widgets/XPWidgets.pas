{
   Copyright 2005-2012 Sandy Barbour and Ben Supnik
   
   All rights reserved.  See license.txt for usage.
   
   X-Plane SDK Version: 2.1.1                                                  
}

UNIT XPWidgets;
INTERFACE
{
   WIDGETS - THEORY OF OPERATION AND NOTES 
   
   Widgets are persistent view 'objects' for X-Plane.  A widget is an object 
   referenced by its opaque handle (widget ID) and the APIs in this file.  You 
   cannot access the widget's guts directly.  Every Widget has the following 
   intrinsic data: 
   
   - A bounding box defined in global screen coordinates with 0,0 in the 
   bottom left and +y = up, +x = right. 
   
   - A visible box, which is the intersection of the bounding box with the 
   widget's parents visible box. 
   
   - Zero or one parent widgets.  (Always zero if the widget is a root widget. 
   
   
   - Zero or more child widgets. 
   
   - Whether the widget is a root.  Root widgets are the top level plugin 
   windows. 
   
   - Whether the widget is visible. 
   
   - A text string descriptor, whose meaning varies from widget to widget. 
   
   - An arbitrary set of 32 bit integral properties defined by 32-bit integral 
   keys.  This is how specific widgets  
   
   store specific data. 
   
   - A list of widget callbacks proc that implements the widgets behaviors. 
   
   The Widgets library sends messages to widgets to request specific behaviors 
   or notify the widget of things. 
   
   Widgets may have more than one callback function, in which case messages 
   are sent to the most recently added callback function until the message is 
   handled.  Messages may also be sent to parents or children; see the 
   XPWidgetDefs.h header file for the different widget message dispatching 
   functions.  By adding a callback function to a window you can 'subclass' 
   its behavior. 
   
   A set of standard widgets are provided that serve common UI purposes.  You 
   can also customize or implement entirely custom widgets. 
   
   Widgets are different than other view hierarchies (most notably Win32, 
   which they bear a striking resemblance to) in the following ways: 
   
   - Not all behavior can be patched.  State that is managed by the XPWidgets 
   DLL and not by individual widgets  cannot be customized. 
   
   - All coordinates are in global screen coordinates.  Coordinates are not 
   relative to an enclosing widget, nor are they relative to a display window. 
   
   
   - Widget messages are always dispatched synchronously, and there is no 
   concept of scheduling an update or a dirty region.  Messages originate from 
   X-Plane as the sim cycle goes by.  Since x-plane is constantly redrawing, 
   so are widgets; there is no need to mark a part of a widget as 'needing 
   redrawing' because redrawing happens frequently whether the widget needs it 
   or not. 
   
   - Any widget may be a 'root' widget, causing it to be drawn; there is no 
   relationship between widget class and rootness.  Root widgets are 
   imlemented as XPLMDisply windows.                                           
}

USES   XPWidgetDefs;
   {$A4}
{$IFDEF MSWINDOWS}
   {$DEFINE DELPHI}
{$ENDIF}
{___________________________________________________________________________
 * WIDGET CREATION AND MANAGEMENT
 ___________________________________________________________________________}
{
                                                                               
}


   {
    XPCreateWidget
    
    This function creates a new widget and returns the new widget's ID to you.  
    If the  widget creation fails for some reason, it returns NULL.  Widget 
    creation will fail either if you pass a bad class ID or if there is not 
    adequate memory. 
    
    Input Parameters: 
    
    - Top, left, bottom, and right in global screen coordinates defining the 
    widget's  location on the screen. 
    
    - inVisible is 1 if the widget should be drawn, 0 to start the widget as 
    hidden. 
    
    - inDescriptor is a null terminated string that will become the widget's 
    descriptor. 
    
    - inIsRoot is 1 if this is going to be a root widget, 0 if it will not be. 
    
    - inContainer is the ID of this widget's container.  It must be 0 for a 
    root widget. for a non-root widget, pass the widget ID of the widget to 
    place this widget within. If this widget is not going to start inside 
    another widget, pass 0; this new widget will then just be floating off in 
    space (and will not be drawn until it is placed in a widget. 
    
    - inClass is the class of the widget to draw.  Use one of the predefined 
    class-IDs to create a standard widget. 
    
    A note on widget embedding: a widget is only called (and will be drawn, 
    etc.) if  it is placed within a widget that will be called.  Root widgets 
    are always called. So it is possible to have whole chains of widgets that 
    are simply not called.   You can preconstruct widget trees and then place 
    them into root widgets later to activate them if you wish.                  
   }
   FUNCTION XPCreateWidget(
                                        inLeft              : integer;    
                                        inTop               : integer;    
                                        inRight             : integer;    
                                        inBottom            : integer;    
                                        inVisible           : integer;    
                                        inDescriptor        : Pchar;    
                                        inIsRoot            : integer;    
                                        inContainer         : XPWidgetID;    
                                        inClass             : XPWidgetClass) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPCreateCustomWidget
    
    This function is the same as XPCreateWidget except that instead of passing 
    a class ID, you pass your widget callback function pointer defining the 
    widget.  Use this function to define a custom widget.  All parameters are 
    the same as XPCreateWidget, except that the widget class has been replaced 
    with the widget function.                                                   
   }
   FUNCTION XPCreateCustomWidget(
                                        inLeft              : integer;    
                                        inTop               : integer;    
                                        inRight             : integer;    
                                        inBottom            : integer;    
                                        inVisible           : integer;    
                                        inDescriptor        : Pchar;    
                                        inIsRoot            : integer;    
                                        inContainer         : XPWidgetID;    
                                        inCallback          : XPWidgetFunc_t) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPDestroyWidget
    
    This class destroys a widget.  Pass in the ID of the widget to kill.  If 
    you pass 1 for inDestroyChilren, the widget's children will be destroyed 
    first, then this widget will be destroyed.  (Furthermore, the widget's 
    children will be destroyed with the inDestroyChildren flag set to 1, so the 
    destruction will recurse down the  widget tree.)  If you pass 0 for this 
    flag, the child widgets will simply end up with their parent set to 0.      
   }
   PROCEDURE XPDestroyWidget(
                                        inWidget            : XPWidgetID;    
                                        inDestroyChildren   : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPSendMessageToWidget
    
    This sends any message to a widget.  You should probably not go around 
    simulating the predefined messages that the widgets library defines for 
    you.  You may however define custom messages for your widgets and send them 
    with this method. 
    
    This method supports several dispatching patterns; see XPDispatchMode for 
    more info. The function returns 1 if the message was handled, 0 if it was 
    not. 
    
    For each widget that receives the message (see the dispatching modes), each 
    widget  function from the most recently installed to the oldest one 
    receives the message in order until it is handled.                          
   }
   FUNCTION XPSendMessageToWidget(
                                        inWidget            : XPWidgetID;    
                                        inMessage           : XPWidgetMessage;    
                                        inMode              : XPDispatchMode;    
                                        inParam1            : intptr_t;    
                                        inParam2            : intptr_t) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * WIDGET POSITIONING AND VISIBILITY
 ___________________________________________________________________________}
{
                                                                               
}


   {
    XPPlaceWidgetWithin
    
    This function changes which container a widget resides in.  You may NOT use 
    this function on a root widget!  inSubWidget is the widget that will be 
    moved.  Pass a widget ID in inContainer to make inSubWidget be a child of 
    inContainer.  It will become the last/closest widget in the container.  
    Pass 0 to remove the widget from any container.  Any call to this other 
    than passing the widget ID of the old parent of the affected widget will 
    cause the widget to be removed from its old parent. Placing a widget within 
    its own parent simply makes it the last widget. 
    
    NOTE: this routine does not reposition the sub widget in global 
    coordinates.  If the container has layout management code, it will 
    reposition the subwidget for you, otherwise you must do it with 
    SetWidgetGeometry.                                                          
   }
   PROCEDURE XPPlaceWidgetWithin(
                                        inSubWidget         : XPWidgetID;    
                                        inContainer         : XPWidgetID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPCountChildWidgets
    
    This routine returns the number of widgets another widget contains.         
   }
   FUNCTION XPCountChildWidgets(
                                        inWidget            : XPWidgetID) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetNthChildWidget
    
    This routine returns the widget ID of a child widget by index.  Indexes are 
    0 based, from 0 to one minus the number of widgets in the parent, 
    inclusive.  If the index is invalid, 0 is returned.                         
   }
   FUNCTION XPGetNthChildWidget(
                                        inWidget            : XPWidgetID;    
                                        inIndex             : integer) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetParentWidget
    
    This routine returns the parent of a widget, or 0 if the widget has no 
    parent.   Root widgets never have parents and therefore always return 0.    
   }
   FUNCTION XPGetParentWidget(
                                        inWidget            : XPWidgetID) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPShowWidget
    
    This routine makes a widget visible if it is not already.  Note that if a 
    widget is not in a rooted widget hierarchy or one of its parents is not 
    visible, it will  still not be visible to the user.                         
   }
   PROCEDURE XPShowWidget(
                                        inWidget            : XPWidgetID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPHideWidget
    
    Makes a widget invisible.  See XPShowWidget for considerations of when a 
    widget might not be visible despite its own visibility state.               
   }
   PROCEDURE XPHideWidget(
                                        inWidget            : XPWidgetID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPIsWidgetVisible
    
    This returns 1 if a widget is visible, 0 if it is not.  Note that this 
    routine takes into consideration whether a parent is invisible.  Use this 
    routine to tell if the user can see the widget.                             
   }
   FUNCTION XPIsWidgetVisible(
                                        inWidget            : XPWidgetID) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPFindRootWidget
    
    XPFindRootWidget returns the Widget ID of the root widget that contains the 
    passed in widget or NULL if the passed in widget is not in a rooted 
    hierarchy.                                                                  
   }
   FUNCTION XPFindRootWidget(
                                        inWidget            : XPWidgetID) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPBringRootWidgetToFront
    
    This routine makes the specified widget be in the front most widget 
    hierarchy. If this widget is a root widget, its widget hierarchy comes to 
    front, otherwise the widget's root is brought to the front.  If this widget 
    is not in an active widget hiearchy (e.g. there is no root widget at the 
    top of the tree), this routine does nothing.                                
   }
   PROCEDURE XPBringRootWidgetToFront(
                                        inWidget            : XPWidgetID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPIsWidgetInFront
    
    This routine returns true if this widget's hierarchy is the front most 
    hierarchy. It returns false if the widget's hierarchy is not in front, or 
    if the widget is not in a rooted hierarchy.                                 
   }
   FUNCTION XPIsWidgetInFront(
                                        inWidget            : XPWidgetID) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetGeometry
    
    This routine returns the bounding box of a widget in global coordinates. 
    Pass NULL for any parameter you are not interested in.                      
   }
   PROCEDURE XPGetWidgetGeometry(
                                        inWidget            : XPWidgetID;    
                                        outLeft             : Pinteger;    { Can be nil }
                                        outTop              : Pinteger;    { Can be nil }
                                        outRight            : Pinteger;    { Can be nil }
                                        outBottom           : Pinteger);    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPSetWidgetGeometry
    
    This function changes the bounding box of a widget.                         
   }
   PROCEDURE XPSetWidgetGeometry(
                                        inWidget            : XPWidgetID;    
                                        inLeft              : integer;    
                                        inTop               : integer;    
                                        inRight             : integer;    
                                        inBottom            : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetForLocation
    
    Given a widget and a location, this routine returns the widget ID of the  
    child of that widget that owns that location.  If inRecursive is true then 
    this will return a child of a child of a widget as it tries to find the 
    deepest widget at that location.  If inVisibleOnly is true, then only 
    visible widgets are considered, otherwise all widgets are considered.  The 
    widget ID passed for inContainer will be returned if the location is in 
    that widget but not in a child widget.  0 is returned if the location is 
    not in the container. 
    
    NOTE: if a widget's geometry extends outside its parents geometry, it will 
    not be returned by this call for mouse locations outside the parent 
    geometry.  The  parent geometry limits the child's eligibility for mouse 
    location.                                                                   
   }
   FUNCTION XPGetWidgetForLocation(
                                        inContainer         : XPWidgetID;    
                                        inXOffset           : integer;    
                                        inYOffset           : integer;    
                                        inRecursive         : integer;    
                                        inVisibleOnly       : integer) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetExposedGeometry
    
    This routine returns the bounds of the area of a widget that is completely 
    within its parent widgets.  Since a widget's bounding box can be outside 
    its parent, part of its area will not be elligible for mouse clicks and 
    should not draw.  Use XPGetWidgetGeometry to find out what area defines 
    your widget's shape, but use this routine to find out what area to actually 
    draw into.  Note that the widget library does not use OpenGL clipping to 
    keep frame rates up, although you could use it internally.                  
   }
   PROCEDURE XPGetWidgetExposedGeometry(
                                        inWidgetID          : XPWidgetID;    
                                        outLeft             : Pinteger;    { Can be nil }
                                        outTop              : Pinteger;    { Can be nil }
                                        outRight            : Pinteger;    { Can be nil }
                                        outBottom           : Pinteger);    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * ACCESSING WIDGET DATA
 ___________________________________________________________________________}
{
                                                                               
}


   {
    XPSetWidgetDescriptor
    
    Every widget has a descriptor, which is a text string.  What the text 
    string is used for varies from widget to widget; for example, a push 
    button's text is its descriptor, a caption shows its descriptor, and a text 
    field's descriptor is the text being edited.  In other words, the usage for 
    the text varies from widget to widget, but this API provides a universal 
    and convenient way to get at it.  While not all UI widgets need their 
    descriptor, many do.                                                        
   }
   PROCEDURE XPSetWidgetDescriptor(
                                        inWidget            : XPWidgetID;    
                                        inDescriptor        : Pchar);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetDescriptor
    
    This routine returns the widget's descriptor.  Pass in the length of the 
    buffer you are going to receive the descriptor in.  The descriptor will be 
    null terminated for you.  This routine returns the length of the actual 
    descriptor; if you pass NULL for outDescriptor, you can get the 
    descriptor's length without getting its text. If the length of the 
    descriptor exceeds your buffer length, the buffer will not be null 
    terminated (this routine has 'strncpy' semantics).                          
   }
   FUNCTION XPGetWidgetDescriptor(
                                        inWidget            : XPWidgetID;    
                                        outDescriptor       : Pchar;    
                                        inMaxDescLength     : integer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPSetWidgetProperty
    
    This function sets a widget's property.  Properties are arbitrary values 
    associated by a widget by ID.                                               
   }
   PROCEDURE XPSetWidgetProperty(
                                        inWidget            : XPWidgetID;    
                                        inProperty          : XPWidgetPropertyID;    
                                        inValue             : intptr_t);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetProperty
    
    This routine returns the value of a widget's property, or 0 if the property 
    is not defined.  If you need to know whether the property is defined, pass 
    a pointer to an int for inExists; the existence of that property will be 
    returned in the int.  Pass NULL for inExists if you do not need this 
    information. 						                                                         
   }
   FUNCTION XPGetWidgetProperty(
                                        inWidget            : XPWidgetID;    
                                        inProperty          : XPWidgetPropertyID;    
                                        inExists            : Pinteger) : intptr_t;    { Can be nil }
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * KEYBOARD MANAGEMENT
 ___________________________________________________________________________}
{
                                                                               
}


   {
    XPSetKeyboardFocus
    
    XPSetKeyboardFocus controls which widget will receive keystrokes.  Pass the 
    Widget ID of the widget to get the keys.  Note that if the widget does not 
    care about keystrokes, they will go to the parent widget, and if no widget 
    cares about them, they go to  X-Plane. 
    
    If you set the keyboard focus to Widget ID 0, X-Plane gets keyboard focus. 
    
    This routine returns the widget ID that ended up with keyboard focus, or 0 
    for x-plane. 
    
    Keyboard focus is not changed if the new widget will not accept it.  For 
    setting to x-plane, keyboard focus is always accepted. 
    
       }
   FUNCTION XPSetKeyboardFocus(
                                        inWidget            : XPWidgetID) : XPWidgetID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLoseKeyboardFocus
    
    This causes the specified widget to lose focus; focus is passed to its 
    parent, or the next parent that will accept it.  This routine does nothing 
    if this widget does not have focus.                                         
   }
   PROCEDURE XPLoseKeyboardFocus(
                                        inWidget            : XPWidgetID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetWithFocus
    
    This routine returns the widget that has keyboard focus, or 0 if X-Plane 
    has keyboard focus or some other plugin window that does not have widgets 
    has focus.                                                                  
   }
   FUNCTION XPGetWidgetWithFocus: XPWidgetID;
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * CREATING CUSTOM WIDGETS
 ___________________________________________________________________________}
{
                                                                               
}


   {
    XPAddWidgetCallback
    
    This function adds a new widget callback to a widget.  This widget callback 
    supercedes any existing ones and will receive messages first; if it does 
    not handle messages they will go on to be handled by pre-existing widgets. 
    
    The widget function will remain on the widget for the life of the widget.  
    The  creation message will be sent to the new callback immediately with the 
    widget ID, and the destruction message will be sent before the other widget 
    function receives a destruction message. 
    
    This provides a way to 'subclass' an existing widget.  By providing a 
    second hook that only handles certain widget messages, you can customize or 
    extend widget behavior.                                                     
   }
   PROCEDURE XPAddWidgetCallback(
                                        inWidget            : XPWidgetID;    
                                        inNewCallback       : XPWidgetFunc_t);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPGetWidgetClassFunc
    
    Given a widget class, this function returns the callbacks that power that 
    widget class.                                                               
   }
   FUNCTION XPGetWidgetClassFunc(
                                        inWidgetClass       : XPWidgetClass) : XPWidgetFunc_t;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPWIDGETS.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

IMPLEMENTATION
END.
