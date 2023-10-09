#ifndef _XPLMDataAccess_h_
#define _XPLMDataAccess_h_

/*
 * Copyright 2005-2022 Laminar Research, Sandy Barbour and Ben Supnik All
 * rights reserved.  See license.txt for usage. X-Plane SDK Version: 4.0.0
 *
 */

/***************************************************************************
 * XPLMDataAccess
 ***************************************************************************/
/*
 * The data access API gives you a generic, flexible, high performance way to
 * read and write data to and from X-Plane and other plug-ins. For example,
 * this API allows you to read and set the nav radios, get the plane location,
 * determine the current effective graphics frame rate, etc.
 * 
 * The data access APIs are the way that you read and write data from the sim
 * as well as other plugins.
 * 
 * The API works using opaque data references. A data reference is a source of
 * data; you do not know where it comes from, but once you have it you can
 * read the data quickly and possibly write it.
 * 
 * Dataref Lookup
 * --------------
 * 
 * Data references are identified by verbose, permanent string names; by
 * convention these names use path separators to form a hierarchy of datarefs,
 * e.g. (sim/cockpit/radios/nav1_freq_hz). The actual opaque numeric value of
 * the data reference, as returned by the XPLM API, is implementation defined
 * and changes each time X-Plane is launched; therefore you need to look up
 * the dataref by path every time your plugin runs.
 * 
 * The task of looking up a data reference is relatively expensive; look up
 * your data references once based on the verbose path strings, and save the
 * opaque data reference value for the duration of your plugin's operation.
 * Reading and writing data references is relatively fast (the cost is
 * equivalent to two function calls through function pointers).
 * 
 * X-Plane publishes many thousands of datarefs; a complete list may be found
 * in the reference section of the SDK online documentation (from the SDK home
 * page, choose Documentation) and the Resources/plugins/DataRefs.txt file.
 * 
 * Dataref Types
 * -------------
 * 
 * A note on typing: you must know the correct data type to read and write.
 * APIs are provided for reading and writing data in a number of ways. You can
 * also double check the data type for a dataref. Automatic type conversion is
 * not done for you.
 * 
 * Dataref types are a set, e.g. a dataref can be more than one type.  When
 * this happens, you can choose which API you want to use to read.  For
 * example, it is not uncommon for a dataref to be available both as float and
 * double.  This means you can use either XPLMGetDatad or XPLMGetDataf to read
 * it.
 * 
 * Creating New Datarefs
 * ---------------------
 * 
 * X-Plane provides datarefs that come with the sim, but plugins can also
 * create their own datarefs.  A plugin creates a dataref by registering
 * function callbacks to read and write the dataref.  The XPLM will call your
 * plugin each time some other plugin (or X-Plane) tries to read or write the
 * dataref.  You must provide a read (and optional write) callback for each
 * data type you support.
 * 
 * A note for plugins sharing data with other plugins: the load order of
 * plugins is not guaranteed. To make sure that every plugin publishing data
 * has published their data references before other plugins try to subscribe,
 * publish your data references in your start routine but resolve others'
 * datarefs the first time your 'enable' routine is called, or the first time
 * they are needed in code.
 * 
 * When a plugin that created a dataref is unloaded, it becomes "orphaned". 
 * The dataref handle continues to be usable, but the dataref is not writable,
 * and reading it will always return 0 (or 0 items for arrays).  If the plugin
 * is reloaded and re-registers the dataref, the handle becomes un-orphaned
 * and works again.
 * 
 * Introspection: Finding All Datarefs
 * -----------------------------------
 * 
 * In the XPLM400 API, it is possible for a plugin to iterate the entire set
 * of datarefs. This functionality is meant only for "tool" add-ons, like
 * dataref browsers; normally all add-ons  should find the dataref they want
 * by name.
 * 
 * Because datarefs are never destroyed during a run of the simulator (they
 * are orphaned when their providing plugin goes away until a new one
 * re-registers the dataref), the set of  datarefs for a given run of X-Plane
 * can be enumerated by index. A plugin that wants to find all new datarefs
 * can use XPLMCountDataRefs to find the number of datarefs and iterate only 
 * the ones with higher index numbers than the last iteration.
 * 
 * Plugins can also receive notifications when datarefs are registered; see
 * the XPLMPlugin feature-enable API for more details.    
 *
 */

#include "XPLMDefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * READING AND WRITING DATA
 ***************************************************************************/
/*
 * These routines allow you to access data from within X-Plane and sometimes
 * modify it.
 *
 */


/*
 * XPLMDataRef
 * 
 * A dataref is an opaque handle to data provided by the simulator or another
 * plugin. It uniquely identifies one variable (or array of variables) over
 * the lifetime of your plugin. You never hard code these values; you always
 * get them from XPLMFindDataRef.
 *
 */
typedef void * XPLMDataRef;

/*
 * XPLMDataTypeID
 * 
 * This is an enumeration that defines the type of the data behind a data
 * reference. This allows you to sanity check that the data type matches what
 * you expect. But for the most part, you will know the type of data you are
 * expecting from the online documentation.
 * 
 * Data types each take a bit field; it is legal to have a single dataref be
 * more than one type of data.  Whe this happens, you can pick any matching
 * get/set API.
 *
 */
enum {
    /* Data of a type the current XPLM doesn't do.                                */
    xplmType_Unknown                         = 0,

    /* A single 4-byte integer, native endian.                                    */
    xplmType_Int                             = 1,

    /* A single 4-byte float, native endian.                                      */
    xplmType_Float                           = 2,

    /* A single 8-byte double, native endian.                                     */
    xplmType_Double                          = 4,

    /* An array of 4-byte floats, native endian.                                  */
    xplmType_FloatArray                      = 8,

    /* An array of 4-byte integers, native endian.                                */
    xplmType_IntArray                        = 16,

    /* A variable block of data.                                                  */
    xplmType_Data                            = 32,


};
typedef int XPLMDataTypeID;

#if defined(XPLM400)
/*
 * XPLMCountDataRefs
 * 
 * Returns the total number of datarefs that have been registered in X-Plane.
 *
 */
XPLM_API int        XPLMCountDataRefs(void);
#endif /* XPLM400 */

#if defined(XPLM400)
/*
 * XPLMGetDataRefsByIndex
 * 
 * Given an offset and count, this function will return an array of
 * XPLMDataRefs in that range.  The offset/count idiom is useful for things
 * like pagination.
 *
 */
XPLM_API void       XPLMGetDataRefsByIndex(
                         int                  offset,
                         int                  count,
                         XPLMDataRef *        outDataRefs);
#endif /* XPLM400 */

#if defined(XPLM400)
/*
 * XPLMDataRefInfo_t
 * 
 * The XPLMDataRefInfo_t structure contains all of the information about a
 * single data ref.  The structure can be expanded in future SDK APIs to
 * include more features. Always set the structSize member to the size of 
 * your struct in bytes!
 *
 */
typedef struct {
    /* Used to inform XPLMGetDatarefInfo() of the SDK version you compiled        *
     * against; should always be set to sizeof(XPLMDataRefInfo_t)                 */
     int                       structSize;
    /* The full name/path of the data ref                                         */
     const char *              name;
     XPLMDataTypeID            type;
    /* TRUE if the data ref permits writing to it. FALSE if it's read-only.       */
     int                       writable;
    /* The handle to the plugin that registered this dataref.                     */
     XPLMPluginID              owner;
} XPLMDataRefInfo_t;
#endif /* XPLM400 */

#if defined(XPLM400)
/*
 * XPLMGetDataRefInfo
 * 
 * Give a data ref, this routine returns a populated struct containing the
 * available information about the dataref.
 *
 */
XPLM_API void       XPLMGetDataRefInfo(
                         XPLMDataRef          inDataRef,
                         XPLMDataRefInfo_t *  outInfo);
#endif /* XPLM400 */

/*
 * XPLMFindDataRef
 * 
 * Given a C-style string that names the dataref, this routine looks up the
 * actual opaque XPLMDataRef that you use to read and write the data. The
 * string names for datarefs are published on the X-Plane SDK web site.
 * 
 * This function returns NULL if the dataref cannot be found.
 * 
 * NOTE: this function is relatively expensive; save the XPLMDataRef this
 * function returns for future use. Do not look up your dataref by string
 * every time you need to read or write it.
 *
 */
XPLM_API XPLMDataRef XPLMFindDataRef(
                         const char *         inDataRefName);

/*
 * XPLMCanWriteDataRef
 * 
 * Given a dataref, this routine returns true if you can successfully set the
 * data, false otherwise. Some datarefs are read-only.
 * 
 * NOTE: even if a dataref is marked writable, it may not act writable.  This
 * can happen for datarefs that X-Plane writes to on every frame of
 * simulation.  In some cases, the dataref is writable but you have to set a
 * separate "override" dataref to 1 to stop X-Plane from writing it.
 *
 */
XPLM_API int        XPLMCanWriteDataRef(
                         XPLMDataRef          inDataRef);

/*
 * XPLMIsDataRefGood
 * 
 * This function returns true if the passed in handle is a valid dataref that
 * is not orphaned.
 * 
 * Note: there is normally no need to call this function; datarefs returned by
 * XPLMFindDataRef remain valid (but possibly orphaned) unless there is a
 * complete plugin reload (in which case your plugin is reloaded anyway).
 * Orphaned datarefs can be safely read and return 0. Therefore you never need
 * to call XPLMIsDataRefGood to 'check' the safety of a dataref.
 * (XPLMIsDataRefGood performs some slow checking of the handle validity, so
 * it has a perormance cost.)
 *
 */
XPLM_API int        XPLMIsDataRefGood(
                         XPLMDataRef          inDataRef);

/*
 * XPLMGetDataRefTypes
 * 
 * This routine returns the types of the dataref for accessor use. If a
 * dataref is available in multiple data types, the bit-wise OR of these types
 * will be returned.
 *
 */
XPLM_API XPLMDataTypeID XPLMGetDataRefTypes(
                         XPLMDataRef          inDataRef);

/***************************************************************************
 * DATA ACCESSORS
 ***************************************************************************/
/*
 * These routines read and write the data references. For each supported data
 * type there is a reader and a writer.
 * 
 * If the dataref is orphaned, the plugin that provides it is disabled or
 * there is a type mismatch, the functions that read data will return 0 as a
 * default value or not modify the passed in memory. The plugins that write
 * data will not write under these circumstances or if the dataref is
 * read-only.
 * 
 * NOTE: to keep the overhead of reading datarefs low, these routines do not
 * do full validation of a dataref; passing a junk value for a dataref can
 * result in crashing the sim. The get/set APIs do check for NULL.
 * 
 * For array-style datarefs, you specify the number of items to read/write and
 * the offset into the array; the actual number of items read or written is
 * returned. This may be less the number requested to prevent an
 * array-out-of-bounds error.
 *
 */


/*
 * XPLMGetDatai
 * 
 * Read an integer dataref and return its value. The return value is the
 * dataref value or 0 if the dataref is NULL or the plugin is disabled.
 *
 */
XPLM_API int        XPLMGetDatai(
                         XPLMDataRef          inDataRef);

/*
 * XPLMSetDatai
 * 
 * Write a new value to an integer dataref. This routine is a no-op if the
 * plugin publishing the dataref is disabled, the dataref is NULL, or the
 * dataref is not writable.
 *
 */
XPLM_API void       XPLMSetDatai(
                         XPLMDataRef          inDataRef,
                         int                  inValue);

/*
 * XPLMGetDataf
 * 
 * Read a single precision floating point dataref and return its value. The
 * return value is the dataref value or 0.0 if the dataref is NULL or the
 * plugin is disabled.
 *
 */
XPLM_API float      XPLMGetDataf(
                         XPLMDataRef          inDataRef);

/*
 * XPLMSetDataf
 * 
 * Write a new value to a single precision floating point dataref. This
 * routine is a no-op if the plugin publishing the dataref is disabled, the
 * dataref is NULL, or the dataref is not writable.
 *
 */
XPLM_API void       XPLMSetDataf(
                         XPLMDataRef          inDataRef,
                         float                inValue);

/*
 * XPLMGetDatad
 * 
 * Read a double precision floating point dataref and return its value. The
 * return value is the dataref value or 0.0 if the dataref is NULL or the
 * plugin is disabled.
 *
 */
XPLM_API double     XPLMGetDatad(
                         XPLMDataRef          inDataRef);

/*
 * XPLMSetDatad
 * 
 * Write a new value to a double precision floating point dataref. This
 * routine is a no-op if the plugin publishing the dataref is disabled, the
 * dataref is NULL, or the dataref is not writable.
 *
 */
XPLM_API void       XPLMSetDatad(
                         XPLMDataRef          inDataRef,
                         double               inValue);

/*
 * XPLMGetDatavi
 * 
 * Read a part of an integer array dataref. If you pass NULL for outValues,
 * the routine will return the size of the array, ignoring inOffset and inMax.
 * 
 * If outValues is not NULL, then up to inMax values are copied from the
 * dataref into outValues, starting at inOffset in the dataref. If inMax +
 * inOffset is larger than the size of the dataref, less than inMax values
 * will be copied. The number of values copied is returned.
 * 
 * Note: the semantics of array datarefs are entirely implemented by the
 * plugin (or X-Plane) that provides the dataref, not the SDK itself; the
 * above description is how these datarefs are intended to work, but a rogue
 * plugin may have different behavior.
 *
 */
XPLM_API int        XPLMGetDatavi(
                         XPLMDataRef          inDataRef,
                         int *                outValues,              /* Can be NULL */
                         int                  inOffset,
                         int                  inMax);

/*
 * XPLMSetDatavi
 * 
 * Write part or all of an integer array dataref. The values passed by
 * inValues are written into the dataref starting at inOffset. Up to inCount
 * values are written; however if the values would write past the end of the
 * dataref array, then fewer values are written.
 * 
 * Note: the semantics of array datarefs are entirely implemented by the
 * plugin (or X-Plane) that provides the dataref, not the SDK itself; the
 * above description is how these datarefs are intended to work, but a rogue
 * plugin may have different behavior.
 *
 */
XPLM_API void       XPLMSetDatavi(
                         XPLMDataRef          inDataRef,
                         int *                inValues,
                         int                  inoffset,
                         int                  inCount);

/*
 * XPLMGetDatavf
 * 
 * Read a part of a single precision floating point array dataref. If you pass
 * NULL for outValues, the routine will return the size of the array, ignoring
 * inOffset and inMax.
 * 
 * If outValues is not NULL, then up to inMax values are copied from the
 * dataref into outValues, starting at inOffset in the dataref. If inMax +
 * inOffset is larger than the size of the dataref, less than inMax values
 * will be copied. The number of values copied is returned.
 * 
 * Note: the semantics of array datarefs are entirely implemented by the
 * plugin (or X-Plane) that provides the dataref, not the SDK itself; the
 * above description is how these datarefs are intended to work, but a rogue
 * plugin may have different behavior.
 *
 */
XPLM_API int        XPLMGetDatavf(
                         XPLMDataRef          inDataRef,
                         float *              outValues,              /* Can be NULL */
                         int                  inOffset,
                         int                  inMax);

/*
 * XPLMSetDatavf
 * 
 * Write part or all of a single precision floating point array dataref. The
 * values passed by inValues are written into the dataref starting at
 * inOffset. Up to inCount values are written; however if the values would
 * write past the end of the dataref array, then fewer values are written.
 * 
 * Note: the semantics of array datarefs are entirely implemented by the
 * plugin (or X-Plane) that provides the dataref, not the SDK itself; the
 * above description is how these datarefs are intended to work, but a rogue
 * plugin may have different behavior.
 *
 */
XPLM_API void       XPLMSetDatavf(
                         XPLMDataRef          inDataRef,
                         float *              inValues,
                         int                  inoffset,
                         int                  inCount);

/*
 * XPLMGetDatab
 * 
 * Read a part of a byte array dataref. If you pass NULL for outValues, the
 * routine will return the size of the array, ignoring inOffset and inMax.
 * 
 * If outValues is not NULL, then up to inMax values are copied from the
 * dataref into outValues, starting at inOffset in the dataref. If inMax +
 * inOffset is larger than the size of the dataref, less than inMax values
 * will be copied. The number of values copied is returned.
 * 
 * Note: the semantics of array datarefs are entirely implemented by the
 * plugin (or X-Plane) that provides the dataref, not the SDK itself; the
 * above description is how these datarefs are intended to work, but a rogue
 * plugin may have different behavior.
 *
 */
XPLM_API int        XPLMGetDatab(
                         XPLMDataRef          inDataRef,
                         void *               outValue,               /* Can be NULL */
                         int                  inOffset,
                         int                  inMaxBytes);

/*
 * XPLMSetDatab
 * 
 * Write part or all of a byte array dataref. The values passed by inValues
 * are written into the dataref starting at inOffset. Up to inCount values are
 * written; however if the values would write "off the end" of the dataref
 * array, then fewer values are written.
 * 
 * Note: the semantics of array datarefs are entirely implemented by the
 * plugin (or X-Plane) that provides the dataref, not the SDK itself; the
 * above description is how these datarefs are intended to work, but a rogue
 * plugin may have different behavior.
 *
 */
XPLM_API void       XPLMSetDatab(
                         XPLMDataRef          inDataRef,
                         void *               inValue,
                         int                  inOffset,
                         int                  inLength);

/***************************************************************************
 * PUBLISHING YOUR PLUGIN'S DATA
 ***************************************************************************/
/*
 * These functions allow you to create data references that other plug-ins and
 * X-Plane can access via the above data access APIs. Data references
 * published by other plugins operate the same as ones published by X-Plane in
 * all manners except that your data reference will not be available to other
 * plugins if/when your plugin is disabled.
 * 
 * You share data by registering data provider callback functions. When a
 * plug-in requests your data, these callbacks are then called. You provide
 * one callback to return the value when a plugin 'reads' it and another to
 * change the value when a plugin 'writes' it.
 * 
 * Important: you must pick a prefix for your datarefs other than "sim/" -
 * this prefix is reserved for X-Plane. The X-Plane SDK website contains a
 * registry where authors can select a unique first word for dataref names, to
 * prevent dataref collisions between plugins.
 *
 */


/*
 * XPLMGetDatai_f
 * 
 * Data provider function pointers.
 * 
 * These define the function pointers you provide to get or set data. Note
 * that you are passed a generic pointer for each one. This is the same
 * pointer you pass in your register routine; you can use it to locate plugin
 * variables, etc.
 * 
 * The semantics of your callbacks are the same as the dataref accessors above
 * - basically routines like XPLMGetDatai are just pass-throughs from a caller
 * to your plugin. Be particularly mindful in implementing array dataref
 * read-write accessors; you are responsible for avoiding overruns, supporting
 * offset read/writes, and handling a read with a NULL buffer.
 *
 */
typedef int (* XPLMGetDatai_f)(
                         void *               inRefcon);

/*
 * XPLMSetDatai_f
 *
 */
typedef void (* XPLMSetDatai_f)(
                         void *               inRefcon,
                         int                  inValue);

/*
 * XPLMGetDataf_f
 *
 */
typedef float (* XPLMGetDataf_f)(
                         void *               inRefcon);

/*
 * XPLMSetDataf_f
 *
 */
typedef void (* XPLMSetDataf_f)(
                         void *               inRefcon,
                         float                inValue);

/*
 * XPLMGetDatad_f
 *
 */
typedef double (* XPLMGetDatad_f)(
                         void *               inRefcon);

/*
 * XPLMSetDatad_f
 *
 */
typedef void (* XPLMSetDatad_f)(
                         void *               inRefcon,
                         double               inValue);

/*
 * XPLMGetDatavi_f
 *
 */
typedef int (* XPLMGetDatavi_f)(
                         void *               inRefcon,
                         int *                outValues,              /* Can be NULL */
                         int                  inOffset,
                         int                  inMax);

/*
 * XPLMSetDatavi_f
 *
 */
typedef void (* XPLMSetDatavi_f)(
                         void *               inRefcon,
                         int *                inValues,
                         int                  inOffset,
                         int                  inCount);

/*
 * XPLMGetDatavf_f
 *
 */
typedef int (* XPLMGetDatavf_f)(
                         void *               inRefcon,
                         float *              outValues,              /* Can be NULL */
                         int                  inOffset,
                         int                  inMax);

/*
 * XPLMSetDatavf_f
 *
 */
typedef void (* XPLMSetDatavf_f)(
                         void *               inRefcon,
                         float *              inValues,
                         int                  inOffset,
                         int                  inCount);

/*
 * XPLMGetDatab_f
 *
 */
typedef int (* XPLMGetDatab_f)(
                         void *               inRefcon,
                         void *               outValue,               /* Can be NULL */
                         int                  inOffset,
                         int                  inMaxLength);

/*
 * XPLMSetDatab_f
 *
 */
typedef void (* XPLMSetDatab_f)(
                         void *               inRefcon,
                         void *               inValue,
                         int                  inOffset,
                         int                  inLength);

/*
 * XPLMRegisterDataAccessor
 * 
 * This routine creates a new item of data that can be read and written. Pass
 * in the data's full name for searching, the type(s) of the data for
 * accessing, and whether the data can be written to. For each data type you
 * support, pass in a read accessor function and a write accessor function if
 * necessary. Pass NULL for data types you do not support or write accessors
 * if you are read-only.
 * 
 * You are returned a dataref for the new item of data created. You can use
 * this dataref to unregister your data later or read or write from it.
 *
 */
XPLM_API XPLMDataRef XPLMRegisterDataAccessor(
                         const char *         inDataName,
                         XPLMDataTypeID       inDataType,
                         int                  inIsWritable,
                         XPLMGetDatai_f       inReadInt,
                         XPLMSetDatai_f       inWriteInt,
                         XPLMGetDataf_f       inReadFloat,
                         XPLMSetDataf_f       inWriteFloat,
                         XPLMGetDatad_f       inReadDouble,
                         XPLMSetDatad_f       inWriteDouble,
                         XPLMGetDatavi_f      inReadIntArray,
                         XPLMSetDatavi_f      inWriteIntArray,
                         XPLMGetDatavf_f      inReadFloatArray,
                         XPLMSetDatavf_f      inWriteFloatArray,
                         XPLMGetDatab_f       inReadData,
                         XPLMSetDatab_f       inWriteData,
                         void *               inReadRefcon,
                         void *               inWriteRefcon);

/*
 * XPLMUnregisterDataAccessor
 * 
 * Use this routine to unregister any data accessors you may have registered.
 * You unregister a dataref by the XPLMDataRef you get back from registration.
 * Once you unregister a dataref, your function pointer will not be called
 * anymore.
 *
 */
XPLM_API void       XPLMUnregisterDataAccessor(
                         XPLMDataRef          inDataRef);

/***************************************************************************
 * SHARING DATA BETWEEN MULTIPLE PLUGINS
 ***************************************************************************/
/*
 * The data reference registration APIs from the previous section allow a
 * plugin to publish data in a one-owner manner; the plugin that publishes the
 * data reference owns the real memory that the dataref uses. This is
 * satisfactory for most cases, but there are also cases where plugins need to
 * share actual data.
 * 
 * With a shared data reference, no one plugin owns the actual memory for the
 * data reference; the plugin SDK allocates that for you. When the first
 * plugin asks to 'share' the data, the memory is allocated. When the data is
 * changed, every plugin that is sharing the data is notified.
 * 
 * Shared data references differ from the 'owned' data references from the
 * previous section in a few ways:
 * 
 * * With shared data references, any plugin can create the data reference;
 *   with owned plugins one plugin must create the data reference and others
 *   subscribe. (This can be a problem if you don't know which set of plugins
 *   will be present).
 * 
 * * With shared data references, every plugin that is sharing the data is
 *   notified when the data is changed. With owned data references, only the
 *   one owner is notified when the data is changed.
 * 
 * * With shared data references, you cannot access the physical memory of the
 *   data reference; you must use the XPLMGet... and XPLMSet... APIs. With an
 *   owned data reference, the one owning data reference can manipulate the
 *   data reference's memory in any way it sees fit.
 * 
 * Shared data references solve two problems: if you need to have a data
 * reference used by several plugins but do not know which plugins will be
 * installed, or if all plugins sharing data need to be notified when that
 * data is changed, use shared data references.
 *
 */


/*
 * XPLMDataChanged_f
 * 
 * An XPLMDataChanged_f is a callback that the XPLM calls whenever any other
 * plug-in modifies shared data. A refcon you provide is passed back to help
 * identify which data is being changed. In response, you may want to call one
 * of the XPLMGetDataxxx routines to find the new value of the data.
 *
 */
typedef void (* XPLMDataChanged_f)(
                         void *               inRefcon);

/*
 * XPLMShareData
 * 
 * This routine connects a plug-in to shared data, creating the shared data if
 * necessary. inDataName is a standard path for the dataref, and inDataType
 * specifies the type. This function will create the data if it does not
 * exist. If the data already exists but the type does not match, an error is
 * returned, so it is important that plug-in authors collaborate to establish
 * public standards for shared data.
 * 
 * If a notificationFunc is passed in and is not NULL, that notification
 * function will be called whenever the data is modified. The notification
 * refcon will be passed to it. This allows your plug-in to know which shared
 * data was changed if multiple shared data are handled by one callback, or if
 * the plug-in does not use global variables.
 * 
 * A one is returned for successfully creating or finding the shared data; a
 * zero if the data already exists but is of the wrong type.
 *
 */
XPLM_API int        XPLMShareData(
                         const char *         inDataName,
                         XPLMDataTypeID       inDataType,
                         XPLMDataChanged_f    inNotificationFunc,
                         void *               inNotificationRefcon);

/*
 * XPLMUnshareData
 * 
 * This routine removes your notification function for shared data. Call it
 * when done with the data to stop receiving change notifications. Arguments
 * must match XPLMShareData. The actual memory will not necessarily be freed,
 * since other plug-ins could be using it.
 *
 */
XPLM_API int        XPLMUnshareData(
                         const char *         inDataName,
                         XPLMDataTypeID       inDataType,
                         XPLMDataChanged_f    inNotificationFunc,
                         void *               inNotificationRefcon);

#ifdef __cplusplus
}
#endif

#endif
