#ifndef _XPLMDisplay_h_
#define _XPLMDisplay_h_

/*
 * Copyright 2005-2012 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 2.1.1                                                  
 *
 */

/*
 * XPLM Display APIs - THEORY OF OPERATION 
 * 
 * This API provides the basic hooks to draw in X-Plane and create user 
 * interface. All X-Plane drawing is done in OpenGL.  The X-Plane plug-in 
 * manager takes care of properly setting up the OpenGL context and matrices.  
 * You do not decide when in your code's  execution to draw; X-Plane tells you 
 * when it is ready to have your plugin draw. 
 * 
 * X-Plane's drawing strategy is straightforward: every "frame" the screen is 
 * rendered by drawing the 3-d scene (dome, ground, objects, airplanes, etc.)  
 * and then drawing the cockpit on top of it.  Alpha blending is used to 
 * overlay the cockpit over the world (and the gauges over the panel, etc.). 
 * 
 * There are two ways you can draw: directly and in a window. 
 * 
 * Direct drawing involves drawing to the screen before or after X-Plane 
 * finishes a phase of drawing.  When you draw directly, you can specify 
 * whether x-plane is to complete this phase or not.  This allows you to do 
 * three things: draw before x-plane does (under it), draw after x-plane does 
 * (over it), or draw instead of  x-plane. 
 * 
 * To draw directly, you register a callback and specify what phase you want 
 * to intercept.  The plug-in manager will call you over and over to draw that 
 * phase. 
 * 
 * Direct drawing allows you to override scenery, panels, or anything.  Note 
 * that  you cannot assume that you are the only plug-in drawing at this 
 * phase. 
 * 
 * Window drawing provides slightly higher level functionality.  With window 
 * drawing you create a window that takes up a portion of the screen.  Window 
 * drawing is always two dimensional.  Window drawing is front-to-back 
 * controlled; you can  specify that you want your window to be brought on 
 * top, and other plug-ins may put their window on top of you.  Window drawing 
 * also allows you to sign up for  key presses and receive mouse clicks. 
 * 
 * There are three ways to get keystrokes: 
 * 
 * If you create a window, the window can take keyboard focus.  It will then 
 * receive all keystrokes.  If no window has focus, X-Plane receives 
 * keystrokes.  Use this to implement typing in dialog boxes, etc.  Only one 
 * window may have focus at a time; your window will be notified if it loses 
 * focus. 
 * 
 * If you need to associate key strokes with commands/functions in your 
 * plug-in, use a hot key.  A hoy is a key-specific callback.  Hotkeys are 
 * sent based on virtual key strokes, so any key may be distinctly mapped with 
 * any modifiers.  Hot keys  can be remapped by other plug-ins.  As a plug-in, 
 * you don't have to worry about  what your hot key ends up mapped to; other 
 * plug-ins may provide a UI for remapping keystrokes.  So hotkeys allow a 
 * user to resolve conflicts and customize keystrokes. 
 * 
 * If you need low level access to the keystroke stream, install a key 
 * sniffer.  Key sniffers can be installed above everything or right in front 
 * of the sim.                                                                 
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * DRAWING CALLBACKS
 ***************************************************************************/
/*
 * Basic drawing callbacks, for low level intercepting of render loop. The 
 * purpose of drawing callbacks is to provide targeted additions or  
 * replacements to x-plane's graphics environment (for example, to add extra 
 * custom objects, or replace drawing of the AI aircraft).  Do not assume that 
 * the drawing callbacks will be called in the order implied by the 
 * enumerations. Also do not assume that each drawing phase ends before 
 * another begins; they may be nested.                                         
 *
 */



/*
 * XPLMDrawingPhase
 * 
 * This constant indicates which part of drawing we are in.  Drawing is done 
 * from the back to the front.  We get a callback before or after each item. 
 * Metaphases provide access to the beginning and end of the 3d (scene) and 2d 
 * (cockpit) drawing in a manner that is independent of new phases added  via 
 * x-plane implementation. 
 * 
 * WARNING: As X-Plane's scenery evolves, some drawing phases may cease to  
 * exist and new ones may be invented.  If you need a particularly specific 
 * use of these codes, consult Austin and/or be prepared to revise your code 
 * as X-Plane evolves.                                                         
 *
 */
enum {
     /* This is the earliest point at which you can draw in 3-d.                    */
     xplm_Phase_FirstScene                    = 0

     /* Drawing of land and water.                                                  */
    ,xplm_Phase_Terrain                       = 5

     /* Drawing runways and other airport detail.                                   */
    ,xplm_Phase_Airports                      = 10

     /* Drawing roads, trails, trains, etc.                                         */
    ,xplm_Phase_Vectors                       = 15

     /* 3-d objects (houses, smokestacks, etc.                                      */
    ,xplm_Phase_Objects                       = 20

     /* External views of airplanes, both yours and the AI aircraft.                */
    ,xplm_Phase_Airplanes                     = 25

     /* This is the last point at which you can draw in 3-d.                        */
    ,xplm_Phase_LastScene                     = 30

     /* This is the first phase where you can draw in 2-d.                          */
    ,xplm_Phase_FirstCockpit                  = 35

     /* The non-moving parts of the aircraft panel.                                 */
    ,xplm_Phase_Panel                         = 40

     /* The moving parts of the aircraft panel.                                     */
    ,xplm_Phase_Gauges                        = 45

     /* Floating windows from plugins.                                              */
    ,xplm_Phase_Window                        = 50

     /* The last change to draw in 2d.                                              */
    ,xplm_Phase_LastCockpit                   = 55

#if defined(XPLM200)
     /* 3-d Drawing for the local map.  Use regular OpenGL coordinates to draw in   *
      * this phase.                                                                 */
    ,xplm_Phase_LocalMap3D                    = 100

#endif /* XPLM200 */
#if defined(XPLM200)
     /* 2-d Drawing of text over the local map.                                     */
    ,xplm_Phase_LocalMap2D                    = 101

#endif /* XPLM200 */
#if defined(XPLM200)
     /* Drawing of the side-profile view in the local map screen.                   */
    ,xplm_Phase_LocalMapProfile               = 102

#endif /* XPLM200 */

};
typedef int XPLMDrawingPhase;

/*
 * XPLMDrawCallback_f
 * 
 * This is the prototype for a low level drawing callback.  You are passed in 
 * the phase and whether it is before or after.  If you are before the phase, 
 * return 1 to let x-plane draw or 0 to suppress x-plane drawing.  If you are 
 * after the phase the return value is ignored. 
 * 
 * Refcon is a unique value that you specify when registering the callback, 
 * allowing you to slip a pointer to your own data to the callback. 
 * 
 * Upon entry the OpenGL context will be correctly set up for you and OpenGL 
 * will be in 'local' coordinates for 3d drawing and panel coordinates for 2d 
 * drawing.  The OpenGL state (texturing, etc.) will be unknown.               
 *
 */
typedef int (* XPLMDrawCallback_f)(
                                   XPLMDrawingPhase     inPhase,    
                                   int                  inIsBefore,    
                                   void *               inRefcon);    

/*
 * XPLMKeySniffer_f
 * 
 * This is the prototype for a low level key-sniffing function.  Window-based 
 * UI _should not use this_!  The windowing system provides high-level 
 * mediated keyboard access.  By comparison, the key sniffer provides low 
 * level keyboard access. 
 * 
 * Key sniffers are provided to allow libraries to provide non-windowed user  
 * interaction.  For example, the MUI library uses a key sniffer to do pop-up 
 * text entry. 
 * 
 * inKey is the character pressed, inRefCon is a value you supply during 
 * registration. Return 1 to pass the key on to the next sniffer, the window 
 * mgr, x-plane, or whomever is down stream.  Return 0 to consume the key. 
 * 
 * Warning: this API declares virtual keys as a signed character; however the 
 * VKEY #define macros in XPLMDefs.h define the vkeys using unsigned values 
 * (that is 0x80 instead of -0x80).  So you may need to cast the incoming vkey 
 * to an unsigned char to get correct comparisons in C.                        
 *
 */
typedef int (* XPLMKeySniffer_f)(
                                   char                 inChar,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon);    

/*
 * XPLMRegisterDrawCallback
 * 
 * This routine registers a low level drawing callback.  Pass in the phase you 
 * want to be called for and whether you want to be called before or after.  
 * This routine returns 1 if the registration was successful, or 0 if the 
 * phase does not exist in this version of x-plane.  You may register a 
 * callback multiple times for the same or different phases as long as the 
 * refcon is unique each time.                                                 
 *
 */
XPLM_API int                  XPLMRegisterDrawCallback(
                                   XPLMDrawCallback_f   inCallback,    
                                   XPLMDrawingPhase     inPhase,    
                                   int                  inWantsBefore,    
                                   void *               inRefcon);    

/*
 * XPLMUnregisterDrawCallback
 * 
 * This routine unregisters a draw callback.  You must unregister a callback 
 * for each  time you register a callback if you have registered it multiple 
 * times with different refcons.  The routine returns 1 if it can find the 
 * callback to unregister, 0 otherwise.                                        
 *
 */
XPLM_API int                  XPLMUnregisterDrawCallback(
                                   XPLMDrawCallback_f   inCallback,    
                                   XPLMDrawingPhase     inPhase,    
                                   int                  inWantsBefore,    
                                   void *               inRefcon);    

/*
 * XPLMRegisterKeySniffer
 * 
 * This routine registers a key sniffing callback.  You specify whether you 
 * want to sniff before the window system, or only sniff keys the window 
 * system does not consume.  You should ALMOST ALWAYS sniff non-control keys 
 * after the window system.  When the window system consumes a key, it is 
 * because the user has "focused" a window.  Consuming the key or taking 
 * action based on the key will produce very weird results.  Returns 1 if 
 * successful.                                                                 
 *
 */
XPLM_API int                  XPLMRegisterKeySniffer(
                                   XPLMKeySniffer_f     inCallback,    
                                   int                  inBeforeWindows,    
                                   void *               inRefcon);    

/*
 * XPLMUnregisterKeySniffer
 * 
 * This routine unregisters a key sniffer.  You must unregister a key sniffer 
 * for every time you register one with the exact same signature.  Returns 1 
 * if successful.                                                              
 *
 */
XPLM_API int                  XPLMUnregisterKeySniffer(
                                   XPLMKeySniffer_f     inCallback,    
                                   int                  inBeforeWindows,    
                                   void *               inRefcon);    

/***************************************************************************
 * WINDOW API
 ***************************************************************************/
/*
 * Window API, for higher level drawing with UI interaction. 
 * 
 * Note: all 2-d (and thus all window drawing) is done in 'cockpit pixels'.  
 * Even when the OpenGL window contains more than 1024x768 pixels, the cockpit 
 * drawing is magnified so that only 1024x768 pixels are available.            
 *
 */



/*
 * XPLMMouseStatus
 * 
 * When the mouse is clicked, your mouse click routine is called repeatedly.  
 * It is first called with the mouse down message.  It is then called zero or 
 * more times with the mouse-drag message, and finally it is called once with 
 * the mouse up message.  All of these messages will be directed to the same 
 * window.                                                                     
 *
 */
enum {
     xplm_MouseDown                           = 1

    ,xplm_MouseDrag                           = 2

    ,xplm_MouseUp                             = 3


};
typedef int XPLMMouseStatus;

#if defined(XPLM200)
/*
 * XPLMCursorStatus
 * 
 * XPLMCursorStatus describes how you would like X-Plane to manage the cursor. 
 * See XPLMHandleCursor_f for more info.                                       
 *
 */
enum {
     /* X-Plane manages the cursor normally, plugin does not affect the cusrsor.    */
     xplm_CursorDefault                       = 0

     /* X-Plane hides the cursor.                                                   */
    ,xplm_CursorHidden                        = 1

     /* X-Plane shows the cursor as the default arrow.                              */
    ,xplm_CursorArrow                         = 2

     /* X-Plane shows the cursor but lets you select an OS cursor.                  */
    ,xplm_CursorCustom                        = 3


};
typedef int XPLMCursorStatus;
#endif /* XPLM200 */

/*
 * XPLMWindowID
 * 
 * This is an opaque identifier for a window.  You use it to control your 
 * window. When you create a window, you will specify callbacks to handle 
 * drawing and mouse interaction, etc.                                         
 *
 */
typedef void * XPLMWindowID;

/*
 * XPLMDrawWindow_f
 * 
 * This function handles drawing.  You are passed in your window and its 
 * refcon. Draw the window.  You can use XPLM functions to find the current 
 * dimensions of your window, etc.  When this callback is called, the OpenGL 
 * context will be set properly for cockpit drawing. NOTE: Because you are 
 * drawing your window over a background, you can make a translucent window 
 * easily by simply not filling in your entire window's bounds.                
 *
 */
typedef void (* XPLMDrawWindow_f)(
                                   XPLMWindowID         inWindowID,    
                                   void *               inRefcon);    

/*
 * XPLMHandleKey_f
 * 
 * This function is called when a key is pressed or keyboard focus is taken 
 * away from your window.  If losingFocus is 1, you are losign the keyboard 
 * focus, otherwise a key was pressed and inKey contains its character.  You 
 * are also passewd your window and a  refcon. Warning: this API declares 
 * virtual keys as a signed character; however the VKEY #define macros in 
 * XPLMDefs.h define the vkeys using unsigned values (that is 0x80 instead of 
 * -0x80).  So you may need to cast the incoming vkey to an unsigned char to 
 * get correct comparisons in C.                                               
 *
 */
typedef void (* XPLMHandleKey_f)(
                                   XPLMWindowID         inWindowID,    
                                   char                 inKey,    
                                   XPLMKeyFlags         inFlags,    
                                   char                 inVirtualKey,    
                                   void *               inRefcon,    
                                   int                  losingFocus);    

/*
 * XPLMHandleMouseClick_f
 * 
 * You receive this call when the mouse button is pressed down or released.  
 * Between then these two calls is a drag.  You receive the x and y of the 
 * click, your window,  and a refcon.  Return 1 to consume the click, or 0 to 
 * pass it through. 
 * 
 * WARNING: passing clicks through windows (as of this writing) causes mouse 
 * tracking problems in X-Plane; do not use this feature!                      
 *
 */
typedef int (* XPLMHandleMouseClick_f)(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   XPLMMouseStatus      inMouse,    
                                   void *               inRefcon);    

#if defined(XPLM200)
/*
 * XPLMHandleCursor_f
 * 
 * The SDK calls your cursor status callback when the mouse is over your 
 * plugin window.  Return a cursor status code to indicate how you would like 
 * X-Plane to manage the cursor.  If you return xplm_CursorDefault, the SDK 
 * will try lower-Z-order plugin windows, then let the sim manage the cursor. 
 * 
 * Note: you should never show or hide the cursor yourself - these APIs are 
 * typically reference-counted and thus  cannot safely and predictably be used 
 * by the SDK.  Instead return one of xplm_CursorHidden to hide the cursor or 
 * xplm_CursorArrow/xplm_CursorCustom to show the cursor. 
 * 
 * If you want to implement a custom cursor by drawing a cursor in OpenGL, use 
 * xplm_CursorHidden to hide the OS cursor and draw the cursor using a 2-d 
 * drawing callback (after xplm_Phase_Window is probably a good choice).  If 
 * you want to use a custom OS-based cursor, use xplm_CursorCustom to ask 
 * X-Plane to show the cursor but not affect its image.  You can then use an 
 * OS specific call like SetThemeCursor (Mac) or SetCursor/LoadCursor 
 * (Windows).                                                                  
 *
 */
typedef XPLMCursorStatus (* XPLMHandleCursor_f)(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   void *               inRefcon);    
#endif /* XPLM200 */

#if defined(XPLM200)
/*
 * XPLMHandleMouseWheel_f
 * 
 * The SDK calls your mouse wheel callback when one of the mouse wheels is 
 * turned within your window.  Return 1 to consume the  mouse wheel clicks or 
 * 0 to pass them on to a lower window.  (You should consume mouse wheel 
 * clicks even if they do nothing if your window appears opaque to the user.)  
 * The number of clicks indicates how far the wheel was turned since the last 
 * callback. The wheel is 0 for the vertical axis or 1 for the horizontal axis 
 * (for OS/mouse combinations that support this).                              
 *
 */
typedef int (* XPLMHandleMouseWheel_f)(
                                   XPLMWindowID         inWindowID,    
                                   int                  x,    
                                   int                  y,    
                                   int                  wheel,    
                                   int                  clicks,    
                                   void *               inRefcon);    
#endif /* XPLM200 */

#if defined(XPLM200)
/*
 * XPLMCreateWindow_t
 * 
 * The XPMCreateWindow_t structure defines all of the parameters used to 
 * create a window using XPLMCreateWindowEx.  The structure will be expanded 
 * in future SDK APIs to include more features.  Always set the structSize 
 * member to the size of your struct in bytes!                                 
 *
 */
typedef struct {
     int                       structSize;
     int                       left;
     int                       top;
     int                       right;
     int                       bottom;
     int                       visible;
     XPLMDrawWindow_f          drawWindowFunc;
     XPLMHandleMouseClick_f    handleMouseClickFunc;
     XPLMHandleKey_f           handleKeyFunc;
     XPLMHandleCursor_f        handleCursorFunc;
     XPLMHandleMouseWheel_f    handleMouseWheelFunc;
     void *                    refcon;
} XPLMCreateWindow_t;
#endif /* XPLM200 */

/*
 * XPLMGetScreenSize
 * 
 * This routine returns the size of the size of the X-Plane OpenGL window in 
 * pixels.  Please note that this is not the size of the screen when  doing 
 * 2-d drawing (the 2-d screen is currently always 1024x768, and  graphics are 
 * scaled up by OpenGL when doing 2-d drawing for higher-res monitors).  This 
 * number can be used to get a rough idea of the amount of detail the user 
 * will be able to see when drawing in 3-d.                                    
 *
 */
XPLM_API void                 XPLMGetScreenSize(
                                   int *                outWidth,    /* Can be NULL */
                                   int *                outHeight);    /* Can be NULL */

/*
 * XPLMGetMouseLocation
 * 
 * This routine returns the current mouse location in cockpit pixels.  The 
 * bottom left corner of the display is 0,0.  Pass NULL to not receive info 
 * about either parameter.                                                     
 *
 */
XPLM_API void                 XPLMGetMouseLocation(
                                   int *                outX,    /* Can be NULL */
                                   int *                outY);    /* Can be NULL */

/*
 * XPLMCreateWindow
 * 
 * This routine creates a new window.  Pass in the dimensions and offsets to 
 * the window's bottom left corner from the bottom left of the screen.  You 
 * can specify whether the window is initially visible or not.  Also, you pass 
 * in three callbacks to run the window and a refcon.  This function returns a 
 * window ID you can use to refer to the new window. 
 * 
 * NOTE: windows do not have "frames"; you are responsible for drawing the 
 * background and frame of the window.  Higher level libraries have routines 
 * which make this easy.                                                       
 *
 */
XPLM_API XPLMWindowID         XPLMCreateWindow(
                                   int                  inLeft,    
                                   int                  inTop,    
                                   int                  inRight,    
                                   int                  inBottom,    
                                   int                  inIsVisible,    
                                   XPLMDrawWindow_f     inDrawCallback,    
                                   XPLMHandleKey_f      inKeyCallback,    
                                   XPLMHandleMouseClick_f inMouseCallback,    
                                   void *               inRefcon);    

#if defined(XPLM200)
/*
 * XPLMCreateWindowEx
 * 
 * This routine creates a new window - you pass in an XPLMCreateWindow_t 
 * structure with all of the fields set in.  You must set the structSize of 
 * the structure to the size of the  actual structure you used.  Also, you 
 * must provide funtions for every callback - you may not leave them null!  
 * (If you do not support the cursor or mouse wheel, use functions that return 
 * the default values.)  The numeric values of the XPMCreateWindow_t structure 
 * correspond to the parameters of XPLMCreateWindow.                           
 *
 */
XPLM_API XPLMWindowID         XPLMCreateWindowEx(
                                   XPLMCreateWindow_t * inParams);    
#endif /* XPLM200 */

/*
 * XPLMDestroyWindow
 * 
 * This routine destroys a window.  The callbacks are not called after this 
 * call. Keyboard focus is removed from the window before destroying it.       
 *
 */
XPLM_API void                 XPLMDestroyWindow(
                                   XPLMWindowID         inWindowID);    

/*
 * XPLMGetWindowGeometry
 * 
 * This routine returns the position and size of a window in cockpit pixels.  
 * Pass NULL to not receive any paramter.                                      
 *
 */
XPLM_API void                 XPLMGetWindowGeometry(
                                   XPLMWindowID         inWindowID,    
                                   int *                outLeft,    /* Can be NULL */
                                   int *                outTop,    /* Can be NULL */
                                   int *                outRight,    /* Can be NULL */
                                   int *                outBottom);    /* Can be NULL */

/*
 * XPLMSetWindowGeometry
 * 
 * This routine allows you to set the position or height aspects of a window.  
 *
 */
XPLM_API void                 XPLMSetWindowGeometry(
                                   XPLMWindowID         inWindowID,    
                                   int                  inLeft,    
                                   int                  inTop,    
                                   int                  inRight,    
                                   int                  inBottom);    

/*
 * XPLMGetWindowIsVisible
 * 
 * This routine returns whether a window is visible.                           
 *
 */
XPLM_API int                  XPLMGetWindowIsVisible(
                                   XPLMWindowID         inWindowID);    

/*
 * XPLMSetWindowIsVisible
 * 
 * This routine shows or hides a window.                                       
 *
 */
XPLM_API void                 XPLMSetWindowIsVisible(
                                   XPLMWindowID         inWindowID,    
                                   int                  inIsVisible);    

/*
 * XPLMGetWindowRefCon
 * 
 * This routine returns a windows refcon, the unique value you can use for 
 * your own purposes.                                                          
 *
 */
XPLM_API void *               XPLMGetWindowRefCon(
                                   XPLMWindowID         inWindowID);    

/*
 * XPLMSetWindowRefCon
 * 
 * This routine sets a window's reference constant.  Use this to pass data to 
 * yourself in the callbacks.                                                  
 *
 */
XPLM_API void                 XPLMSetWindowRefCon(
                                   XPLMWindowID         inWindowID,    
                                   void *               inRefcon);    

/*
 * XPLMTakeKeyboardFocus
 * 
 * This routine gives a specific window keyboard focus.  Keystrokes will be 
 * sent to  that window.  Pass a window ID of 0 to pass keyboard strokes 
 * directly to X-Plane.                                                        
 *
 */
XPLM_API void                 XPLMTakeKeyboardFocus(
                                   XPLMWindowID         inWindow);    

/*
 * XPLMBringWindowToFront
 * 
 * This routine brings the window to the front of the Z-order.  Windows are 
 * brought to the front when they are created...beyond that you should make 
 * sure you are front before handling mouse clicks.                            
 *
 */
XPLM_API void                 XPLMBringWindowToFront(
                                   XPLMWindowID         inWindow);    

/*
 * XPLMIsWindowInFront
 * 
 * This routine returns true if you pass inthe ID of the frontmost visible 
 * window.                                                                     
 *
 */
XPLM_API int                  XPLMIsWindowInFront(
                                   XPLMWindowID         inWindow);    

/***************************************************************************
 * HOT KEYS
 ***************************************************************************/
/*
 * Hot Keys - keystrokes that can be managed by others.                        
 *
 */



/*
 * XPLMHotKey_f
 * 
 * Your hot key callback simply takes a pointer of your choosing.              
 *
 */
typedef void (* XPLMHotKey_f)(
                                   void *               inRefcon);    

/*
 * XPLMHotKeyID
 * 
 * Hot keys are identified by opaque IDs.                                      
 *
 */
typedef void * XPLMHotKeyID;

/*
 * XPLMRegisterHotKey
 * 
 * This routine registers a hot key.  You specify your preferred key stroke 
 * virtual key/flag combination, a description of what your callback does (so 
 * other plug-ins can describe the plug-in to the user for remapping) and a 
 * callback function and opaque pointer to pass in).  A new hot key ID is 
 * returned.  During execution, the actual key associated with your hot key 
 * may change, but you are insulated from this.                                
 *
 */
XPLM_API XPLMHotKeyID         XPLMRegisterHotKey(
                                   char                 inVirtualKey,    
                                   XPLMKeyFlags         inFlags,    
                                   const char *         inDescription,    
                                   XPLMHotKey_f         inCallback,    
                                   void *               inRefcon);    

/*
 * XPLMUnregisterHotKey
 * 
 * This API unregisters a hot key.  You can only register your own hot keys.   
 *
 */
XPLM_API void                 XPLMUnregisterHotKey(
                                   XPLMHotKeyID         inHotKey);    

/*
 * XPLMCountHotKeys
 * 
 * Returns the number of current hot keys.                                     
 *
 */
XPLM_API int                  XPLMCountHotKeys(void);

/*
 * XPLMGetNthHotKey
 * 
 * Returns a hot key by index, for iteration on all hot keys.                  
 *
 */
XPLM_API XPLMHotKeyID         XPLMGetNthHotKey(
                                   int                  inIndex);    

/*
 * XPLMGetHotKeyInfo
 * 
 * Returns information about the hot key.  Return NULL for any  parameter you 
 * don't want info about.  The description should be at least 512 chars long.  
 *
 */
XPLM_API void                 XPLMGetHotKeyInfo(
                                   XPLMHotKeyID         inHotKey,    
                                   char *               outVirtualKey,    /* Can be NULL */
                                   XPLMKeyFlags *       outFlags,    /* Can be NULL */
                                   char *               outDescription,    /* Can be NULL */
                                   XPLMPluginID *       outPlugin);    /* Can be NULL */

/*
 * XPLMSetHotKeyCombination
 * 
 * XPLMSetHotKeyCombination remaps a hot keys keystrokes.  You may remap 
 * another plugin's keystrokes.                                                
 *
 */
XPLM_API void                 XPLMSetHotKeyCombination(
                                   XPLMHotKeyID         inHotKey,    
                                   char                 inVirtualKey,    
                                   XPLMKeyFlags         inFlags);    

#ifdef __cplusplus
}
#endif

#endif
