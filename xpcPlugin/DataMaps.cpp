//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#include "DataManager.h"

namespace XPC
{
	DREF XPData[134][8]
	{
		{}, // Row 0: Frame Rates
		{ // Row 1: Times
			DREF::None, DREF::TotalRuntime, DREF::TotalFlighttime, DREF::TimerElapsedtime,
		},
		{}, // Row 2: Sim stats
		{ // Row 3: Velocities
			DREF::IndicatedAirspeed, DREF::None, DREF::TrueAirspeed, DREF::GroundSpeed
		},
		{ // Row 4: Mach, VVI, G-Loads
			DREF::MachNumber, DREF::None, DREF::None, DREF::None,
			DREF::GForceNormal, DREF::GForceAxial, DREF::GForceSide
		},
		{ // Row 5: Atmosphere: Weather
			DREF::BarometerSealevelInHg, DREF::TemperaturSealevelC, DREF::None, DREF::WindSpeedKts
		},
		{}, // Row 6: Atmosphere: Aircraft
		{}, // Row 7: System Pressures
		{ // Row 8: Joystick
			DREF::YokePitch, DREF::YokeRoll, DREF::YokeHeading
		},
		{}, // Row 9: Other Flight Controls
		{}, // Row 10: Art stab ail/elv/rud
		{ // Row 11: Control Surfaces
			DREF::Elevator, DREF::Aileron, DREF::Rudder
		},
		{}, // Row 12: Wing Sweep/Trust Vec
		{ // Row 13: trip/flap/slat/s-brakes
			DREF::None, DREF::None, DREF::None, DREF::FlapSetting, DREF::FlapActual
		},
		{ // Row 14: Gear, Braks
			DREF::GearDeploy, DREF::BrakeParking, DREF::BrakeLeft, DREF::BrakeRight,
			DREF::None, DREF::None, DREF::None,
			DREF::GearHandle
		},
		{ // Row 15: MNR (Angular Moments)
			DREF::M, DREF::L, DREF::N
		},
		{ // Row 16: PQR (Angular Velocities)
			DREF::QRad, DREF::PRad, DREF::RRad, DREF::Q, DREF::P, DREF::R
		},
		{ // Row 17: Orientation: pitch, roll, yaw, heading
			DREF::Pitch, DREF::Roll, DREF::HeadingTrue, DREF::HeadingMag, DREF::Quaternion
		},
		{ // Row 18: Orientation: alpha beta hpath vpath slip
			DREF::AngleOfAttack, DREF::Sideslip, DREF::HPath, DREF::VPath, DREF::Sideslip
		},
		{ // Row 19: Mag Compass
			DREF::HeadingMag, DREF::MagneticVariation
		},
		{ // Row 20: Global Position
			DREF::Latitude, DREF::Longitude, DREF::AGL
		},
		{ // Row 21: Local Position, Velocity
			DREF::LocalX, DREF::LocalY, DREF::LocalZ, DREF::LocalVX, DREF::LocalVY, DREF::LocalVZ
		},
		{ // Row 22: All Planes: Lat
			DREF::Latitude, DREF::MP1Lat, DREF::MP2Lat, DREF::MP3Lat, DREF::MP4Lat, DREF::MP5Lat, DREF::MP6Lat, DREF::MP7Lat
		},
		{ // Row 23: All Planes: Lon
			DREF::Longitude, DREF::MP1Lon, DREF::MP2Lon, DREF::MP3Lon, DREF::MP4Lon, DREF::MP5Lon, DREF::MP6Lon, DREF::MP7Lon
		},
		{ // Row 24: All Planes: Alt
			DREF::AGL, DREF::MP1Alt, DREF::MP2Alt, DREF::MP3Alt, DREF::MP4Alt, DREF::MP5Alt, DREF::MP6Alt, DREF::MP7Alt
		},
		{ // Row 25: Throttle Command
			DREF::ThrottleSet
		},
		{ // Row 26: Throttle Actual
			DREF::ThrottleActual
		}
	};
}