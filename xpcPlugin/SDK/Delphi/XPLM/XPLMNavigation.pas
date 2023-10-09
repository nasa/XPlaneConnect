{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMNavigation;
INTERFACE
{
   The XPLM Navigation APIs give you some access to the navigation databases
   inside X-Plane.  X-Plane stores all navigation information in RAM, so by
   using these APIs you can gain access to most information without having to
   go to disk or parse the files yourself.
   
   You can also use this API to program the FMS.  You must use the navigation
   APIs to find the nav-aids you want to program into the FMS, since the FMS
   is powered internally by X-Plane's navigation database.
}

USES
    XPLMDefs;
   {$A4}
{___________________________________________________________________________
 * NAVIGATION DATABASE ACCESS
 ___________________________________________________________________________}

   {
    XPLMNavType
    
    These enumerations define the different types of navaids.  They are each
    defined with a separate bit so that they may be bit-wise added together to
    form sets of nav-aid types.
    
    NOTE: xplm_Nav_LatLon is a specific lat-lon coordinate entered into the
    FMS. It will not exist in the database, and cannot be programmed into the
    FMS. Querying the FMS for navaids will return it.  Use
    XPLMSetFMSEntryLatLon to set a lat/lon waypoint.
   }
TYPE
   XPLMNavType = (
      xplm_Nav_Unknown                         = 0
 
     ,xplm_Nav_Airport                         = 1
 
     ,xplm_Nav_NDB                             = 2
 
     ,xplm_Nav_VOR                             = 4
 
     ,xplm_Nav_ILS                             = 8
 
     ,xplm_Nav_Localizer                       = 16
 
     ,xplm_Nav_GlideSlope                      = 32
 
     ,xplm_Nav_OuterMarker                     = 64
 
     ,xplm_Nav_MiddleMarker                    = 128
 
     ,xplm_Nav_InnerMarker                     = 256
 
     ,xplm_Nav_Fix                             = 512
 
     ,xplm_Nav_DME                             = 1024
 
     ,xplm_Nav_LatLon                          = 2048
 
   );
   PXPLMNavType = ^XPLMNavType;

   {
    XPLMNavRef
    
    XPLMNavRef is an iterator into the navigation database.  The navigation
    database is essentially an array, but it is not necessarily densely
    populated. The only assumption you can safely make is that like-typed
    nav-aids are grouped together.
    
    Use XPLMNavRef to refer to a nav-aid.
    
    XPLM_NAV_NOT_FOUND is returned by functions that return an XPLMNavRef when
    the iterator must be invalid.
   }
   XPLMNavRef = Integer;
   PXPLMNavRef = ^XPLMNavRef;

CONST
   XPLM_NAV_NOT_FOUND   = -1;

   {
    XPLMGetFirstNavAid
    
    This returns the very first navaid in the database.  Use this to traverse
    the entire database.  Returns XPLM_NAV_NOT_FOUND if the nav database is
    empty.
   }
   FUNCTION XPLMGetFirstNavAid: XPLMNavRef;
    cdecl; external XPLM_DLL;

   {
    XPLMGetNextNavAid
    
    Given a valid navaid ref, this routine returns the next navaid.  It returns
    XPLM_NAV_NOT_FOUND if the navaid passed in was invalid or if the navaid
    passed in was the last one in the database.  Use this routine to iterate
    across all like-typed navaids or the entire database.
   }
   FUNCTION XPLMGetNextNavAid(
                                        inNavAidRef         : XPLMNavRef) : XPLMNavRef;
    cdecl; external XPLM_DLL;

   {
    XPLMFindFirstNavAidOfType
    
    This routine returns the ref of the first navaid of the given type in the
    database or XPLM_NAV_NOT_FOUND if there are no navaids of that type in the
    database.  You must pass exactly one navaid type to this routine.
   }
   FUNCTION XPLMFindFirstNavAidOfType(
                                        inType              : XPLMNavType) : XPLMNavRef;
    cdecl; external XPLM_DLL;

   {
    XPLMFindLastNavAidOfType
    
    This routine returns the ref of the last navaid of the given type in the
    database or XPLM_NAV_NOT_FOUND if there are no navaids of that type in the
    database.  You must pass exactly one navaid type to this routine.
   }
   FUNCTION XPLMFindLastNavAidOfType(
                                        inType              : XPLMNavType) : XPLMNavRef;
    cdecl; external XPLM_DLL;

   {
    XPLMFindNavAid
    
    This routine provides a number of searching capabilities for the nav
    database. XPLMFindNavAid will search through every navaid whose type is
    within inType (multiple types may be added together) and return any navaids
    found based on the following rules:
    
    * If inLat and inLon are not NULL, the navaid nearest to that lat/lon will
      be returned, otherwise the last navaid found will be returned.
    
    * If inFrequency is not NULL, then any navaids considered must match this
      frequency.  Note that this will screen out radio beacons that do not have
      frequency data published (like inner markers) but not fixes and airports.
    
    * If inNameFragment is not NULL, only navaids that contain the fragment in
      their name will be returned.
    
    * If inIDFragment is not NULL, only navaids that contain the fragment in
      their IDs will be returned.
    
    This routine provides a simple way to do a number of useful searches:
    * Find the nearest navaid on this frequency.
    * Find the nearest airport.
    * Find the VOR whose ID is "KBOS".
    * Find the nearest airport whose name contains "Chicago".
   }
   FUNCTION XPLMFindNavAid(
                                        inNameFragment      : XPLMString;    { Can be nil }
                                        inIDFragment        : XPLMString;    { Can be nil }
                                        inLat               : PSingle;    { Can be nil }
                                        inLon               : PSingle;    { Can be nil }
                                        inFrequency         : PInteger;    { Can be nil }
                                        inType              : XPLMNavType) : XPLMNavRef;
    cdecl; external XPLM_DLL;

   {
    XPLMGetNavAidInfo
    
    This routine returns information about a navaid.  Any non-null field is
    filled out with information if it is available.
    
    Frequencies are in the nav.dat convention as described in the X-Plane nav
    database FAQ: NDB frequencies are exact, all others are multiplied by 100.
    
    The buffer for IDs should be at least 6 chars and the buffer for names
    should be at least 41 chars, but since these values are likely to go up, I
    recommend passing at least 32 chars for IDs and 256 chars for names when
    possible.
    
    The outReg parameter tells if the navaid is within the local "region" of
    loaded DSFs.  (This information may not be particularly useful to plugins.)
    The parameter is a single byte value 1 for true or 0 for false, not a C
    string.
   }
   PROCEDURE XPLMGetNavAidInfo(
                                        inRef               : XPLMNavRef;
                                        outType             : PXPLMNavType;    { Can be nil }
                                        outLatitude         : PSingle;    { Can be nil }
                                        outLongitude        : PSingle;    { Can be nil }
                                        outHeight           : PSingle;    { Can be nil }
                                        outFrequency        : PInteger;    { Can be nil }
                                        outHeading          : PSingle;    { Can be nil }
                                        outID               : XPLMString;    { Can be nil }
                                        outName             : XPLMString;    { Can be nil }
                                        outReg              : XPLMString);    { Can be nil }
    cdecl; external XPLM_DLL;

{___________________________________________________________________________
 * FLIGHT MANAGEMENT COMPUTER
 ___________________________________________________________________________}
{
   Note: the FMS works based on an array of entries.  Indices into the array
   are zero-based.  Each entry is a navaid plus an altitude.  The FMS tracks
   the currently displayed entry and the entry that it is flying to.
   
   The FMS must be programmed with contiguous entries, so clearing an entry at
   the end shortens the effective flight plan.  There is a max of 100
   waypoints in the flight plan.
}


   {
    XPLMCountFMSEntries
    
    This routine returns the number of entries in the FMS.
   }
   FUNCTION XPLMCountFMSEntries: Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMGetDisplayedFMSEntry
    
    This routine returns the index of the entry the pilot is viewing.
   }
   FUNCTION XPLMGetDisplayedFMSEntry: Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMGetDestinationFMSEntry
    
    This routine returns the index of the entry the FMS is flying to.
   }
   FUNCTION XPLMGetDestinationFMSEntry: Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMSetDisplayedFMSEntry
    
    This routine changes which entry the FMS is showing to the index specified.
   }
   PROCEDURE XPLMSetDisplayedFMSEntry(
                                        inIndex             : Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMSetDestinationFMSEntry
    
    This routine changes which entry the FMS is flying the aircraft toward.
   }
   PROCEDURE XPLMSetDestinationFMSEntry(
                                        inIndex             : Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMGetFMSEntryInfo
    
    This routine returns information about a given FMS entry. If the entry is
    an airport or navaid, a reference to a nav entry can be returned allowing
    you to find additional information (such as a frequency, ILS heading, name,
    etc.). Note that this reference can be XPLM_NAV_NOT_FOUND until the
    information has been looked up asynchronously, so after flightplan changes,
    it might take up to a second for this field to become populated. The other
    information is available immediately. For a lat/lon entry, the lat/lon is
    returned by this routine but the navaid cannot be looked up (and the
    reference will be XPLM_NAV_NOT_FOUND). FMS name entry buffers should be at
    least 256 chars in length.
    
    WARNING: Due to a bug in X-Plane prior to 11.31, the navaid reference will
    not be set to XPLM_NAV_NOT_FOUND while no data is available, and instead
    just remain the value of the variable that you passed the pointer to.
    Therefore, always initialize the variable to XPLM_NAV_NOT_FOUND before
    passing the pointer to this function.
   }
   PROCEDURE XPLMGetFMSEntryInfo(
                                        inIndex             : Integer;
                                        outType             : PXPLMNavType;    { Can be nil }
                                        outID               : XPLMString;    { Can be nil }
                                        outRef              : PXPLMNavRef;    { Can be nil }
                                        outAltitude         : PInteger;    { Can be nil }
                                        outLat              : PSingle;    { Can be nil }
                                        outLon              : PSingle);    { Can be nil }
    cdecl; external XPLM_DLL;

   {
    XPLMSetFMSEntryInfo
    
    This routine changes an entry in the FMS to have the destination navaid
    passed in and the altitude specified.  Use this only for airports, fixes,
    and radio-beacon navaids.  Currently of radio beacons, the FMS can only
    support VORs and NDBs. Use the routines below to clear or fly to a lat/lon.
   }
   PROCEDURE XPLMSetFMSEntryInfo(
                                        inIndex             : Integer;
                                        inRef               : XPLMNavRef;
                                        inAltitude          : Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMSetFMSEntryLatLon
    
    This routine changes the entry in the FMS to a lat/lon entry with the given
    coordinates.
   }
   PROCEDURE XPLMSetFMSEntryLatLon(
                                        inIndex             : Integer;
                                        inLat               : Single;
                                        inLon               : Single;
                                        inAltitude          : Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMClearFMSEntry
    
    This routine clears the given entry, potentially shortening the flight
    plan.
   }
   PROCEDURE XPLMClearFMSEntry(
                                        inIndex             : Integer);
    cdecl; external XPLM_DLL;

{___________________________________________________________________________
 * GPS RECEIVER
 ___________________________________________________________________________}
{
   These APIs let you read data from the GPS unit.
}

   {
    XPLMGetGPSDestinationType
    
    This routine returns the type of the currently selected GPS destination,
    one of fix, airport, VOR or NDB.
   }
   FUNCTION XPLMGetGPSDestinationType: XPLMNavType;
    cdecl; external XPLM_DLL;

   {
    XPLMGetGPSDestination
    
    This routine returns the current GPS destination.
   }
   FUNCTION XPLMGetGPSDestination: XPLMNavRef;
    cdecl; external XPLM_DLL;


IMPLEMENTATION

END.
