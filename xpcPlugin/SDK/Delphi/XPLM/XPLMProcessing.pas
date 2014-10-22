{
   Copyright 2005-2012 Sandy Barbour and Ben Supnik
   
   All rights reserved.  See license.txt for usage.
   
   X-Plane SDK Version: 2.1.1                                                  
}

UNIT XPLMProcessing;
INTERFACE
{
   This API allows you to get regular callbacks during the flight loop, the 
   part of X-Plane where the plane's position calculates the physics of 
   flight, etc.  Use these APIs to accomplish periodic tasks like logging data 
   and performing I/O. 
   
   WARNING: Do NOT use these callbacks to draw!  You cannot draw during flight 
   loop callbacks.  Use the drawing callbacks (see XPLMDisplay for more info) 
   for graphics.                                                               
}

USES   XPLMDefs;
   {$A4}
{$IFDEF MSWINDOWS}
   {$DEFINE DELPHI}
{$ENDIF}
{___________________________________________________________________________
 * FLIGHT LOOP CALLBACKS
 ___________________________________________________________________________}
{
                                                                               
}



{$IFDEF XPLM210}
   {
    XPLMFlightLoopPhaseType
    
    You can register a flight loop callback to run either before or after the 
    flight model is integrated by X-Plane.                                      
   }
TYPE
   XPLMFlightLoopPhaseType = (
     { Your callback runs before X-Plane integrates the flight model.              }
      xplm_FlightLoop_Phase_BeforeFlightModel  = 0
 
     { Your callback runs after X-Plane integrates the flight model.               }
     ,xplm_FlightLoop_Phase_AfterFlightModel   = 1
 
   );
   PXPLMFlightLoopPhaseType = ^XPLMFlightLoopPhaseType;
{$ENDIF}

{$IFDEF XPLM210}
   {
    XPLMFlightLoopID
    
    This is an opaque identifier for a flight loop callback.  You can use this 
    identifier to easily track and remove your callbacks, or to use the new 
    flight loop APIs.                                                           
   }
   XPLMFlightLoopID = pointer;
   PXPLMFlightLoopID = ^XPLMFlightLoopID;
{$ENDIF}

   {
    XPLMFlightLoop_f
    
    This is your flight loop callback.  Each time the flight loop is iterated 
    through, you receive this call at the end.  You receive a time since you 
    were last called and a time since the last loop, as well as a loop counter. 
    The 'phase' parameter is deprecated and should be ignored.				 
    
    Your return value controls when you will next be called. Return 0 to stop 
    receiving callbacks.  Pass a positive number to specify how many seconds 
    until the next callback.  (You will be called at or after this time, not 
    before.)  Pass a negative number to specify how many loops must go by until 
    you are called.  For example, -1.0 means call me the very next loop.  Try 
    to run your flight loop as infrequently as is practical, and suspend it 
    (using return value 0) when you do not need it; lots of flight loop 
    callbacks that do nothing lowers x-plane's frame rate. 
    
    Your callback will NOT be unregistered if you return 0; it will merely be 
    inactive. 
    
    The reference constant you passed to your loop is passed back to you.       
   }
     XPLMFlightLoop_f = FUNCTION(
                                    inElapsedSinceLastCall: single;    
                                    inElapsedTimeSinceLastFlightLoop: single;    
                                    inCounter           : integer;    
                                    inRefcon            : pointer) : single; cdecl;   

{$IFDEF XPLM210}
   {
    XPLMCreateFlightLoop_t
    
    XPLMCreateFlightLoop_t contains the parameters to create a new flight loop 
    callback.  The strsucture can be expanded in future SDKs - always set 
    structSize to the size of your structure in bytes.                          
   }
   XPLMCreateFlightLoop_t = RECORD
     structSize               : integer;
     phase                    : XPLMFlightLoopPhaseType;
     callbackFunc             : XPLMFlightLoop_f;
     refcon                   : pointer;
   END;
   PXPLMCreateFlightLoop_t = ^XPLMCreateFlightLoop_t;
{$ENDIF}

   {
    XPLMGetElapsedTime
    
    This routine returns the elapsed time since the sim started up in decimal 
    seconds.                                                                    
   }
   FUNCTION XPLMGetElapsedTime: single;
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMGetCycleNumber
    
    This routine returns a counter starting at zero for each sim cycle 
    computed/video frame rendered.                                              
   }
   FUNCTION XPLMGetCycleNumber: integer;
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMRegisterFlightLoopCallback
    
    This routine registers your flight loop callback.  Pass in a pointer to a 
    flight loop function and a refcon.  inInterval defines when you will be 
    called.  Pass in a positive number to specify seconds from registration 
    time to the next callback. Pass in a negative number to indicate when you 
    will be called (e.g. pass -1 to be called at the next cylcle).  Pass 0 to 
    not be called; your callback will be inactive.                              
   }
   PROCEDURE XPLMRegisterFlightLoopCallback(
                                        inFlightLoop        : XPLMFlightLoop_f;    
                                        inInterval          : single;    
                                        inRefcon            : pointer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMUnregisterFlightLoopCallback
    
    This routine unregisters your flight loop callback.  Do NOT call it from 
    your  flight loop callback.  Once your flight loop callback is 
    unregistered, it will not be called again.                                  
   }
   PROCEDURE XPLMUnregisterFlightLoopCallback(
                                        inFlightLoop        : XPLMFlightLoop_f;    
                                        inRefcon            : pointer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

   {
    XPLMSetFlightLoopCallbackInterval
    
    This routine sets when a callback will be called.  Do NOT call it from your 
    callback; use the return value of the callback to change your callback 
    interval from inside your callback. 
    
    inInterval is formatted the same way as in XPLMRegisterFlightLoopCallback; 
    positive for seconds, negative for cycles, and 0 for deactivating the 
    callback.  If  inRelativeToNow is 1, times are from the time of this call; 
    otherwise they are from the time the callback was last called (or the time 
    it was registered if it has never been called.                              
   }
   PROCEDURE XPLMSetFlightLoopCallbackInterval(
                                        inFlightLoop        : XPLMFlightLoop_f;    
                                        inInterval          : single;    
                                        inRelativeToNow     : integer;    
                                        inRefcon            : pointer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}

{$IFDEF XPLM210}
   {
    XPLMCreateFlightLoop
    
    This routine creates a flight loop callback and returns its ID.  The flight 
    loop callback is created using the input param struct, and is inited to be 
    unscheduled.                                                                
   }
   FUNCTION XPLMCreateFlightLoop(
                                        inParams            : PXPLMCreateFlightLoop_t) : XPLMFlightLoopID;    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
{$ENDIF}

{$IFDEF XPLM210}
   {
    XPLMDestroyFlightLoop
    
    This routine destroys a flight loop callback by ID.                         
   }
   PROCEDURE XPLMDestroyFlightLoop(
                                        inFlightLoopID      : XPLMFlightLoopID);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
{$ENDIF}

{$IFDEF XPLM210}
   {
    XPLMScheduleFlightLoop
    
    This routine schedules a flight loop callback for future execution.  If 
    inInterval is negative, it is run in  a certain number of frames based on 
    the absolute value of the input.  If the interval is positive, it is a 
    duration in seconds. 
    
    If inRelativeToNow is true, ties are interpretted relative to the time this 
    routine is called; otherwise they are relative to the last call time or the 
    time the flight loop was registered (if never called). 
    
    THREAD SAFETY: it is legal to call this routine from any thread under the 
    following conditions: 
    
    1. The call must be between the beginning of an XPLMEnable and the end of 
    an XPLMDisable sequence.  (That is, you must not call this routine from 
    thread activity when your plugin was supposed to be disabled.  Since 
    plugins are only enabled while loaded, this also implies you cannot run 
    this routine outside an XPLMStart/XPLMStop sequence.) 
    
    2. You may not call this routine re-entrantly for a single flight loop ID.  
    (That is, you can't enable from multiple threads at the same time.) 
    
    3. You must call this routine between the time after XPLMCreateFlightLoop 
    returns a value and the time you call XPLMDestroyFlightLoop.  (That is, you 
    must ensure that your threaded activity is within the life of the object.  
    The SDK does not check this for you, nor does it synchronize destruction of 
    the object.) 
    
    4. The object must be unscheduled if this routine is to be called from a 
    thread other than the main thread.                                          
   }
   PROCEDURE XPLMScheduleFlightLoop(
                                        inFlightLoopID      : XPLMFlightLoopID;    
                                        inInterval          : single;    
                                        inRelativeToNow     : integer);    
{$IFDEF DELPHI}
                                       cdecl; external 'XPLM.DLL';
{$ELSE}
                                       cdecl; external '';
{$ENDIF}
{$ENDIF}

IMPLEMENTATION
END.
