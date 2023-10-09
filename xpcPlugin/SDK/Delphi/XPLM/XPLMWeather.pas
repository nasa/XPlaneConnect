{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMWeather;
INTERFACE
{
   This provides access to the X-Plane 12 enhanced weather system.
}

USES
    XPLMDefs;
   {$A4}
{$IFDEF XPLM400}
{___________________________________________________________________________
 * WEATHER ACCESS
 ___________________________________________________________________________}

   {
    XPLMWeatherInfoWinds_t
   }
TYPE
   XPLMWeatherInfoWinds_t = RECORD
     { Altitude MSL, meters                                                       }
     alt_msl                  : Single;
     { Wind speed, meters/sec                                                     }
     speed                    : Single;
     { Direction (true)                                                           }
     direction                : Single;
     { Gust speed, meters/sec                                                     }
     gust_speed               : Single;
     { Shear arc, degrees i.e. 50% of this arc in either direction from base      }
     shear                    : Single;
     { Clear-air turbulence ratio                                                 }
     turbulence               : Single;
   END;
   PXPLMWeatherInfoWinds_t = ^XPLMWeatherInfoWinds_t;

   {
    XPLMWeatherInfoClouds_t
   }
   XPLMWeatherInfoClouds_t = RECORD
     { Cloud type, float enum                                                     }
     cloud_type               : Single;
     { Coverage ratio                                                             }
     coverage                 : Single;
     { Altitude MSL, meters                                                       }
     alt_top                  : Single;
     { Altitude MSL, meters                                                       }
     alt_base                 : Single;
   END;
   PXPLMWeatherInfoClouds_t = ^XPLMWeatherInfoClouds_t;

   {
    XPLMWeatherInfo_t
    
    Basic weather conditions at a specific point.
   }
   XPLMWeatherInfo_t = RECORD
     { The size of the struct.                                                    }
     structSize               : Integer;
     { Temperature at the given altitude in Celsius                               }
     temperature_alt          : Single;
     { Dewpoint at the given altitude in Celsius                                  }
     dewpoint_alt             : Single;
     { Pressure at the given altitude in Pascals                                  }
     pressure_alt             : Single;
     { Precipitation rate at the given altitude                                   }
     precip_rate_alt          : Single;
     { Wind direction at the given altitude                                       }
     wind_dir_alt             : Single;
     { Wind speed at the given altitude, meters/sec                               }
     wind_spd_alt             : Single;
     { Turbulence ratio at the given altitude                                     }
     turbulence_alt           : Single;
     { Height of water waves in meters                                            }
     wave_height              : Single;
     { Length of water waves in meters                                            }
     wave_length              : Single;
     { Direction from which water waves are coming                                }
     wave_dir                 : Integer;
     { Speed of wave advance in meters/sec                                        }
     wave_speed               : Single;
     { Base visibility at 0 altitude, meters                                      }
     visibility               : Single;
     { Base precipitation ratio at 0 altitude                                     }
     precip_rate              : Single;
     { Climb rate due to thermals, meters/sec                                     }
     thermal_climb            : Single;
     { Pressure at 0 altitude in Pascals                                          }
     pressure_sl              : Single;
     { Defined wind layers. Not all layers are always defined.                    }
     wind_layers[13]          : XPLMWeatherInfoWinds_t;
     { Defined cloud layers. Not all layers are always defined.                   }
     cloud_layers[3]          : XPLMWeatherInfoClouds_t;
   END;
   PXPLMWeatherInfo_t = ^XPLMWeatherInfo_t;

   {
    XPLMGetMETARForAirport
    
    Get the last known METAR report for an airport by ICAO code. Note that the
    actual weather at that airport may have evolved significantly since the
    last downloaded METAR. outMETAR must point to a char buffer of at least 150
    characters. This call is not intended to be used per-frame.
   }
   PROCEDURE XPLMGetMETARForAirport(
                                        airport_id          : XPLMString;
                                        outMETAR            : PXPLMFixedString150_t);
    cdecl; external XPLM_DLL;

   {
    XPLMGetWeatherAtLocation
    
    Get the current weather conditions at a given location. Note that this does
    not work world-wide, only within the surrounding region. Return 1 if
    detailed weather was found, 0 if not. This call is not intended to be used
    per-frame.
   }
   FUNCTION XPLMGetWeatherAtLocation(
                                        latitude            : Real;
                                        longitude           : Real;
                                        altitude_m          : Real;
                                        out_info            : PXPLMWeatherInfo_t) : Integer;
    cdecl; external XPLM_DLL;

{$ENDIF XPLM400}

IMPLEMENTATION

END.
