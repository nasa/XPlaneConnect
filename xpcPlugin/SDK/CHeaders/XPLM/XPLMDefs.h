#ifndef _XPLMDefs_h_
#define _XPLMDefs_h_

/*
 * Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
 * rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
 *
 */

/***************************************************************************
 * XPLMDefs
 ***************************************************************************/
/*
 * This file is contains the cross-platform and basic definitions for the
 * X-Plane SDK.
 * 
 * The preprocessor macros APL, LIN and IBM must be defined to specify the
 * compilation target; define APL to 1 to compile on Mac, IBM to 1 to compile
 * on Windows and LIN to 1 to compile on Linux. Only one compilation target
 * may be used at a time. You must specify these macro definitions before
 * including XPLMDefs.h or any other XPLM headers.  You can do this using the
 * -D command line option or a preprocessor header.
 *
 */


#ifdef __cplusplus
extern "C" {
#endif

#if IBM
#include <windows.h>
#endif
#include <stdint.h>
/***************************************************************************
 * DLL Definitions
 ***************************************************************************/
/*
 * These definitions control the importing and exporting of functions within
 * the DLL.
 * 
 * You can prefix your five required callbacks with the PLUGIN_API macro to
 * declare them as exported C functions.  The XPLM_API macro identifies
 * functions that are provided to you via the plugin SDK.  (Link against
 * XPLM.lib to use these functions.)
 *
 */


#ifdef __cplusplus
	#if APL
        #if __GNUC__ >= 4
            #define PLUGIN_API extern "C" __attribute__((visibility("default")))
        #elif __MACH__
			#define PLUGIN_API extern "C"
		#else		
			#define PLUGIN_API extern "C" __declspec(dllexport)
		#endif
	#elif IBM
		#define PLUGIN_API extern "C" __declspec(dllexport)
	#elif LIN
		#if __GNUC__ >= 4
			#define PLUGIN_API extern "C" __attribute__((visibility("default")))
		#else
			#define PLUGIN_API extern "C"
		#endif
	#else
		#error "Platform not defined!"
	#endif
#else
	#if APL
        #if __GNUC__ >= 4
            #define PLUGIN_API __attribute__((visibility("default")))
        #elif __MACH__
			#define PLUGIN_API 
		#else
			#define PLUGIN_API __declspec(dllexport)
		#endif		
	#elif IBM
		#define PLUGIN_API __declspec(dllexport)
	#elif LIN
        #if __GNUC__ >= 4
            #define PLUGIN_API __attribute__((visibility("default")))
		#else
			#define PLUGIN_API
		#endif		
	#else
		#error "Platform not defined!"
	#endif
#endif

#if APL
	#if XPLM
        #if __GNUC__ >= 4
            #define XPLM_API __attribute__((visibility("default")))
        #elif __MACH__
			#define XPLM_API 
		#else
			#define XPLM_API __declspec(dllexport)
		#endif
	#else
		#define XPLM_API 
	#endif
#elif IBM
	#if XPLM
		#define XPLM_API __declspec(dllexport)
	#else
		#define XPLM_API __declspec(dllimport)
	#endif
#elif LIN
	#if XPLM
		#if __GNUC__ >= 4
            #define XPLM_API __attribute__((visibility("default")))
		#else
			#define XPLM_API
		#endif
	#else
		#define XPLM_API
	#endif	
#else
	#error "Platform not defined!"
#endif

/***************************************************************************
 * GLOBAL DEFINITIONS
 ***************************************************************************/
/*
 * These definitions are used in all parts of the SDK.
 *
 */


/*
 * XPLMPluginID
 * 
 * Each plug-in is identified by a unique integer ID.  This ID can be used to
 * disable or enable a plug-in, or discover what plug-in is 'running' at the
 * time.  A plug-in ID is unique within the currently running instance of
 * X-Plane unless plug-ins are reloaded.  Plug-ins may receive a different
 * unique ID each time they are loaded. This includes the unloading and
 * reloading of plugins that are part of the user's aircraft.
 * 
 * For persistent identification of plug-ins, use XPLMFindPluginBySignature in
 * XPLMUtiltiies.h .
 * 
 * -1 indicates no plug-in.
 *
 */
typedef int XPLMPluginID;

/* No plugin.                                                                 */
#define XPLM_NO_PLUGIN_ID    (-1)

/* X-Plane itself                                                             */
#define XPLM_PLUGIN_XPLANE   (0)

/* The current XPLM revision is 4.00 (400).                                   */
#define kXPLM_Version        (400)

/*
 * XPLMKeyFlags
 * 
 * These bitfields define modifier keys in a platform independent way. When a
 * key is pressed, a series of messages are sent to your plugin.  The down
 * flag is set in the first of these messages, and the up flag in the last. 
 * While the key is held down, messages are sent with neither flag set to
 * indicate that the key is being held down as a repeated character.
 * 
 * The control flag is mapped to the control flag on Macintosh and PC. 
 * Generally X-Plane uses the control key and not the command key on
 * Macintosh, providing a consistent interface across platforms that does not
 * necessarily match the Macintosh user interface guidelines.  There is not
 * yet a way for plugins to access the Macintosh control keys without using
 * #ifdefed code.
 *
 */
enum {
    /* The shift key is down                                                      */
    xplm_ShiftFlag                           = 1,

    /* The option or alt key is down                                              */
    xplm_OptionAltFlag                       = 2,

    /* The control key is down                                                    */
    xplm_ControlFlag                         = 4,

    /* The key is being pressed down                                              */
    xplm_DownFlag                            = 8,

    /* The key is being released                                                  */
    xplm_UpFlag                              = 16,


};
typedef int XPLMKeyFlags;

/***************************************************************************
 * ASCII CONTROL KEY CODES
 ***************************************************************************/
/*
 * These definitions define how various control keys are mapped to ASCII key
 * codes. Not all key presses generate an ASCII value, so plugin code should
 * be prepared to see null characters come from the keyboard...this usually
 * represents a key stroke that has no equivalent ASCII, like a page-down
 * press.  Use virtual key codes to find these key strokes.
 * 
 * ASCII key codes take into account modifier keys; shift keys will affect
 * capitals and punctuation; control key combinations may have no vaild ASCII
 * and produce NULL.  To detect control-key combinations, use virtual key
 * codes, not ASCII keys.
 *
 */


#define XPLM_KEY_RETURN      13

#define XPLM_KEY_ESCAPE      27

#define XPLM_KEY_TAB         9

#define XPLM_KEY_DELETE      8

#define XPLM_KEY_LEFT        28

#define XPLM_KEY_RIGHT       29

#define XPLM_KEY_UP          30

#define XPLM_KEY_DOWN        31

#define XPLM_KEY_0           48

#define XPLM_KEY_1           49

#define XPLM_KEY_2           50

#define XPLM_KEY_3           51

#define XPLM_KEY_4           52

#define XPLM_KEY_5           53

#define XPLM_KEY_6           54

#define XPLM_KEY_7           55

#define XPLM_KEY_8           56

#define XPLM_KEY_9           57

#define XPLM_KEY_DECIMAL     46

/***************************************************************************
 * VIRTUAL KEY CODES
 ***************************************************************************/
/*
 * These are cross-platform defines for every distinct keyboard press on the
 * computer. Every physical key on the keyboard has a virtual key code.  So
 * the "two" key on the top row of the main keyboard has a different code from
 * the "two" key on the numeric key pad.  But the 'w' and 'W' character are
 * indistinguishable by virtual key code because they are the same physical
 * key (one with and one without the shift key).
 * 
 * Use virtual key codes to detect keystrokes that do not have ASCII
 * equivalents, allow the user to map the numeric keypad separately from the
 * main keyboard, and detect control key and other modifier-key combinations
 * that generate ASCII control key sequences (many of which are not available
 * directly via character keys in the SDK).
 * 
 * To assign virtual key codes we started with the Microsoft set but made some
 * additions and changes.  A few differences:
 * 
 * 1. Modifier keys are not available as virtual key codes.  You cannot get
 *    distinct modifier press and release messages.  Please do not try to use
 *    modifier keys as regular keys; doing so will almost certainly interfere
 *    with users' abilities to use the native X-Plane key bindings.
 * 2. Some keys that do not exist on both Mac and PC keyboards are removed.
 * 3. Do not assume that the values of these keystrokes are interchangeable
 *    with MS v-keys.
 *
 */


#define XPLM_VK_BACK         0x08

#define XPLM_VK_TAB          0x09

#define XPLM_VK_CLEAR        0x0C

#define XPLM_VK_RETURN       0x0D

#define XPLM_VK_ESCAPE       0x1B

#define XPLM_VK_SPACE        0x20

#define XPLM_VK_PRIOR        0x21

#define XPLM_VK_NEXT         0x22

#define XPLM_VK_END          0x23

#define XPLM_VK_HOME         0x24

#define XPLM_VK_LEFT         0x25

#define XPLM_VK_UP           0x26

#define XPLM_VK_RIGHT        0x27

#define XPLM_VK_DOWN         0x28

#define XPLM_VK_SELECT       0x29

#define XPLM_VK_PRINT        0x2A

#define XPLM_VK_EXECUTE      0x2B

#define XPLM_VK_SNAPSHOT     0x2C

#define XPLM_VK_INSERT       0x2D

#define XPLM_VK_DELETE       0x2E

#define XPLM_VK_HELP         0x2F

/* XPLM_VK_0 thru XPLM_VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39)  */
#define XPLM_VK_0            0x30

#define XPLM_VK_1            0x31

#define XPLM_VK_2            0x32

#define XPLM_VK_3            0x33

#define XPLM_VK_4            0x34

#define XPLM_VK_5            0x35

#define XPLM_VK_6            0x36

#define XPLM_VK_7            0x37

#define XPLM_VK_8            0x38

#define XPLM_VK_9            0x39

/* XPLM_VK_A thru XPLM_VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)  */
#define XPLM_VK_A            0x41

#define XPLM_VK_B            0x42

#define XPLM_VK_C            0x43

#define XPLM_VK_D            0x44

#define XPLM_VK_E            0x45

#define XPLM_VK_F            0x46

#define XPLM_VK_G            0x47

#define XPLM_VK_H            0x48

#define XPLM_VK_I            0x49

#define XPLM_VK_J            0x4A

#define XPLM_VK_K            0x4B

#define XPLM_VK_L            0x4C

#define XPLM_VK_M            0x4D

#define XPLM_VK_N            0x4E

#define XPLM_VK_O            0x4F

#define XPLM_VK_P            0x50

#define XPLM_VK_Q            0x51

#define XPLM_VK_R            0x52

#define XPLM_VK_S            0x53

#define XPLM_VK_T            0x54

#define XPLM_VK_U            0x55

#define XPLM_VK_V            0x56

#define XPLM_VK_W            0x57

#define XPLM_VK_X            0x58

#define XPLM_VK_Y            0x59

#define XPLM_VK_Z            0x5A

#define XPLM_VK_NUMPAD0      0x60

#define XPLM_VK_NUMPAD1      0x61

#define XPLM_VK_NUMPAD2      0x62

#define XPLM_VK_NUMPAD3      0x63

#define XPLM_VK_NUMPAD4      0x64

#define XPLM_VK_NUMPAD5      0x65

#define XPLM_VK_NUMPAD6      0x66

#define XPLM_VK_NUMPAD7      0x67

#define XPLM_VK_NUMPAD8      0x68

#define XPLM_VK_NUMPAD9      0x69

#define XPLM_VK_MULTIPLY     0x6A

#define XPLM_VK_ADD          0x6B

#define XPLM_VK_SEPARATOR    0x6C

#define XPLM_VK_SUBTRACT     0x6D

#define XPLM_VK_DECIMAL      0x6E

#define XPLM_VK_DIVIDE       0x6F

#define XPLM_VK_F1           0x70

#define XPLM_VK_F2           0x71

#define XPLM_VK_F3           0x72

#define XPLM_VK_F4           0x73

#define XPLM_VK_F5           0x74

#define XPLM_VK_F6           0x75

#define XPLM_VK_F7           0x76

#define XPLM_VK_F8           0x77

#define XPLM_VK_F9           0x78

#define XPLM_VK_F10          0x79

#define XPLM_VK_F11          0x7A

#define XPLM_VK_F12          0x7B

#define XPLM_VK_F13          0x7C

#define XPLM_VK_F14          0x7D

#define XPLM_VK_F15          0x7E

#define XPLM_VK_F16          0x7F

#define XPLM_VK_F17          0x80

#define XPLM_VK_F18          0x81

#define XPLM_VK_F19          0x82

#define XPLM_VK_F20          0x83

#define XPLM_VK_F21          0x84

#define XPLM_VK_F22          0x85

#define XPLM_VK_F23          0x86

#define XPLM_VK_F24          0x87

/* The following definitions are extended and are not based on the Microsoft  *
 * key set.                                                                   */
#define XPLM_VK_EQUAL        0xB0

#define XPLM_VK_MINUS        0xB1

#define XPLM_VK_RBRACE       0xB2

#define XPLM_VK_LBRACE       0xB3

#define XPLM_VK_QUOTE        0xB4

#define XPLM_VK_SEMICOLON    0xB5

#define XPLM_VK_BACKSLASH    0xB6

#define XPLM_VK_COMMA        0xB7

#define XPLM_VK_SLASH        0xB8

#define XPLM_VK_PERIOD       0xB9

#define XPLM_VK_BACKQUOTE    0xBA

#define XPLM_VK_ENTER        0xBB

#define XPLM_VK_NUMPAD_ENT   0xBC

#define XPLM_VK_NUMPAD_EQ    0xBD

/*
 * XPLMFixedString150_t
 * 
 * A container for a fixed-size string buffer of 150 characters.
 *
 */
typedef struct {
    /* The size of the struct.                                                    */
     char                      buffer[150];
} XPLMFixedString150_t;
#ifdef __cplusplus
}
#endif

#endif
