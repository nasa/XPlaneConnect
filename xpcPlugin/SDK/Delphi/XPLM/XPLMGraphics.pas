{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMGraphics;
INTERFACE
{
   A few notes on coordinate systems:
   
   X-Plane uses three kinds of coordinates.  Global coordinates are specified
   as latitude, longitude and elevation.  This coordinate system never changes
   but is not very precise.
   
   OpenGL (or 'local') coordinates are cartesian and move with the aircraft. 
   They offer more precision and are used for 3-d OpenGL drawing.  The X axis
   is aligned east-west with positive X meaning east.  The Y axis is aligned
   straight up and down at the point 0,0,0 (but since the Earth is round it is
   not truly straight up and down at other points).  The Z axis is aligned
   north-south at 0, 0, 0 with positive Z pointing south (but since the Earth
   is round it isn't exactly north-south as you move east or west of 0, 0, 0).
   One unit is one meter and the point 0,0,0 is on the surface of the Earth at
   sea level for some latitude and longitude picked by the sim such that the
   user's aircraft is reasonably nearby.
   
   2-d Panel coordinates are 2d, with the X axis horizontal and the Y axis
     vertical. The point 0,0 is the bottom left and 1024,768 is the upper
     right of the screen. This is true no matter what resolution the user's
     monitor is in; when running in higher resolution, graphics will be
     scaled.
   
   Use X-Plane's routines to convert between global and local coordinates.  Do
   not attempt to do this conversion yourself; the precise 'roundness' of
   X-Plane's physics model may not match your own, and (to make things
   weirder) the user can potentially customize the physics of the current
   planet.
}

USES
    XPLMDefs;
   {$A4}
{___________________________________________________________________________
 * X-PLANE GRAPHICS
 ___________________________________________________________________________}
{
   These routines allow you to use OpenGL with X-Plane.
}


   {
    XPLMTextureID
    
    XPLM Texture IDs name well-known textures in the sim for you to use. This
    allows you to recycle textures from X-Plane, saving VRAM.
    
    *Warning*: do not use these enums.  The only remaining use they have is to
     access the legacy compatibility v10 UI texture; if you need this, get it
     via the Widgets library.
   }
TYPE
   XPLMTextureID = (
     { The bitmap that contains window outlines, button outlines, fonts, etc.     }
      xplm_Tex_GeneralInterface                = 0
 
{$IFDEF XPLM_DEPRECATED}
     { The exterior paint for the user's aircraft (daytime).                      }
     ,xplm_Tex_AircraftPaint                   = 1
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { The exterior light map for the user's aircraft.                            }
     ,xplm_Tex_AircraftLiteMap                 = 2
{$ENDIF XPLM_DEPRECATED}
 
   );
   PXPLMTextureID = ^XPLMTextureID;

   {
    XPLMSetGraphicsState
    
    XPLMSetGraphicsState changes OpenGL's fixed function pipeline state.  You
    are not responsible for restoring any state that is accessed via
    XPLMSetGraphicsState, but you are responsible for not accessing this state
    directly.
    
    - inEnableFog - enables or disables fog, equivalent to: glEnable(GL_FOG);
    - inNumberTexUnits - enables or disables a number of multitexturing units.
      If the number is 0, 2d texturing is disabled entirely, as in
      glDisable(GL_TEXTURE_2D);  Otherwise, 2d texturing is enabled, and a
      number of multitexturing units are enabled sequentially, starting with
      unit 0, e.g. glActiveTextureARB(GL_TEXTURE0_ARB); glEnable
      (GL_TEXTURE_2D);
    - inEnableLighting - enables or disables OpenGL lighting, e.g.
      glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    - inEnableAlphaTesting - enables or disables the alpha test per pixel, e.g.
      glEnable(GL_ALPHA_TEST);
    - inEnableAlphaBlending - enables or disables alpha blending per pixel,
      e.g. glEnable(GL_BLEND);
    - inEnableDepthTesting - enables per pixel depth testing, as in
      glEnable(GL_DEPTH_TEST);
    - inEnableDepthWriting - enables writing back of depth information to the
      depth buffer, as in glDepthMask(GL_TRUE);
    
    The purpose of this function is to change OpenGL state while keeping
    X-Plane aware of the state changes; this keeps X-Plane from getting
    surprised by OGL state changes, and prevents X-Plane and plug-ins from
    having to set all state before all draws; XPLMSetGraphicsState internally
    skips calls to change state that is already properly enabled.
    
    X-Plane does not have a 'default' OGL state for plug-ins with respect to
    the above state vector; plug-ins should totally set OGL state using this
    API before drawing.  Use XPLMSetGraphicsState instead of any of the above
    OpenGL calls.
    
    WARNING: Any routine that performs drawing (e.g. XPLMDrawString or widget
    code) may change X-Plane's state.  Always set state before drawing after
    unknown code has executed.
    
    *Deprecation Warnings*: X-Plane's lighting and fog environment is
     significantly more complex than the fixed function pipeline can express;
     do not assume that lighting and fog state is a good approximation for 3-d
     drawing.  Prefer to use XPLMInstancing to draw objects.  All calls to
     XPLMSetGraphicsState should have no fog or lighting.
   }
   PROCEDURE XPLMSetGraphicsState(
                                        inEnableFog         : Integer;
                                        inNumberTexUnits    : Integer;
                                        inEnableLighting    : Integer;
                                        inEnableAlphaTesting: Integer;
                                        inEnableAlphaBlending: Integer;
                                        inEnableDepthTesting: Integer;
                                        inEnableDepthWriting: Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMBindTexture2d
    
     XPLMBindTexture2d changes what texture is bound to the 2d texturing
     target. This routine caches the current 2d texture across all texturing
     units in the sim and plug-ins, preventing extraneous binding.  For
     example, consider several plug-ins running in series; if they all use the
     'general interface' bitmap to do UI, calling this function will skip the
     rebinding of the general interface texture on all but the first plug-in,
     which can provide better frame rates on some graphics cards.
    
    inTextureID is the ID of the texture object to bind; inTextureUnit is a
    zero-based texture unit (e.g. 0 for the first one), up to a maximum of 4
    units.  (This number may increase in future versions of X-Plane.)
    
    Use this routine instead of glBindTexture(GL_TEXTURE_2D, ....);
   }
   PROCEDURE XPLMBindTexture2d(
                                        inTextureNum        : Integer;
                                        inTextureUnit       : Integer);
    cdecl; external XPLM_DLL;

   {
    XPLMGenerateTextureNumbers
    
    Use this routine instead of glGenTextures to generate new texture object
    IDs. This routine historically ensured that plugins don't use texure IDs
    that X-Plane is reserving for its own use.
   }
   PROCEDURE XPLMGenerateTextureNumbers(
                                        outTextureIDs       : PInteger;
                                        inCount             : Integer);
    cdecl; external XPLM_DLL;

{$IFDEF XPLM_DEPRECATED}
   {
    XPLMGetTexture
    
    XPLMGetTexture returns the OpenGL texture ID of an X-Plane texture based on
    a generic identifying code.  For example, you can get the texture for
    X-Plane's UI bitmaps.
   }
   FUNCTION XPLMGetTexture(
                                        inTexture           : XPLMTextureID) : Integer;
    cdecl; external XPLM_DLL;
{$ENDIF XPLM_DEPRECATED}

   {
    XPLMWorldToLocal
    
    This routine translates coordinates from latitude, longitude, and altitude
    to local scene coordinates. Latitude and longitude are in decimal degrees,
    and altitude is in meters MSL (mean sea level).  The XYZ coordinates are in
    meters in the local OpenGL coordinate system.
   }
   PROCEDURE XPLMWorldToLocal(
                                        inLatitude          : Real;
                                        inLongitude         : Real;
                                        inAltitude          : Real;
                                        outX                : PReal;
                                        outY                : PReal;
                                        outZ                : PReal);
    cdecl; external XPLM_DLL;

   {
    XPLMLocalToWorld
    
    This routine translates a local coordinate triplet back into latitude,
    longitude, and altitude.  Latitude and longitude are in decimal degrees,
    and altitude is in meters MSL (mean sea level).  The XYZ coordinates are in
    meters in the local OpenGL coordinate system.
    
    NOTE: world coordinates are less precise than local coordinates; you should
    try to avoid round tripping from local to world and back.
   }
   PROCEDURE XPLMLocalToWorld(
                                        inX                 : Real;
                                        inY                 : Real;
                                        inZ                 : Real;
                                        outLatitude         : PReal;
                                        outLongitude        : PReal;
                                        outAltitude         : PReal);
    cdecl; external XPLM_DLL;

   {
    XPLMDrawTranslucentDarkBox
    
    This routine draws a translucent dark box, partially obscuring parts of the
    screen but making text easy to read.  This is the same graphics primitive
    used by X-Plane to show text files.
   }
   PROCEDURE XPLMDrawTranslucentDarkBox(
                                        inLeft              : Integer;
                                        inTop               : Integer;
                                        inRight             : Integer;
                                        inBottom            : Integer);
    cdecl; external XPLM_DLL;

{___________________________________________________________________________
 * X-PLANE TEXT
 ___________________________________________________________________________}

   {
    XPLMFontID
    
    X-Plane features some fixed-character fonts.  Each font may have its own
    metrics.
    
    WARNING: Some of these fonts are no longer supported or may have changed
    geometries. For maximum copmatibility, see the comments below.
    
    Note: X-Plane 7 supports proportional-spaced fonts.  Since no measuring
    routine is available yet, the SDK will normally draw using a fixed-width
    font.  You can use a dataref to enable proportional font drawing on XP7 if
    you want to.
   }
TYPE
   XPLMFontID = (
     { Mono-spaced font for user interface.  Available in all versions of the SDK.}
      xplmFont_Basic                           = 0
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_Menus                           = 1
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_Metal                           = 2
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_Led                             = 3
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_LedWide                         = 4
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_PanelHUD                        = 5
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_PanelEFIS                       = 6
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_PanelGPS                        = 7
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_RadiosGA                        = 8
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_RadiosBC                        = 9
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_RadiosHM                        = 10
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_RadiosGANarrow                  = 11
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_RadiosBCNarrow                  = 12
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_RadiosHMNarrow                  = 13
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_Timer                           = 14
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_FullRound                       = 15
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_SmallRound                      = 16
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM_DEPRECATED}
     { Deprecated, do not use.                                                    }
     ,xplmFont_Menus_Localized                 = 17
{$ENDIF XPLM_DEPRECATED}
 
{$IFDEF XPLM200}
     { Proportional UI font.                                                      }
     ,xplmFont_Proportional                    = 18
{$ENDIF XPLM200}
 
   );
   PXPLMFontID = ^XPLMFontID;

   {
    XPLMDrawString
    
    This routine draws a NULL terminated string in a given font.  Pass in the
    lower left pixel that the character is to be drawn onto.  Also pass the
    character and font ID. This function returns the x offset plus the width of
    all drawn characters. The color to draw in is specified as a pointer to an
    array of three floating point colors, representing RGB intensities from 0.0
    to 1.0.
   }
   PROCEDURE XPLMDrawString(
                                        inColorRGB          : PSingle;
                                        inXOffset           : Integer;
                                        inYOffset           : Integer;
                                        inChar              : XPLMString;
                                        inWordWrapWidth     : PInteger;    { Can be nil }
                                        inFontID            : XPLMFontID);
    cdecl; external XPLM_DLL;

   {
    XPLMDrawNumber
    
    This routine draws a number similar to the digit editing fields in
    PlaneMaker and data output display in X-Plane.  Pass in a color, a
    position, a floating point value, and formatting info.  Specify how many
    integer and how many decimal digits to show and whether to show a sign, as
    well as a character set. This routine returns the xOffset plus width of the
    string drawn.
   }
   PROCEDURE XPLMDrawNumber(
                                        inColorRGB          : PSingle;
                                        inXOffset           : Integer;
                                        inYOffset           : Integer;
                                        inValue             : Real;
                                        inDigits            : Integer;
                                        inDecimals          : Integer;
                                        inShowSign          : Integer;
                                        inFontID            : XPLMFontID);
    cdecl; external XPLM_DLL;

   {
    XPLMGetFontDimensions
    
    This routine returns the width and height of a character in a given font.
    It also tells you if the font only supports numeric digits.  Pass NULL if
    you don't need a given field.  Note that for a proportional font the width
    will be an arbitrary, hopefully average width.
   }
   PROCEDURE XPLMGetFontDimensions(
                                        inFontID            : XPLMFontID;
                                        outCharWidth        : PInteger;    { Can be nil }
                                        outCharHeight       : PInteger;    { Can be nil }
                                        outDigitsOnly       : PInteger);    { Can be nil }
    cdecl; external XPLM_DLL;

{$IFDEF XPLM200}
   {
    XPLMMeasureString
    
    This routine returns the width in pixels of a string using a given font. 
    The string is passed as a pointer plus length (and does not need to be null
    terminated); this is used to allow for measuring substrings. The return
    value is floating point; it is possible that future font drawing may allow
    for fractional pixels.
   }
   FUNCTION XPLMMeasureString(
                                        inFontID            : XPLMFontID;
                                        inChar              : XPLMString;
                                        inNumChars          : Integer) : Single;
    cdecl; external XPLM_DLL;
{$ENDIF XPLM200}


IMPLEMENTATION

END.
