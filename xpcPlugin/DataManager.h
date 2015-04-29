//Copyright (c) 2013-2015 United States Government as represented by the Administrator of the
//National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPC_DATAMANAGER_H
#define XPC_DATAMANAGER_H

#include <string>

namespace XPC
{
	/// Represents named datarefs used by X-Plane Connect
	enum class DREF : unsigned long
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

	/// Marshals data between the plugin and X-Plane.
	///
	/// \author Jason Watkins
	/// \version 1.0.1
	/// \since 1.0.0
	/// \date Intial Version: 2015-04-13
	/// \date Last Updated: 2015-04-29
	class DataManager
	{
	public:
		/// Initializes the internal data used by DataManager to translate DREF values
		/// into X-Plane internal data.
		static void Initialize();

		/// Gets a dataref based on its name.
		///
		/// \param dref   The name of the dref to get.
		/// \param values An array in which the result of the operation will be stored.
		/// \param size   The size of the values array.
		/// \returns      The number of elements placed in the values array. For scalar
		///               drefs, this will be one. For array drefs, this will be the
		///               lesser of the size and the number of elements in the dref.
		///
		/// \remarks The first time this method is called for a given dataref, it must
		///          perform a relatively expensive lookup operation to translate the
		///          given string into an X-Plane internal pointer. This value is cached,
		///          so subsequent calls will incure minimal extra overhead compared to
		///          the other methods in this class.
		///
		/// \remarks For simplicity, this method is provided with only one output type.
		///          For most integer and double drefs, this is unlikely to cause issues.
		///          However, for drefs where the entire integer range may be used, or
		///          doubles where high precision is required, using this method may result
		///          in a loss of precision. In that case, consider using one of the
		///          strongly typed methods instead.
		static int Get(std::string dref, float values[], int size);

		/// Gets the value of a double dataref.
		///
		/// \param dref     The dataref to get.
		/// \param aircraft The aircraft number for which to get the data.
		/// \returns        The value of the dref specified if that dref has the type
		///                 double; otherwise an undefined value.
		///
		/// \remarks This method does not verify the type of the dref requested. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the value returned is determined by the X-Plane
		///          plugin manager, and is considered undefined by the plugin.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static double GetDouble(DREF dref, char aircraft = 0);

		/// Gets the value of a float dataref.
		///
		/// \param dref     The dataref to get.
		/// \param aircraft The aircraft number for which to get the data.
		/// \returns        The value of the dref specified if that dref has the type
		///                 float; otherwise an undefined value.
		///
		/// \remarks This method does not verify the type of the dref requested. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the value returned is determined by the X-Plane
		///          plugin manager, and is considered undefined by the plugin.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static float GetFloat(DREF dref, char aircraft = 0);

		/// Gets the value of an integer dataref.
		///
		/// \param dref     The dataref to get.
		/// \param aircraft The aircraft number for which to get the data.
		/// \returns        The value of the dref specified if that dref has the type
		///                 int; otherwise an undefined value.
		///
		/// \remarks This method does not verify the type of the dref requested. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the value returned is determined by the X-Plane
		///          plugin manager, and is considered undefined by the plugin.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static int GetInt(DREF dref, char aircraft = 0);

		/// Gets the value of a float array dataref.
		///
		/// \param dref     The dataref to get.
		/// \param values   An array in which the result of the operation will be stored.
		/// \param size     The size of the values array.
		/// \param aircraft The aircraft number for which to get the data.
		/// \returns        The number of elements placed in the values array. This will
		///                 be the lesser of the size and the number of elements in the dref.
		///
		/// \remarks This method does not verify the type of the dref requested. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the value returned is determined by the X-Plane
		///          plugin manager, and is considered undefined by the plugin.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static int GetFloatArray(DREF dref, float values[], int size, char aircraft = 0);


		/// Gets the value of an int array dataref.
		///
		/// \param dref     The dataref to get.
		/// \param values   An array in which the result of the operation will be stored.
		/// \param size     The size of the values array.
		/// \param aircraft The aircraft number for which to get the data.
		/// \returns        The number of elements placed in the values array. This will
		///                 be the lesser of the size and the number of elements in the dref.
		///
		/// \remarks This method does not verify the type of the dref requested. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the value returned is determined by the X-Plane
		///          plugin manager, and is considered undefined by the plugin.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static int GetIntArray(DREF dref, int values[], int size, char aircraft = 0);

		/// Sets the value of a dataref
		///
		/// \param dref   The name of the dref to set.
		/// \param values The value to set the dref to.
		/// \param size   The number of items stored in values.
		///
		/// \remarks The first time this method is called for a given dataref, it must
		///          perform a relatively expensive lookup operation to translate the
		///          given string into an X-Plane internal pointer. This value is cached,
		///          so subsequent calls will incure minimal extra overhead compared to
		///          the other methods in this class.
		///
		/// \remarks For simplicity, this method is provided with only one input type.
		///          For most integer and double drefs, this is unlikely to cause issues.
		///          However, for drefs where the entire integer range may be used, or
		///          doubles where high precision is required, using this method may result
		///          in a loss of precision. In that case, consider using one of the
		///          strongly typed methods instead.
		static void Set(std::string dref, float values[], int size);

		/// Sets the value of a double dataref.
		///
		/// \param dref     The dataref to set.
		/// \param value    The value to set the dref to.
		/// \param aircraft The aircraft number for which to get the data.
		///
		/// \remarks This method does not verify the type of the dref being set. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the operation will fail silently.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static void Set(DREF dref, double value, char aircraft = 0);

		/// Sets the value of a float dataref.
		///
		/// \param dref     The dataref to set.
		/// \param value    The value to set the dref to.
		/// \param aircraft The aircraft number for which to get the data.
		///
		/// \remarks This method does not verify the type of the dref being set. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the operation will fail silently.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static void Set(DREF dref, float value, char aircraft = 0);

		/// Sets the value of an integer dataref.
		///
		/// \param dref     The dataref to set.
		/// \param value    The value to set the dref to.
		/// \param aircraft The aircraft number for which to get the data.
		///
		/// \remarks This method does not verify the type of the dref being set. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the operation will fail silently.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static void Set(DREF dref, int value, char aircraft = 0);

		/// Sets the value of a float array dataref.
		///
		/// \param dref     The dataref to set.
		/// \param values   The value to set the dref to.
		/// \param size     The number of items stored in values.
		/// \param aircraft The aircraft number for which to get the data.
		///
		/// \remarks This method does not verify the type of the dref being set. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the operation will fail silently.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static void Set(DREF dref, float values[], int size, char aircraft = 0);

		/// Sets the value of an integer array dataref.
		///
		/// \param dref     The dataref to set.
		/// \param values   The value to set the dref to.
		/// \param size     The number of items stored in values.
		/// \param aircraft The aircraft number for which to get the data.
		///
		/// \remarks This method does not verify the type of the dref being set. It is
		///          the responsibility of the caller to check the X-Plane documentation
		///          and call the appropriate overload of this method. If the wrong
		///          overload is called, the operation will fail silently.
		///
		/// \remarks Although any combination of dref and aircraft may be passed to this
		///          method, most drefs are not valid for multiplayer aircraft. All drefs
		///          that are not prefixed with 'MP' are valid for the player aircraft.
		///          'MP' drefs should be called with aircraft 0.
		static void Set(DREF dref, int values[], int size, char aircraft = 0);

		/// Sets the landing gear for the specified airplane.
		///
		/// \param gear      The value to set the gear to. 0 for gear down, 1 for gear up.
		/// \param immediate Whether the gear should be forced to the specified position.
		///                  If immediate is false, only the gear handle dref will be set.
		/// \param aircraft  The aircraft to set the landing gear status on.
		static void SetGear(float gear, bool immediate, char aircraft = 0);

		/// Sets the position of the specified aircraft on the Earth.
		///
		/// \param pos      An array containing latitude, longitude and altitude in
		///                 fractional degrees and meters above sea level.
		/// \param aircraft The aircraft to set the position of.
		static void SetPosition(float pos[3], char aircraft = 0);

		/// Sets the orientation of the specified aircraft.
		///
		/// \param orient   An array containing the pitch, roll, and yaw orientations
		///                 to set, all in fractional degrees.
		/// \param aircraft The aircraft to set the orientation of.
		static void SetOrientation(float orient[3], char aircraft = 0);

		/// Sets flaps on the the player aircraft.
		///
		/// \param value The flaps settings. Should be between 0.0 (no flaps) and 1.0 (full flaps).
		static void SetFlaps(float value);
	};
}

#endif