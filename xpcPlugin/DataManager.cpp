//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
//
//X-Plane API
//Copyright(c) 2008, Sandy Barbour and Ben Supnik All rights reserved.
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
//associated documentation files(the "Software"), to deal in the Software without restriction,
//including without limitation the rights to use, copy, modify, merge, publish, distribute,
//sublicense, and / or sell copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//  * Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//  * Neither the names of the authors nor that of X - Plane or Laminar Research
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission from the authors or
//    Laminar Research, respectively.
#include "DataManager.h"
#include "DataMaps.h"
#include "Log.h"

#include "XPLMDataAccess.h"
#include "XPLMGraphics.h"

#include <cmath>
#include <cstdio>
#include <map>

namespace XPC
{
	using namespace std;

	static map<DREF, XPLMDataRef> drefs;
	static map<DREF, XPLMDataRef> mdrefs[20];
	static map<string, XPLMDataRef> sdrefs;

	void DataManager::Initialize()
	{
		drefs.insert(make_pair(DREF_None, XPLMFindDataRef("sim/test/test_float")));

		drefs.insert(make_pair(DREF_Pause, XPLMFindDataRef("sim/operation/override/override_planepath")));
		drefs.insert(make_pair(DREF_PauseAI, XPLMFindDataRef("sim/operation/override/override_plane_ai_autopilot")));

		drefs.insert(make_pair(DREF_TotalRuntime, XPLMFindDataRef("sim/time/total_running_time_sec")));
		drefs.insert(make_pair(DREF_TotalFlighttime, XPLMFindDataRef("sim/time/total_flight_time_sec")));
		drefs.insert(make_pair(DREF_TimerElapsedtime, XPLMFindDataRef("sim/time/timer_elapsed_time_sec")));

		drefs.insert(make_pair(DREF_IndicatedAirspeed, XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed")));
		drefs.insert(make_pair(DREF_TrueAirspeed, XPLMFindDataRef("sim/flightmodel/position/true_airspeed")));
		drefs.insert(make_pair(DREF_GroundSpeed, XPLMFindDataRef("sim/flightmodel/position/groundspeed")));

		drefs.insert(make_pair(DREF_MachNumber, XPLMFindDataRef("sim/flightmodel/misc/machno")));
		drefs.insert(make_pair(DREF_GForceNormal, XPLMFindDataRef("sim/flightmodel2/misc/gforce_normal")));
		drefs.insert(make_pair(DREF_GForceAxial, XPLMFindDataRef("sim/flightmodel2/misc/gforce_axial")));
		drefs.insert(make_pair(DREF_GForceSide, XPLMFindDataRef("sim/flightmodel2/misc/gforce_side")));

		drefs.insert(make_pair(DREF_BarometerSealevelInHg, XPLMFindDataRef("sim/weather/barometer_sealevel_inhg")));
		drefs.insert(make_pair(DREF_TemperaturSealevelC, XPLMFindDataRef("sim/weather/temperature_sealevel_c")));
		drefs.insert(make_pair(DREF_WindSpeedKts, XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_speed_kts")));

		drefs.insert(make_pair(DREF_YokePitch, XPLMFindDataRef("sim/joystick/yoke_pitch_ratio")));
		drefs.insert(make_pair(DREF_YokeRoll, XPLMFindDataRef("sim/joystick/yoke_roll_ratio")));
		drefs.insert(make_pair(DREF_YokeHeading, XPLMFindDataRef("sim/joystick/yoke_heading_ratio")));

		drefs.insert(make_pair(DREF_Elevator, XPLMFindDataRef("sim/cockpit2/controls/yoke_pitch_ratio")));
		drefs.insert(make_pair(DREF_Aileron, XPLMFindDataRef("sim/cockpit2/controls/yoke_roll_ratio")));
		drefs.insert(make_pair(DREF_Rudder, XPLMFindDataRef("sim/cockpit2/controls/yoke_heading_ratio")));

		drefs.insert(make_pair(DREF_FlapSetting, XPLMFindDataRef("sim/flightmodel/controls/flaprqst")));
		drefs.insert(make_pair(DREF_FlapActual, XPLMFindDataRef("sim/flightmodel/controls/flaprat")));

		drefs.insert(make_pair(DREF_GearDeploy, XPLMFindDataRef("sim/aircraft/parts/acf_gear_deploy")));
		drefs.insert(make_pair(DREF_GearHandle, XPLMFindDataRef("sim/cockpit/switches/gear_handle_status")));
		drefs.insert(make_pair(DREF_BrakeParking, XPLMFindDataRef("sim/flightmodel/controls/parkbrakel")));
		drefs.insert(make_pair(DREF_BrakeLeft, XPLMFindDataRef("sim/cockpit2/controls/left_brake_ratio")));
		drefs.insert(make_pair(DREF_BrakeRight, XPLMFindDataRef("sim/cockpit2/controls/right_brake_ratio")));

		drefs.insert(make_pair(DREF_M, XPLMFindDataRef("sim/flightmodel/position/M")));
		drefs.insert(make_pair(DREF_L, XPLMFindDataRef("sim/flightmodel/position/L")));
		drefs.insert(make_pair(DREF_N, XPLMFindDataRef("sim/flightmodel/position/N")));

		drefs.insert(make_pair(DREF_QRad, XPLMFindDataRef("sim/flightmodel/position/Qrad")));
		drefs.insert(make_pair(DREF_PRad, XPLMFindDataRef("sim/flightmodel/position/Prad")));
		drefs.insert(make_pair(DREF_RRad, XPLMFindDataRef("sim/flightmodel/position/Rrad")));
		drefs.insert(make_pair(DREF_Q, XPLMFindDataRef("sim/flightmodel/position/Q")));
		drefs.insert(make_pair(DREF_P, XPLMFindDataRef("sim/flightmodel/position/P")));
		drefs.insert(make_pair(DREF_R, XPLMFindDataRef("sim/flightmodel/position/R")));

		drefs.insert(make_pair(DREF_Pitch, XPLMFindDataRef("sim/flightmodel/position/theta")));
		drefs.insert(make_pair(DREF_Roll, XPLMFindDataRef("sim/flightmodel/position/phi")));
		drefs.insert(make_pair(DREF_HeadingTrue, XPLMFindDataRef("sim/flightmodel/position/psi")));
		drefs.insert(make_pair(DREF_HeadingMag, XPLMFindDataRef("sim/flightmodel/position/magpsi")));
		drefs.insert(make_pair(DREF_Quaternion, XPLMFindDataRef("sim/flightmodel/position/q")));

		drefs.insert(make_pair(DREF_AngleOfAttack, XPLMFindDataRef("sim/flightmodel/position/alpha")));
		drefs.insert(make_pair(DREF_Sideslip, XPLMFindDataRef("sim/cockpit2/gauges/indicators/sideslip_degrees")));
		drefs.insert(make_pair(DREF_HPath, XPLMFindDataRef("sim/flightmodel/position/hpath")));
		drefs.insert(make_pair(DREF_VPath, XPLMFindDataRef("sim/flightmodel/position/vpath")));

		drefs.insert(make_pair(DREF_VPath, XPLMFindDataRef("sim/flightmodel/position/magnetic_variation")));

		drefs.insert(make_pair(DREF_Latitude, XPLMFindDataRef("sim/flightmodel/position/latitude")));
		drefs.insert(make_pair(DREF_Longitude, XPLMFindDataRef("sim/flightmodel/position/longitude")));
		drefs.insert(make_pair(DREF_AGL, XPLMFindDataRef("sim/flightmodel/position/y_agl")));
        drefs.insert(make_pair(DREF_Elevation, XPLMFindDataRef("sim/flightmodel/position/elevation")));

		drefs.insert(make_pair(DREF_LocalX, XPLMFindDataRef("sim/flightmodel/position/local_x")));
		drefs.insert(make_pair(DREF_LocalY, XPLMFindDataRef("sim/flightmodel/position/local_y")));
		drefs.insert(make_pair(DREF_LocalZ, XPLMFindDataRef("sim/flightmodel/position/local_z")));
		drefs.insert(make_pair(DREF_LocalVX, XPLMFindDataRef("sim/flightmodel/position/local_vx")));
		drefs.insert(make_pair(DREF_LocalVY, XPLMFindDataRef("sim/flightmodel/position/local_vy")));
		drefs.insert(make_pair(DREF_LocalVZ, XPLMFindDataRef("sim/flightmodel/position/local_vz")));

		drefs.insert(make_pair(DREF_ThrottleSet, XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro")));
		drefs.insert(make_pair(DREF_ThrottleActual, XPLMFindDataRef("sim/flightmodel2/engines/throttle_used_ratio")));

		drefs.insert(make_pair(DREF_MP1Lat, XPLMFindDataRef("sim/multiplayer/position/plane1_lat")));
		drefs.insert(make_pair(DREF_MP2Lat, XPLMFindDataRef("sim/multiplayer/position/plane2_lat")));
		drefs.insert(make_pair(DREF_MP3Lat, XPLMFindDataRef("sim/multiplayer/position/plane3_lat")));
		drefs.insert(make_pair(DREF_MP4Lat, XPLMFindDataRef("sim/multiplayer/position/plane4_lat")));
		drefs.insert(make_pair(DREF_MP5Lat, XPLMFindDataRef("sim/multiplayer/position/plane5_lat")));
		drefs.insert(make_pair(DREF_MP6Lat, XPLMFindDataRef("sim/multiplayer/position/plane6_lat")));
		drefs.insert(make_pair(DREF_MP7Lat, XPLMFindDataRef("sim/multiplayer/position/plane7_lat")));

		drefs.insert(make_pair(DREF_MP1Lon, XPLMFindDataRef("sim/multiplayer/position/plane1_lon")));
		drefs.insert(make_pair(DREF_MP2Lon, XPLMFindDataRef("sim/multiplayer/position/plane2_lon")));
		drefs.insert(make_pair(DREF_MP3Lon, XPLMFindDataRef("sim/multiplayer/position/plane3_lon")));
		drefs.insert(make_pair(DREF_MP4Lon, XPLMFindDataRef("sim/multiplayer/position/plane4_lon")));
		drefs.insert(make_pair(DREF_MP5Lon, XPLMFindDataRef("sim/multiplayer/position/plane5_lon")));
		drefs.insert(make_pair(DREF_MP6Lon, XPLMFindDataRef("sim/multiplayer/position/plane6_lon")));
		drefs.insert(make_pair(DREF_MP7Lon, XPLMFindDataRef("sim/multiplayer/position/plane7_lon")));

		drefs.insert(make_pair(DREF_MP1Alt, XPLMFindDataRef("sim/multiplayer/position/plane1_el")));
		drefs.insert(make_pair(DREF_MP2Alt, XPLMFindDataRef("sim/multiplayer/position/plane2_el")));
		drefs.insert(make_pair(DREF_MP3Alt, XPLMFindDataRef("sim/multiplayer/position/plane3_el")));
		drefs.insert(make_pair(DREF_MP4Alt, XPLMFindDataRef("sim/multiplayer/position/plane4_el")));
		drefs.insert(make_pair(DREF_MP5Alt, XPLMFindDataRef("sim/multiplayer/position/plane5_el")));
		drefs.insert(make_pair(DREF_MP6Alt, XPLMFindDataRef("sim/multiplayer/position/plane6_el")));
		drefs.insert(make_pair(DREF_MP7Alt, XPLMFindDataRef("sim/multiplayer/position/plane7_el")));

		char multi[256];
		for (int i = 1; i < 20; i++)
		{
			sprintf(multi, "sim/multiplayer/position/plane%i_x", i);
			mdrefs[i][DREF_LocalX] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_y", i);
			mdrefs[i][DREF_LocalY] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_z", i);
			mdrefs[i][DREF_LocalZ] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_lat", i);
			mdrefs[i][DREF_Latitude] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_lon", i);
			mdrefs[i][DREF_Longitude] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_el", i);
			mdrefs[i][DREF_Elevation] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_the", i);
			mdrefs[i][DREF_Pitch] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_phi", i);
			mdrefs[i][DREF_Roll] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_psi", i);
			mdrefs[i][DREF_HeadingTrue] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_gear_deploy", i);
			mdrefs[i][DREF_GearDeploy] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_flap_ratio", i);
			mdrefs[i][DREF_FlapSetting] = XPLMFindDataRef(multi); // Can't set the actual flap setting on npc aircraft
			mdrefs[i][DREF_FlapActual] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_flap_ratio2", i);
			mdrefs[i][DREF_FlapActual2] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_spoiler_ratio", i);
			mdrefs[i][DREF_Spoiler] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_speedbrake_ratio", i);
			mdrefs[i][DREF_BrakeSpeed] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_slat_ratio", i);
			mdrefs[i][DREF_Slats] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_wing_sweep", i);
			mdrefs[i][DREF_Sweep] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_throttle", i);
			mdrefs[i][DREF_ThrottleActual] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_yolk_pitch", i);
			mdrefs[i][DREF_YokePitch] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_yolk_roll", i);
			mdrefs[i][DREF_YokeRoll] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_yolk_yaw", i);
			mdrefs[i][DREF_YokeHeading] = XPLMFindDataRef(multi);
		}

		// Row 0: Frame Rates
		// Row 1: Times
		XPData[1][1] = DREF_TotalRuntime;
		XPData[1][2] = DREF_TotalFlighttime;
		XPData[1][3] = DREF_TimerElapsedtime;
		// Row 2: Sim stats
		// Row 3: Velocities
		XPData[3][0] = DREF_IndicatedAirspeed;
		XPData[3][2] = DREF_TrueAirspeed;
		XPData[3][4] = DREF_GroundSpeed;
		// Row 4: Mach, VVI, G-Loads
		XPData[4][0] = DREF_MachNumber;
		XPData[4][4] = DREF_GForceNormal;
		XPData[4][5] = DREF_GForceAxial;
		XPData[4][6] = DREF_GForceSide;
		// Row 5: Atmosphere: Weather
		XPData[5][0] = DREF_BarometerSealevelInHg;
		XPData[5][1] = DREF_TemperaturSealevelC;
		XPData[5][3] = DREF_WindSpeedKts;
		// Row 6: Atmosphere: Aircraft
		// Row 7: System Pressures
		// Row 8: Joystick
		XPData[8][0] = DREF_YokePitch;
		XPData[8][1] = DREF_YokeRoll;
		XPData[8][2] = DREF_YokeHeading;
		// Row 9: Other Flight Controls
		// Row 10: Art stab ail/elv/rud
		// Row 11: Control Surfaces
		XPData[11][0] = DREF_Elevator;
		XPData[11][1] = DREF_Aileron;
		XPData[11][2] = DREF_Rudder;
		// Row 12: Wing Sweep/Trust Vec
		// Row 13: trip/flap/slat/s-brakes
		XPData[13][3] = DREF_FlapSetting;
		XPData[13][4] = DREF_FlapActual;
		// Row 14: Gear, Brakes
		XPData[14][0] = DREF_GearDeploy;
		XPData[14][1] = DREF_BrakeParking;
		XPData[14][2] = DREF_BrakeLeft;
		XPData[14][3] = DREF_BrakeRight;
		XPData[14][7] = DREF_GearHandle;
		// Row 15: MNR (Angular Moments)
		XPData[15][0] = DREF_M;
		XPData[15][1] = DREF_L;
		XPData[15][2] = DREF_N;
		// Row 16: PQR (Angular Velocities)
		XPData[16][0] = DREF_QRad;
		XPData[16][1] = DREF_PRad;
		XPData[16][2] = DREF_RRad;
		XPData[16][3] = DREF_Q;
		XPData[16][4] = DREF_P;
		XPData[16][5] = DREF_R;
		// Row 17: Orientation: pitch, roll, yaw, heading
		XPData[17][0] = DREF_Pitch;
		XPData[17][1] = DREF_Roll;
		XPData[17][2] = DREF_HeadingTrue;
		XPData[17][3] = DREF_HeadingMag;
		XPData[17][4] = DREF_Quaternion;
		// Row 18: Orientation: alpha beta hpath vpath slip
		XPData[18][0] = DREF_AngleOfAttack;
		XPData[18][1] = DREF_Sideslip;
		XPData[18][2] = DREF_HPath;
		XPData[18][3] = DREF_VPath;
		XPData[18][4] = DREF_Sideslip;
		// Row 19: Mag Compass
		XPData[19][0] = DREF_HeadingMag;
		XPData[19][1] = DREF_MagneticVariation;
		// Row 20: Global Position
		XPData[20][0] = DREF_Latitude;
		XPData[20][1] = DREF_Longitude;
		XPData[20][2] = DREF_Elevation;
		XPData[20][3] = DREF_AGL;
		// Row 21: Local Position, Velocity
		XPData[21][0] = DREF_LocalX;
		XPData[21][1] = DREF_LocalY;
		XPData[21][2] = DREF_LocalZ;
		XPData[21][3] = DREF_LocalVX;
		XPData[21][4] = DREF_LocalVY;
		XPData[21][5] = DREF_LocalVZ;
		// Row 22: All Planes: Lat
		XPData[22][0] = DREF_Latitude;
		XPData[22][1] = DREF_MP1Lat;
		XPData[22][2] = DREF_MP2Lat;
		XPData[22][3] = DREF_MP3Lat;
		XPData[22][4] = DREF_MP4Lat;
		XPData[22][5] = DREF_MP5Lat;
		XPData[22][6] = DREF_MP6Lat;
		XPData[22][7] = DREF_MP7Lat;
		// Row 23: All Planes: Lon
		XPData[23][0] = DREF_Longitude;
		XPData[23][1] = DREF_MP1Lon;
		XPData[23][2] = DREF_MP2Lon;
		XPData[23][3] = DREF_MP3Lon;
		XPData[23][4] = DREF_MP4Lon;
		XPData[23][5] = DREF_MP5Lon;
		XPData[23][6] = DREF_MP6Lon;
		XPData[23][7] = DREF_MP7Lon;
		// Row 22: All Planes: Alt
		XPData[24][0] = DREF_AGL;
		XPData[24][1] = DREF_MP1Alt;
		XPData[24][2] = DREF_MP2Alt;
		XPData[24][3] = DREF_MP3Alt;
		XPData[24][4] = DREF_MP4Alt;
		XPData[24][5] = DREF_MP5Alt;
		XPData[24][6] = DREF_MP6Alt;
		XPData[24][7] = DREF_MP7Alt;
		// Row 25: Throttle Command
		XPData[25][0] = DREF_ThrottleSet;
		// Row 26: Throttle Actual
		XPData[26][0] = DREF_ThrottleActual;
	}

	int DataManager::Get(string dref, float values[], int size)
	{
		XPLMDataRef& xdref = sdrefs[dref];
		if (xdref == NULL)
		{
			xdref = XPLMFindDataRef(dref.c_str());
		}
		if (!xdref) // DREF does not exist
		{
#if LOG_VERBOSITY > 0
			Log::FormatLine("[DMAN] ERROR: invalid DREF %s", dref.c_str());
#endif
			return 0;
		}

		XPLMDataTypeID dataType = XPLMGetDataRefTypes(xdref);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %s (x:%X) Type: %i", dref.c_str(), xdref, dataType);
#endif
		// XPLMDataTypeID is a bit flag, so it may contain more than one of the
		// following types. We prefer types as close to float as possible.
		if ((dataType & 2) == 2) // Float
		{
			values[0] = XPLMGetDataf(xdref);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value was %f", values[0]);
#endif
			return 1;
		}
		if ((dataType & 8) == 8) // Float array
		{
			int drefSize = XPLMGetDatavf(xdref, NULL, 0, 0);
			if (drefSize > size)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: dref size is larger than available space");
				Log::FormatLine("             Actual dref size: %i, Available size: %i", drefSize, size);
#endif
				drefSize = size;
			}
			XPLMGetDatavf(xdref, values, 0, drefSize);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value count was %i", drefSize);
#endif
			return drefSize;
		}
		if ((dataType & 4) == 4) // Double
		{
			values[0] = (float)XPLMGetDatad(xdref);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value was %f", values[0]);
#endif
			return 1;
		}
		if ((dataType & 1) == 1) // Integer
		{
			values[0] = (float)XPLMGetDatai(xdref);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value was %i", (int)values[0]);
#endif
			return 1;
		}
		if ((dataType & 16) == 16) // Integer array
		{
			int iValues[200];
			int drefSize = XPLMGetDatavi(xdref, NULL, 0, 0);
			if (drefSize > size)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: dref size is larger than available space");
				Log::FormatLine("             Actual dref size: %i, Available size: %i", drefSize, size);
#endif
				drefSize = size;
			}
			if (drefSize > 200)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: dref size is larger than temp buffer");
				Log::FormatLine("             Actual dref size: %i, Temp buffer size: 200", drefSize);
#endif
				drefSize = 200;
			}
			XPLMGetDatavi(xdref, iValues, 0, drefSize);
			for (int i = 0; i < drefSize; ++i)
			{
				values[i] = (float)iValues[i];
			}
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value count was %i", drefSize);
#endif
			return drefSize;
		}
		if ((dataType & 32) == 32) // Byte array
		{
			char bValues[1024];
			int drefSize = XPLMGetDatab(xdref, NULL, 0, 0);
			if (drefSize > size)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: dref size is larger than available space");
				Log::FormatLine("             Actual dref size: %i, Available size: %i", drefSize, size);
#endif
				drefSize = size;
			}
			if (drefSize > 1024)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: dref size is larger than temp buffer");
				Log::FormatLine("             Actual dref size: %i, Temp buffer size: 1024", drefSize);
#endif
				drefSize = 1024;
			}
			XPLMGetDatab(xdref, bValues, 0, drefSize);
			for (int i = 0; i < drefSize; ++i)
			{
				values[i] = (float)bValues[i];
			}
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value count was %i", drefSize);
#endif
			return drefSize;
		}

		// No match
#if LOG_VERBOSITY > 0
		Log::WriteLine("[DMAN] ERROR: Unrecognized data type.");
#endif
		return 0;
	}

	double DataManager::GetDouble(DREF dref, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
		double value = XPLMGetDatad(xdref);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %i (x:%X) result %f for a/c %i", dref, xdref, value, aircraft);
#endif
		return value;
	}

	float DataManager::GetFloat(DREF dref, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
		float value = XPLMGetDataf(xdref);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %i (x:%X) result %f for a/c %i", dref, xdref, value, aircraft);
#endif
		return value;
	}

	int DataManager::GetInt(DREF dref, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
		int value = XPLMGetDatai(xdref);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %i (x:%X) result %i for a/c %i", dref, xdref, value, aircraft);
#endif
		return value;
	}

	int DataManager::GetFloatArray(DREF dref, float values[], int size, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
		int resultSize = XPLMGetDatavf(xdref, values, 0, size);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %i (x:%X) result size %i for a/c %i", dref, xdref, resultSize, aircraft);
#endif
		return resultSize;
	}

	int DataManager::GetIntArray(DREF dref, int values[], int size, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
		int resultSize = XPLMGetDatavi(xdref, values, 0, size);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %i (x:%X) result size %i for a/c %i", dref, xdref, resultSize, aircraft);
#endif
		return resultSize;
	}

	void DataManager::Set(DREF dref, double value, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Setting DREF %i (x:%X) to %f for a/c %i", dref, xdref, value, aircraft);
#endif
		XPLMSetDatad(xdref, value);
	}

	void DataManager::Set(DREF dref, float value, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Set DREF %i (x:%X) to %f for a/c %i", dref, xdref, value, aircraft);
#endif
		XPLMSetDataf(xdref, value);
	}

	void DataManager::Set(DREF dref, int value, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Set DREF %i (x:%X) to %i for a/c %i", dref, xdref, value, aircraft);
#endif
		XPLMSetDatai(xdref, value);
	}

	void DataManager::Set(DREF dref, float values[], int size, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Set DREF %i (x:%X) (%i values) for a/c %i", dref, xdref, size, aircraft);
#endif
		int drefSize = XPLMGetDatavf(xdref, NULL, 0, 0);
		drefSize = min(drefSize, size);
		XPLMSetDatavf(xdref, values, 0, drefSize);
	}
	
	void DataManager::Set(DREF dref, int values[], int size, char aircraft)
	{
		const XPLMDataRef& xdref = aircraft == 0 ? drefs[dref] : mdrefs[aircraft][dref];
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Setting DREF %i (x:%X) (%i values) for a/c %i", dref, xdref, size, aircraft);
#endif
		int drefSize = XPLMGetDatavi(xdref, NULL, 0, 0);
		drefSize = min(drefSize, size);
		XPLMSetDatavi(xdref, values, 0, drefSize);
	}

	void DataManager::Set(string dref, float values[], int size)
	{
		XPLMDataRef& xdref = sdrefs[dref];
		if (xdref == NULL)
		{
			xdref = XPLMFindDataRef(dref.c_str());
		}
		if (!xdref)
		{
			// DREF does not exist
#if LOG_VERBOSITY > 0
			Log::FormatLine("[DMAN] ERROR: invalid DREF %s", dref.c_str());
#endif
			return;
		}
		if (isnan(values[0]))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Value must be a number (NaN received)");
#endif
			return;
		}

		XPLMDataTypeID dataType = XPLMGetDataRefTypes(xdref);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Setting DREF %s (x:%X) Type: %i", dref.c_str(), xdref, dataType);
#endif
		if ((dataType & 2) == 2) // Float
		{
			XPLMSetDataf(xdref, values[0]);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value was %f", values[0]);
#endif
		}
		else if ((dataType & 8) == 8) // Float Array
		{
			int drefSize = XPLMGetDatavf(xdref, NULL, 0, 0);
#if LOG_VERBOSITY > 1
			if (size > drefSize)
			{
				Log::WriteLine("[DMAN] WARN: Provided size is larger than actual dref size");
				Log::FormatLine("             Actual dref size: %i, Provided buffer size: %i", drefSize, size);
			}
#endif
			drefSize = min(drefSize, size);
			XPLMSetDatavf(xdref, values, 0, drefSize);
#if LOG_VERBOSITY > 4
				Log::FormatLine("[DMAN] -- value count was %i", drefSize);
#endif
		}
		else if ((dataType & 4) == 4) // Double
		{
			XPLMSetDatad(xdref, values[0]);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value was %f", values[0]);
#endif
		}
		else if ((dataType & 1) == 1) // Integer
		{
			XPLMSetDatai(xdref, (int)values[0]);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value was %i", (int)values[0]);
#endif
		}
		else if ((dataType & 16) == 16) // Integer Array
		{
			int iValues[200];
			int drefSize = XPLMGetDatavi(xdref, NULL, 0, 0);
#if LOG_VERBOSITY > 1
			if (size > drefSize)
			{
				Log::WriteLine("[DMAN] WARN: Provided size is larger than actual dref size");
				Log::FormatLine("             Actual dref size: %i, Provided buffer size: %i", drefSize, size);
			}
#endif
			drefSize = min(drefSize, size);
			if (drefSize > 200)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: drefSize larger than temp buffer size.");
				Log::FormatLine("             Actual dref size: %i, Temp buffer size: 200", drefSize);
#endif
				drefSize = 200;
			}
			for (int i = 0; i < drefSize; ++i)
			{
				iValues[i] = (int)values[i];
			}
			XPLMSetDatavi(xdref, iValues, 0, drefSize);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value count was %i", drefSize);
#endif
		}
		else if ((dataType & 32) == 32) // Byte Array
		{
			char bValues[1024];
			int drefSize = XPLMGetDatab(xdref, NULL, 0, 0);
#if LOG_VERBOSITY > 1
			if (size > drefSize)
			{
				Log::WriteLine("[DMAN] WARN: Provided size is larger than actual dref size");
				Log::FormatLine("             Actual dref size: %i, Provided buffer size: %i", drefSize, size);
			}
#endif
			drefSize = min(drefSize, size);
			if (drefSize > 1024)
			{
#if LOG_VERBOSITY > 1
				Log::WriteLine("[DMAN] WARN: drefSize larger than temp buffer size.");
				Log::FormatLine("             Actual dref size: %i, Temp buffer size: 1024", drefSize);
#endif
				drefSize = 1024;
			}
			for (int i = 0; i < drefSize; ++i)
			{
				bValues[i] = (char)values[i];
			}
			XPLMSetDatab(xdref, bValues, 0, drefSize);
#if LOG_VERBOSITY > 4
			Log::FormatLine("[DMAN] -- value count was %i", drefSize);
#endif
		}
		else
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Unknown type.");
#endif
		}


#if LOG_VERBOSITY > 1
		if (!XPLMCanWriteDataRef(xdref))
		{
			Log::WriteLine("[DMAN] WARN: dref is not writable. The write operation probably failed.");
		}
#endif
	}

	void DataManager::SetGear(float gear, bool immediate, char aircraft)
	{
#if LOG_VERBOSITY > 3
		Log::FormatLine("[DMAN] Setting gear (value:%f, immediate:%i) for aircraft %i", gear, immediate, aircraft);
#endif
		if (isnan(gear) || gear < 0 || gear > 1)
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[GEAR] ERROR: Value must be 0 or 1");
#endif
			return;
		}

		if ((gear < -8.5 && gear > -9.5) || (gear < -997.9 && gear > -999.1))
		{
			return;
		}

		float gearArray[10];
		for (int i = 0; i < 10; i++)
		{
			gearArray[i] = gear;
		}

		if (aircraft == 0) // Own aircraft
		{
			Set(DREF_GearHandle, (int)gear);
			if (immediate)
			{
				Set(DREF_GearDeploy, gearArray, 10);
			}
		}
		else // Multiplayer aircraft
		{
			Set(DREF_GearDeploy, gearArray, 10, aircraft);
		}
	}

	void DataManager::SetPosition(float pos[3], char aircraft)
	{
#if LOG_VERBOSITY > 3
		Log::FormatLine("[DMAN] Setting position (%f, %f, %f) for aircraft %i", pos[0], pos[1], pos[2], aircraft);
#endif
		if (isnan(pos[0] + pos[1] + pos[2]))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Position must be a number (NaN received)");
#endif
			return;
		}

		if (pos[0] < -997.9 && pos[0] > -999.1)
		{
			pos[0] = (float)GetDouble(DREF_Latitude, aircraft);
		}
		if (pos[1] < -997.9 && pos[1] > -999.1)
		{
			pos[1] = (float)GetDouble(DREF_Longitude, aircraft);
		}
		if (pos[2] < -997.9 && pos[2] > -999.1)
		{
			pos[2] = (float)GetDouble(DREF_Elevation, aircraft);
		}

		double local[3];
		XPLMWorldToLocal(pos[0], pos[1], pos[2], &local[0], &local[1], &local[2]);
		// If the sim is paused, setting global position won't update the
		// local position, so set them just in case.
		Set(DREF_LocalX, local[0], aircraft);
		Set(DREF_LocalY, local[1], aircraft);
		Set(DREF_LocalZ, local[2], aircraft);
		// If the sim is unpaused, this will override the above settings.
		// TODO: Are these setable when paused? Are these necessary?
		Set(DREF_Latitude, (double)pos[0], aircraft);
		Set(DREF_Longitude, (double)pos[1], aircraft);
		Set(DREF_Elevation, (double)pos[2], aircraft);
	}

	void DataManager::SetOrientation(float orient[3], char aircraft)
	{
#if LOG_VERBOSITY > 3
		Log::FormatLine("[DMAN] Setting orientation (%f, %f, %f) for aircraft %i",
			orient[0], orient[1], orient[2], aircraft);
#endif
		if (isnan(orient[0] + orient[1] + orient[2]))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Orientation must be a number (NaN received)");
#endif
			return;
		}

		if (orient[0] < -997.9 && orient[0] > -999.1)
		{
			orient[0] = GetFloat(DREF_Pitch, aircraft);
		}
		if (orient[1] < -997.9 && orient[1] > -999.1)
		{
			orient[1] = GetFloat(DREF_Roll, aircraft);
		}
		if (orient[2] < -997.9 && orient[2] > -999.1)
		{
			orient[2] = GetFloat(DREF_HeadingTrue, aircraft);
		}


		// If the sim is paused, setting the quaternion won't update these values,
		// so we set them here just in case. This also covers multiplayer aircraft,
		// which we can't set the quaternion on.
		Set(DREF_Pitch, orient[0], aircraft);
		Set(DREF_Roll, orient[1], aircraft);
		Set(DREF_HeadingTrue, orient[2], aircraft);
		if (aircraft == 0) // Player aircraft
		{
			// Convert to Quaternions
			// from: http://www.xsquawkbox.net/xpsdk/mediawiki/MovingThePlane
			//	     http://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290.pdf
			float q[4];
			float halfRad = 0.00872664625997F;
			orient[2] = halfRad * orient[2];
			orient[0] = halfRad * orient[0];
			orient[1] = halfRad * orient[1];
			q[0] = cos(orient[2]) * cos(orient[0]) * cos(orient[1]) + sin(orient[2]) * sin(orient[0]) * sin(orient[1]);
			q[1] = cos(orient[2]) * cos(orient[0]) * sin(orient[1]) - sin(orient[2]) * sin(orient[0]) * cos(orient[1]);
			q[2] = cos(orient[2]) * sin(orient[0]) * cos(orient[1]) + sin(orient[2]) * cos(orient[0]) * sin(orient[1]);
			q[3] = sin(orient[2]) * cos(orient[0]) * cos(orient[1]) - cos(orient[2]) * sin(orient[0]) * sin(orient[1]);

			// If the sim is un-paused, this will overwrite the pitch/roll/yaw
			// values set above.
			Set(DREF_Quaternion, q, 4);
		}
	}

	void DataManager::SetFlaps(float value)
	{
		if (isnan(value))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Flap value must be a number (NaN received)");
#endif
			return;
		}
		if (value < -997.9 && value > -999.1)
		{
			return;
		}

		value = (float)fmaxl(value, 0);
		value = (float)fminl(value, 1);

		Set(DREF_FlapSetting, value);
		Set(DREF_FlapActual, value);
	}
}