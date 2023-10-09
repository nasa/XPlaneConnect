{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMProcessing;
INTERFACE
{
   This API allows you to get regular callbacks during the flight loop, the
   part of X-Plane where the plane's position calculates the physics of
   flight, etc. Use these APIs to accomplish periodic tasks like logging data
   and performing I/O.
   
   You can receive a callback either just before or just after the per-frame
   physics calculations happen - you can use post-flightmodel callbacks to
   "patch" the flight model after it has run.
   
   If the user has set the number of flight model iterations per frame greater
   than one your plugin will _not_ see this; these integrations run on the
   sub-section of the flight model where iterations improve responsiveness
   (e.g. physical integration, not simple systems tracking) and are thus
   opaque to plugins.
   
   Flight loop scheduling, when scheduled by time, is scheduled by a "first
   callback after the deadline" schedule, e.g. your callbacks will always be
   slightly late to ensure that we don't run faster than your deadline.
   
   WARNING: Do NOT use these callbacks to draw! You cannot draw during flight
   loop callbacks. Use the drawing callbacks (see XPLMDisplay for more info)
   for graphics or the XPLMInstance functions for aircraft or models. (One
   exception: you can use a post-flight loop callback to update your own
   off-screen FBOs.)
}

USES
    XPLMDefs;
   {$A4}
{___________________________________________________________________________
 * FLIGHT LOOP CALLBACKS
 ___________________________________________________________________________}

{$IFDEF XPLM210}
   {
    XPLMFlightLoopPhaseType
    
    You can register a flight loop callback to run either before or after the
    flight model is integrated by X-Plane.
   }
TYPE
   XPLMFlightLoopPhaseType = (
     { Your callback runs before X-Plane integrates the flight model.             }
      xplm_FlightLoop_Phase_BeforeFlightModel  = 0
 
     { Your callback runs after X-Plane integrates the flight model.              }
     ,xplm_FlightLoop_Phase_AfterFlightModel   = 1
 
   );
   PXPLMFlightLoopPhaseType = ^XPLMFlightLoopPhaseType;
{$ENDIF XPLM210}

{$IFDEF XPLM210}
   {
    XPLMFlightLoopID
    
    This is an opaque identifier for a flight loop callback. You can use this
    identifier to easily track and remove your callbacks, or to use the new
    flight loop APIs.
   }
   XPLMFlightLoopID = pointer;
   PXPLMFlightLoopID = ^XPLMFlightLoopID;
{$ENDIF XPLM210}

   {
    XPLMFlightLoop_f
    
    This is your flight loop callback. Each time the flight loop is iterated
    through, you receive this call at the end.
    
    Flight loop callbacks receive a number of input timing parameters. These
    input timing parameters are not particularly useful; you may need to track
    your own timing data (e.g. by reading datarefs). The input parameters are:
    
    - inElapsedSinceLastCall: the wall time since your last callback.
    - inElapsedTimeSinceLastFlightLoop: the wall time since any flight loop was
      dispatched.
    - inCounter: a monotonically increasing counter, bumped once per flight
      loop dispatch from the sim.
    - inRefcon: your own pointer constant provided when you registered yor
      callback.
    
    Your return value controls when you will next be called.
    
     - Return 0 to stop receiving callbacks.
     - Return a positive number to specify how many seconds until the next
       callback. (You will be called at or after this time, not before.)
     - Return a negative number to specify how many loops must go by until you
       are called. For example, -1.0 means call me the very next loop.
    
    Try to run your flight loop as infrequently as is practical, and suspend it
    (using return value 0) when you do not need it; lots of flight loop
    callbacks that do nothing lowers X-Plane's frame rate.
    
    Your callback will NOT be unregistered if you return 0; it will merely be
    inactive.
   }
TYPE
     XPLMFlightLoop_f = FUNCTION(
                                    inElapsedSinceLastCall: Single;
                                    inElapsedTimeSinceLastFlightLoop: Single;
                                    inCounter           : Integer;
                                    inRefcon            : pointer) : Single; cdecl;

{$IFDEF XPLM210}
   {
    XPLMCreateFlightLoop_t
    
    XPLMCreateFlightLoop_t contains the parameters to create a new flight loop
    callback. The structure may be expanded in future SDKs - always set
    structSize to the size of your structure in bytes.
   }
TYPE
   XPLMCreateFlightLoop_t = RECORD
     structSize               : Integer;
     phase                    : XPLMFlightLoopPhaseType;
     callbackFunc             : XPLMFlightLoop_f;
     refcon                   : pointer;
   END;
   PXPLMCreateFlightLoop_t = ^XPLMCreateFlightLoop_t;
{$ENDIF XPLM210}

   {
    XPLMGetElapsedTime
    
    This routine returns the elapsed time since the sim started up in decimal
    seconds. This is a wall timer; it keeps counting upward even if the sim is
    pasued.
    
    __WARNING__: XPLMGetElapsedTime is not a very good timer!  It lacks
    precision in both its data type and its source.  Do not attempt to use it
    for timing critical applications like network multiplayer.
   }
   FUNCTION XPLMGetElapsedTime: Single;
    cdecl; external XPLM_DLL;

   {
    XPLMGetCycleNumber
    
    This routine returns a counter starting at zero for each sim cycle
    computed/video frame rendered.
   }
   FUNCTION XPLMGetCycleNumber: Integer;
    cdecl; external XPLM_DLL;

   {
    XPLMRegisterFlightLoopCallback
    
    This routine registers your flight loop callback. Pass in a pointer to a
    flight loop function and a refcon (an optional reference value determined
    by you). inInterval defines when you will be called. Pass in a positive
    number to specify seconds from registration time to the next callback. Pass
    in a negative number to indicate when you will be called (e.g. pass -1 to
    be called at the next cylcle). Pass 0 to not be called; your callback will
    be inactive.
    
    (This legacy function only installs pre-flight-loop callbacks; use
    XPLMCreateFlightLoop for more control.)
   }
   PROCEDURE XPLMRegisterFlightLoopCallback(
                                        inFlightLoop        : XPLMFlightLoop_f;
                                        inInterval          : Single;
                                        inRefcon            : pointer);
    cdecl; external XPLM_DLL;

   {
    XPLMUnregisterFlightLoopCallback
    
    This routine unregisters your flight loop callback. Do NOT call it from
    your flight loop callback. Once your flight loop callback is unregistered,
    it will not be called again.
    
    Only use this on flight loops registered via
    XPLMRegisterFlightLoopCallback.
   }
   PROCEDURE XPLMUnregisterFlightLoopCallback(
                                        inFlightLoop        : XPLMFlightLoop_f;
                                        inRefcon            : pointer);
    cdecl; external XPLM_DLL;

   {
    XPLMSetFlightLoopCallbackInterval
    
    This routine sets when a callback will be called. Do NOT call it from your
    callback; use the return value of the callback to change your callback
    interval from inside your callback.
    
    inInterval is formatted the same way as in XPLMRegisterFlightLoopCallback;
    positive for seconds, negative for cycles, and 0 for deactivating the
    callback. If inRelativeToNow is 1, times are from the time of this call;
    otherwise they are from the time the callback was last called (or the time
    it was registered if it has never been called.
   }
   PROCEDURE XPLMSetFlightLoopCallbackInterval(
                                        inFlightLoop        : XPLMFlightLoop_f;
                                        inInterval          : Single;
                                        inRelativeToNow     : Integer;
                                        inRefcon            : pointer);
    cdecl; external XPLM_DLL;

{$IFDEF XPLM210}
   {
    XPLMCreateFlightLoop
    
    This routine creates a flight loop callback and returns its ID. The flight
    loop callback is created using the input param struct, and is inited to be
    unscheduled.
   }
   FUNCTION XPLMCreateFlightLoop(
                                        inParams            : PXPLMCreateFlightLoop_t) : XPLMFlightLoopID;
    cdecl; external XPLM_DLL;
{$ENDIF XPLM210}

{$IFDEF XPLM210}
   {
    XPLMDestroyFlightLoop
    
    This routine destroys a flight loop callback by ID. Only call it on flight
    loops created with the newer XPLMCreateFlightLoop API.
   }
   PROCEDURE XPLMDestroyFlightLoop(
                                        inFlightLoopID      : XPLMFlightLoopID);
    cdecl; external XPLM_DLL;
{$ENDIF XPLM210}

{$IFDEF XPLM210}
   {
    XPLMScheduleFlightLoop
    
    This routine schedules a flight loop callback for future execution. If
    inInterval is negative, it is run in a certain number of frames based on
    the absolute value of the input. If the interval is positive, it is a
    duration in seconds.
    
    If inRelativeToNow is true, times are interpreted relative to the time this
    routine is called; otherwise they are relative to the last call time or the
    time the flight loop was registered (if never called).
   }
   PROCEDURE XPLMScheduleFlightLoop(
                                        inFlightLoopID      : XPLMFlightLoopID;
                                        inInterval          : Single;
                                        inRelativeToNow     : Integer);
    cdecl; external XPLM_DLL;
{$ENDIF XPLM210}


IMPLEMENTATION

END.
