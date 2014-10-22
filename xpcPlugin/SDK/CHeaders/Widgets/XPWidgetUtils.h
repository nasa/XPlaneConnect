#ifndef _XPWidgetUtils_h_
#define _XPWidgetUtils_h_

/*
 * Copyright 2005-2012 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 2.1.1                                                  
 *
 */

/*
 * XPWidgetUtils - USAGE NOTES 
 * 
 * The XPWidgetUtils library contains useful functions that make writing and 
 * using  widgets less of a pain. 
 * 
 * One set of functions are the widget behavior functions.  These functions 
 * each add specific useful behaviors to widgets.  They can be used in two 
 * manners: 
 * 
 * 1. You can add a widget behavior function to a widget as a callback proc 
 * using the XPAddWidgetCallback function.  The widget will gain that 
 * behavior.  Remember that the last function you add has highest priority.  
 * You can use this to change or augment the behavior of an existing finished 
 * widget. 
 * 
 * 2. You can call a widget function from inside your own widget function.  
 * This allows you to include useful behaviors in custom-built widgets.  A 
 * number of the standard widgets get their behavior from this library.  To do 
 * this, call the behavior function from your function first.  If it returns 
 * 1, that means it handled the event and you don't need to; simply return 1.  
 *
 */

#include "XPWidgetDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * GENERAL UTILITIES
 ***************************************************************************/
/*
 *                                                                             
 *
 */




/*
 * Convenience accessors
 *
 * It can be clumsy accessing the variables passed in by pointer to a struct
 * for mouse and reshape messages; these accessors let you simply pass in the param
 * right from the arguments of your widget proc and get back the value you want.
 *
 */
#define	MOUSE_X(param) (((XPMouseState_t *) (param))->x)
#define	MOUSE_Y(param) (((XPMouseState_t *) (param))->y)

#define DELTA_X(param) (((XPWidgetGeometryChange_t *) (param))->dx)
#define DELTA_Y(param) (((XPWidgetGeometryChange_t *) (param))->dy)
#define DELTA_W(param) (((XPWidgetGeometryChange_t *) (param))->dwidth)
#define DELTA_H(param) (((XPWidgetGeometryChange_t *) (param))->dheight)

#define KEY_CHAR(param) (((XPKeyState_t *) (param))->key)
#define KEY_FLAGS(param) (((XPKeyState_t *) (param))->flags)
#define KEY_VKEY(param) (((XPKeyState_t *) (param))->vkey)

#define IN_RECT(x, y, l, t, r, b)	\
	(((x) >= (l)) && ((x) <= (r)) && ((y) >= (b)) && ((y) <= (t)))

/*
 * XPWidgetCreate_t
 * 
 * This structure contains all of the parameters needed to create a wiget. It 
 * is used with XPUCreateWidgets to create widgets in bulk from an array.  All 
 * parameters correspond to those of XPCreateWidget except for the container 
 * index. If the container index is equal to the index of a widget in the 
 * array, the widget in the array passed to XPUCreateWidgets is used as the 
 * parent of this widget.  Note that if you pass an index greater than your 
 * own position in the array, the parent you are requesting will not exist 
 * yet. If the container index is NO_PARENT, the parent widget is specified as 
 * NULL. If the container index is PARAM_PARENT, the widget passed into 
 * XPUCreateWidgets is used.                                                   
 *
 */
typedef struct {
     int                       left;
     int                       top;
     int                       right;
     int                       bottom;
     int                       visible;
     const char *              descriptor;
     int                       isRoot;
     int                       containerIndex;
     XPWidgetClass             widgetClass;
} XPWidgetCreate_t;

#define NO_PARENT            -1

#define PARAM_PARENT         -2

#define WIDGET_COUNT(x)	((sizeof(x) / sizeof(XPWidgetCreate_t)))
		
/*
 * XPUCreateWidgets
 * 
 * This function creates a series of widgets from a table...see 
 * XPCreateWidget_t above.  Pass in an array of widget creation structures and 
 * an array of widget IDs that will receive each widget. 
 * 
 * Widget parents are specified by index into the created widget table, 
 * allowing  you to create nested widget structures.  You can create multiple 
 * widget trees in one table.  Generally you should create widget trees from 
 * the top down. 
 * 
 * You can also pass in a widget ID that will be used when the widget's parent 
 * is listed as PARAM_PARENT; this allows you to embed widgets created with  
 * XPUCreateWidgets in a widget created previously.                            
 *
 */
WIDGET_API void                 XPUCreateWidgets(
                                   const XPWidgetCreate_t * inWidgetDefs,    
                                   int                  inCount,    
                                   XPWidgetID           inParamParent,    
                                   XPWidgetID *         ioWidgets);    

/*
 * XPUMoveWidgetBy
 * 
 * Simply moves a widget by an amount, +x = right, +y=up, without resizing the 
 * widget.                                                                     
 *
 */
WIDGET_API void                 XPUMoveWidgetBy(
                                   XPWidgetID           inWidget,    
                                   int                  inDeltaX,    
                                   int                  inDeltaY);    

/***************************************************************************
 * LAYOUT MANAGERS
 ***************************************************************************/
/*
 * The layout managers are widget behavior functions for handling where 
 * widgets move. Layout managers can be called from a widget function or 
 * attached to a widget later.                                                 
 *
 */



/*
 * XPUFixedLayout
 * 
 * This function causes the widget to maintain its children in fixed position 
 * relative to itself as it is resized.  Use this on the top level 'window' 
 * widget for your window.                                                     
 *
 */
WIDGET_API int                  XPUFixedLayout(
                                   XPWidgetMessage      inMessage,    
                                   XPWidgetID           inWidget,    
                                   intptr_t             inParam1,    
                                   intptr_t             inParam2);    

/***************************************************************************
 * WIDGET PROC BEHAVIORS
 ***************************************************************************/
/*
 * These widget behavior functions add other useful behaviors to widgets. 
 * These functions cannot be attached to a widget; they must be called from 
 * your widget function.                                                       
 *
 */



/*
 * XPUSelectIfNeeded
 * 
 * This causes the widget to bring its window to the foreground if it is not 
 * already.  inEatClick specifies whether clicks in the background should be 
 * consumed by bringin the window to the foreground.                           
 *
 */
WIDGET_API int                  XPUSelectIfNeeded(
                                   XPWidgetMessage      inMessage,    
                                   XPWidgetID           inWidget,    
                                   intptr_t             inParam1,    
                                   intptr_t             inParam2,    
                                   int                  inEatClick);    

/*
 * XPUDefocusKeyboard
 * 
 * This causes a click in the widget to send keyboard focus back to X-Plane. 
 * This stops editing of any text fields, etc.                                 
 *
 */
WIDGET_API int                  XPUDefocusKeyboard(
                                   XPWidgetMessage      inMessage,    
                                   XPWidgetID           inWidget,    
                                   intptr_t             inParam1,    
                                   intptr_t             inParam2,    
                                   int                  inEatClick);    

/*
 * XPUDragWidget
 * 
 * XPUDragWidget drags the widget in response to mouse clicks.  Pass in not 
 * only the event, but the global coordinates of the drag region, which might 
 * be a sub-region of your widget (for example, a title bar).                  
 *
 */
WIDGET_API int                  XPUDragWidget(
                                   XPWidgetMessage      inMessage,    
                                   XPWidgetID           inWidget,    
                                   intptr_t             inParam1,    
                                   intptr_t             inParam2,    
                                   int                  inLeft,    
                                   int                  inTop,    
                                   int                  inRight,    
                                   int                  inBottom);    

#ifdef __cplusplus
}
#endif

#endif
