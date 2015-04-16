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
#include "Log.h"

#include "XPLMDataAccess.h"
#include "XPLMGraphics.h"

#include <cmath>
#include <unordered_map>

using namespace std;
namespace XPC
{
	static std::unordered_map<DREF, XPLMDataRef> drefs;
	static std::unordered_map<DREF, XPLMDataRef> mdrefs[20];
	static std::unordered_map<std::string, XPLMDataRef> sdrefs;

	void DataManager::Initialize()
	{
		drefs = std::unordered_map<DREF, XPLMDataRef>(
		{
			{ DREF::None, XPLMFindDataRef("sim/test/test_float") },

			{ DREF::Pause, XPLMFindDataRef("sim/operation/override/override_planepath") },
			{ DREF::PauseAI, XPLMFindDataRef("sim/operation/override/override_plane_ai_autopilot") },

			{ DREF::TotalRuntime, XPLMFindDataRef("sim/time/total_running_time_sec") },
			{ DREF::TotalFlighttime, XPLMFindDataRef("sim/time/total_flight_time_sec") },
			{ DREF::TimerElapsedtime, XPLMFindDataRef("sim/time/timer_elapsed_time_sec") },

			{ DREF::IndicatedAirspeed, XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed") },
			{ DREF::TrueAirspeed, XPLMFindDataRef("sim/flightmodel/position/true_airspeed") },
			{ DREF::GroundSpeed, XPLMFindDataRef("sim/flightmodel/position/groundspeed") },

			{ DREF::MachNumber, XPLMFindDataRef("sim/flightmodel/misc/machno") },
			{ DREF::GForceNormal, XPLMFindDataRef("sim/flightmodel2/misc/gforce_normal") },
			{ DREF::GForceAxial, XPLMFindDataRef("sim/flightmodel2/misc/gforce_axial") },
			{ DREF::GForceSide, XPLMFindDataRef("sim/flightmodel2/misc/gforce_side") },

			{ DREF::BarometerSealevelInHg, XPLMFindDataRef("sim/weather/barometer_sealevel_inhg") },
			{ DREF::TemperaturSealevelC, XPLMFindDataRef("sim/weather/temperature_sealevel_c") },
			{ DREF::WindSpeedKts, XPLMFindDataRef("sim/cockpit2/gauges/indicators/wind_speed_kts") },

			{ DREF::YokePitch, XPLMFindDataRef("sim/joystick/yoke_pitch_ratio") },
			{ DREF::YokeRoll, XPLMFindDataRef("sim/joystick/yoke_roll_ratio") },
			{ DREF::YokeHeading, XPLMFindDataRef("sim/joystick/yoke_heading_ratio") },

			{ DREF::Elevator, XPLMFindDataRef("sim/cockpit2/controls/yoke_pitch_ratio") },
			{ DREF::Aileron, XPLMFindDataRef("sim/cockpit2/controls/yoke_roll_ratio") },
			{ DREF::Rudder, XPLMFindDataRef("sim/cockpit2/controls/yoke_heading_ratio") },

			{ DREF::FlapSetting, XPLMFindDataRef("sim/flightmodel/controls/flaprqst") },
			{ DREF::FlapActual, XPLMFindDataRef("sim/flightmodel/controls/flaprat") },

			{ DREF::GearDeploy, XPLMFindDataRef("sim/aircraft/parts/acf_gear_deploy") },
			{ DREF::GearHandle, XPLMFindDataRef("sim/cockpit/switches/gear_handle_status") },
			{ DREF::BrakeParking, XPLMFindDataRef("sim/flightmodel/controls/parkbrakel") },
			{ DREF::BrakeLeft, XPLMFindDataRef("sim/cockpit2/controls/left_brake_ratio") },
			{ DREF::BrakeRight, XPLMFindDataRef("sim/cockpit2/controls/right_brake_ratio") },

			{ DREF::M, XPLMFindDataRef("sim/flightmodel/position/M") },
			{ DREF::L, XPLMFindDataRef("sim/flightmodel/position/L") },
			{ DREF::N, XPLMFindDataRef("sim/flightmodel/position/N") },

			{ DREF::QRad, XPLMFindDataRef("sim/flightmodel/position/Qrad") },
			{ DREF::PRad, XPLMFindDataRef("sim/flightmodel/position/Prad") },
			{ DREF::RRad, XPLMFindDataRef("sim/flightmodel/position/Rrad") },
			{ DREF::Q, XPLMFindDataRef("sim/flightmodel/position/Q") },
			{ DREF::P, XPLMFindDataRef("sim/flightmodel/position/P") },
			{ DREF::R, XPLMFindDataRef("sim/flightmodel/position/R") },

			{ DREF::Pitch, XPLMFindDataRef("sim/flightmodel/position/theta") },
			{ DREF::Roll, XPLMFindDataRef("sim/flightmodel/position/phi") },
			{ DREF::HeadingTrue, XPLMFindDataRef("sim/flightmodel/position/psi") },
			{ DREF::HeadingMag, XPLMFindDataRef("sim/flightmodel/position/magpsi") },
			{ DREF::Quaternion, XPLMFindDataRef("sim/flightmodel/position/q") },

			{ DREF::AngleOfAttack, XPLMFindDataRef("sim/flightmodel/position/alpha") },
			{ DREF::Sideslip, XPLMFindDataRef("sim/cockpit2/gauges/indicators/sideslip_degrees") },
			{ DREF::HPath, XPLMFindDataRef("sim/flightmodel/position/hpath") },
			{ DREF::VPath, XPLMFindDataRef("sim/flightmodel/position/vpath") },

			{ DREF::VPath, XPLMFindDataRef("sim/flightmodel/position/magnetic_variation") },

			{ DREF::Latitude, XPLMFindDataRef("sim/flightmodel/position/latitude") },
			{ DREF::Longitude, XPLMFindDataRef("sim/flightmodel/position/longitude") },
			{ DREF::AGL, XPLMFindDataRef("sim/flightmodel/position/y_agl") },

			{ DREF::LocalX, XPLMFindDataRef("sim/flightmodel/position/local_x") },
			{ DREF::LocalY, XPLMFindDataRef("sim/flightmodel/position/local_y") },
			{ DREF::LocalZ, XPLMFindDataRef("sim/flightmodel/position/local_z") },
			{ DREF::LocalVX, XPLMFindDataRef("sim/flightmodel/position/local_vx") },
			{ DREF::LocalVY, XPLMFindDataRef("sim/flightmodel/position/local_vy") },
			{ DREF::LocalVZ, XPLMFindDataRef("sim/flightmodel/position/local_vz") },

			{ DREF::ThrottleSet, XPLMFindDataRef("sim/flightmodel/engine/ENGN_thro") },
			{ DREF::ThrottleActual, XPLMFindDataRef("sim/flightmodel2/engines/throttle_used_ratio") },

			{ DREF::MP1Lat, XPLMFindDataRef("sim/multiplayer/position/plane1_lat") },
			{ DREF::MP2Lat, XPLMFindDataRef("sim/multiplayer/position/plane2_lat") },
			{ DREF::MP3Lat, XPLMFindDataRef("sim/multiplayer/position/plane3_lat") },
			{ DREF::MP4Lat, XPLMFindDataRef("sim/multiplayer/position/plane4_lat") },
			{ DREF::MP5Lat, XPLMFindDataRef("sim/multiplayer/position/plane5_lat") },
			{ DREF::MP6Lat, XPLMFindDataRef("sim/multiplayer/position/plane6_lat") },
			{ DREF::MP7Lat, XPLMFindDataRef("sim/multiplayer/position/plane7_lat") },

			{ DREF::MP1Lon, XPLMFindDataRef("sim/multiplayer/position/plane1_lon") },
			{ DREF::MP2Lon, XPLMFindDataRef("sim/multiplayer/position/plane2_lon") },
			{ DREF::MP3Lon, XPLMFindDataRef("sim/multiplayer/position/plane3_lon") },
			{ DREF::MP4Lon, XPLMFindDataRef("sim/multiplayer/position/plane4_lon") },
			{ DREF::MP5Lon, XPLMFindDataRef("sim/multiplayer/position/plane5_lon") },
			{ DREF::MP6Lon, XPLMFindDataRef("sim/multiplayer/position/plane6_lon") },
			{ DREF::MP7Lon, XPLMFindDataRef("sim/multiplayer/position/plane7_lon") },

			{ DREF::MP1Alt, XPLMFindDataRef("sim/multiplayer/position/plane1_el") },
			{ DREF::MP2Alt, XPLMFindDataRef("sim/multiplayer/position/plane2_el") },
			{ DREF::MP3Alt, XPLMFindDataRef("sim/multiplayer/position/plane3_el") },
			{ DREF::MP4Alt, XPLMFindDataRef("sim/multiplayer/position/plane4_el") },
			{ DREF::MP5Alt, XPLMFindDataRef("sim/multiplayer/position/plane5_el") },
			{ DREF::MP6Alt, XPLMFindDataRef("sim/multiplayer/position/plane6_el") },
			{ DREF::MP7Alt, XPLMFindDataRef("sim/multiplayer/position/plane7_el") },
		});

		char multi[256];
		for (int i = 1; i < 20; i++)
		{
			sprintf(multi, "sim/multiplayer/position/plane%i_x", i); // X
			mdrefs[i][DREF::LocalX] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_y", i); // Y
			mdrefs[i][DREF::LocalY] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_z", i); // Z
			mdrefs[i][DREF::LocalZ] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_the", i); // Theta (Pitch)
			mdrefs[i][DREF::Pitch] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_phi", i); // Phi (Roll)
			mdrefs[i][DREF::Roll] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_psi", i); // Psi (Heading-True)
			mdrefs[i][DREF::HeadingTrue] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_gear_deploy", i); // Landing Gear
			mdrefs[i][DREF::GearDeploy] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_flap_ratio", i);
			mdrefs[i][DREF::FlapSetting] = XPLMFindDataRef(multi); // Can't set the actual flap setting on npc aircraft
			mdrefs[i][DREF::FlapActual] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_flap_ratio2", i);
			mdrefs[i][DREF::FlapActual2] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_spoiler_ratio", i);
			mdrefs[i][DREF::Spoiler] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_speedbrake_ratio", i);
			mdrefs[i][DREF::BrakeSpeed] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_slat_ratio", i);
			mdrefs[i][DREF::Slats] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_wing_sweep", i);
			mdrefs[i][DREF::Sweep] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_throttle", i);
			mdrefs[i][DREF::ThrottleActual] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_yolk_pitch", i);
			mdrefs[i][DREF::YokePitch] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_yolk_roll", i);
			mdrefs[i][DREF::YokeRoll] = XPLMFindDataRef(multi);
			sprintf(multi, "sim/multiplayer/position/plane%i_yolk_yaw", i);
			mdrefs[i][DREF::YokeHeading] = XPLMFindDataRef(multi);
		}
	}

	int DataManager::Get(std::string dref, float values[], int size)
	{
		XPLMDataRef& xdref = sdrefs[dref];
		if (xdref == nullptr)
		{
			xdref = XPLMFindDataRef(dref.c_str());
		}
		if (!xdref)
		{
			// DREF does not exist
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: invalid DREF");
#endif
			return 0;
		}

		XPLMDataTypeID dataType = XPLMGetDataRefTypes(xdref);
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Get DREF %s (x:%X) type %i", dref.c_str(), xdref, dataType);
#endif
		switch (dataType)
		{
		case 1: // Integer
		{
			values[0] = (float)XPLMGetDatai(xdref);
#if LOG_VERBOSITY > 4
			Log::FormatLine("		value was %i", (int)values[0]);
#endif
			return 1;
		}
		case 4: // Double
		{
			values[0] = (float)XPLMGetDatad(xdref);
#if LOG_VERBOSITY > 4
			Log::FormatLine("		value was %f", values[0]);
#endif
			return 1;
		}
		case 8: // Float Array
		{
			int drefSize = XPLMGetDatavf(xdref, NULL, 0, 0);
			drefSize = min(drefSize, size);
			XPLMGetDatavf(xdref, values, 0, drefSize);
#if LOG_VERBOSITY > 4
			Log::FormatLine("		value count was %i", drefSize);
#endif
			return drefSize;
		}
		case 16: // Integer Array
		{
			int iValues[64];
			int drefSize = XPLMGetDatavi(xdref, NULL, 0, 0);
			drefSize = min(drefSize, size);
			drefSize = min(drefSize, 64);
			XPLMGetDatavi(xdref, iValues, 0, drefSize);
			for (int i = 0; i < drefSize; ++i)
			{
				values[i] = (float)iValues[i];
			}
#if LOG_VERBOSITY > 4
			Log::FormatLine("		value count was %i", drefSize);
#endif
			return drefSize;
		}
		default: // Assume float
		{
			values[0] = XPLMGetDataf(xdref);
#if LOG_VERBOSITY > 4
			Log::FormatLine("		value was %f", values[0]);
#endif
			return 1;
		}
		}
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

	void DataManager::Set(std::string dref, float values[], int size)
	{
		XPLMDataRef& xdref = sdrefs[dref];
		if (xdref == nullptr)
		{
			xdref = XPLMFindDataRef(dref.c_str());
		}
#if LOG_VERBOSITY > 4
		Log::FormatLine("[DMAN] Setting DREF %s (x:%X) (%i values)", dref.c_str(), xdref, size);
#endif
		if (!xdref)
		{
			// DREF does not exist
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: invalid DREF");
#endif
			return;
		}
		if (std::isnan(values[0]))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Value must be a number (NaN received)");
#endif
			return;
		}

		XPLMDataTypeID dataType = XPLMGetDataRefTypes(xdref);
		switch (dataType)
		{
		case 1: // Integer
		{
			XPLMSetDatai(xdref, (int)values[0]);
			break;
		}
		case 4: // Double
		{
			XPLMSetDatad(xdref, values[0]);
			break;
		}
		case 8: // Float Array
		{
			int drefSize = XPLMGetDatavf(xdref, NULL, 0, 0);
			drefSize = min(drefSize, size);
			XPLMSetDatavf(xdref, values, 0, drefSize);
			break;
		}
		case 16: // Integer Array
		{
			int iValues[64];
			int drefSize = XPLMGetDatavi(xdref, NULL, 0, 0);
			drefSize = min(drefSize, size);
			drefSize = min(drefSize, 64);
			for (int i = 0; i < drefSize; ++i)
			{
				iValues[i] = (int)values[i];
			}
			XPLMSetDatavi(xdref, iValues, 0, drefSize);
			break;
		}
		default: // Assume float
		{
			XPLMSetDataf(xdref, values[0]);
			break;
		}
		}
	}

	void DataManager::SetGear(float gear, bool immediate, char aircraft)
	{
#if LOG_VERBOSITY > 3
		Log::FormatLine("[DMAN] Setting gear (value:%f, immediate:%i) for aircraft %i", gear, immediate, aircraft);
#endif
		float gearArray[10];

		if ((gear < -8.5f && gear > 9.5f) || (gear < -997.9f && gear > -999.1f))
		{
			return;
		}
		if (std::isnan(gear) || gear < 0 || gear > 1)
		{
			Log::WriteLine("[GEAR] ERROR: Value must be 0 or 1");
			return;
		}

		for (int i = 0; i < 10; i++)
		{
			gearArray[i] = gear;
		}

		if (aircraft == 0) // Own aircraft
		{
			Set(DREF::GearHandle, (int)gear);
			if (immediate)
			{
				Set(DREF::GearDeploy, gearArray, 10);
			}
		}
		else // Multiplayer aircraft
		{
			XPLMSetDatavf(mdrefs[aircraft][DREF::GearDeploy], gearArray, 0, 10);
		}
	}

	void DataManager::SetPosition(float pos[3], char aircraft)
	{
#if LOG_VERBOSITY > 3
		Log::FormatLine("[DMAN] Setting position (%f, %f, %f) for aircraft %i", pos[0], pos[1], pos[2], aircraft);
#endif
		if (std::isnan(pos[0] + pos[1] + pos[2]))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Position must be a number (NaN received)");
#endif
			return;
		}

		double local[3] = { 0 };
		XPLMWorldToLocal(pos[0], pos[1], pos[2], &local[0], &local[1], &local[2]);
		if (aircraft == 0) // Player aircraft
		{
			// If the sim is paused, setting global position won't update the
			// local position, so set them just in case.
			Set(DREF::LocalX, local[0]);
			Set(DREF::LocalY, local[1]);
			Set(DREF::LocalZ, local[2]);
			// If the sim is unpaused, this will override the above settings.
			Set(DREF::Latitude, (double)pos[0]);
			Set(DREF::Longitude, (double)pos[1]);
			Set(DREF::AGL, (double)pos[2]);
		}
		else // Multiplayer
		{
			XPLMSetDatad(mdrefs[aircraft][DREF::LocalX], local[0]);
			XPLMSetDatad(mdrefs[aircraft][DREF::LocalY], local[1]);
			XPLMSetDatad(mdrefs[aircraft][DREF::LocalZ], local[2]);
		}
	}

	void DataManager::SetOrientation(float orient[3], char aircraft)
	{
#if LOG_VERBOSITY > 3
		Log::FormatLine("[DMAN] Setting orientation (%f, %f, %f) for aircraft %i",
			orient[0], orient[1], orient[2], aircraft);
#endif
		if (std::isnan(orient[0] + orient[1] + orient[2]))
		{
#if LOG_VERBOSITY > 0
			Log::WriteLine("[DMAN] ERROR: Orientation must be a number (NaN received)");
#endif
			return;
		}

		if (aircraft == 0) // Player aircraft
		{
			// If the sim is paused, setting the quaternion won't update these values,
			// so we set them here just in case.
			Set(DREF::Pitch, orient[0]);
			Set(DREF::Roll, orient[1]);
			Set(DREF::HeadingTrue, orient[2]);

			// Convert to Quaternions
			// from: http://www.xsquawkbox.net/xpsdk/mediawiki/MovingThePlane
			//	   http://ntrs.nasa.gov/archive/nasa/casi.ntrs.nasa.gov/19770024290.pdf
			float q[4] = { 0 };
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
			Set(DREF::Quaternion, q, 4);
		}
		else // Multiplayer
		{
			XPLMSetDataf(mdrefs[aircraft][DREF::Pitch], orient[0]);
			XPLMSetDataf(mdrefs[aircraft][DREF::Roll], orient[1]);
			XPLMSetDataf(mdrefs[aircraft][DREF::HeadingTrue], orient[2]);
		}
	}

	void DataManager::SetFlaps(float value)
	{
		if (std::isnan(value))
		{
			Log::WriteLine("[FLAP] ERROR: Value must be a number (NaN received)");
			return;
		}
		value = fmaxl(value, 0);
		value = fminl(value, 1);

		Set(DREF::FlapSetting, value);
		Set(DREF::FlapActual, value);
	}
}