{
   Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
   rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
}

UNIT XPLMSound;
INTERFACE
{
   This provides a minimal interface into the FMOD audio system. On the
   simplest level, you can request that X-Plane plays an in-memory audio
   buffer. This will work without linking to FMOD yourself. If you want to do
   anything more, such as modifying the sound, or loading banks and triggering
   your own events, you can get a pointer to the FMOD Studio instance.
}

USES
    XPLMDefs;
   {$A4}
{$IFDEF XPLM400}
{___________________________________________________________________________
 * FMOD ACCESS
 ___________________________________________________________________________}

   {
    XPLMAudioBus
    
    This enumeration states the type of audio you wish to play - that is, the
    part of the simulated environment that the audio belongs in. If you use
    FMOD directly, note that COM1, COM2, Pilot and GND exist in a different
    FMOD bank so you may see these channels being unloaded/reloaded
    independently of the others.
   }
TYPE
   XPLMAudioBus = (
     { Incoming speech on COM1                                                    }
      xplm_AudioRadioCom1                      = 0
 
     { Incoming speech on COM2                                                    }
     ,xplm_AudioRadioCom2                      = 1
 
     { Pilot's own speech                                                         }
     ,xplm_AudioRadioPilot                     = 2
 
     { Copilot's own speech                                                       }
     ,xplm_AudioRadioCopilot                   = 3
 
     ,xplm_AudioExteriorAircraft               = 4
 
     ,xplm_AudioExteriorEnvironment            = 5
 
     ,xplm_AudioExteriorUnprocessed            = 6
 
     ,xplm_AudioInterior                       = 7
 
     ,xplm_AudioUI                             = 8
 
     { Dedicated ground vehicle cable                                             }
     ,xplm_AudioGround                         = 9
 
     { Master bus. Not normally to be used directly.                              }
     ,xplm_Master                              = 10
 
   );
   PXPLMAudioBus = ^XPLMAudioBus;

   {
    XPLMBankID
    
    These values are returned as the parameter of the
    "XPLM_MSG_FMOD_BANK_LOADED" and "XPLM_MSG_FMOD_BANK_UNLOADING" messages.
   }
   XPLMBankID = (
     { Master bank. Handles all aircraft and environmental audio.                 }
      xplm_MasterBank                          = 0
 
     { Radio bank. Handles COM1/COM2/GND/Pilot/Copilot.                           }
     ,xplm_RadioBank                           = 1
 
   );
   PXPLMBankID = ^XPLMBankID;


   {
    XPLMGetFMODStudio
    
    Get a handle to the FMOD Studio, allowing you to load/process whatever else
    you need. This also gives access to the underlying system via
    FMOD::Studio::System::getCoreSystem() / FMOD_Studio_System_GetCoreSystem()
    .
   }
   FUNCTION XPLMGetFMODStudio: PFMOD_STUDIO_SYSTEM;
    cdecl; external XPLM_DLL;

   {
    XPLMGetFMODChannelGroup
    
    Get a reference to a particular channel group - that is, an output channel.
    See the table above for values.
   }
   FUNCTION XPLMGetFMODChannelGroup(
                                        audioType           : XPLMAudioBus) : PFMOD_CHANNELGROUP;
    cdecl; external XPLM_DLL;


   {
    XPLMPCMComplete_f
    
    If you use XPLMPlayPCMOnBus() you may use this optional callback to find
    out when the FMOD::Channel is complete, if you need to deallocate memory
    for example.
   }
TYPE
     XPLMPCMComplete_f = PROCEDURE(
                                    inRefcon            : pointer;
                                    status              : FMOD_RESULT); cdecl;

   {
    XPLMPlayPCMOnBus
    
    Play an in-memory audio buffer on a given audio bus. The resulting FMOD
    channel is returned. PAY ATTENTION TO THE CALLBACK - when the sample
    completes or is stopped by X-Plane, the channel will go away. It's up to
    you to listen for the callback and invalidate any copy of the channel
    pointer you have lying around. The callback is optional because if you have
    no intention of interacting with the sound after it's launched, then you
    don't need to keep the channel pointer at all. The sound is not started
    instantly. Instead, it will be started the next time X-Plane refreshes the
    sound system, typically at the start of the next frame. This allows you to
    set the initial position for the sound, if required. The callback will be
    called on the same thread as the sound is created from, and will be called
    only once per sound. If the call fails and you provide a callback function,
    you will get a callback with an FMOD status code.
   }
   FUNCTION XPLMPlayPCMOnBus(
                                        audioBuffer         : pointer;
                                        bufferSize          : uint32_t;
                                        soundFormat         : FMOD_SOUND_FORMAT;
                                        freqHz              : Integer;
                                        numChannels         : Integer;
                                        loop                : Integer;
                                        audioType           : XPLMAudioBus;
                                        inCallback          : XPLMPCMComplete_f;
                                        inRefcon            : pointer) : PFMOD_CHANNEL;    { Can be nil }
    cdecl; external XPLM_DLL;

   {
    XPLMStopAudio
    
    Stop playing an active FMOD channel. If you defined a completion callback,
    this will be called. After this, the FMOD::Channel* will no longer be valid
    and must not be used in any future calls.
   }
   FUNCTION XPLMStopAudio(
                                        fmod_channel        : PFMOD_CHANNEL) : FMOD_RESULT;
    cdecl; external XPLM_DLL;

   {
    XPLMSetAudioPosition
    
    Move the given audio channel (i.e. a single sound) to a specific location
    in local co-ordinates. This will set the sound to 3D if it is not already.
   }
   FUNCTION XPLMSetAudioPosition(
                                        fmod_channel        : PFMOD_CHANNEL;
                                        position            : PFMOD_VECTOR;
                                        velocity            : PFMOD_VECTOR) : FMOD_RESULT;
    cdecl; external XPLM_DLL;

   {
    XPLMSetAudioFadeDistance
    
    Set the minimum and maximum fade distances for a given sound. This is
    highly unlikely to be 0 - please see
    https://documentation.help/FMOD-Studio-API/FMOD_Sound_Set3DMinMaxDistance.html
    for full details. This will set the sound to 3D if it is not already. You
    can set a 3D sound back to 2D by passing negative values for both min amd
    max.
   }
   FUNCTION XPLMSetAudioFadeDistance(
                                        fmod_channel        : PFMOD_CHANNEL;
                                        min_fade_distance   : Single;
                                        max_fade_distance   : Single) : FMOD_RESULT;
    cdecl; external XPLM_DLL;

   {
    XPLMSetAudioVolume
    
    Set the current volume of an active FMOD channel. This should be used to
    handle changes in the audio source volume, not for fading with distance.
    Values from 0 to 1 are normal, above 1 can be used to artificially amplify
    a sound.
   }
   FUNCTION XPLMSetAudioVolume(
                                        fmod_channel        : PFMOD_CHANNEL;
                                        source_volume       : Single) : FMOD_RESULT;
    cdecl; external XPLM_DLL;

   {
    XPLMSetAudioPitch
    
    Change the current pitch of an active FMOD channel.
   }
   FUNCTION XPLMSetAudioPitch(
                                        fmod_channel        : PFMOD_CHANNEL;
                                        audio_pitch_hz      : Single) : FMOD_RESULT;
    cdecl; external XPLM_DLL;

   {
    XPLMSetAudioCone
    
    Set a directional cone for an active FMOD channel. The orientation vector
    is in local coordinates. This will set the sound to 3D if it is not
    already.
   }
   FUNCTION XPLMSetAudioCone(
                                        fmod_channel        : PFMOD_CHANNEL;
                                        inside_angle        : Single;
                                        outside_angle       : Single;
                                        outside_volume      : Single;
                                        orientation         : PFMOD_VECTOR) : FMOD_RESULT;
    cdecl; external XPLM_DLL;

{$ENDIF XPLM400}

IMPLEMENTATION

END.
