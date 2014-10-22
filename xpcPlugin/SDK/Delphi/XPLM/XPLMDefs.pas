{
   Copyright 2005-2012 Sandy Barbour and Ben Supnik
   
   All rights reserved.  See license.txt for usage.
   
   X-Plane SDK Version: 2.1.1                                                  
}

UNIT XPLMDefs;
INTERFACE
{
   This file is contains the cross-platform and basic definitions for the 
   X-Plane SDK. 
   
   The preprocessor macros APL and IBM must be defined to specify the 
   compilation target; define APL to 1 and IBM 0 to compile on Macintosh and 
   APL to 0 and IBM to 1 for Windows. You must specify these macro definitions 
   before including XPLMDefs.h or any other XPLM headers.  You can do this 
   using the -D command line option or a preprocessor header.                  
}

   {$A4}
{$IFDEF MSWINDOWS}
   {$DEFINE DELPHI}
{$ENDIF}
{$IFDEF LINUX}
   {$DEFINE KYLIX}
{$ENDIF}
TYPE
{$IFNDEF DELPHI}
{$IFNDEF KYLIX}
   Pchar = ^char;
   Ppchar = ^Pchar;
   Psingle = ^single;
   Pinteger = ^integer;
{$ENDIF}
{$ENDIF}
   Preal = ^real;
   Plongint = ^longint;
{___________________________________________________________________________
 * DLL Definitions
 ___________________________________________________________________________}
{
   These definitions control the importing and exporting of functions within 
   the DLL. 
   
   You can prefix your five required callbacks with the PLUGIN_API macro to 
   declare  them as exported C functions.  The XPLM_API macro identifies 
   functions that are provided to you via the plugin SDK.  (Link against 
   XPLM.lib to use these functions.)                                           
}




{___________________________________________________________________________
 * GLOBAL DEFINITIONS
 ___________________________________________________________________________}
{
   These definitions are used in all parts of the SDK.                         
}



TYPE
   {
    XPLMPluginID
    
    Each plug-in is identified by a unique integer ID.  This ID can be used to 
    disable or enable a plug-in, or discover what plug-in is 'running' at the 
    time.  A plug-in ID is unique within the currently running instance of 
    X-Plane unless plug-ins are reloaded.  Plug-ins may receive a different 
    unique ID each time they are loaded. 
    
    For persistent identification of plug-ins, use XPLMFindPluginBySignature in 
    XPLMUtiltiies.h 
    
    -1 indicates no plug-in.                                                    
   }
   XPLMPluginID = integer;
   PXPLMPluginID = ^XPLMPluginID;

CONST
    { No plugin.                                                                  }
   XPLM_NO_PLUGIN_ID    = (-1);

    { X-Plane itself                                                              }
   XPLM_PLUGIN_XPLANE   = (0);

    { The current XPLM revision is 2.10 (210).                                    }
   kXPLM_Version        = (210);

   {
    XPLMKeyFlags
    
    These bitfields define modifier keys in a platform independent way. When a 
    key is pressed, a series of messages are sent to your plugin.  The down 
    flag is set in the first of these messages, and the up flag in the last.  
    While the key is held down, messages are sent with neither to indicate that 
    the key is being held down as a repeated character. 
    
    The control flag is mapped to the control flag on Macintosh and PC.  
    Generally X-Plane uses the control key and not the command key on 
    Macintosh, providing a consistent interface across platforms that does not 
    necessarily match the Macintosh user interface guidelines.  There is not 
    yet a way for plugins to access the Macintosh control keys without using 
    #ifdefed code.                                                              
   }
TYPE
   XPLMKeyFlags = (
     { The shift key is down                                                       }
      xplm_ShiftFlag                           = 1
 
     { The option or alt key is down                                               }
     ,xplm_OptionAltFlag                       = 2
 
     { The control key is down*                                                    }
     ,xplm_ControlFlag                         = 4
 
     { The key is being pressed down                                               }
     ,xplm_DownFlag                            = 8
 
     { The key is being released                                                   }
     ,xplm_UpFlag                              = 16
 
   );
   PXPLMKeyFlags = ^XPLMKeyFlags;

{___________________________________________________________________________
 * ASCII CONTROL KEY CODES
 ___________________________________________________________________________}
{
   These definitions define how various control keys are mapped to ASCII key 
   codes. Not all key presses generate an ASCII value, so plugin code should 
   be prepared to see null characters come from the keyboard...this usually 
   represents a key stroke that has no equivalent ASCII, like a page-down 
   press.  Use virtual key codes to find these key strokes. ASCII key codes 
   take into account modifier keys; shift keys will affect capitals and 
   punctuation; control key combinations may have no vaild ASCII and produce 
   NULL.  To detect control-key combinations, use virtual key codes, not ASCII 
   keys.                                                                       
}



CONST
   XPLM_KEY_RETURN      = 13;

   XPLM_KEY_ESCAPE      = 27;

   XPLM_KEY_TAB         = 9;

   XPLM_KEY_DELETE      = 8;

   XPLM_KEY_LEFT        = 28;

   XPLM_KEY_RIGHT       = 29;

   XPLM_KEY_UP          = 30;

   XPLM_KEY_DOWN        = 31;

   XPLM_KEY_0           = 48;

   XPLM_KEY_1           = 49;

   XPLM_KEY_2           = 50;

   XPLM_KEY_3           = 51;

   XPLM_KEY_4           = 52;

   XPLM_KEY_5           = 53;

   XPLM_KEY_6           = 54;

   XPLM_KEY_7           = 55;

   XPLM_KEY_8           = 56;

   XPLM_KEY_9           = 57;

   XPLM_KEY_DECIMAL     = 46;

{___________________________________________________________________________
 * VIRTUAL KEY CODES
 ___________________________________________________________________________}
{
   These are cross-platform defines for every distinct keyboard press on the 
   computer. Every physical key on the keyboard has a virtual key code.  So 
   the "two" key on the  top row of the main keyboard has a different code 
   from the "two" key on the numeric key pad.  But the 'w' and 'W' character 
   are indistinguishable by virtual key code  because they are the same 
   physical key (one with and one without the shift key). 
   
   Use virtual key codes to detect keystrokes that do not have ASCII 
   equivalents, allow the user to map the numeric keypad separately from the 
   main keyboard, and detect control key and other modifier-key combinations 
   that generate ASCII control key sequences (many of which are not available 
   directly via character keys in the SDK).			 
   
   To assign virtual key codes we started with the Microsoft set but made some 
   additions and changes.  A few differences: 
   
   1. Modifier keys are not available as virtual key codes.  You cannot get 
   distinct modifier press and release messages.  Please do not try to use 
   modifier keys as regular keys; doing so will almost certainly interfere 
   with users' abilities to use the native x-plane key bindings. 
   
   2. Some keys that do not exist on both Mac and PC keyboards are removed. 
   
   3. Do not assume that the values of these keystrokes are interchangeable 
   with MS v-keys.                                                             
}



CONST
   XPLM_VK_BACK         = $08;

   XPLM_VK_TAB          = $09;

   XPLM_VK_CLEAR        = $0C;

   XPLM_VK_RETURN       = $0D;

   XPLM_VK_ESCAPE       = $1B;

   XPLM_VK_SPACE        = $20;

   XPLM_VK_PRIOR        = $21;

   XPLM_VK_NEXT         = $22;

   XPLM_VK_END          = $23;

   XPLM_VK_HOME         = $24;

   XPLM_VK_LEFT         = $25;

   XPLM_VK_UP           = $26;

   XPLM_VK_RIGHT        = $27;

   XPLM_VK_DOWN         = $28;

   XPLM_VK_SELECT       = $29;

   XPLM_VK_PRINT        = $2A;

   XPLM_VK_EXECUTE      = $2B;

   XPLM_VK_SNAPSHOT     = $2C;

   XPLM_VK_INSERT       = $2D;

   XPLM_VK_DELETE       = $2E;

   XPLM_VK_HELP         = $2F;

    { XPLM_VK_0 thru XPLM_VK_9 are the same as ASCII '0' thru '9' (0x30 - 0x39)   }
   XPLM_VK_0            = $30;

   XPLM_VK_1            = $31;

   XPLM_VK_2            = $32;

   XPLM_VK_3            = $33;

   XPLM_VK_4            = $34;

   XPLM_VK_5            = $35;

   XPLM_VK_6            = $36;

   XPLM_VK_7            = $37;

   XPLM_VK_8            = $38;

   XPLM_VK_9            = $39;

    { XPLM_VK_A thru XPLM_VK_Z are the same as ASCII 'A' thru 'Z' (0x41 - 0x5A)   }
   XPLM_VK_A            = $41;

   XPLM_VK_B            = $42;

   XPLM_VK_C            = $43;

   XPLM_VK_D            = $44;

   XPLM_VK_E            = $45;

   XPLM_VK_F            = $46;

   XPLM_VK_G            = $47;

   XPLM_VK_H            = $48;

   XPLM_VK_I            = $49;

   XPLM_VK_J            = $4A;

   XPLM_VK_K            = $4B;

   XPLM_VK_L            = $4C;

   XPLM_VK_M            = $4D;

   XPLM_VK_N            = $4E;

   XPLM_VK_O            = $4F;

   XPLM_VK_P            = $50;

   XPLM_VK_Q            = $51;

   XPLM_VK_R            = $52;

   XPLM_VK_S            = $53;

   XPLM_VK_T            = $54;

   XPLM_VK_U            = $55;

   XPLM_VK_V            = $56;

   XPLM_VK_W            = $57;

   XPLM_VK_X            = $58;

   XPLM_VK_Y            = $59;

   XPLM_VK_Z            = $5A;

   XPLM_VK_NUMPAD0      = $60;

   XPLM_VK_NUMPAD1      = $61;

   XPLM_VK_NUMPAD2      = $62;

   XPLM_VK_NUMPAD3      = $63;

   XPLM_VK_NUMPAD4      = $64;

   XPLM_VK_NUMPAD5      = $65;

   XPLM_VK_NUMPAD6      = $66;

   XPLM_VK_NUMPAD7      = $67;

   XPLM_VK_NUMPAD8      = $68;

   XPLM_VK_NUMPAD9      = $69;

   XPLM_VK_MULTIPLY     = $6A;

   XPLM_VK_ADD          = $6B;

   XPLM_VK_SEPARATOR    = $6C;

   XPLM_VK_SUBTRACT     = $6D;

   XPLM_VK_DECIMAL      = $6E;

   XPLM_VK_DIVIDE       = $6F;

   XPLM_VK_F1           = $70;

   XPLM_VK_F2           = $71;

   XPLM_VK_F3           = $72;

   XPLM_VK_F4           = $73;

   XPLM_VK_F5           = $74;

   XPLM_VK_F6           = $75;

   XPLM_VK_F7           = $76;

   XPLM_VK_F8           = $77;

   XPLM_VK_F9           = $78;

   XPLM_VK_F10          = $79;

   XPLM_VK_F11          = $7A;

   XPLM_VK_F12          = $7B;

   XPLM_VK_F13          = $7C;

   XPLM_VK_F14          = $7D;

   XPLM_VK_F15          = $7E;

   XPLM_VK_F16          = $7F;

   XPLM_VK_F17          = $80;

   XPLM_VK_F18          = $81;

   XPLM_VK_F19          = $82;

   XPLM_VK_F20          = $83;

   XPLM_VK_F21          = $84;

   XPLM_VK_F22          = $85;

   XPLM_VK_F23          = $86;

   XPLM_VK_F24          = $87;

    { The following definitions are extended and are not based on the Microsoft   }
    { key set.                                                                    }
   XPLM_VK_EQUAL        = $B0;

   XPLM_VK_MINUS        = $B1;

   XPLM_VK_RBRACE       = $B2;

   XPLM_VK_LBRACE       = $B3;

   XPLM_VK_QUOTE        = $B4;

   XPLM_VK_SEMICOLON    = $B5;

   XPLM_VK_BACKSLASH    = $B6;

   XPLM_VK_COMMA        = $B7;

   XPLM_VK_SLASH        = $B8;

   XPLM_VK_PERIOD       = $B9;

   XPLM_VK_BACKQUOTE    = $BA;

   XPLM_VK_ENTER        = $BB;

   XPLM_VK_NUMPAD_ENT   = $BC;

   XPLM_VK_NUMPAD_EQ    = $BD;

IMPLEMENTATION
END.
