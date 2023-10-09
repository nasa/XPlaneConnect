{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMPlugin;
INTERFACE
{
   These APIs provide facilities to find and work with other plugins and
   manage other plugins.
}

USES
    XPLMDefs;
   {$A4}
{___________________________________________________________________________
 * FINDING PLUGINS
 ___________________________________________________________________________}
{
   These APIs allow you to find another plugin or yourself, or iterate across
   all plugins.  For example, if you wrote an FMS plugin that needed to talk
   to an autopilot plugin, you could use these APIs to locate the autopilot
   plugin.
}


   {
    XPLMGetMyID
    
    This routine returns the plugin ID of the calling plug-in.  Call this to
    get your own ID.
   }
   FUNCTION XPLMGetMyID: XPLMPluginID;
    cdecl; external XPLM_DLL;

   {
    XPLMCountPlugins
    
    This routine returns the total number of plug-ins that are loaded, both
    disabled and enabled.
   }
   FUNCTION XPLMCountPlugins: Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMGetNthPlugin
    
    This routine returns the ID of a plug-in by index.  Index is 0 based from 0
    to XPLMCountPlugins-1, inclusive. Plugins may be returned in any arbitrary
    order.
   }
   FUNCTION XPLMGetNthPlugin(
                                        inIndex             : Integer) : XPLMPluginID;
    cdecl; external XPLM_DLL;

   {
    XPLMFindPluginByPath
    
    This routine returns the plug-in ID of the plug-in whose file exists at the
    passed in absolute file system path.  XPLM_NO_PLUGIN_ID is returned if the
    path does not point to a currently loaded plug-in.
   }
   FUNCTION XPLMFindPluginByPath(
                                        inPath              : XPLMString) : XPLMPluginID;
    cdecl; external XPLM_DLL;

   {
    XPLMFindPluginBySignature
    
    This routine returns the plug-in ID of the plug-in whose signature matches
    what is passed in or XPLM_NO_PLUGIN_ID if no running plug-in has this
    signature.  Signatures are the best way to identify another plug-in as they
    are independent of the file system path of a plug-in or the human-readable
    plug-in name, and should be unique for all plug-ins.  Use this routine to
    locate another plugin that your plugin interoperates with
   }
   FUNCTION XPLMFindPluginBySignature(
                                        inSignature         : XPLMString) : XPLMPluginID;
    cdecl; external XPLM_DLL;

   {
    XPLMGetPluginInfo
    
    This routine returns information about a plug-in.  Each parameter should be
    a pointer to a buffer of at least
    256 characters, or NULL to not receive the information.
    
    outName - the human-readable name of the plug-in. outFilePath - the
    absolute file path to the file that contains this plug-in. outSignature - a
    unique string that identifies this plug-in. outDescription - a
    human-readable description of this plug-in.
   }
   PROCEDURE XPLMGetPluginInfo(
                                        inPlugin            : XPLMPluginID;
                                        outName             : XPLMString;    { Can be nil }
                                        outFilePath         : XPLMString;    { Can be nil }
                                        outSignature        : XPLMString;    { Can be nil }
                                        outDescription      : XPLMString);    { Can be nil }
    cdecl; external XPLM_DLL;

{___________________________________________________________________________
 * ENABLING/DISABLING PLUG-INS
 ___________________________________________________________________________}
{
   These routines are used to work with plug-ins and manage them.  Most
   plugins will not need to use these APIs.
}


   {
    XPLMIsPluginEnabled
    
    Returns whether the specified plug-in is enabled for running.
   }
   FUNCTION XPLMIsPluginEnabled(
                                        inPluginID          : XPLMPluginID) : Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMEnablePlugin
    
    This routine enables a plug-in if it is not already enabled. It returns 1
    if the plugin was enabled or successfully enables itself, 0 if it does not.
    Plugins may fail to enable (for example, if resources cannot be acquired)
    by returning 0 from their XPluginEnable callback.
   }
   FUNCTION XPLMEnablePlugin(
                                        inPluginID          : XPLMPluginID) : Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMDisablePlugin
    
    This routine disableds an enabled plug-in.
   }
   PROCEDURE XPLMDisablePlugin(
                                        inPluginID          : XPLMPluginID);
    cdecl; external XPLM_DLL;

   {
    XPLMReloadPlugins
    
    This routine reloads all plug-ins.  Once this routine is called and you
    return from the callback you were within (e.g. a menu select callback) you
    will receive your XPluginDisable and XPluginStop callbacks and your DLL
    will be unloaded, then the start process happens as if the sim was starting
    up.
   }
   PROCEDURE XPLMReloadPlugins;
    cdecl; external XPLM_DLL;

{___________________________________________________________________________
 * INTERPLUGIN MESSAGING
 ___________________________________________________________________________}
{
   Plugin messages are defined as 32-bit integers.  Messages below 0x00FFFFFF
   are reserved for X-Plane and the plugin SDK.
   
   Messages come with a pointer parameter; the meaning of this pointer depends
   on the message itself. In some messages, the pointer parameter  contains an
   actual typed pointer to data that can be inspected in the plugin; in these
   cases the documentation will state that the parameter "points to"
   information.
   
   in other cases, the value of the pointer is actually an integral number
   stuffed into the pointer's storage. In these second cases, the pointer
   parameter needs to be cast, not dereferenced. In these caess, the
   documentation will state that the parameter "contains" a value, which will
   always be an integral type.
   
   Some messages don't use the pointer parameter - in this case your plugin
   should ignore it.
   
   Messages have two conceptual uses: notifications and commands.  Commands
   are sent from one plugin to another to induce behavior; notifications are
   sent from one plugin to all others for informational purposes.  It is
   important that commands and notifications not have the same values because
   this could cause a notification sent by one plugin to accidentally induce a
   command in another.
   
   By convention, plugin-defined notifications should have the high bit set
   (e.g. be greater or equal to unsigned 0x8000000) while commands should have
   this bit be cleared.
   
   The following messages are sent to your plugin by X-Plane.
}


CONST
    { This message is sent to your plugin whenever the user's plane crashes. The }
    { parameter is ignored.                                                      }
   XPLM_MSG_PLANE_CRASHED = 101;

    { This message is sent to your plugin whenever a new plane is loaded.  The   }
    { parameter contains the index number of the plane being loaded; 0 indicates }
    { the user's plane.                                                          }
   XPLM_MSG_PLANE_LOADED = 102;

    { This messages is sent whenever the user's plane is positioned at a new     }
    { airport. The parameter is ignored.                                         }
   XPLM_MSG_AIRPORT_LOADED = 103;

    { This message is sent whenever new scenery is loaded.  Use datarefs to      }
    { determine the new scenery files that were loaded. The parameter is ignored.}
   XPLM_MSG_SCENERY_LOADED = 104;

    { This message is sent whenever the user adjusts the number of X-Plane       }
    { aircraft models.  You must use XPLMCountPlanes to find out how many planes }
    { are now available.  This message will only be sent in XP7 and higher       }
    { because in XP6 the number of aircraft is not user-adjustable. The parameter}
    { is ignored.                                                                }
   XPLM_MSG_AIRPLANE_COUNT_CHANGED = 105;

{$IFDEF XPLM200}
CONST
    { This message is sent to your plugin whenever a plane is unloaded.  The     }
    { parameter contains the index number of the plane being unloaded; 0         }
    { indicates the user's plane.  The parameter is of type int, passed as the   }
    { value of the pointer.  (That is: the parameter is an int, not a pointer to }
    { an int.)                                                                   }
   XPLM_MSG_PLANE_UNLOADED = 106;
{$ENDIF XPLM200}

{$IFDEF XPLM210}
CONST
    { This message is sent to your plugin right before X-Plane writes its        }
    { preferences file.  You can use this for two purposes: to write your own    }
    { preferences, and to modify any datarefs to influence preferences output.   }
    { For example, if your plugin temporarily modifies saved preferences, you can}
    { put them back to their default values here to avoid having the tweaks be   }
    { persisted if your plugin is not loaded on the next invocation of X-Plane.  }
    { The parameter is ignored.                                                  }
   XPLM_MSG_WILL_WRITE_PREFS = 107;
{$ENDIF XPLM210}

{$IFDEF XPLM210}
    { This message is sent to your plugin right after a livery is loaded for an  }
    { airplane.  You can use this to check the new livery (via datarefs) and     }
    { react accordingly.  The parameter contains the index number of the aircraft}
    { whose livery is changing.                                                  }
   XPLM_MSG_LIVERY_LOADED = 108;
{$ENDIF XPLM210}

{$IFDEF XPLM301}
CONST
    { Sent to your plugin right before X-Plane enters virtual reality mode (at   }
    { which time any windows that are not positioned in VR mode will no longer be}
    { visible to the user). The parameter is unused and should be ignored.       }
   XPLM_MSG_ENTERED_VR  = 109;
{$ENDIF XPLM301}

{$IFDEF XPLM301}
    { Sent to your plugin right before X-Plane leaves virtual reality mode (at   }
    { which time you may want to clean up windows that are positioned in VR      }
    { mode). The parameter is unused and should be ignored.                      }
   XPLM_MSG_EXITING_VR  = 110;
{$ENDIF XPLM301}

{$IFDEF XPLM303}
CONST
    { Sent to your plugin if another plugin wants to take over AI planes. If you }
    { are a synthetic traffic provider,  that probably means a plugin for an     }
    { online network has connected and wants to supply aircraft flown by real    }
    { humans and you should cease to provide synthetic traffic. If however you   }
    { are providing online traffic from real humans,  you probably don't want to }
    { disconnect, in which case you just ignore this message. The sender is the  }
    { plugin ID of the plugin asking for control of the planes now. You can use  }
    { it to find out who is requesting and whether you should yield to them.     }
    { Synthetic traffic providers should always yield to online networks. The    }
    { parameter is unused and should be ignored.                                 }
   XPLM_MSG_RELEASE_PLANES = 111;
{$ENDIF XPLM303}

{$IFDEF XPLM400}
CONST
    { Sent to your plugin after FMOD sound banks are loaded. The parameter is the}
    { XPLMBankID enum in XPLMSound.h, 0 for the master bank and 1 for the radio  }
    { bank.                                                                      }
   XPLM_MSG_FMOD_BANK_LOADED = 112;
{$ENDIF XPLM400}

{$IFDEF XPLM400}
    { Sent to your plugin before FMOD sound banks are unloaded. Any associated   }
    { resources should be cleaned up at this point. The parameter is the         }
    { XPLMBankID enum in XPLMSound.h, 0 for the master bank and 1 for the radio  }
    { bank.                                                                      }
   XPLM_MSG_FMOD_BANK_UNLOADING = 113;
{$ENDIF XPLM400}

{$IFDEF XPLM400}
    { Sent to your plugin per-frame (at-most) when/if datarefs are added. It will}
    { include the new data ref total count so that your plugin can keep a local  }
    { cache of the total, see what's changed and know which ones to inquire about}
    { if it cares.                                                               }
    {                                                                            }
    { This message is only sent to plugins that enable the                       }
    { XPLM_WANTS_DATAREF_NOTIFICATIONS feature.                                  }
   XPLM_MSG_DATAREFS_ADDED = 114;
{$ENDIF XPLM400}

   {
    XPLMSendMessageToPlugin
    
    This function sends a message to another plug-in or X-Plane.  Pass
    XPLM_NO_PLUGIN_ID to broadcast to all plug-ins.  Only enabled plug-ins with
    a message receive function receive the message.
   }
   PROCEDURE XPLMSendMessageToPlugin(
                                        inPlugin            : XPLMPluginID;
                                        inMessage           : Integer;
                                        inParam             : pointer);
    cdecl; external XPLM_DLL;

{$IFDEF XPLM200}
{___________________________________________________________________________
 * Plugin Features API
 ___________________________________________________________________________}
{
   The plugin features API allows your plugin to "sign up" for additional
   capabilities and plugin system features that are normally disabled for
   backward compatibility or performance.  This allows advanced plugins to
   "opt-in"  to new behavior.
   
   Each feature is defined by a permanent string name.  The feature string
   names will vary with the particular installation of X-Plane, so plugins
   should not expect a feature to be guaranteed present.
   
   XPLM_WANTS_REFLECTIONS
   ----------------------
   
   Available in the SDK 2.0 and later for X-Plane 9, enabling this capability
   causes your plugin to receive drawing hook callbacks when X-Plane builds
   its off-screen reflection and shadow rendering passes. Plugins should
   enable this and examine the dataref sim/graphics/view/plane_render_type to
   determine whether the drawing callback is for a reflection, shadow
   calculation, or the main screen. Rendering can be simlified or omitted for
   reflections, and non-solid drawing should be skipped for shadow
   calculations.
   
   **Note**: direct drawing via draw callbacks is not recommended; use the
     XPLMInstance API to create object models instead.
   
   XPLM_USE_NATIVE_PATHS
   ---------------------
   
   available in the SDK 2.1 and later for X-Plane 10, this modifies the plugin
   system to use Unix-style paths on all operating systems. With this enabled:
   
   * OS X paths will match the native OS X Unix.
   * Windows will use forward slashes but preserve C:\ or another drive letter
     when using complete file paths.
   * Linux uses its native file system path scheme.
   
   Without this enabled:
   
   * OS X will use CFM file paths separated by a colon.
   * Windows will use back-slashes and conventional DOS paths.
   * Linux uses its native file system path scheme.
   
   All plugins should enable this feature on OS X to access the native file
   system.
   
   XPLM_USE_NATIVE_WIDGET_WINDOWS
   ------------------------------
   
   Available in the SDK 3.0.2 SDK, this capability tells the widgets library
   to use new, modern X-Plane backed XPLMDisplay windows to anchor all widget
   trees.  Without it, widgets will always use legacy windows.
   
   Plugins should enable this to allow their widget hierarchies to respond to
   the user's UI size settings and to map widget-based windwos to a VR HMD.
   
   Before enabling this, make sure any custom widget code in your plugin is
   prepared to cope with the UI coordinate system not being th same as the
   OpenGL window coordinate system.
   
   XPLM_WANTS_DATAREF_NOTIFICATIONS
   --------------------------------
   
   Available in the SDK 4.0.0, this capability tells X-Plane to to send the
   enabling plugin the new XPLM_MSG_DATAREFS_ADDED message any time new
   datarefs are added. The SDK will coalesce consecutive dataref registrations
   to minimize the number of messages sent.
}


   {
    XPLMFeatureEnumerator_f
    
    You pass an XPLMFeatureEnumerator_f to get a list of all features supported
    by a given version running version of X-Plane.  This routine is called once
    for each feature.
   }
TYPE
     XPLMFeatureEnumerator_f = PROCEDURE(
                                    inFeature           : XPLMString;
                                    inRef               : pointer); cdecl;

   {
    XPLMHasFeature
    
    This returns 1 if the given installation of X-Plane supports a feature, or
    0 if it does not.
   }
   FUNCTION XPLMHasFeature(
                                        inFeature           : XPLMString) : Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMIsFeatureEnabled
    
    This returns 1 if a feature is currently enabled for your plugin, or 0 if
    it is not enabled.  It is an error to call this routine with an unsupported
    feature.
   }
   FUNCTION XPLMIsFeatureEnabled(
                                        inFeature           : XPLMString) : Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMEnableFeature
    
    This routine enables or disables a feature for your plugin.  This will
    change the running behavior of X-Plane and your plugin in some way,
    depending on the feature.
   }
   PROCEDURE XPLMEnableFeature(
                                        inFeature           : XPLMString;
                                        inEnable            : Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMEnumerateFeatures
    
    This routine calls your enumerator callback once for each feature that this
    running version of X-Plane supports. Use this routine to determine all of
    the features that X-Plane can support.
   }
   PROCEDURE XPLMEnumerateFeatures(
                                        inEnumerator        : XPLMFeatureEnumerator_f;
                                        inRef               : pointer);
    cdecl; external XPLM_DLL;

{$ENDIF XPLM200}

IMPLEMENTATION

END.
