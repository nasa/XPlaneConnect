-------------------------------------------------------------------------------
                              THE X-PLANE PLUGIN SDK
-------------------------------------------------------------------------------

This download contains the files necessary to build plugins for X-Plane.  The 
X-Plane plugin website is:

https://developer.x-plane.com/sdk/

The website contains full documentation on the SDK including tech notes, sample
plugins, sample code, contact information, and links to the latest versions of
this SDK.

-------------------------------------------------------------------------------
                                  SDK FILES
-------------------------------------------------------------------------------

license.txt	Copyright information for this download.
README.txt	This document
CHeaders	Header files for compiling C/C++ plugins
Delphi		Interfaces for compiling Pascal plugins
Libraries	Import libraries for linking on Windows
            and frameworks for linking on Mac.

Note: there are no import/link-time libraries for Linux; on Linux, plugins
simply leave SDK symbols undefined and they are discovered at runtime.  The 
SDK website explains this process in more detail.

-------------------------------------------------------------------------------
                              RELEASE NOTES
-------------------------------------------------------------------------------

This section contains per-release notes for the history of the X-Plane SDK.

X-Plane SDK Release 4.0.1 3/16/2023

The 4.0.1 SDK fixes the XPLMSound.h header, which contained invalid C code
(C++ struct definitons for FMOD_VECTOR and FMOD_ enums.)

X-Plane SDK Release 4.0.0 3/8/2023

The 4.0.0 SDK adds support for ARM64 Macs.  The 4.0 SDK is supported by X-Plane
12.04 (but can be used with X-Plane 12.00 if not using the weather, sound or 
dataref introspection APIs). New features:

 - M1/ARM64 support for Mac
 - Avionics display callbacks provide direct drawing to avionic "devices"
 - New XPLMSound API
 - New XPLMWeather API
 - New dataref APIs to get the set of all datarefs programmatically and be 
   notified of new datarefs ("introspection").  

X-Plane SDK Release 3.0.2 4/29/2020

The SDK 3.0.2 adds the modern 3-d drawing callback for interoperability with
Metal and Vulkan, and deprecates most older drawing callbacks.

X-Plane SDK Release 3.0.1 3/5/2018

The SDK 3.0.1 API adds new messages and APIs to support VR.

X-Plane SDK Release 3.0.0 11/2/7/2017

The SDK 3.0 API supports new features and new packaging for plugins. The 3.0
SDK requires X-Plane 11.0 or newer. New features include:

 - Display APIs to match X-Plane 11's UI.
 - New map APIs. Legacy 2-d map draw callbacks are deprecated.
 - Aircraft-plugins get their own menu
 - Aircraft placement by lat-lon-elevation.
 - Magnetic variation queries
 - Chinese language support
 - New instancing API

The 3.0 SDK supports a new plugin packaging schema:

    <plugin name>/<ABI>/<plugin name>.xpl

where ABI is one of mac_x64, win_x64 or lin_x64. The new schema is preferred,
so you can pack a version of your plugin that requires 3.0 with this scheme
and include a legacy 2.x plugin using hte old scheme for X-Plane 10 
compatibility.

Please use the new scheme where possible - having a unique file name for each
DLL makes crash reports easier to read and triage.

The 3.0 SDK drops support for 32-bit plugins; if you need to ship a 32-bit 
plugin for 32-bit X-Plane 10, shipping using two schemes and two binaries may
be the best option.

X-Plane SDK Release 2.1.3 11/14/13

Fixed XPC Wrappers to use int and intptr_t instead of long.  This fixes
crashes for plugins on 64-bit Windows.

X-Plane SDK Release 2.1.2 RC2 1/15/13

Removed headers from frameworks, as they don't work; updated README.

X-Plane SDK Release 2.1.2 RC1 1/12/13

The 2.1.2 SDK adds frameworks for the XPLM and XPWidgets; Mac developers
can link directly against these frameworks and avoid unresolved symbols 
and flat namespace problems.  The frameworks produce plugins that will
work on X-Plane 8, 9, and 10 depending on the plugin CPU architecture,
minimum system SDK, and XPLM API revision number.

X-Plane SDK Release 2.1.1 RC1 10/29/12

The 2.1.1 update to the SDK provides 64-bit build materials.

X-Plane SDK Release 2.1.0 RC1 3/31/12

This is the first release of the version 2.1 X-Plane SDK.  This version of the 
SDK exposes new APIs.

This API also replaces all references to "long" with int or intptr_t,
depending on whether the integer needs to be wide enough to hold coerced
pointers.  Most of the time, int is used; the notable exception is the widgets
library where params and properties can contain pointers to user data.

This change is not an ABI change - compiled plugins will work unmodified.
However for some compilers, you may need to replace long with int or intptr_t
in your code.

X-Plane SDK Release 2.0.1 RC1 7/21/10

This release adds symbol visibility macros for GCC 4 on Linux and corrects a few
function documentation comments.

X-Plane SDK Release 2.0 RC1 7/11/08

This release includes a corrected XPLM.lib for windows with exports for some of
the new 2.0 APIs.

X-Plane SDK Release 2.0 Beta 2 4/23/08

This release includes new APIs for reading and writing data files and drawing 
hooks for the local map screen, as well as some minor tweaks:

- Sim version is 2.0 in the headers.
- unload plane msg marked as 2.0 only.
- New enumerations for additional languages.
- Function level docs improved.

X-Plane SDK Release 2.0 Beta 1 1/19/08

This is the first release of the version 2.0 X-Plane SDK.  CFM support has
been removed, and the license has been simplified, reflecting that it only has
to cover the SDK include/import lib files and not the sample code or examples.

X-Plane SDK Release 1.0.2 1/5/05

The headers of the SDK are modified to support Kylix.  No changes for Mac, 
Windows, or C users.  Headers now have SDK version numbers.

X-Plane SDK Release 1.0.1 12/29/04

The headers of this SDK are modified to support Linux complication.  No changes
for Mac and Windows users.

X-Plane SDK Release Candidate 1

Only one slight change in the enums: the enum xpProperty_SubWindowHasCloseBoxes
in XPStandardWidgets.h has been changed to xpProperty_MainWindowHasCloseBoxes.  
Its value has not been changed, so you will need to search-and-replace your code
when using this version of the SDK, but already-compiled plugins will experience 
no different operation.

The documentation has been revised for all headers to revise changes made to the 
SDK over the course of beta.

X-Plane SDK Beta 5

This version of the SDK features a number of enumeration changes to reflect the
X-Plane interface more correctly.  This became crucial when X-Plane 7's new user 
interface was released.  With X-Plane in release candidates hopefully beta 5 of 
the SDK could be the last one.  Please see:

www.xsquawkbox.net/xpsdk/newui.html

For a comprehensive description of all the enumeration changes.  For most 
plugins (no developers reported using the deprecated enumerations), a simple 
search and replace should suffice.  Plugins compiled against the beta 4 SDK that
do not use now-unsupported graphics will continue to work correctly.

X-Plane SDK Beta 4

This release corrects two problems with the Pascal headers: function pointer 
types are now declared cdecl (since this is how the SDK calls them), and the 
import library for the widget callbacks is now XPWIDGETS.DLL as it should be.

X-Plane SDK Beta 3

This release finally features full documentation and a stable widgets API, as 
well as a few other minor bug fixes.

Starting with beta 3, the DLLs necessary to run plugins ship with X-Plane 660.  
The SDK will work with X-Plane 660 RC3 and later.  The XPWidgets DLL now lives 
in the Resources/plugins folder.

Starting with beta 3, extra plugins, documentation, sample code, and sample 
projects are now featured directly on the web in the new X-Plane SDK library.  
They are not included in the SDK zip file; the zip file only contains headers 
and lib files for the SDK.

X-Plane SDK Beta 2

You must recompile your plugin for the beta 2 plugin SDK!  Plugins compiled 
against the beta 1 SDK will not work with X-Plane 660 or the new XPLM.DLL.

A huge number of data refs have been added.  Unfortunately the documentation 
is thin.  Use the data ref tester plugin to view the data refs in real time 
and find what you need.

The data ref APIs have also changed to allow for arrays of integers as well 
as floats.  Some sim variables are now arrays that were previously many 
individual items.

A new drawing phase is available for replacing aircraft graphics.  The 
texturing APIs in XPLMGraphics have been revised.  The most notable change is 
that you cannot use the SDK to load your textures.  (This functionality was 
broken and never worked in beta 1.)  See the x-plane-dev list for sample code 
on how to load your own bitmaps.

X-Plane can reload plugins on the fly.  Use the Plugin Enabler plugin to reload 
your plugin.  On the Mac you can throw the old DLL in the trash and put a new 
one in its place to reload a new version of the plugin.  On the PC, an alert 
comes up; while this alert is up you can swap your plugins' DLL.  This allows 
you to recompile your plugin without rebooting the sim.

Delphi Pascal interfaces and sample code are in the SDK.  Thanks to Billy 
Verreynne for his hard work on this.

