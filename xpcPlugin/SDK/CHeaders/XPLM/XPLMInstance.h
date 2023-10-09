#ifndef _XPLMInstance_h_
#define _XPLMInstance_h_

/*
 * Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
 * rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
 *
 */

/***************************************************************************
 * XPLMInstance
 ***************************************************************************/
/*
 * This API provides instanced drawing of X-Plane objects (.obj files). In
 * contrast to old drawing APIs, which required you to draw your own objects
 * per-frame, the instancing API allows you to simply register an OBJ for
 * drawing, then move or manipulate it later (as needed).
 * 
 * This provides one tremendous benefit: it keeps all dataref operations for
 * your object in one place. Because datarefs access may be done from the main
 * thread only, allowing dataref access anywhere is a serious performance
 * bottleneck for the simulator - the whole simulator has to pause and wait
 * for each dataref access. This performance penalty will only grow worse as
 * X-Plane moves toward an ever more heavily multithreaded engine.
 * 
 * The instancing API allows X-Plane to isolate all dataref manipulations for
 * all plugin object drawing to one place, potentially providing huge
 * performance gains.
 * 
 * Here's how it works:
 * 
 * When an instance is created, it provides a list of all datarefs you want to
 * manipulate for the OBJ in the future. This list of datarefs replaces the
 * ad-hoc collections of dataref objects previously used by art assets. Then,
 * per-frame, you can manipulate the instance by passing in a "block" of
 * packed floats representing the current values of the datarefs for your
 * instance. (Note that the ordering of this set of packed floats must exactly
 * match the ordering of the datarefs when you created your instance.)
 *
 */

#include "XPLMDefs.h"
#include "XPLMScenery.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * Instance Creation and Destruction
 ***************************************************************************/
/*
 * Registers and unregisters instances.
 *
 */


/*
 * XPLMInstanceRef
 * 
 * An opaque handle to an instance.
 *
 */
typedef void * XPLMInstanceRef;

/*
 * XPLMCreateInstance
 * 
 * XPLMCreateInstance creates a new instance, managed by your plug-in, and
 * returns a handle to the instance. A few important requirements:
 * 
 * * The object passed in must be fully loaded and returned from the XPLM
 *   before you can create your instance; you cannot pass a null obj ref, nor
 *   can you change the ref later.
 * 
 * * If you use any custom datarefs in your object, they must be registered
 *   before the object is loaded. This is true even if their data will be
 *   provided via the instance dataref list.
 * 
 * * The instance dataref array must be a valid pointer to a null-terminated
 *   array.  That is, if you do not want any datarefs, you must pass a pointer
 *   to a one-element array containing a null item.  You cannot pass null for
 *   the array itself.
 *
 */
XPLM_API XPLMInstanceRef XPLMCreateInstance(
                         XPLMObjectRef        obj,
                         const char **        datarefs);

/*
 * XPLMDestroyInstance
 * 
 * XPLMDestroyInstance destroys and deallocates your instance; once called,
 * you are still responsible for releasing the OBJ ref.
 * 
 * Tip: you can release your OBJ ref after you call XPLMCreateInstance as long
 * as you never use it again; the instance will maintain its own reference to
 * the OBJ and the object OBJ be deallocated when the instance is destroyed.
 *
 */
XPLM_API void       XPLMDestroyInstance(
                         XPLMInstanceRef      instance);

/***************************************************************************
 * Instance Manipulation
 ***************************************************************************/

/*
 * XPLMInstanceSetPosition
 * 
 * Updates both the position of the instance and all datarefs you registered
 * for it.  Call this from a flight loop callback or UI callback.
 * 
 * __DO_NOT__ call XPLMInstanceSetPosition from a drawing callback; the whole
 * point of instancing is that you do not need any drawing callbacks. Setting
 * instance data from a drawing callback may have undefined consequences, and
 * the drawing callback hurts FPS unnecessarily.  
 * 
 * The memory pointed to by the data pointer must be large enough to hold one
 * float for every dataref you have registered, and must contain valid
 * floating point data.
 * 
 * BUG: before X-Plane 11.50, if you have no dataref registered, you must
 * still pass a valid pointer for data and not null.
 *
 */
XPLM_API void       XPLMInstanceSetPosition(
                         XPLMInstanceRef      instance,
                         const XPLMDrawInfo_t * new_position,
                         const float *        data);

#ifdef __cplusplus
}
#endif

#endif
