{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMScenery;
INTERFACE
{
   This package contains APIs to interact with X-Plane's scenery system.
}

USES
    XPLMDefs;
   {$A4}
{$IFDEF XPLM200}
{___________________________________________________________________________
 * Terrain Y-Testing
 ___________________________________________________________________________}
{
   The Y-testing API allows you to locate the physical scenery mesh. This
   would be used to place dynamic graphics on top of the ground in a plausible
   way or do physics interactions.
   
   The Y-test API works via probe objects, which are allocated by your plugin
   and used to query terrain. Probe objects exist both to capture which
   algorithm you have requested (see probe types) and also to cache query
   information.
   
   Performance Guidelines
   ----------------------
   
   It is generally faster to use the same probe for nearby points and
   different probes for different points. Try not to allocate more than
   "hundreds" of probes at most. Share probes if you need more. Generally,
   probing operations are expensive, and should be avoided via caching when
   possible.
   
   Y testing returns a location on the terrain, a normal vector, and a
   velocity vector. The normal vector tells you the slope of the terrain at
   that point. The velocity vector tells you if that terrain is moving (and is
   in meters/second). For example, if your Y test hits the aircraft carrier
   deck, this tells you the velocity of that point on the deck.
   
   Note: the Y-testing API is limited to probing the loaded scenery area,
   which is approximately 300x300 km in X-Plane 9. Probes outside this area
   will return the height of a 0 MSL sphere.
}


   {
    XPLMProbeType
    
    XPLMProbeType defines the type of terrain probe - each probe has a
    different algorithm. (Only one type of probe is provided right now, but
    future APIs will expose more flexible or powerful or useful probes.
   }
TYPE
   XPLMProbeType = (
     { The Y probe gives you the location of the tallest physical scenery along   }
     { the Y axis going through the queried point.                                }
      xplm_ProbeY                              = 0
 
   );
   PXPLMProbeType = ^XPLMProbeType;

   {
    XPLMProbeResult
    
    Probe results - possible results from a probe query.
   }
   XPLMProbeResult = (
     { The probe hit terrain and returned valid values.                           }
      xplm_ProbeHitTerrain                     = 0
 
     { An error in the API call.  Either the probe struct size is bad, the probe  }
     { is invalid, or the type is mismatched for the specific query call.         }
     ,xplm_ProbeError                          = 1
 
     { The probe call succeeded but there is no terrain under this point (perhaps }
     { it is off the side of the planet?)                                         }
     ,xplm_ProbeMissed                         = 2
 
   );
   PXPLMProbeResult = ^XPLMProbeResult;

   {
    XPLMProbeRef
    
    An XPLMProbeRef is an opaque handle to a probe, used for querying the
    terrain.
   }
   XPLMProbeRef = pointer;
   PXPLMProbeRef = ^XPLMProbeRef;

   {
    XPLMProbeInfo_t
    
    XPLMProbeInfo_t contains the results of a probe call. Make sure to set
    structSize to the size of the struct before using it.
   }
   XPLMProbeInfo_t = RECORD
     { Size of structure in bytes - always set this before calling the XPLM.      }
     structSize               : Integer;
     { Resulting X location of the terrain point we hit, in local OpenGL          }
     { coordinates.                                                               }
     locationX                : Single;
     { Resulting Y location of the terrain point we hit, in local OpenGL          }
     { coordinates.                                                               }
     locationY                : Single;
     { Resulting Z location of the terrain point we hit, in local OpenGL          }
     { coordinates.                                                               }
     locationZ                : Single;
     { X component of the normal vector to the terrain we found.                  }
     normalX                  : Single;
     { Y component of the normal vector to the terrain we found.                  }
     normalY                  : Single;
     { Z component of the normal vector to the terrain we found.                  }
     normalZ                  : Single;
     { X component of the velocity vector of the terrain we found.                }
     velocityX                : Single;
     { Y component of the velocity vector of the terrain we found.                }
     velocityY                : Single;
     { Z component of the velocity vector of the terrain we found.                }
     velocityZ                : Single;
     { Tells if the surface we hit is water (otherwise it is land).               }
     is_wet                   : Integer;
   END;
   PXPLMProbeInfo_t = ^XPLMProbeInfo_t;

   {
    XPLMCreateProbe
    
    Creates a new probe object of a given type and returns.
   }
   FUNCTION XPLMCreateProbe(
                                        inProbeType         : XPLMProbeType) : XPLMProbeRef;
    cdecl; external XPLM_DLL;

   {
    XPLMDestroyProbe
    
    Deallocates an existing probe object.
   }
   PROCEDURE XPLMDestroyProbe(
                                        inProbe             : XPLMProbeRef);
    cdecl; external XPLM_DLL;

   {
    XPLMProbeTerrainXYZ
    
    Probes the terrain. Pass in the XYZ coordinate of the probe point, a probe
    object, and an XPLMProbeInfo_t struct that has its structSize member set
    properly. Other fields are filled in if we hit terrain, and a probe result
    is returned.
   }
   FUNCTION XPLMProbeTerrainXYZ(
                                        inProbe             : XPLMProbeRef;
                                        inX                 : Single;
                                        inY                 : Single;
                                        inZ                 : Single;
                                        outInfo             : PXPLMProbeInfo_t) : XPLMProbeResult;
    cdecl; external XPLM_DLL;

{$ENDIF XPLM200}
{$IFDEF XPLM300}
{___________________________________________________________________________
 * Magnetic Variation
 ___________________________________________________________________________}
{
   Use the magnetic variation (more properly, the "magnetic declination") API
   to find the offset of magnetic north from true north at a given latitude
   and longitude within the simulator.
   
   In the real world, the Earth's magnetic field is irregular, such that true
   north (the direction along a meridian toward the north pole) does not
   necessarily match what a magnetic compass shows as north.
   
   Using this API ensures that you present the same offsets to users as
   X-Plane's built-in instruments.
}


   {
    XPLMGetMagneticVariation
    
    Returns X-Plane's simulated magnetic variation (declination) at the
    indication latitude and longitude.
   }
   FUNCTION XPLMGetMagneticVariation(
                                        latitude            : Real;
                                        longitude           : Real) : Single;
    cdecl; external XPLM_DLL;

   {
    XPLMDegTrueToDegMagnetic
    
    Converts a heading in degrees relative to true north into a value relative
    to magnetic north at the user's current location.
   }
   FUNCTION XPLMDegTrueToDegMagnetic(
                                        headingDegreesTrue  : Single) : Single;
    cdecl; external XPLM_DLL;

   {
    XPLMDegMagneticToDegTrue
    
    Converts a heading in degrees relative to magnetic north at the user's
    current location into a value relative to true north.
   }
   FUNCTION XPLMDegMagneticToDegTrue(
                                        headingDegreesMagnetic: Single) : Single;
    cdecl; external XPLM_DLL;

{$ENDIF XPLM300}
{___________________________________________________________________________
 * Object Drawing
 ___________________________________________________________________________}
{
   The object drawing routines let you load and draw X-Plane OBJ files.
   Objects are loaded by file path and managed via an opaque handle. X-Plane
   naturally reference counts objects, so it is important that you balance
   every successful call to XPLMLoadObject with a call to XPLMUnloadObject!
}


{$IFDEF XPLM200}
TYPE
   {
    XPLMObjectRef
    
    An XPLMObjectRef is a opaque handle to an .obj file that has been loaded
    into memory.
   }
   XPLMObjectRef = pointer;
   PXPLMObjectRef = ^XPLMObjectRef;
{$ENDIF XPLM200}

{$IFDEF XPLM200}
   {
    XPLMDrawInfo_t
    
    The XPLMDrawInfo_t structure contains positioning info for one object that
    is to be drawn. Be sure to set structSize to the size of the structure for
    future expansion.
   }
   XPLMDrawInfo_t = RECORD
     { Set this to the size of this structure!                                    }
     structSize               : Integer;
     { X location of the object in local coordinates.                             }
     x                        : Single;
     { Y location of the object in local coordinates.                             }
     y                        : Single;
     { Z location of the object in local coordinates.                             }
     z                        : Single;
     { Pitch in degres to rotate the object, positive is up.                      }
     pitch                    : Single;
     { Heading in local coordinates to rotate the object, clockwise.              }
     heading                  : Single;
     { Roll to rotate the object.                                                 }
     roll                     : Single;
   END;
   PXPLMDrawInfo_t = ^XPLMDrawInfo_t;
{$ENDIF XPLM200}

{$IFDEF XPLM210}
   {
    XPLMObjectLoaded_f
    
    You provide this callback when loading an object asynchronously; it will be
    called once the object is loaded. Your refcon is passed back. The object
    ref passed in is the newly loaded object (ready for use) or NULL if an
    error occured.
    
    If your plugin is disabled, this callback will be delivered as soon as the
    plugin is re-enabled. If your plugin is unloaded before this callback is
    ever called, the SDK will release the object handle for you.
   }
TYPE
     XPLMObjectLoaded_f = PROCEDURE(
                                    inObject            : XPLMObjectRef;
                                    inRefcon            : pointer); cdecl;
{$ENDIF XPLM210}

{$IFDEF XPLM200}
   {
    XPLMLoadObject
    
    This routine loads an OBJ file and returns a handle to it. If X-Plane has
    already loaded the object, the handle to the existing object is returned.
    Do not assume you will get the same handle back twice, but do make sure to
    call unload once for every load to avoid "leaking" objects. The object will
    be purged from memory when no plugins and no scenery are using it.
    
    The path for the object must be relative to the X-System base folder. If
    the path is in the root of the X-System folder you may need to prepend ./
    to it; loading objects in the root of the X-System folder is STRONGLY
    discouraged - your plugin should not dump art resources in the root folder!
    
    XPLMLoadObject will return NULL if the object cannot be loaded (either
    because it is not found or the file is misformatted). This routine will
    load any object that can be used in the X-Plane scenery system.
    
    It is important that the datarefs an object uses for animation already be
    registered before you load the object. For this reason it may be necessary
    to defer object loading until the sim has fully started.
   }
   FUNCTION XPLMLoadObject(
                                        inPath              : XPLMString) : XPLMObjectRef;
    cdecl; external XPLM_DLL;
{$ENDIF XPLM200}

{$IFDEF XPLM210}
   {
    XPLMLoadObjectAsync
    
    This routine loads an object asynchronously; control is returned to you
    immediately while X-Plane loads the object. The sim will not stop flying
    while the object loads. For large objects, it may be several seconds before
    the load finishes.
    
    You provide a callback function that is called once the load has completed.
    Note that if the object cannot be loaded, you will not find out until the
    callback function is called with a NULL object handle.
    
    There is no way to cancel an asynchronous object load; you must wait for
    the load to complete and then release the object if it is no longer
    desired.
   }
   PROCEDURE XPLMLoadObjectAsync(
                                        inPath              : XPLMString;
                                        inCallback          : XPLMObjectLoaded_f;
                                        inRefcon            : pointer);
    cdecl; external XPLM_DLL;
{$ENDIF XPLM210}

{$IFDEF XPLM_DEPRECATED}
   {
    XPLMDrawObjects
    
    __Deprecation Warning__: use XPLMInstancing to draw 3-d objects by creating
    instances, rather than these APIs from draw callbacks.
    
    XPLMDrawObjects draws an object from an OBJ file one or more times. You
    pass in the object and an array of XPLMDrawInfo_t structs, one for each
    place you would like the object to be drawn.
    
    X-Plane will attempt to cull the objects based on LOD and visibility, and
    will pick the appropriate LOD.
    
    Lighting is a boolean; pass 1 to show the night version of object with
    night-only lights lit up. Pass 0 to show the daytime version of the object.
    
    earth_relative controls the coordinate system. If this is 1, the rotations
    you specify are applied to the object after its coordinate system is
    transformed from local to earth-relative coordinates -- that is, an object
    with no rotations will point toward true north and the Y axis will be up
    against gravity. If this is 0, the object is drawn with your rotations from
    local coordanates -- that is, an object with no rotations is drawn pointing
    down the -Z axis and the Y axis of the object matches the local coordinate
    Y axis.
   }
   PROCEDURE XPLMDrawObjects(
                                        inObject            : XPLMObjectRef;
                                        inCount             : Integer;
                                        inLocations         : PXPLMDrawInfo_t;
                                        lighting            : Integer;
                                        earth_relative      : Integer);
    cdecl; external XPLM_DLL;
{$ENDIF XPLM_DEPRECATED}

{$IFDEF XPLM200}
   {
    XPLMUnloadObject
    
    This routine marks an object as no longer being used by your plugin.
    Objects are reference counted: once no plugins are using an object, it is
    purged from memory. Make sure to call XPLMUnloadObject once for each
    successful call to XPLMLoadObject.
   }
   PROCEDURE XPLMUnloadObject(
                                        inObject            : XPLMObjectRef);
    cdecl; external XPLM_DLL;
{$ENDIF XPLM200}

{$IFDEF XPLM200}
{___________________________________________________________________________
 * Library Access
 ___________________________________________________________________________}
{
   The library access routines allow you to locate scenery objects via the
   X-Plane library system. Right now library access is only provided for
   objects, allowing plugin-drawn objects to be extended using the library
   system.
}


   {
    XPLMLibraryEnumerator_f
    
    An XPLMLibraryEnumerator_f is a callback you provide that is called once
    for each library element that is located. The returned paths will be
    relative to the X-System folder.
   }
TYPE
     XPLMLibraryEnumerator_f = PROCEDURE(
                                    inFilePath          : XPLMString;
                                    inRef               : pointer); cdecl;

   {
    XPLMLookupObjects
    
    This routine looks up a virtual path in the library system and returns all
    matching elements. You provide a callback - one virtual path may match many
    objects in the library. XPLMLookupObjects returns the number of objects
    found.
    
    The latitude and longitude parameters specify the location the object will
    be used. The library system allows for scenery packages to only provide
    objects to certain local locations. Only objects that are allowed at the
    latitude/longitude you provide will be returned.
   }
   FUNCTION XPLMLookupObjects(
                                        inPath              : XPLMString;
                                        inLatitude          : Single;
                                        inLongitude         : Single;
                                        enumerator          : XPLMLibraryEnumerator_f;
                                        ref                 : pointer) : Integer;
    cdecl; external XPLM_DLL;

{$ENDIF XPLM200}

IMPLEMENTATION

END.
