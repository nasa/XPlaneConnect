// Copyright (c) 2013-2018 United States Government as represented by the Administrator of the
// National Aeronautics and Space Administration. All Rights Reserved.
#ifndef XPCPLUGIN_DATAMANAGER_H_
#define XPCPLUGIN_DATAMANAGER_H_

#include <string>

namespace XPC
{
	/// Represents named datarefs used by X-Plane Connect
	enum DREF
	{
		DREF_None = 0,

		DREF_Pause,
		DREF_PauseAI,

		// Times
		DREF_TotalRuntime = 100,
		DREF_TotalFlighttime,
		DREF_TimerElapsedtime,

		// Velocities
		DREF_IndicatedAirspeed = 300,
		DREF_TrueAirspeed,
		DREF_GroundSpeed,

		// Mach, VVI, G-loads
		DREF_MachNumber = 400,
		DREF_GForceNormal,
		DREF_GForceAxial,
		DREF_GForceSide,

		// Atmosphere: Weather
		DREF_BarometerSealevelInHg = 500,
		DREF_TemperaturSealevelC,
		DREF_WindSpeedKts,

		// Joystick
		DREF_YokePitch = 800,
		DREF_YokeRoll,
		DREF_YokeHeading,

		// Control Surfaces
		DREF_Elevator = 1100,
		DREF_Aileron,
		DREF_Rudder,

		// Flaps
		DREF_FlapSetting = 1300,
		DREF_FlapActual,

		// Gear & Brakes
		DREF_GearDeploy = 1400,
		DREF_GearHandle,
		DREF_BrakeParking,
		DREF_BrakeLeft,
		DREF_BrakeRight,

		// MNR (Angular Moments)
		DREF_M = 1500,
		DREF_L,
		DREF_N,

		// PQR (Angular Velocities)
		DREF_QRad = 1600,
		DREF_PRad,
		DREF_RRad,
		DREF_Q,
		DREF_P,
		DREF_R,

		// Orientation: pitch, roll, yaw, heading
		DREF_Pitch = 1700,
		DREF_Roll,
		DREF_HeadingTrue,
		DREF_HeadingMag,
		DREF_Quaternion,

		// Orientation: alpha beta hpath vpath slip
		DREF_AngleOfAttack = 1800,
		DREF_Sideslip,
		DREF_HPath,
		DREF_VPath,

		DREF_MagneticVariation = 1901,

		// Global Position
		DREF_Latitude = 2000,
		DREF_Longitude,
		DREF_Elevation,
		DREF_AGL,

		// Local Postion & Velocity
		DREF_LocalX = 2100,
		DREF_LocalY,
		DREF_LocalZ,
		DREF_LocalVX,
		DREF_LocalVY,
		DREF_LocalVZ,

		DREF_ThrottleSet = 2200,
		DREF_ThrottleActual = 2300,

		// Multiplayer Aircraft
		DREF_FlapActual2,
		DREF_Spoiler,
		DREF_SpeedBrakeSet,
		DREF_SpeedBrakeActual,
		DREF_Sweep,
		DREF_Slats,

		// Mulitplayer positon
		DREF_MP1Lat,
		DREF_MP2Lat,
		DREF_MP3Lat,
		DREF_MP4Lat,
		DREF_MP5Lat,
		DREF_MP6Lat,
		DREF_MP7Lat,

		DREF_MP1Lon,
		DREF_MP2Lon,
		DREF_MP3Lon,
		DREF_MP4Lon,
		DREF_MP5Lon,
		DREF_MP6Lon,
		DREF_MP7Lon,

		DREF_MP1Alt,
		DREF_MP2Alt,
		DREF_MP3Alt,
		DREF_MP4Alt,
		DREF_MP5Alt,
		DREF_MP6Alt,
		DREF_MP7Alt
	};

	/// Maps X-Plane dataref lines to XPC DREF values.
	extern DREF XPData[134][8];

	/// Contains methods to martial data between the plugin and X-Plane.
	///
	/// \author Jason Watkins
	/// \version 1.1
	/// \since 1.0.0
	/// \date Intial Version: 2015-04-13
	/// \date Last Updated: 2015-05-14
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
		///          so subsequent calls will incur minimal extra overhead compared to
		///          the other methods in this class.
		///
		/// \remarks For simplicity, this method is provided with only one output type.
		///          For most integer and double drefs, this is unlikely to cause issues.
		///          However, for drefs where the entire integer range may be used, or
		///          doubles where high precision is required, using this method may result
		///          in a loss of precision. In that case, consider using one of the
		///          strongly typed methods instead.
		static int Get(const std::string& dref, float values[], int size);

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
		static void Set(const std::string& dref, float values[], int size);

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
		static void SetPosition(double pos[3], char aircraft = 0);

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

		/// Executes a command
 		///
 		/// \param comm   The name of the command to execute.
 		static void Execute(const std::string& comm);

		/// Gets a default value that indicates that a dataref should not be changed.
		static float GetDefaultValue();

		/// Checks whether the given value should be treated as a default value.
		///
		/// \param value The value to check.
		/// \returns     true if value is a default value; otherwise false.
		static bool IsDefault(double value);
	};
}
#endif
