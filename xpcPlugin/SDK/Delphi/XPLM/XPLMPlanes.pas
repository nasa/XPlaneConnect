{
   Copyright 2005-2012 Sandy Barbour and Ben Supnik
   
   All rights reserved.  See license.txt for usage.
   
   X-Plane SDK Version: 2.1.1                                                  
}

UNIT XPLMPlanes;
INTERFACE
{
   The XPLMPlanes APIs allow you to control the various aircraft in x-plane, 
   both the user's and the sim's.                                              
}

USES   XPLMDefs;
   {$A4}
{$IFDEF MSWINDOWS}
   {$DEFINE DELPHI}
{$ENDIF}
{___________________________________________________________________________
 * USER AIRCRAFT ACCESS
 ___________________________________________________________________________}
{
                                                                               
}


   {
    XPLMSetUsersAircraft
    
    This routine changes the user's aircraft.  Note that this will reinitialize 
    the user to be on the nearest airport's first runway.  Pass in a full path 
    (hard drive and everything including the .acf extension) to the .acf file.  
   }
   PROCEDURE XPLMSetUsersAircraft(
                                        inAircraftPath      : Pchar);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
   {
    XPLMPlaceUserAtAirport
    
    This routine places the user at a given airport.  Specify the airport by 
    its ICAO code (e.g. 'KBOS').                                                
   }
   PROCEDURE XPLMPlaceUserAtAirport(
                                        inAirportCode       : Pchar);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
{___________________________________________________________________________
 * GLOBAL AIRCRAFT ACCESS
 ___________________________________________________________________________}
{
                                                                               
}


CONST
    { The user's aircraft is always index 0.                                      }
   XPLM_USER_AIRCRAFT   = 0;
   {
    XPLMPlaneDrawState_t
    
    This structure contains additional plane parameter info to be passed to 
    draw plane.  Make sure to fill in the size of the structure field with 
    sizeof(XPLMDrawPlaneState_t) so that the XPLM can tell how many fields you 
    knew about when compiling your plugin (since more fields may be added 
    later). 
    
    Most of these fields are ratios from 0 to 1 for control input.  X-Plane 
    calculates what the actual controls look like based on the .acf file for 
    that airplane.  Note for the yoke inputs, this is what the pilot of the 
    plane has commanded (post artificial stability system if there were one) 
    and affects aelerons, rudder, etc.  It is not  necessarily related to the 
    actual position of the plane!                                               
   }
TYPE
   XPLMPlaneDrawState_t = RECORD
     { The size of the draw state struct.                                          }
     structSize               : integer;
     { A ratio from [0..1] describing how far the landing gear is extended.        }
     gearPosition             : single;
     { Ratio of flap deployment, 0 = up, 1 = full deploy.                          }
     flapRatio                : single;
     { Ratio of spoiler deployment, 0 = none, 1 = full deploy.                     }
     spoilerRatio             : single;
     { Ratio of speed brake deployment, 0 = none, 1 = full deploy.                 }
     speedBrakeRatio          : single;
     { Ratio of slat deployment, 0 = none, 1 = full deploy.                        }
     slatRatio                : single;
     { Wing sweep ratio, 0 = forward, 1 = swept.                                   }
     wingSweep                : single;
     { Thrust power, 0 = none, 1 = full fwd, -1 = full reverse.                    }
     thrust                   : single;
     { Total pitch input for this plane.                                           }
     yokePitch                : single;
     { Total Heading input for this plane.                                         }
     yokeHeading              : single;
     { Total Roll input for this plane.                                            }
     yokeRoll                 : single;
   END;
   PXPLMPlaneDrawState_t = ^XPLMPlaneDrawState_t;
   {
    XPLMCountAircraft
    
    This function returns the number of aircraft X-Plane is capable of having, 
    as well as the number of aircraft that are currently active.  These numbers 
    count the user's aircraft.  It can also return the plugin that is currently 
    controlling aircraft.  In X-Plane 7, this routine reflects the number of 
    aircraft the user has enabled in the rendering options window.              
   }
   PROCEDURE XPLMCountAircraft(
                                        outTotalAircraft    : Pinteger;    
                                        outActiveAircraft   : Pinteger;    
                                        outController       : PXPLMPluginID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
   {
    XPLMGetNthAircraftModel
    
    This function returns the aircraft model for the Nth aircraft.  Indices are 
    zero based, with zero being the user's aircraft.  The file name should be 
    at least 256 chars in length; the path should be at least 512 chars in 
    length.                                                                     
   }
   PROCEDURE XPLMGetNthAircraftModel(
                                        inIndex             : integer;    
                                        outFileName         : Pchar;    
                                        outPath             : Pchar);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
{___________________________________________________________________________
 * EXCLUSIVE AIRCRAFT ACCESS
 ___________________________________________________________________________}
{
   The following routines require exclusive access to the airplane APIs. Only 
   one plugin may have this access at a time.                                  
}



   {
    XPLMPlanesAvailable_f
    
    Your airplanes available callback is called when another plugin gives up 
    access to the multiplayer planes.  Use this to wait for access to 
    multiplayer.                                                                
   }
TYPE
     XPLMPlanesAvailable_f = PROCEDURE(
                                    inRefcon            : pointer); cdecl;   

   {
    XPLMAcquirePlanes
    
    XPLMAcquirePlanes grants your plugin exclusive access to the aircraft.  It 
    returns 1 if you gain access, 0 if you do not. inAircraft - pass in an 
    array of pointers to strings specifying the planes you want loaded.  For 
    any plane index you do not want loaded, pass a 0-length string.  Other 
    strings should be full paths with the .acf extension.  NULL terminates this 
    array, or pass NULL if there are no planes you want loaded. If you pass in 
    a callback and do not receive access to the planes your callback will be 
    called when the airplanes are available. If you do receive airplane access, 
    your callback will not be called.                                           
   }
   FUNCTION XPLMAcquirePlanes(
                                        inAircraft          : PPchar;    { Can be nil }
                                        inCallback          : XPLMPlanesAvailable_f;    
                                        inRefcon            : pointer) : integer;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMReleasePlanes
    
    Call this function to release access to the planes.  Note that if you are 
    disabled, access to planes is released for you and you must reacquire it.   
   }
   PROCEDURE XPLMReleasePlanes;
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetActiveAircraftCount
    
    This routine sets the number of active planes.  If you pass in a number 
    higher than the total number of planes availables, only the total number of 
    planes available is actually used.                                          
   }
   PROCEDURE XPLMSetActiveAircraftCount(
                                        inCount             : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetAircraftModel
    
    This routine loads an aircraft model.  It may only be called if you  have 
    exclusive access to the airplane APIs.  Pass in the path of the  model with 
    the .acf extension.  The index is zero based, but you  may not pass in 0 
    (use XPLMSetUsersAircraft to load the user's aircracft).                    
   }
   PROCEDURE XPLMSetAircraftModel(
                                        inIndex             : integer;    
                                        inAircraftPath      : Pchar);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMDisableAIForPlane
    
    This routine turns off X-Plane's AI for a given plane.  The plane will 
    continue to draw and be a real plane in X-Plane, but will not  move itself. 
   }
   PROCEDURE XPLMDisableAIForPlane(
                                        inPlaneIndex        : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMDrawAircraft
    
    This routine draws an aircraft.  It can only be called from a 3-d drawing 
    callback.  Pass in the position of the plane in OpenGL local coordinates 
    and the orientation of the plane.  A 1 for full drawing indicates that the 
    whole plane must be drawn; a 0 indicates you only need the nav lights 
    drawn. (This saves rendering time when planes are far away.)                
   }
   PROCEDURE XPLMDrawAircraft(
                                        inPlaneIndex        : integer;    
                                        inX                 : single;    
                                        inY                 : single;    
                                        inZ                 : single;    
                                        inPitch             : single;    
                                        inRoll              : single;    
                                        inYaw               : single;    
                                        inFullDraw          : integer;    
                                        inDrawStateInfo     : PXPLMPlaneDrawState_t);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMReinitUsersPlane
    
    This function recomputes the derived flight model data from the aircraft 
    structure in memory.  If you have used the data access layer to modify the 
    aircraft structure, use this routine to resynchronize x-plane; since 
    X-plane works at least partly from derived values, the sim will not behave 
    properly until this is called. 
    
    WARNING: this routine does not necessarily place the airplane at the 
    airport; use XPLMSetUsersAircraft to be compatible.  This routine is 
    provided to do special experimentation with flight models without resetting 
    flight.                                                                     
   }
   PROCEDURE XPLMReinitUsersPlane;
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

IMPLEMENTATION
END.
