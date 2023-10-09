#ifndef _XPLMSound_h_
#define _XPLMSound_h_

/*
 * Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
 * rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
 *
 */

/***************************************************************************
 * XPLMSound
 ***************************************************************************/
/*
 * This provides a minimal interface into the FMOD audio system. On the
 * simplest level, you can request that X-Plane plays an in-memory audio
 * buffer. This will work without linking to FMOD yourself. If you want to do
 * anything more, such as modifying the sound, or loading banks and triggering
 * your own events, you can get a pointer to the FMOD Studio instance.
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(XPLM400)
/***************************************************************************
 * FMOD ACCESS
 ***************************************************************************/

/*
 * XPLMAudioBus
 * 
 * This enumeration states the type of audio you wish to play - that is, the
 * part of the simulated environment that the audio belongs in. If you use
 * FMOD directly, note that COM1, COM2, Pilot and GND exist in a different
 * FMOD bank so you may see these channels being unloaded/reloaded
 * independently of the others.
 *
 */
enum {
    /* Incoming speech on COM1                                                    */
    xplm_AudioRadioCom1                      = 0,

    /* Incoming speech on COM2                                                    */
    xplm_AudioRadioCom2                      = 1,

    /* Pilot's own speech                                                         */
    xplm_AudioRadioPilot                     = 2,

    /* Copilot's own speech                                                       */
    xplm_AudioRadioCopilot                   = 3,

    xplm_AudioExteriorAircraft               = 4,

    xplm_AudioExteriorEnvironment            = 5,

    xplm_AudioExteriorUnprocessed            = 6,

    xplm_AudioInterior                       = 7,

    xplm_AudioUI                             = 8,

    /* Dedicated ground vehicle cable                                             */
    xplm_AudioGround                         = 9,

    /* Master bus. Not normally to be used directly.                              */
    xplm_Master                              = 10,


};
typedef int XPLMAudioBus;

/*
 * XPLMBankID
 * 
 * These values are returned as the parameter of the
 * "XPLM_MSG_FMOD_BANK_LOADED" and "XPLM_MSG_FMOD_BANK_UNLOADING" messages.
 *
 */
enum {
    /* Master bank. Handles all aircraft and environmental audio.                 */
    xplm_MasterBank                          = 0,

    /* Radio bank. Handles COM1/COM2/GND/Pilot/Copilot.                           */
    xplm_RadioBank                           = 1,


};
typedef int XPLMBankID;


/* 
 * If you want to get full access to FMOD sound features, you need to include fmod.h or fmod.hpp yourself FIRST.
 * If you only need the basic wrapper functions which allow 3D placement and playback on a specified channel, there
 * is no need to link with full FMOD.
 * It is recommended but not required that you don't mix methods - either use native FMOD calls or stick entirely
 * within the functions of the X-Plane SDK.
 *
 * If you choose to use the advanced method, be aware that it is your responsibility to ensure that any resources,
 * especially callbacks, are cleaned up when needed. The sound system may well be completely rebuild when the master 
 * banks are reloaded, when aircraft are reloaded, or your plugin is unloaded.
 *
 * IMPORTANT: For all calls which use, or return, FMOD base types like FMOD_Studio_System*, these are fully interchangeable
 *            with their C++ equivalents. See https://www.fmod.com/docs/2.02/api/white-papers-handle-system.html .
 */
#if defined(_FMOD_COMMON_H)

/*
 * XPLMGetFMODStudio
 * 
 * Get a handle to the FMOD Studio, allowing you to load/process whatever else
 * you need. This also gives access to the underlying system via
 * FMOD::Studio::System::getCoreSystem() / FMOD_Studio_System_GetCoreSystem()
 * .
 *
 */
XPLM_API FMOD_STUDIO_SYSTEM* XPLMGetFMODStudio(void);

/*
 * XPLMGetFMODChannelGroup
 * 
 * Get a reference to a particular channel group - that is, an output channel.
 * See the table above for values.
 *
 */
XPLM_API FMOD_CHANNELGROUP* XPLMGetFMODChannelGroup(
                         XPLMAudioBus         audioType);


#else
/*
 * These definitions are enough to play a basic sound without linking to the full FMOD distribution. You can still position it in 3D
 * and change other basic parameters. In all cases where an FMOD_RESULT is returned, the full range of FMOD_RESULT codes are used - the
 * status will in almost all situations be coming straight from FMOD - so the single definition here is purely to create a matching
 * datatype and allow simple "is OK" and "is not OK" tests.
*/

typedef enum FMOD_RESULT
{
    FMOD_OK,
} FMOD_RESULT;
typedef enum FMOD_SOUND_FORMAT
{
	FMOD_SOUND_FORMAT_PCM16 = 2
} FMOD_SOUND_FORMAT;
typedef struct FMOD_VECTOR
{
	float x, y, z;
} FMOD_VECTOR;
typedef void FMOD_CHANNEL;
#endif

/*
 * XPLMPCMComplete_f
 * 
 * If you use XPLMPlayPCMOnBus() you may use this optional callback to find
 * out when the FMOD::Channel is complete, if you need to deallocate memory
 * for example.
 *
 */
typedef void (* XPLMPCMComplete_f)(
                         void *               inRefcon,
                         FMOD_RESULT          status);

/*
 * XPLMPlayPCMOnBus
 * 
 * Play an in-memory audio buffer on a given audio bus. The resulting FMOD
 * channel is returned. PAY ATTENTION TO THE CALLBACK - when the sample
 * completes or is stopped by X-Plane, the channel will go away. It's up to
 * you to listen for the callback and invalidate any copy of the channel
 * pointer you have lying around. The callback is optional because if you have
 * no intention of interacting with the sound after it's launched, then you
 * don't need to keep the channel pointer at all. The sound is not started
 * instantly. Instead, it will be started the next time X-Plane refreshes the
 * sound system, typically at the start of the next frame. This allows you to
 * set the initial position for the sound, if required. The callback will be
 * called on the same thread as the sound is created from, and will be called
 * only once per sound. If the call fails and you provide a callback function,
 * you will get a callback with an FMOD status code.
 *
 */
XPLM_API FMOD_CHANNEL* XPLMPlayPCMOnBus(
                         void *               audioBuffer,
                         uint32_t             bufferSize,
                         FMOD_SOUND_FORMAT    soundFormat,
                         int                  freqHz,
                         int                  numChannels,
                         int                  loop,
                         XPLMAudioBus         audioType,
                         XPLMPCMComplete_f    inCallback,
                         void *               inRefcon);              /* Can be NULL */

/*
 * XPLMStopAudio
 * 
 * Stop playing an active FMOD channel. If you defined a completion callback,
 * this will be called. After this, the FMOD::Channel* will no longer be valid
 * and must not be used in any future calls.
 *
 */
XPLM_API FMOD_RESULT XPLMStopAudio(
                         FMOD_CHANNEL*        fmod_channel);

/*
 * XPLMSetAudioPosition
 * 
 * Move the given audio channel (i.e. a single sound) to a specific location
 * in local co-ordinates. This will set the sound to 3D if it is not already.
 *
 */
XPLM_API FMOD_RESULT XPLMSetAudioPosition(
                         FMOD_CHANNEL*        fmod_channel,
                         FMOD_VECTOR*         position,
                         FMOD_VECTOR*         velocity);

/*
 * XPLMSetAudioFadeDistance
 * 
 * Set the minimum and maximum fade distances for a given sound. This is
 * highly unlikely to be 0 - please see
 * https://documentation.help/FMOD-Studio-API/FMOD_Sound_Set3DMinMaxDistance.html
 * for full details. This will set the sound to 3D if it is not already. You
 * can set a 3D sound back to 2D by passing negative values for both min amd
 * max.
 *
 */
XPLM_API FMOD_RESULT XPLMSetAudioFadeDistance(
                         FMOD_CHANNEL*        fmod_channel,
                         float                min_fade_distance,
                         float                max_fade_distance);

/*
 * XPLMSetAudioVolume
 * 
 * Set the current volume of an active FMOD channel. This should be used to
 * handle changes in the audio source volume, not for fading with distance.
 * Values from 0 to 1 are normal, above 1 can be used to artificially amplify
 * a sound.
 *
 */
XPLM_API FMOD_RESULT XPLMSetAudioVolume(
                         FMOD_CHANNEL*        fmod_channel,
                         float                source_volume);

/*
 * XPLMSetAudioPitch
 * 
 * Change the current pitch of an active FMOD channel.
 *
 */
XPLM_API FMOD_RESULT XPLMSetAudioPitch(
                         FMOD_CHANNEL*        fmod_channel,
                         float                audio_pitch_hz);

/*
 * XPLMSetAudioCone
 * 
 * Set a directional cone for an active FMOD channel. The orientation vector
 * is in local coordinates. This will set the sound to 3D if it is not
 * already.
 *
 */
XPLM_API FMOD_RESULT XPLMSetAudioCone(
                         FMOD_CHANNEL*        fmod_channel,
                         float                inside_angle,
                         float                outside_angle,
                         float                outside_volume,
                         FMOD_VECTOR*         orientation);

#endif /* XPLM400 */
#ifdef __cplusplus
}
#endif

#endif
