-------------------------------------------------------------------------------
                              THE X-PLANE PLUGIN SDK
-------------------------------------------------------------------------------

This download contains the files necessary to build plugins for X-Plane.  The 
X-Plane plugin website is:

http://www.xsquawkbox.net/xpsdk/

The website contains full documentation on the SDK including tech notes, sample
plugins, sample code, contact information, and links to the latest versions of
this SDK.

The X-Plane SDK authors can be reached at:

xplanesdk@xsquawkbox.net

Please do not email Austin or Laminar Research for SDK questions or support;
the SDK is a third party effort.

the X-Plane developer mailing list is an unlisted yahoo group frequented by 
many X-Plane developers.

x-plane-dev@yahoogroups.com

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

Mac CFM plugins are not supported by the SDK versions 2.0 and higher; the 
2.0 SDK requires X-Plane 9.0 or newer, and X-Plane 9 will not run on 
Mac OS 9.  Therefore CFM plugins are not useful (and are probably 
counterproductive since they cannot support x86 code).  If you have a CFM 
plugin, continue to use the 1.0 SDK to build it. You will have to port to 
Mach-O if you want to use 2.0 features.

-------------------------------------------------------------------------------
                              RELEASE NOTES
-------------------------------------------------------------------------------

This section contains per-release notes for the history of the X-Plane SDK.

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

