#ifndef _XPLMWeather_h_
#define _XPLMWeather_h_

/*
 * Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
 * rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
 *
 */

/***************************************************************************
 * XPLMWeather
 ***************************************************************************/
/*
 * This provides access to the X-Plane 12 enhanced weather system.
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(XPLM400)
/***************************************************************************
 * WEATHER ACCESS
 ***************************************************************************/

/*
 * XPLMWeatherInfoWinds_t
 *
 */
typedef struct {
    /* Altitude MSL, meters                                                       */
     float                     alt_msl;
    /* Wind speed, meters/sec                                                     */
     float                     speed;
    /* Direction (true)                                                           */
     float                     direction;
    /* Gust speed, meters/sec                                                     */
     float                     gust_speed;
    /* Shear arc, degrees i.e. 50% of this arc in either direction from base      */
     float                     shear;
    /* Clear-air turbulence ratio                                                 */
     float                     turbulence;
} XPLMWeatherInfoWinds_t;

/*
 * XPLMWeatherInfoClouds_t
 *
 */
typedef struct {
    /* Cloud type, float enum                                                     */
     float                     cloud_type;
    /* Coverage ratio                                                             */
     float                     coverage;
    /* Altitude MSL, meters                                                       */
     float                     alt_top;
    /* Altitude MSL, meters                                                       */
     float                     alt_base;
} XPLMWeatherInfoClouds_t;

/*
 * XPLMWeatherInfo_t
 * 
 * Basic weather conditions at a specific point.
 *
 */
typedef struct {
    /* The size of the struct.                                                    */
     int                       structSize;
    /* Temperature at the given altitude in Celsius                               */
     float                     temperature_alt;
    /* Dewpoint at the given altitude in Celsius                                  */
     float                     dewpoint_alt;
    /* Pressure at the given altitude in Pascals                                  */
     float                     pressure_alt;
    /* Precipitation rate at the given altitude                                   */
     float                     precip_rate_alt;
    /* Wind direction at the given altitude                                       */
     float                     wind_dir_alt;
    /* Wind speed at the given altitude, meters/sec                               */
     float                     wind_spd_alt;
    /* Turbulence ratio at the given altitude                                     */
     float                     turbulence_alt;
    /* Height of water waves in meters                                            */
     float                     wave_height;
    /* Length of water waves in meters                                            */
     float                     wave_length;
    /* Direction from which water waves are coming                                */
     int                       wave_dir;
    /* Speed of wave advance in meters/sec                                        */
     float                     wave_speed;
    /* Base visibility at 0 altitude, meters                                      */
     float                     visibility;
    /* Base precipitation ratio at 0 altitude                                     */
     float                     precip_rate;
    /* Climb rate due to thermals, meters/sec                                     */
     float                     thermal_climb;
    /* Pressure at 0 altitude in Pascals                                          */
     float                     pressure_sl;
    /* Defined wind layers. Not all layers are always defined.                    */
     XPLMWeatherInfoWinds_t    wind_layers[13];
    /* Defined cloud layers. Not all layers are always defined.                   */
     XPLMWeatherInfoClouds_t   cloud_layers[3];
} XPLMWeatherInfo_t;

/*
 * XPLMGetMETARForAirport
 * 
 * Get the last known METAR report for an airport by ICAO code. Note that the
 * actual weather at that airport may have evolved significantly since the
 * last downloaded METAR. outMETAR must point to a char buffer of at least 150
 * characters. This call is not intended to be used per-frame.
 *
 */
XPLM_API void       XPLMGetMETARForAirport(
                         const char *         airport_id,
                         XPLMFixedString150_t * outMETAR);

/*
 * XPLMGetWeatherAtLocation
 * 
 * Get the current weather conditions at a given location. Note that this does
 * not work world-wide, only within the surrounding region. Return 1 if
 * detailed weather was found, 0 if not. This call is not intended to be used
 * per-frame.
 *
 */
XPLM_API int        XPLMGetWeatherAtLocation(
                         double               latitude,
                         double               longitude,
                         double               altitude_m,
                         XPLMWeatherInfo_t *  out_info);

#endif /* XPLM400 */
#ifdef __cplusplus
}
#endif

#endif
