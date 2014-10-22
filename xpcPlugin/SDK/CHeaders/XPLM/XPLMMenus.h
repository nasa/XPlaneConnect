#ifndef _XPLMMenus_h_
#define _XPLMMenus_h_

/*
 * Copyright 2005-2012 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 2.1.1                                                  
 *
 */

/*
 * XPLMMenus - Theory of Operation 
 * 
 * Plug-ins can create menus in the menu bar of X-Plane.  This is done  by 
 * creating a menu and then creating items.  Menus are referred to by an 
 * opaque ID.  Items are referred to by index number.  For each menu and item 
 * you specify a void *.  Per menu you specify a handler function that is 
 * called with each void * when the menu item is picked.  Menu item indices 
 * are zero based.                                                             
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * XPLM MENUS
 ***************************************************************************/
/*
 *                                                                             
 *
 */



/*
 * XPLMMenuCheck
 * 
 * These enumerations define the various 'check' states for an X-Plane menu.  
 * 'checking' in x-plane actually appears as a light which may or may not be 
 * lit.  So there are  three possible states.                                  
 *
 */
enum {
     /* there is no symbol to the left of the menu item.                            */
     xplm_Menu_NoCheck                        = 0

     /* the menu has a mark next to it that is unmarked (not lit).                  */
    ,xplm_Menu_Unchecked                      = 1

     /* the menu has a mark next to it that is checked (lit).                       */
    ,xplm_Menu_Checked                        = 2


};
typedef int XPLMMenuCheck;

/*
 * XPLMMenuID
 * 
 * This is a unique ID for each menu you create.                               
 *
 */
typedef void * XPLMMenuID;

/*
 * XPLMMenuHandler_f
 * 
 * A menu handler function takes two reference pointers, one for the menu 
 * (specified when the menu was created) and one for the item (specified when 
 * the item was created).                                                      
 *
 */
typedef void (* XPLMMenuHandler_f)(
                                   void *               inMenuRef,    
                                   void *               inItemRef);    

/*
 * XPLMFindPluginsMenu
 * 
 * This function returns the ID of the plug-ins menu, which is created for you 
 * at startup.                                                                 
 *
 */
XPLM_API XPLMMenuID           XPLMFindPluginsMenu(void);

/*
 * XPLMCreateMenu
 * 
 * This function creates a new menu and returns its ID.  It returns NULL if 
 * the menu cannot be created.  Pass in a parent menu ID and an item index to 
 * create a submenu, or NULL for the parent menu to put the menu in the menu 
 * bar.  The menu's name is only used if the menu is in the menubar.  You also 
 * pass a handler function and a menu reference value. Pass NULL for the 
 * handler if you do not need callbacks from the menu (for example, if it only 
 * contains submenus). 
 * 
 * Important: you must pass a valid, non-empty menu title even if the menu is 
 * a submenu where the title is not visible.                                   
 *
 */
XPLM_API XPLMMenuID           XPLMCreateMenu(
                                   const char *         inName,    
                                   XPLMMenuID           inParentMenu,    
                                   int                  inParentItem,    
                                   XPLMMenuHandler_f    inHandler,    
                                   void *               inMenuRef);    

/*
 * XPLMDestroyMenu
 * 
 * This function destroys a menu that you have created.  Use this to remove a 
 * submenu if necessary.  (Normally this function will not be necessary.)      
 *
 */
XPLM_API void                 XPLMDestroyMenu(
                                   XPLMMenuID           inMenuID);    

/*
 * XPLMClearAllMenuItems
 * 
 * This function removes all menu items from a menu, allowing you to rebuild 
 * it.  Use this function if you need to change the number of items on a menu. 
 *
 */
XPLM_API void                 XPLMClearAllMenuItems(
                                   XPLMMenuID           inMenuID);    

/*
 * XPLMAppendMenuItem
 * 
 * This routine appends a new menu item to the bottom of a menu and returns 
 * its index. Pass in the menu to add the item to, the items name, and a void 
 * * ref for this item. If you pass in inForceEnglish, this menu item will be 
 * drawn using the english character set no matter what language x-plane is 
 * running in.  Otherwise the menu item will be drawn localized.  (An example 
 * of why you'd want to do this is for a proper name.)  See XPLMUtilities for 
 * determining the current langauge.                                           
 *
 */
XPLM_API int                  XPLMAppendMenuItem(
                                   XPLMMenuID           inMenu,    
                                   const char *         inItemName,    
                                   void *               inItemRef,    
                                   int                  inForceEnglish);    

/*
 * XPLMAppendMenuSeparator
 * 
 * This routine adds a seperator to the end of a menu.                         
 *
 */
XPLM_API void                 XPLMAppendMenuSeparator(
                                   XPLMMenuID           inMenu);    

/*
 * XPLMSetMenuItemName
 * 
 * This routine changes the name of an existing menu item.  Pass in the menu 
 * ID and the index of the menu item.                                          
 *
 */
XPLM_API void                 XPLMSetMenuItemName(
                                   XPLMMenuID           inMenu,    
                                   int                  inIndex,    
                                   const char *         inItemName,    
                                   int                  inForceEnglish);    

/*
 * XPLMCheckMenuItem
 * 
 * Set whether a menu item is checked.  Pass in the menu ID and item index.    
 *
 */
XPLM_API void                 XPLMCheckMenuItem(
                                   XPLMMenuID           inMenu,    
                                   int                  index,    
                                   XPLMMenuCheck        inCheck);    

/*
 * XPLMCheckMenuItemState
 * 
 * This routine returns whether a menu item is checked or not. A menu item's 
 * check mark may be on or off, or a menu may not have an icon at all.         
 *
 */
XPLM_API void                 XPLMCheckMenuItemState(
                                   XPLMMenuID           inMenu,    
                                   int                  index,    
                                   XPLMMenuCheck *      outCheck);    

/*
 * XPLMEnableMenuItem
 * 
 * Sets whether this menu item is enabled.  Items start out enabled.           
 *
 */
XPLM_API void                 XPLMEnableMenuItem(
                                   XPLMMenuID           inMenu,    
                                   int                  index,    
                                   int                  enabled);    

#if defined(XPLM210)
/*
 * XPLMRemoveMenuItem
 * 
 * Removes one item from a menu.  Note that all menu items below are moved up 
 * one; your plugin must track the change in index numbers.                    
 *
 */
XPLM_API void                 XPLMRemoveMenuItem(
                                   XPLMMenuID           inMenu,    
                                   int                  inIndex);    
#endif /* XPLM210 */

#ifdef __cplusplus
}
#endif

#endif
