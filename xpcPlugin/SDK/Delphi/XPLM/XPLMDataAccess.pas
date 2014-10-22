{
   Copyright 2005-2012 Sandy Barbour and Ben Supnik
   
   All rights reserved.  See license.txt for usage.
   
   X-Plane SDK Version: 2.1.1                                                  
}

UNIT XPLMDataAccess;
INTERFACE
{
   XPLM Data Access API - Theory of Operation 
   
   The data access API gives you a generic, flexible, high performance way to 
   read and write data to and from X-Plane and other plug-ins.  For example, 
   this API allows you to read and set the nav radios, get the plane location, 
   determine the current effective graphics frame rate, etc. 
   
   The data access APIs are the way that you read and write data from the sim 
   as well as other plugins. 
   
   The API works using opaque data references.  A data reference is a source 
   of data; you do not know where it comes from, but once you have it you can 
   read the data quickly and possibly write it.  To get a data reference, you 
   look it  up. 
   
   Data references are identified by verbose string names 
   (sim/cockpit/radios/nav1_freq_hz). The actual numeric value of the data 
   reference is implementation defined and is likely to change each time the 
   simulator is run (or the plugin that provides the datareference is 
   reloaded). 
   
   The task of looking up a data reference is relatively expensive; look up 
   your  data references once based on verbose strings, and save the opaque 
   data reference value for the duration of your plugin's operation.  Reading 
   and writing data references is relatively fast (the cost is equivalent to 
   two function calls through function pointers). 
   
   This allows data access to be high performance, while leaving in 
   abstraction; since data references are opaque and are searched for, the 
   underlying data access system can be rebuilt. 
   
   A note on typing: you must know the correct data type to read and write.  
   APIs are provided for reading and writing data in a number of ways.  You 
   can also double check the data type for a data ref.  Note that automatic 
   conversion is not done for you. 
   
   A note for plugins sharing data with other plugins: the load order of 
   plugins is not guaranteed.  To make sure that every plugin publishing data 
   has published their data references before other plugins try to subscribe, 
   publish your data references in your start routine but resolve them the 
   first time your 'enable' routine is called, or the first time they are 
   needed in code. 
   
   X-Plane publishes well over 1000 datarefs; a complete list may be found in 
   the  reference section of the SDK online documentation (from the SDK home 
   page, choose Documentation).		                                              
}

USES   XPLMDefs;
   {$A4}
{$IFDEF MSWINDOWS}
   {$DEFINE DELPHI}
{$ENDIF}
{___________________________________________________________________________
 * READING AND WRITING DATA
 ___________________________________________________________________________}
{
   These routines allow you to access a wide variety of data from within 
   x-plane and modify some of it.                                              
}



TYPE
   {
    XPLMDataRef
    
    A data ref is an opaque handle to data provided by the simulator or another 
    plugin.  It uniquely identifies one variable (or array of variables) over 
    the lifetime of your plugin.  You never hard code these values; you always 
    get them from XPLMFindDataRef.                                              
   }
   XPLMDataRef = pointer;
   PXPLMDataRef = ^XPLMDataRef;

   {
    XPLMDataTypeID
    
    This is an enumeration that defines the type of the data behind a data 
    reference. This allows you to sanity check that the data type matches what 
    you expect. But for the most part, you will know the type of data you are 
    expecting from the online documentation. 
    
    Data types each take a bit field, so sets of data types may be formed.      
   }
   XPLMDataTypeID = (
     { Data of a type the current XPLM doesn't do.                                 }
      xplmType_Unknown                         = 0
 
     { A single 4-byte integer, native endian.                                     }
     ,xplmType_Int                             = 1
 
     { A single 4-byte float, native endian.                                       }
     ,xplmType_Float                           = 2
 
     { A single 8-byte double, native endian.                                      }
     ,xplmType_Double                          = 4
 
     { An array of 4-byte floats, native endian.                                   }
     ,xplmType_FloatArray                      = 8
 
     { An array of 4-byte integers, native endian.                                 }
     ,xplmType_IntArray                        = 16
 
     { A variable block of data.                                                   }
     ,xplmType_Data                            = 32
 
   );
   PXPLMDataTypeID = ^XPLMDataTypeID;

   {
    XPLMFindDataRef
    
    Given a c-style string that names the data ref, this routine looks up the 
    actual opaque XPLMDataRef that you use to read and write the data. The 
    string names for datarefs are published on the x-plane SDK web site.				 
    
    This function returns NULL if the data ref cannot be found. 
    
    NOTE: this function is relatively expensive; save the XPLMDataRef this 
    function returns for future use.  Do not look up your data ref by string  
    every time you need to read or write it.                                    
   }
   FUNCTION XPLMFindDataRef(
                                        inDataRefName       : Pchar) : XPLMDataRef;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMCanWriteDataRef
    
    Given a data ref, this routine returns true if you can successfully set  
    the data, false otherwise.  Some datarefs are read-only.                    
   }
   FUNCTION XPLMCanWriteDataRef(
                                        inDataRef           : XPLMDataRef) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMIsDataRefGood
    
    WARNING: This function is deprecated and should not be used. Datarefs are 
    valid until plugins are reloaded or the sim quits.  Plugins sharing 
    datarefs should support these semantics by not unregistering datarefs 
    during operation.  (You should however unregister datarefs when your plugin 
    is unloaded, as part of general resource cleanup.) 
    
    This function returns whether a data ref is still valid.  If it returns 
    false, you should refind the data ref from its original string.  Calling an 
    accessor function on a bad data ref will return a default value, typically 
    0 or 0-length data.                                                         
   }
   FUNCTION XPLMIsDataRefGood(
                                        inDataRef           : XPLMDataRef) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetDataRefTypes
    
    This routine returns the types of the data ref for accessor use.  If a data 
    ref is available in multiple data types, they will all be returned.         
   }
   FUNCTION XPLMGetDataRefTypes(
                                        inDataRef           : XPLMDataRef) : XPLMDataTypeID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * DATA ACCESSORS
 ___________________________________________________________________________}
{
   These routines read and write the data references.  For each supported data 
   type there is a  reader and a writer. 
   
   If the data ref is invalid or the plugin that provides it is disabled or 
   there is a type mismatch, the functions that read data will return 0 as a 
   default value or not modify the passed in memory. The plugins that write 
   data will not write under these circumstances or if the data ref is 
   read-only. NOTE: to keep the overhead of reading datarefs low, these 
   routines do not do full validation of a  dataref; passing a junk value for 
   a dataref can result in crashing the sim. 
   
   For array-style datarefs, you specify the number of items to read/write and 
   the offset into the array; the actual number of items read or written is 
   returned.  This may be less to prevent an array-out-of-bounds error.        
}



   {
    XPLMGetDatai
    
    Read an integer data ref and return its value. The return value is the 
    dataref value or 0 if the dataref is invalid/NULL or the plugin is 
    disabled.                                                                   
   }
   FUNCTION XPLMGetDatai(
                                        inDataRef           : XPLMDataRef) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetDatai
    
    Write a new value to an integer data ref.   This routine is a no-op if the 
    plugin publishing the dataref is disabled, the dataref is invalid, or the 
    dataref is not writable.                                                    
   }
   PROCEDURE XPLMSetDatai(
                                        inDataRef           : XPLMDataRef;    
                                        inValue             : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetDataf
    
    Read a single precision floating point dataref and return its value. The 
    return value is the dataref value or 0.0 if the dataref is invalid/NULL or 
    the plugin is disabled.                                                     
   }
   FUNCTION XPLMGetDataf(
                                        inDataRef           : XPLMDataRef) : single;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetDataf
    
    Write a new value to a single precision floating point data ref.   This 
    routine is a no-op if the plugin publishing the dataref is disabled, the 
    dataref is invalid, or the dataref is not writable.                         
   }
   PROCEDURE XPLMSetDataf(
                                        inDataRef           : XPLMDataRef;    
                                        inValue             : single);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetDatad
    
    Read a double precision floating point dataref and return its value. The 
    return value is the dataref value or 0.0 if the dataref is invalid/NULL or 
    the plugin is disabled.                                                     
   }
   FUNCTION XPLMGetDatad(
                                        inDataRef           : XPLMDataRef) : real;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetDatad
    
    Write a new value to a double precision floating point data ref.   This 
    routine is a no-op if the plugin publishing the dataref is disabled, the 
    dataref is invalid, or the dataref is not writable.                         
   }
   PROCEDURE XPLMSetDatad(
                                        inDataRef           : XPLMDataRef;    
                                        inValue             : real);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetDatavi
    
    Read a part of an integer array dataref.  If you pass NULL for outVaules, 
    the routine will return the size of the array, ignoring inOffset and inMax. 
    
    
    If outValues is not NULL, then up to inMax values are copied from the 
    dataref into outValues, starting at inOffset in the dataref. If inMax + 
    inOffset is larger than the size of the dataref, less than inMax values 
    will be copied.  The number of values copied is returned. 
    
    Note: the semantics of array datarefs are entirely implemented by the 
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the 
    above description is how these datarefs are intended to work, but a rogue 
    plugin may have different behavior.                                         
   }
   FUNCTION XPLMGetDatavi(
                                        inDataRef           : XPLMDataRef;    
                                        outValues           : Pinteger;    { Can be nil }
                                        inOffset            : integer;    
                                        inMax               : integer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetDatavi
    
    Write part or all of an integer array dataref.  The values passed by 
    inValues are written into the dataref starting at  inOffset.  Up to inCount 
    values are written; however if the values would write "off the end" of the 
    dataref array, then fewer values are written. 
    
    Note: the semantics of array datarefs are entirely implemented by the 
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the 
    above description is how these datarefs are intended to work, but a rogue 
    plugin may have different behavior.				                                     
   }
   PROCEDURE XPLMSetDatavi(
                                        inDataRef           : XPLMDataRef;    
                                        inValues            : Pinteger;    
                                        inoffset            : integer;    
                                        inCount             : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetDatavf
    
    Read a part of a single precision floating point array dataref.  If you 
    pass NULL for outVaules, the routine will return the size of the array, 
    ignoring inOffset and inMax. 
    
    If outValues is not NULL, then up to inMax values are copied from the 
    dataref into outValues, starting at inOffset in the dataref. If inMax + 
    inOffset is larger than the size of the dataref, less than inMax values 
    will be copied.  The number of values copied is returned. 
    
    Note: the semantics of array datarefs are entirely implemented by the 
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the 
    above description is how these datarefs are intended to work, but a rogue 
    plugin may have different behavior.                                         
   }
   FUNCTION XPLMGetDatavf(
                                        inDataRef           : XPLMDataRef;    
                                        outValues           : Psingle;    { Can be nil }
                                        inOffset            : integer;    
                                        inMax               : integer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetDatavf
    
    Write part or all of a single precision floating point array dataref.  The 
    values passed by inValues are written into the dataref starting at  
    inOffset.  Up to inCount values are written; however if the values would 
    write "off the end" of the dataref array, then fewer values are written. 
    
    Note: the semantics of array datarefs are entirely implemented by the 
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the 
    above description is how these datarefs are intended to work, but a rogue 
    plugin may have different behavior.				                                     
   }
   PROCEDURE XPLMSetDatavf(
                                        inDataRef           : XPLMDataRef;    
                                        inValues            : Psingle;    
                                        inoffset            : integer;    
                                        inCount             : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetDatab
    
    Read a part of a byte array dataref.  If you pass NULL for outVaules, the 
    routine will return the size of the array, ignoring inOffset and inMax. 
    
    If outValues is not NULL, then up to inMax values are copied from the 
    dataref into outValues, starting at inOffset in the dataref. If inMax + 
    inOffset is larger than the size of the dataref, less than inMax values 
    will be copied.  The number of values copied is returned. 
    
    Note: the semantics of array datarefs are entirely implemented by the 
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the 
    above description is how these datarefs are intended to work, but a rogue 
    plugin may have different behavior.                                         
   }
   FUNCTION XPLMGetDatab(
                                        inDataRef           : XPLMDataRef;    
                                        outValue            : pointer;    { Can be nil }
                                        inOffset            : integer;    
                                        inMaxBytes          : integer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetDatab
    
    Write part or all of a byte array dataref.  The values passed by inValues 
    are written into the dataref starting at  inOffset.  Up to inCount values 
    are written; however if the values would write "off the end" of the dataref 
    array, then fewer values are written. 
    
    Note: the semantics of array datarefs are entirely implemented by the 
    plugin (or X-Plane) that provides the dataref, not the SDK itself; the 
    above description is how these datarefs are intended to work, but a rogue 
    plugin may have different behavior.				                                     
   }
   PROCEDURE XPLMSetDatab(
                                        inDataRef           : XPLMDataRef;    
                                        inValue             : pointer;    
                                        inOffset            : integer;    
                                        inLength            : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * PUBLISHING YOUR PLUGINS DATA
 ___________________________________________________________________________}
{
   These functions allow you to create data references that other plug-ins can 
   access via the above data access APIs.  Data references published by other 
   plugins operate the same as ones published by x-plane in all manners except 
   that your data reference will not be available to other plugins if/when 
   your plugin is disabled. 
   
   You share data by registering data provider callback functions.  When a 
   plug-in requests your data, these callbacks are then called.  You provide 
   one callback to return the value when a plugin 'reads' it and another to 
   change the value when a plugin 'writes' it. 
   
   Important: you must pick a prefix for your datarefs other than "sim/" - 
   this prefix is reserved for X-Plane.  The X-Plane SDK website contains a 
   registry where authors can select a unique first word for dataref names, to 
   prevent dataref collisions between plugins.                                 
}



   {
    XPLMGetDatai_f
    
    Data provider function pointers. 
    
    These define the function pointers you provide to get or set data.  Note 
    that you are passed a generic pointer for each one.  This is the same 
    pointer you pass in your register routine; you can use it to find global 
    variables, etc. 
    
    The semantics of your callbacks are the same as the dataref accessor above 
    - basically routines like XPLMGetDatai are just pass-throughs from a caller 
    to your plugin.  Be particularly mindful in implementing array dataref 
    read-write accessors; you are responsible for avoiding overruns, supporting 
    offset read/writes, and handling a read with a NULL buffer.			              
   }
TYPE
     XPLMGetDatai_f = FUNCTION(
                                    inRefcon            : pointer) : integer; cdecl;   

   {
    XPLMSetDatai_f
    
   }
     XPLMSetDatai_f = PROCEDURE(
                                    inRefcon            : pointer;    
                                    inValue             : integer); cdecl;   

   {
    XPLMGetDataf_f
    
   }
     XPLMGetDataf_f = FUNCTION(
                                    inRefcon            : pointer) : single; cdecl;   

   {
    XPLMSetDataf_f
    
   }
     XPLMSetDataf_f = PROCEDURE(
                                    inRefcon            : pointer;    
                                    inValue             : single); cdecl;   

   {
    XPLMGetDatad_f
    
   }
     XPLMGetDatad_f = FUNCTION(
                                    inRefcon            : pointer) : real; cdecl;   

   {
    XPLMSetDatad_f
    
   }
     XPLMSetDatad_f = PROCEDURE(
                                    inRefcon            : pointer;    
                                    inValue             : real); cdecl;   

   {
    XPLMGetDatavi_f
    
   }
     XPLMGetDatavi_f = FUNCTION(
                                    inRefcon            : pointer;    
                                    outValues           : Pinteger;    { Can be nil }
                                    inOffset            : integer;    
                                    inMax               : integer) : integer; cdecl;   

   {
    XPLMSetDatavi_f
    
   }
     XPLMSetDatavi_f = PROCEDURE(
                                    inRefcon            : pointer;    
                                    inValues            : Pinteger;    
                                    inOffset            : integer;    
                                    inCount             : integer); cdecl;   

   {
    XPLMGetDatavf_f
    
   }
     XPLMGetDatavf_f = FUNCTION(
                                    inRefcon            : pointer;    
                                    outValues           : Psingle;    { Can be nil }
                                    inOffset            : integer;    
                                    inMax               : integer) : integer; cdecl;   

   {
    XPLMSetDatavf_f
    
   }
     XPLMSetDatavf_f = PROCEDURE(
                                    inRefcon            : pointer;    
                                    inValues            : Psingle;    
                                    inOffset            : integer;    
                                    inCount             : integer); cdecl;   

   {
    XPLMGetDatab_f
    
   }
     XPLMGetDatab_f = FUNCTION(
                                    inRefcon            : pointer;    
                                    outValue            : pointer;    { Can be nil }
                                    inOffset            : integer;    
                                    inMaxLength         : integer) : integer; cdecl;   

   {
    XPLMSetDatab_f
    
   }
     XPLMSetDatab_f = PROCEDURE(
                                    inRefcon            : pointer;    
                                    inValue             : pointer;    
                                    inOffset            : integer;    
                                    inLength            : integer); cdecl;   

   {
    XPLMRegisterDataAccessor
    
    This routine creates a new item of data that can be read and written.  Pass 
    in the data's full name for searching, the type(s) of the data for 
    accessing, and whether the data can be written to.  For each data type you 
    support, pass in a read accessor function and a write accessor function if 
    necessary.  Pass NULL for data types you do not support or write accessors 
    if you are read-only. 
    
    You are returned a data ref for the new item of data created.  You can use 
    this  data ref to unregister your data later or read or write from it.      
   }
   FUNCTION XPLMRegisterDataAccessor(
                                        inDataName          : Pchar;    
                                        inDataType          : XPLMDataTypeID;    
                                        inIsWritable        : integer;    
                                        inReadInt           : XPLMGetDatai_f;    
                                        inWriteInt          : XPLMSetDatai_f;    
                                        inReadFloat         : XPLMGetDataf_f;    
                                        inWriteFloat        : XPLMSetDataf_f;    
                                        inReadDouble        : XPLMGetDatad_f;    
                                        inWriteDouble       : XPLMSetDatad_f;    
                                        inReadIntArray      : XPLMGetDatavi_f;    
                                        inWriteIntArray     : XPLMSetDatavi_f;    
                                        inReadFloatArray    : XPLMGetDatavf_f;    
                                        inWriteFloatArray   : XPLMSetDatavf_f;    
                                        inReadData          : XPLMGetDatab_f;    
                                        inWriteData         : XPLMSetDatab_f;    
                                        inReadRefcon        : pointer;    
                                        inWriteRefcon       : pointer) : XPLMDataRef;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMUnregisterDataAccessor
    
    Use this routine to unregister any data accessors you may have registered. 
    You unregister a data ref by the XPLMDataRef you get back from 
    registration. Once you unregister a data ref, your function pointer will 
    not be called anymore. 
    
    For maximum compatibility, do not unregister your data accessors until 
    final shutdown (when your XPluginStop routine is called).  This allows 
    other plugins to find your data reference once and use it for their entire 
    time of operation.                                                          
   }
   PROCEDURE XPLMUnregisterDataAccessor(
                                        inDataRef           : XPLMDataRef);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{___________________________________________________________________________
 * SHARING DATA BETWEEN MULTIPLE PLUGINS
 ___________________________________________________________________________}
{
   The data reference registration APIs from the previous section allow a 
   plugin to publish data in a one-owner manner; the plugin that publishes the 
   data reference owns the real memory that the data ref uses.  This is 
   satisfactory for most cases, but there are also cases where plugnis need to 
   share actual data.   
   
   With a shared data reference, no one plugin owns the actual memory for the 
   data reference; the plugin SDK allocates that for you.  When the first 
   plugin asks to 'share' the data, the memory is allocated.  When the data is 
   changed, every plugin that is sharing the data is notified. 
   
   Shared data references differ from the 'owned' data references from the 
   previous section in a few ways: 
   
   - With shared data references, any plugin can create the data reference; 
   with owned plugins one plugin must create the data reference and others 
   subscribe.  (This can be a problem if you don't know which set of plugins 
   will be present). 
   
   - With shared data references, every plugin that is sharing the data is 
   notified when the data is changed.  With owned data references, only the 
   one owner is notified when the data is changed. 
   
   - With shared data references, you cannot access the physical memory of the 
   data reference; you must use the XPLMGet... and XPLMSet... APIs.  With an 
   owned data reference, the one  owning data reference can manipulate the 
   data reference's memory in any way it sees fit. 
   
   Shared data references solve two problems: if you need to have a data 
   reference used by  several plugins but do not know which plugins will be 
   installed, or if all plugins sharing data need to be notified when that 
   data is changed, use shared data references.                                
}



   {
    XPLMDataChanged_f
    
    An XPLMDataChanged_f is a callback that the XPLM calls whenever any other 
    plug-in modifies shared data.  A refcon you provide is passed back to help 
    identify which data is being changed. In response, you may want to call one 
    of the XPLMGetDataxxx routines to find the new value of the data.           
   }
TYPE
     XPLMDataChanged_f = PROCEDURE(
                                    inRefcon            : pointer); cdecl;   

   {
    XPLMShareData
    
    This routine connects a plug-in to shared data, creating the shared data if 
    necessary. inDataName is a standard path for the data ref, and inDataType 
    specifies the type. This function will create the data if it does not 
    exist.  If the data already exists but the type does not match, an error is 
    returned, so it is important that plug-in authors  collaborate to establish 
    public standards for shared data. 
    
    If a notificationFunc is passed in and is not NULL, that notification 
    function will be  called whenever the data is modified.  The notification 
    refcon will be passed to it. This allows your plug-in to know which shared 
    data was changed if multiple shared data are handled by one callback, or if 
    the plug-in does not use global variables. 
    
    A one is returned for successfully creating or finding the shared data; a 
    zero if  the data already exists but is of the wrong type.                  
   }
   FUNCTION XPLMShareData(
                                        inDataName          : Pchar;    
                                        inDataType          : XPLMDataTypeID;    
                                        inNotificationFunc  : XPLMDataChanged_f;    
                                        inNotificationRefcon: pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMUnshareData
    
    This routine removes your notification function for shared data.  Call it 
    when done with  the data to stop receiving change notifications.  Arguments 
    must match XPLMShareData. The actual memory will not necessarily be freed, 
    since other plug-ins could be using it.                                     
   }
   FUNCTION XPLMUnshareData(
                                        inDataName          : Pchar;    
                                        inDataType          : XPLMDataTypeID;    
                                        inNotificationFunc  : XPLMDataChanged_f;    
                                        inNotificationRefcon: pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

IMPLEMENTATION
END.
