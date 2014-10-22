#ifndef _XPLMUtilities_h_
#define _XPLMUtilities_h_

/*
 * Copyright 2005-2012 Sandy Barbour and Ben Supnik
 * 
 * All rights reserved.  See license.txt for usage.
 * 
 * X-Plane SDK Version: 2.1.1                                                  
 *
 */

/*
 *                                                                             
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * X-PLANE USER INTERACTION
 ***************************************************************************/
/*
 * The user interaction APIs let you simulate commands the user can do with a 
 * joystick, keyboard etc.  Note that it is generally safer for future 
 * compatibility to use one of these commands than to manipulate the 
 * underlying sim data.                                                        
 *
 */



/*
 * XPLMCommandKeyID
 * 
 * These enums represent all the keystrokes available within x-plane.  They 
 * can be sent to x-plane directly.  For example, you can reverse thrust using 
 * these  enumerations.                                                        
 *
 */
enum {
          xplm_key_pause=0,
          xplm_key_revthrust,
          xplm_key_jettison,
          xplm_key_brakesreg,
          xplm_key_brakesmax,
          xplm_key_gear,
          xplm_key_timedn,
          xplm_key_timeup,
          xplm_key_fadec,
          xplm_key_otto_dis,
          xplm_key_otto_atr,
          xplm_key_otto_asi,
          xplm_key_otto_hdg,
          xplm_key_otto_gps,
          xplm_key_otto_lev,
          xplm_key_otto_hnav,
          xplm_key_otto_alt,
          xplm_key_otto_vvi,
          xplm_key_otto_vnav,
          xplm_key_otto_nav1,
          xplm_key_otto_nav2,
          xplm_key_targ_dn,
          xplm_key_targ_up,
          xplm_key_hdgdn,
          xplm_key_hdgup,
          xplm_key_barodn,
          xplm_key_baroup,
          xplm_key_obs1dn,
          xplm_key_obs1up,
          xplm_key_obs2dn,
          xplm_key_obs2up,
          xplm_key_com1_1,
          xplm_key_com1_2,
          xplm_key_com1_3,
          xplm_key_com1_4,
          xplm_key_nav1_1,
          xplm_key_nav1_2,
          xplm_key_nav1_3,
          xplm_key_nav1_4,
          xplm_key_com2_1,
          xplm_key_com2_2,
          xplm_key_com2_3,
          xplm_key_com2_4,
          xplm_key_nav2_1,
          xplm_key_nav2_2,
          xplm_key_nav2_3,
          xplm_key_nav2_4,
          xplm_key_adf_1,
          xplm_key_adf_2,
          xplm_key_adf_3,
          xplm_key_adf_4,
          xplm_key_adf_5,
          xplm_key_adf_6,
          xplm_key_transpon_1,
          xplm_key_transpon_2,
          xplm_key_transpon_3,
          xplm_key_transpon_4,
          xplm_key_transpon_5,
          xplm_key_transpon_6,
          xplm_key_transpon_7,
          xplm_key_transpon_8,
          xplm_key_flapsup,
          xplm_key_flapsdn,
          xplm_key_cheatoff,
          xplm_key_cheaton,
          xplm_key_sbrkoff,
          xplm_key_sbrkon,
          xplm_key_ailtrimL,
          xplm_key_ailtrimR,
          xplm_key_rudtrimL,
          xplm_key_rudtrimR,
          xplm_key_elvtrimD,
          xplm_key_elvtrimU,
          xplm_key_forward,
          xplm_key_down,
          xplm_key_left,
          xplm_key_right,
          xplm_key_back,
          xplm_key_tower,
          xplm_key_runway,
          xplm_key_chase,
          xplm_key_free1,
          xplm_key_free2,
          xplm_key_spot,
          xplm_key_fullscrn1,
          xplm_key_fullscrn2,
          xplm_key_tanspan,
          xplm_key_smoke,
          xplm_key_map,
          xplm_key_zoomin,
          xplm_key_zoomout,
          xplm_key_cycledump,
          xplm_key_replay,
          xplm_key_tranID,
          xplm_key_max
};
typedef int XPLMCommandKeyID;

/*
 * XPLMCommandButtonID
 * 
 * These are enumerations for all of the things you can do with a joystick 
 * button in X-Plane.  They currently match the buttons menu in the equipment 
 * setup dialog, but these enums will be stable even if they change in 
 * X-Plane.                                                                    
 *
 */
enum {
          xplm_joy_nothing=0,
          xplm_joy_start_all,
          xplm_joy_start_0,
          xplm_joy_start_1,
          xplm_joy_start_2,
          xplm_joy_start_3,
          xplm_joy_start_4,
          xplm_joy_start_5,
          xplm_joy_start_6,
          xplm_joy_start_7,
          xplm_joy_throt_up,
          xplm_joy_throt_dn,
          xplm_joy_prop_up,
          xplm_joy_prop_dn,
          xplm_joy_mixt_up,
          xplm_joy_mixt_dn,
          xplm_joy_carb_tog,
          xplm_joy_carb_on,
          xplm_joy_carb_off,
          xplm_joy_trev,
          xplm_joy_trm_up,
          xplm_joy_trm_dn,
          xplm_joy_rot_trm_up,
          xplm_joy_rot_trm_dn,
          xplm_joy_rud_lft,
          xplm_joy_rud_cntr,
          xplm_joy_rud_rgt,
          xplm_joy_ail_lft,
          xplm_joy_ail_cntr,
          xplm_joy_ail_rgt,
          xplm_joy_B_rud_lft,
          xplm_joy_B_rud_rgt,
          xplm_joy_look_up,
          xplm_joy_look_dn,
          xplm_joy_look_lft,
          xplm_joy_look_rgt,
          xplm_joy_glance_l,
          xplm_joy_glance_r,
          xplm_joy_v_fnh,
          xplm_joy_v_fwh,
          xplm_joy_v_tra,
          xplm_joy_v_twr,
          xplm_joy_v_run,
          xplm_joy_v_cha,
          xplm_joy_v_fr1,
          xplm_joy_v_fr2,
          xplm_joy_v_spo,
          xplm_joy_flapsup,
          xplm_joy_flapsdn,
          xplm_joy_vctswpfwd,
          xplm_joy_vctswpaft,
          xplm_joy_gear_tog,
          xplm_joy_gear_up,
          xplm_joy_gear_down,
          xplm_joy_lft_brake,
          xplm_joy_rgt_brake,
          xplm_joy_brakesREG,
          xplm_joy_brakesMAX,
          xplm_joy_speedbrake,
          xplm_joy_ott_dis,
          xplm_joy_ott_atr,
          xplm_joy_ott_asi,
          xplm_joy_ott_hdg,
          xplm_joy_ott_alt,
          xplm_joy_ott_vvi,
          xplm_joy_tim_start,
          xplm_joy_tim_reset,
          xplm_joy_ecam_up,
          xplm_joy_ecam_dn,
          xplm_joy_fadec,
          xplm_joy_yaw_damp,
          xplm_joy_art_stab,
          xplm_joy_chute,
          xplm_joy_JATO,
          xplm_joy_arrest,
          xplm_joy_jettison,
          xplm_joy_fuel_dump,
          xplm_joy_puffsmoke,
          xplm_joy_prerotate,
          xplm_joy_UL_prerot,
          xplm_joy_UL_collec,
          xplm_joy_TOGA,
          xplm_joy_shutdown,
          xplm_joy_con_atc,
          xplm_joy_fail_now,
          xplm_joy_pause,
          xplm_joy_rock_up,
          xplm_joy_rock_dn,
          xplm_joy_rock_lft,
          xplm_joy_rock_rgt,
          xplm_joy_rock_for,
          xplm_joy_rock_aft,
          xplm_joy_idle_hilo,
          xplm_joy_lanlights,
          xplm_joy_max
};
typedef int XPLMCommandButtonID;

/*
 * XPLMHostApplicationID
 * 
 * The plug-in system is based on Austin's cross-platform OpenGL framework and 
 * could theoretically be adapted to  run in other apps like WorldMaker.  The 
 * plug-in system also runs against a test harness for internal development 
 * and could be adapted to another flight sim (in theory at least).  So an ID 
 * is providing allowing plug-ins to  indentify what app they are running 
 * under.                                                                      
 *
 */
enum {
     xplm_Host_Unknown                        = 0

    ,xplm_Host_XPlane                         = 1

    ,xplm_Host_PlaneMaker                     = 2

    ,xplm_Host_WorldMaker                     = 3

    ,xplm_Host_Briefer                        = 4

    ,xplm_Host_PartMaker                      = 5

    ,xplm_Host_YoungsMod                      = 6

    ,xplm_Host_XAuto                          = 7


};
typedef int XPLMHostApplicationID;

/*
 * XPLMLanguageCode
 * 
 * These enums define what language the sim is running in.  These enumerations 
 * do not imply that the sim can or does run in all of these languages; they 
 * simply provide a known encoding in the event that a given sim version is 
 * localized to a certain language.                                            
 *
 */
enum {
     xplm_Language_Unknown                    = 0

    ,xplm_Language_English                    = 1

    ,xplm_Language_French                     = 2

    ,xplm_Language_German                     = 3

    ,xplm_Language_Italian                    = 4

    ,xplm_Language_Spanish                    = 5

    ,xplm_Language_Korean                     = 6

#if defined(XPLM200)
    ,xplm_Language_Russian                    = 7

#endif /* XPLM200 */
#if defined(XPLM200)
    ,xplm_Language_Greek                      = 8

#endif /* XPLM200 */
#if defined(XPLM200)
    ,xplm_Language_Japanese                   = 9

#endif /* XPLM200 */
#if defined(XPLM200)
    ,xplm_Language_Chinese                    = 10

#endif /* XPLM200 */

};
typedef int XPLMLanguageCode;

#if defined(XPLM200)
/*
 * XPLMDataFileType
 * 
 * These enums define types of data files you can load or unload using the 
 * SDK.                                                                        
 *
 */
enum {
     /* A situation (.sit) file, which starts off a flight in a given               *
      * configuration.                                                              */
     xplm_DataFile_Situation                  = 1

     /* A situation movie (.smo) file, which replays a past flight.                 */
    ,xplm_DataFile_ReplayMovie                = 2


};
typedef int XPLMDataFileType;
#endif /* XPLM200 */

#if defined(XPLM200)
/*
 * XPLMError_f
 * 
 * An XPLM error callback is a function that you provide to receive debugging 
 * information from the plugin SDK.  See XPLMSetErrorCallback for more 
 * information.  NOTE: for the sake of debugging, your error callback will be 
 * called even if your plugin is not enabled, allowing you to receive debug 
 * info in your XPluginStart and XPluginStop  callbacks.  To avoid causing 
 * logic errors in the management code, do not call any other plugin routines 
 * from your error callback - it is only meant for logging!                    
 *
 */
typedef void (* XPLMError_f)(
                                   const char *         inMessage);    
#endif /* XPLM200 */

/*
 * XPLMSimulateKeyPress
 * 
 * This function simulates a key being pressed for x-plane.  The keystroke 
 * goes directly to x-plane; it is never sent to any plug-ins.  However, since 
 * this is a raw key stroke it may be mapped by the keys file or enter text 
 * into a field. 
 * 
 * WARNING: This function will be deprecated; do not use it.  Instead use 
 * XPLMCommandKeyStroke.                                                       
 *
 */
XPLM_API void                 XPLMSimulateKeyPress(
                                   int                  inKeyType,    
                                   int                  inKey);    

/*
 * XPLMSpeakString
 * 
 * This function displays the string in a translucent overlay over the current 
 * display and also speaks the string if text-to-speech is enabled.  The 
 * string is spoken asynchronously, this function returns immediately.         
 *
 */
XPLM_API void                 XPLMSpeakString(
                                   const char *         inString);    

/*
 * XPLMCommandKeyStroke
 * 
 * This routine simulates a command-key stroke.  However, the keys are done by 
 * function, not by actual letter, so this function works even if the user has 
 * remapped their keyboard.  Examples of things you might do with this include 
 * pausing the simulator.                                                      
 *
 */
XPLM_API void                 XPLMCommandKeyStroke(
                                   XPLMCommandKeyID     inKey);    

/*
 * XPLMCommandButtonPress
 * 
 * This function simulates any of the actions that might be taken by pressing 
 * a joystick button.  However, this lets you call the command directly rather 
 * than have to know which button is mapped where.  Important: you must 
 * release each button you press.  The APIs are separate so that you can 'hold 
 * down' a button for a fixed amount of time.                                  
 *
 */
XPLM_API void                 XPLMCommandButtonPress(
                                   XPLMCommandButtonID  inButton);    

/*
 * XPLMCommandButtonRelease
 * 
 * This function simulates any of the actions that might be taken by pressing 
 * a joystick button.  See XPLMCommandButtonPress                              
 *
 */
XPLM_API void                 XPLMCommandButtonRelease(
                                   XPLMCommandButtonID  inButton);    

/*
 * XPLMGetVirtualKeyDescription
 * 
 * Given a virtual key code (as defined in XPLMDefs.h) this routine returns a 
 * human-readable string describing the character.  This routine is provided 
 * for showing users what keyboard mappings they have set up.  The string may 
 * read 'unknown' or be a blank or NULL string if the virtual key is unknown.  
 *
 */
XPLM_API const char *         XPLMGetVirtualKeyDescription(
                                   char                 inVirtualKey);    

/***************************************************************************
 * X-PLANE MISC
 ***************************************************************************/
/*
 *                                                                             
 *
 */



/*
 * XPLMReloadScenery
 * 
 * XPLMReloadScenery reloads the current set of scenery.  You can use this 
 * function in two typical ways: simply call it to reload the scenery, picking 
 * up any new installed scenery, .env files, etc. from disk.  Or, change the 
 * lat/ref and lon/ref data refs and then call this function to shift the 
 * scenery environment.                                                        
 *
 */
XPLM_API void                 XPLMReloadScenery(void);

/*
 * XPLMGetSystemPath
 * 
 * This function returns the full path to the X-System folder.  Note that this 
 * is a directory path, so it ends in a trailing : or /.  The buffer you pass 
 * should be at least 512 characters long.                                     
 *
 */
XPLM_API void                 XPLMGetSystemPath(
                                   char *               outSystemPath);    

/*
 * XPLMGetPrefsPath
 * 
 * This routine returns a full path to the proper directory to store 
 * preferences in. It ends in a : or /.  The buffer you pass should be at 
 * least 512 characters long.                                                  
 *
 */
XPLM_API void                 XPLMGetPrefsPath(
                                   char *               outPrefsPath);    

/*
 * XPLMGetDirectorySeparator
 * 
 * This routine returns a string with one char and a null terminator that is 
 * the directory separator for the current platform.  This allows you to write 
 * code that concatinates directory paths without having to #ifdef for 
 * platform.                                                                   
 *
 */
XPLM_API const char *         XPLMGetDirectorySeparator(void);

/*
 * XPLMExtractFileAndPath
 * 
 * Given a full path to a file, this routine separates the path from the file. 
 * If the path is a partial directory (e.g. ends in : or \) the trailing 
 * directory separator is removed.  This routine works in-place; a pointer to 
 * the file part of the buffer is returned; the original buffer still starts 
 * with the path.                                                              
 *
 */
XPLM_API char *               XPLMExtractFileAndPath(
                                   char *               inFullPath);    

/*
 * XPLMGetDirectoryContents
 * 
 * This routine returns a list of files in a directory (specified by a full 
 * path, no trailing : or \).  The output is returned as a list of NULL 
 * terminated strings. An index array (if specified) is filled with pointers 
 * into the strings.  This routine The last file is indicated by a zero-length 
 * string (and NULL in the indices).  This routine will return 1 if you had 
 * capacity for all files or 0 if you did not.  You can also skip a given 
 * number of files. 
 * 
 * inDirectoryPath - a null terminated C string containing the full path to 
 * the directory with no trailing directory char. 
 * 
 * inFirstReturn - the zero-based index of the first file in the directory to 
 * return.  (Usually zero to fetch all in one pass.) 
 * 
 * outFileNames - a buffer to receive a series of sequential null terminated 
 * C-string file names.  A zero-length C string will be appended to the very 
 * end. 
 * 
 * inFileNameBufSize - the size of the file name buffer in bytes. 
 * 
 * outIndices - a pointer to an array of character pointers that will become 
 * an index into the directory.  The last file will be followed by a NULL 
 * value.  Pass NULL if you do not want indexing information. 
 * 
 * inIndexCount - the max size of the index in entries. 
 * 
 * outTotalFiles - if not NULL, this is filled in with the number of files in 
 * the  directory. 
 * 
 * outReturnedFiles - if not NULL, the number of files returned by this 
 * iteration. 
 * 
 * Return value - 1 if all info could be returned, 0 if there was a buffer 
 * overrun. 
 * 
 * WARNING: Before X-Plane 7 this routine did not properly iterate through 
 * directories.  If X-Plane 6 compatibility is needed, use your own code to 
 * iterate directories.                                                        
 *
 */
XPLM_API int                  XPLMGetDirectoryContents(
                                   const char *         inDirectoryPath,    
                                   int                  inFirstReturn,    
                                   char *               outFileNames,    
                                   int                  inFileNameBufSize,    
                                   char **              outIndices,    /* Can be NULL */
                                   int                  inIndexCount,    
                                   int *                outTotalFiles,    /* Can be NULL */
                                   int *                outReturnedFiles);    /* Can be NULL */

/*
 * XPLMInitialized
 * 
 * This function returns 1 if X-Plane has properly initialized the plug-in 
 * system. If this routine returns 0, many XPLM functions will not work. 
 * 
 * NOTE: Under normal circumstances a plug-in should never be running while 
 * the  plug-in manager is not initialized. 
 * 
 * WARNING: This function is generally not needed and may be deprecated in the 
 * future.                                                                     
 *
 */
XPLM_API int                  XPLMInitialized(void);

/*
 * XPLMGetVersions
 * 
 * This routine returns the revision of both X-Plane and the XPLM DLL.  All 
 * versions are three-digit decimal numbers (e.g. 606 for version 6.06 of 
 * X-Plane); the current revision of the XPLM is 200 (2.00).  This routine 
 * also returns the host ID of the app running us. 
 * 
 * The most common use of this routine is to special-case around x-plane 
 * version-specific behavior.                                                  
 *
 */
XPLM_API void                 XPLMGetVersions(
                                   int *                outXPlaneVersion,    
                                   int *                outXPLMVersion,    
                                   XPLMHostApplicationID * outHostID);    

/*
 * XPLMGetLanguage
 * 
 * This routine returns the langauge the sim is running in.                    
 *
 */
XPLM_API XPLMLanguageCode     XPLMGetLanguage(void);

/*
 * XPLMDebugString
 * 
 * This routine outputs a C-style string to the Log.txt file.  The file is 
 * immediately flushed so you will not lose  data.  (This does cause a 
 * performance penalty.)                                                       
 *
 */
XPLM_API void                 XPLMDebugString(
                                   const char *         inString);    

#if defined(XPLM200)
/*
 * XPLMSetErrorCallback
 * 
 * XPLMSetErrorCallback installs an error-reporting callback for your plugin.  
 * Normally the plugin system performs minimum diagnostics to maximize 
 * performance.  When you install an error callback, you will receive calls 
 * due to certain plugin errors, such as passing bad parameters or incorrect 
 * data. 
 * 
 * The intention is for you to install the error callback during debug 
 * sections and put a break-point inside your callback.  This will cause you 
 * to break into the debugger from within the SDK at the point in your plugin 
 * where you made an illegal call. 
 * 
 * Installing an error callback may activate error checking code that would 
 * not normally run, and this may adversely affect performance, so do not 
 * leave error callbacks installed in shipping plugins.                        
 *
 */
XPLM_API void                 XPLMSetErrorCallback(
                                   XPLMError_f          inCallback);    
#endif /* XPLM200 */

#if defined(XPLM200)
/*
 * XPLMFindSymbol
 * 
 * This routine will attempt to find the symbol passed in the inString 
 * parameter. If the symbol is found a pointer the function is returned, 
 * othewise the function will return NULL.                                     
 *
 */
XPLM_API void *               XPLMFindSymbol(
                                   const char *         inString);    
#endif /* XPLM200 */

#if defined(XPLM200)
/*
 * XPLMLoadDataFile
 * 
 * Loads a data file of a given type.  Paths must be relative to the X-System 
 * folder. To clear the replay, pass a NULL file name (this is only valid with 
 * replay movies, not sit files).                                              
 *
 */
XPLM_API int                  XPLMLoadDataFile(
                                   XPLMDataFileType     inFileType,    
                                   const char *         inFilePath);    /* Can be NULL */
#endif /* XPLM200 */

#if defined(XPLM200)
/*
 * XPLMSaveDataFile
 * 
 * Saves the current situation or replay; paths are relative to the X-System 
 * folder.                                                                     
 *
 */
XPLM_API int                  XPLMSaveDataFile(
                                   XPLMDataFileType     inFileType,    
                                   const char *         inFilePath);    
#endif /* XPLM200 */

#if defined(XPLM200)
/***************************************************************************
 * X-PLANE COMMAND MANAGEMENT
 ***************************************************************************/
/*
 * The command management APIs let plugins interact with the command-system in 
 * X-Plane, the abstraction behind keyboard presses and joystick buttons.  
 * This API lets you create new commands and modify the behavior (or get 
 * notification) of existing ones. 
 * 
 * An X-Plane command consists of three phases: a beginning, continuous 
 * repetition, and an ending.  The command may be repeated zero times in  the 
 * event that the user presses a button only momentarily.			                   
 *
 */




/*
 * XPLMCommandPhase
 * 
 * The phases of a command.                                                    
 *
 */
enum {
     /* The command is being started.                                               */
     xplm_CommandBegin                        = 0

     /* The command is continuing to execute.                                       */
    ,xplm_CommandContinue                     = 1

     /* The command has ended.                                                      */
    ,xplm_CommandEnd                          = 2


};
typedef int XPLMCommandPhase;

/*
 * XPLMCommandRef
 * 
 * A command ref is an opaque identifier for an X-Plane command.  Command 
 * references stay the same for the life of your plugin but not between 
 * executions of X-Plane.  Command refs are used to execute commands, create 
 * commands, and create callbacks for particular commands. 
 * 
 * Note that a command is not "owned" by a particular plugin.  Since many 
 * plugins may participate in a command's execution, the command does not go 
 * away if the plugin that created it is unloaded.                             
 *
 */
typedef void * XPLMCommandRef;

/*
 * XPLMCommandCallback_f
 * 
 * A command callback is a function in your plugin that is called when a 
 * command is pressed.  Your callback receives the commadn  reference for the 
 * particular command, the phase of the command that is executing, and a 
 * reference pointer that you specify when registering the callback. 
 * 
 * Your command handler should return 1 to let processing of the command 
 * continue to other plugins and X-Plane, or 0 to halt  processing, 
 * potentially bypassing X-Plane code.                                         
 *
 */
typedef int (* XPLMCommandCallback_f)(
                                   XPLMCommandRef       inCommand,    
                                   XPLMCommandPhase     inPhase,    
                                   void *               inRefcon);    

/*
 * XPLMFindCommand
 * 
 * XPLMFindCommand looks up a command by name, and returns its command 
 * reference or NULL if the command does not exist.                            
 *
 */
XPLM_API XPLMCommandRef       XPLMFindCommand(
                                   const char *         inName);    

/*
 * XPLMCommandBegin
 * 
 * XPLMCommandBegin starts the execution of a command, specified by its 
 * command reference.  The  command is "held down" until XPLMCommandEnd is 
 * called.                                                                     
 *
 */
XPLM_API void                 XPLMCommandBegin(
                                   XPLMCommandRef       inCommand);    

/*
 * XPLMCommandEnd
 * 
 * XPLMCommandEnd ends the execution of a given command that was started with 
 * XPLMCommandBegin.                                                           
 *
 */
XPLM_API void                 XPLMCommandEnd(
                                   XPLMCommandRef       inCommand);    

/*
 * XPLMCommandOnce
 * 
 * This executes a given command momentarily, that is, the command begins and 
 * ends immediately.                                                           
 *
 */
XPLM_API void                 XPLMCommandOnce(
                                   XPLMCommandRef       inCommand);    

/*
 * XPLMCreateCommand
 * 
 * XPLMCreateCommand creates a new command for a given string.  If the command 
 * already exists, the  existing command reference is returned.  The 
 * description may appear in user interface contexts, such as the joystick 
 * configuration screen.                                                       
 *
 */
XPLM_API XPLMCommandRef       XPLMCreateCommand(
                                   const char *         inName,    
                                   const char *         inDescription);    

/*
 * XPLMRegisterCommandHandler
 * 
 * XPLMRegisterCommandHandler registers a callback to be called when a command 
 * is executed.  You provide a callback with a reference pointer. 
 * 
 * If inBefore is true, your command handler callback will be executed before 
 * X-Plane executes the command, and returning 0 from your callback will 
 * disable X-Plane's processing of the command.  If inBefore is  false, your 
 * callback will run after X-Plane.  (You can register a single callback both 
 * before and after a command.)                                                
 *
 */
XPLM_API void                 XPLMRegisterCommandHandler(
                                   XPLMCommandRef       inComand,    
                                   XPLMCommandCallback_f inHandler,    
                                   int                  inBefore,    
                                   void *               inRefcon);    

/*
 * XPLMUnregisterCommandHandler
 * 
 * XPLMUnregisterCommandHandler removes a command callback registered with 
 * XPLMRegisterCommandHandler.                                                 
 *
 */
XPLM_API void                 XPLMUnregisterCommandHandler(
                                   XPLMCommandRef       inComand,    
                                   XPLMCommandCallback_f inHandler,    
                                   int                  inBefore,    
                                   void *               inRefcon);    

#endif /* XPLM200 */
#ifdef __cplusplus
}
#endif

#endif
