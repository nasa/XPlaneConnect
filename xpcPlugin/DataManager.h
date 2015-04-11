//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_DATAMANAGER_H
#define XPC_DATAMANAGER_H

#include <cstdint>
#include <string>

namespace XPC
{
	enum class DREF
	{
		None = 0,

		Pause,
		PauseAI,

		// Times
		TotalRuntime = 100,
		TotalFlighttime,
		TimerElapsedtime,

		// Velocities
		IndicatedAirspeed = 300,
		TrueAirspeed,
		GroundSpeed,

		// Mach, VVI, G-loads
		MachNumber = 400,
		GForceNormal,
		GForceAxial,
		GForceSide,

		// Atmosphere: Weather
		BarometerSealevelInHg = 500,
		TemperaturSealevelC,
		WindSpeedKts,

		// Joystick
		YokePitch = 800,
		YokeRoll,
		YokeHeading,

		// Control Surfaces
		Elevator = 1100,
		Aileron,
		Rudder,

		// Flaps
		FlapSetting = 1300,
		FlapActual,

		// Gear & Brakes
		GearDeploy = 1400,
		GearHandle,
		BrakeParking,
		BrakeLeft,
		BrakeRight,

		// MNR (Angular Moments)
		M = 1500,
		L,
		N,

		//PQR (Angular Velocities)
		QRad = 1600,
		PRad,
		RRad,
		Q,
		P,
		R,

		// Orientation: pitch, roll, yaw, heading
		Pitch = 1700,
		Roll,
		HeadingTrue,
		HeadingMag,
		Quaternion,

		// Orientation: alpha beta hpath vpath slip
		AngleOfAttack = 1800,
		Sideslip,
		HPath,
		VPath,

		MagneticVariation = 1901,

		// Global Position
		Latitude = 2000,
		Longitude,
		AGL,

		// Local Postion & Velocity
		LocalX = 2100,
		LocalY,
		LocalZ,
		LocalVX,
		LocalVY,
		LocalVZ,

		ThrottleSet = 2200,
		ThrottleActual = 2300,

		// Multiplayer Aircraft
		FlapActual2,
		Spoiler,
		BrakeSpeed,
		Sweep,
		Slats,

		// Mulitplayer positon
		MP1Lat,
		MP2Lat,
		MP3Lat,
		MP4Lat,
		MP5Lat,
		MP6Lat,
		MP7Lat,

		MP1Lon,
		MP2Lon,
		MP3Lon,
		MP4Lon,
		MP5Lon,
		MP6Lon,
		MP7Lon,

		MP1Alt,
		MP2Alt,
		MP3Alt,
		MP4Alt,
		MP5Alt,
		MP6Alt,
		MP7Alt,
	};

	class DataManager
	{
	public:
		static void Initialize();

		static std::size_t Get(std::string dref, float values[], std::size_t size);

		static double GetDouble(DREF dref, std::uint8_t aircraft = 0);

		static float GetFloat(DREF dref, std::uint8_t aircraft = 0);

		static int GetInt(DREF dref, std::uint8_t aircraft = 0);

		static std::size_t GetFloatArray(DREF dref, float values[], std::size_t size, std::uint8_t aircraft = 0);

		static std::size_t GetIntArray(DREF dref, int values[], std::size_t size, std::uint8_t aircraft = 0);

		static void Set(DREF dref, double value, std::uint8_t aircraft = 0);

		static void Set(DREF dref, float value, std::uint8_t aircraft = 0);

		static void Set(DREF dref, int value, std::uint8_t aircraft = 0);

		static void Set(DREF dref, float values[], std::size_t size, std::uint8_t aircraft = 0);

		static void Set(DREF dref, int values[], std::size_t size, std::uint8_t aircraft = 0);

		static void Set(std::string dref, float values[], std::size_t size);

		static void SetGear(float gear, bool immediate, std::uint8_t aircraft = 0);

		static void SetPosition(float pos[3], std::uint8_t aircraft = 0);

		static void SetOrientation(float orient[3], std::uint8_t aircraft = 0);

		static void SetFlaps(float value);
	};
}

#endif