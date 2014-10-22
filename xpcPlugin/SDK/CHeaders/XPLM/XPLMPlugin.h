#ifndef _XPLMPlugin_h_
#define _XPLMPlugin_h_

/*
 * Copyright 2005-2012 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 2.1.1                                                  
 *
 */

/*
 * These APIs provide facilities to find and work with other plugins and 
 * manage other plugins.                                                       
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * FINDING PLUGINS
 ***************************************************************************/
/*
 * These APIs allow you to find another plugin or yourself, or iterate across 
 * all plugins.  For example, if you wrote an FMS plugin that needed to talk 
 * to an autopilot plugin, you could use these APIs to locate the autopilot 
 * plugin.                                                                     
 *
 */



/*
 * XPLMGetMyID
 * 
 * This routine returns the plugin ID of the calling plug-in.  Call this to 
 * get your own ID.                                                            
 *
 */
XPLM_API XPLMPluginID         XPLMGetMyID(void);

/*
 * XPLMCountPlugins
 * 
 * This routine returns the total number of plug-ins that are loaded, both 
 * disabled and enabled.                                                       
 *
 */
XPLM_API int                  XPLMCountPlugins(void);

/*
 * XPLMGetNthPlugin
 * 
 * This routine returns the ID of a plug-in by index.  Index is 0 based from 0 
 * to XPLMCountPlugins-1, inclusive. Plugins may be returned in any arbitrary 
 * order.                                                                      
 *
 */
XPLM_API XPLMPluginID         XPLMGetNthPlugin(
                                   int                  inIndex);    

/*
 * XPLMFindPluginByPath
 * 
 * This routine returns the plug-in ID of the plug-in whose file exists at the 
 * passed in absolute file system path.  XPLM_NO_PLUGIN_ID is returned if the 
 * path does not point to a currently loaded plug-in.                          
 *
 */
XPLM_API XPLMPluginID         XPLMFindPluginByPath(
                                   const char *         inPath);    

/*
 * XPLMFindPluginBySignature
 * 
 * This routine returns the plug-in ID of the plug-in whose signature matches 
 * what is passed in or XPLM_NO_PLUGIN_ID if no running plug-in has this 
 * signature.  Signatures are the best way to identify another plug-in as they 
 * are independent of the file system path of a plug-in or the human-readable 
 * plug-in name, and should be unique for all plug-ins.  Use this routine to 
 * locate another plugin that your plugin interoperates with                   
 *
 */
XPLM_API XPLMPluginID         XPLMFindPluginBySignature(
                                   const char *         inSignature);    

/*
 * XPLMGetPluginInfo
 * 
 * This routine returns information about a plug-in.  Each parameter should be 
 * a pointer to a buffer of at least 256 characters, or NULL to not receive 
 * the information. 
 * 
 * outName - the human-readable name of the plug-in.   outFilePath - the 
 * absolute file path to the file that contains this plug-in. outSignature - a 
 * unique string that identifies this plug-in. outDescription - a 
 * human-readable description of this plug-in.                                 
 *
 */
XPLM_API void                 XPLMGetPluginInfo(
                                   XPLMPluginID         inPlugin,    
                                   char *               outName,    /* Can be NULL */
                                   char *               outFilePath,    /* Can be NULL */
                                   char *               outSignature,    /* Can be NULL */
                                   char *               outDescription);    /* Can be NULL */

/***************************************************************************
 * ENABLING/DISABLING PLUG-INS
 ***************************************************************************/
/*
 * These routines are used to work with plug-ins and manage them.  Most 
 * plugins will not need to use these APIs.                                    
 *
 */



/*
 * XPLMIsPluginEnabled
 * 
 * Returns whether the specified plug-in is enabled for running.               
 *
 */
XPLM_API int                  XPLMIsPluginEnabled(
                                   XPLMPluginID         inPluginID);    

/*
 * XPLMEnablePlugin
 * 
 * This routine enables a plug-in if it is not already enabled.  It returns 1 
 * if the plugin was enabled or successfully enables itself, 0 if it does not. 
 * Plugins may fail to enable (for example, if resources cannot be acquired) 
 * by returning 0 from their XPluginEnable callback.                           
 *
 */
XPLM_API int                  XPLMEnablePlugin(
                                   XPLMPluginID         inPluginID);    

/*
 * XPLMDisablePlugin
 * 
 * This routine disableds an enabled plug-in.                                  
 *
 */
XPLM_API void                 XPLMDisablePlugin(
                                   XPLMPluginID         inPluginID);    

/*
 * XPLMReloadPlugins
 * 
 * This routine reloads all plug-ins.  Once this routine is called and you 
 * return from the callback you were within (e.g. a menu select callback) you 
 * will receive your XPluginDisable and XPluginStop callbacks and your  DLL 
 * will be unloaded, then the start process happens as if the sim was starting 
 * up.                                                                         
 *
 */
XPLM_API void                 XPLMReloadPlugins(void);

/***************************************************************************
 * INTERPLUGIN MESSAGING
 ***************************************************************************/
/*
 * Plugin messages are defined as 32-bit integers.  Messages below 0x00FFFFFF 
 * are reserved for X-Plane and the plugin SDK.   
 * 
 * Messages have two conceptual uses: notifications and commands.  Commands 
 * are  sent from one plugin to another to induce behavior; notifications are 
 * sent  from one plugin to all others for informational purposes.  It is 
 * important that commands and notifications not have the same values because 
 * this could cause a notification sent by one plugin to accidentally induce a 
 * command in another. 
 * 
 * By convention, plugin-defined notifications should have the high bit set  
 * (e.g. be greater or equal to unsigned 0x8000000) while commands should have 
 * this bit be cleared. 
 * 
 * The following messages are sent to your plugin by x-plane.                  
 *
 */



/* This message is sent to your plugin whenever the user's plane crashes.      */
#define XPLM_MSG_PLANE_CRASHED 101

/* This message is sent to your plugin whenever a new plane is loaded.  The    *
 * parameter is the number of the plane being loaded; 0 indicates the user's   *
 * plane.                                                                      */
#define XPLM_MSG_PLANE_LOADED 102

/* This messages is called whenever the user's plane is positioned at a new    *
 * airport.                                                                    */
#define XPLM_MSG_AIRPORT_LOADED 103

/* This message is sent whenever new scenery is loaded.  Use datarefs to       *
 * determine the new scenery files that were loaded.                           */
#define XPLM_MSG_SCENERY_LOADED 104

/* This message is sent whenever the user adjusts the number of X-Plane        *
 * aircraft models.  You must use XPLMCountPlanes to find out how many planes  *
 * are now available.  This message will only be sent in XP7 and higher        *
 * because in XP6 the number of aircraft is not user-adjustable.               */
#define XPLM_MSG_AIRPLANE_COUNT_CHANGED 105

#if defined(XPLM200)
/* This message is sent to your plugin whenever a plane is unloaded.  The      *
 * parameter is the number of the plane being unloaded; 0 indicates the user's *
 * plane.  The parameter is of type int, passed as the value of the pointer.   *
 * (That is: the parameter is an int, not a pointer to an int.)                */
#define XPLM_MSG_PLANE_UNLOADED 106
#endif /* XPLM200 */

#if defined(XPLM210)
/* This message is sent to your plugin right before X-Plane writes its         *
 * preferences file.  You can use this for two purposes: to write your own     *
 * preferences, and to modify any datarefs to influence preferences output.    *
 * For example, if your plugin temporarily modifies saved preferences, you can *
 * put them back to their default values here to avoid  having the tweaks be   *
 * persisted if your plugin is not loaded on the next invocation of X-Plane.   */
#define XPLM_MSG_WILL_WRITE_PREFS 107
#endif /* XPLM210 */

#if defined(XPLM210)
/* This message is sent to your plugin right after a livery is loaded for an   *
 * airplane.  You can use this to check the new livery (via datarefs) and      *
 * react accordingly.  The parameter is of type int, passed as the value of a  *
 * pointer and represents the aicraft plane number - 0 is the user's plane.    */
#define XPLM_MSG_LIVERY_LOADED 108
#endif /* XPLM210 */

/*
 * XPLMSendMessageToPlugin
 * 
 * This function sends a message to another plug-in or X-Plane.  Pass 
 * XPLM_NO_PLUGIN_ID to broadcast to all plug-ins.  Only enabled plug-ins with 
 * a message receive function receive the message.                             
 *
 */
XPLM_API void                 XPLMSendMessageToPlugin(
                                   XPLMPluginID         inPlugin,    
                                   int                  inMessage,    
                                   void *               inParam);    

#if defined(XPLM200)
/***************************************************************************
 * Plugin Features API
 ***************************************************************************/
/*
 * The plugin features API allows your plugin to "sign up" for additional 
 * capabilities and plugin system features that are normally disabled for 
 * backward compatibility.  This allows advanced plugins to "opt-in" to new 
 * behavior. 
 * 
 * Each feature is defined by a permanent string name.  The feature string 
 * names will vary with the particular  installation of X-Plane, so plugins 
 * should not expect a feature to be guaranteed present.                       
 *
 */




/*
 * XPLMFeatureEnumerator_f
 * 
 * You pass an XPLMFeatureEnumerator_f to get a list of all features supported 
 * by a given version running version of X-Plane.  This routine is called once 
 * for each feature.                                                           
 *
 */
typedef void (* XPLMFeatureEnumerator_f)(
                                   const char *         inFeature,    
                                   void *               inRef);    

/*
 * XPLMHasFeature
 * 
 * This returns 1 if the given installation of X-Plane supports a feature, or 
 * 0 if it does not.                                                           
 *
 */
XPLM_API int                  XPLMHasFeature(
                                   const char *         inFeature);    

/*
 * XPLMIsFeatureEnabled
 * 
 * This returns 1 if a feature is currently enabled for your plugin, or 0 if 
 * it is not enabled.  It is an error to call this routine with an unsupported 
 * feature.                                                                    
 *
 */
XPLM_API int                  XPLMIsFeatureEnabled(
                                   const char *         inFeature);    

/*
 * XPLMEnableFeature
 * 
 * This routine enables or disables a feature for your plugin.  This will 
 * change the running behavior of X-Plane and your plugin in some way, 
 * depending on the feature.                                                   
 *
 */
XPLM_API void                 XPLMEnableFeature(
                                   const char *         inFeature,    
                                   int                  inEnable);    

/*
 * XPLMEnumerateFeatures
 * 
 * This routine calls your enumerator callback once for each feature that this 
 * running version of X-Plane supports. Use this routine to determine all of 
 * the features that X-Plane can support.                                      
 *
 */
XPLM_API void                 XPLMEnumerateFeatures(
                                   XPLMFeatureEnumerator_f inEnumerator,    
                                   void *               inRef);    

#endif /* XPLM200 */
#ifdef __cplusplus
}
#endif

#endif
