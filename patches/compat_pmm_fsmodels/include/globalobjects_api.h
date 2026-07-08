#ifndef __GLOBAL_OBJECTS_API__
#define __GLOBAL_OBJECTS_API__

#include "global.h"
#include "modding.h"

#define YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME "yazmt_mm_global_objects"

typedef void *GlobalObjectsSegmentMap[0x10];

// Helper macro for converting segmented pointers to global pointers.
//
// If you are trying to get a pointer to a vanilla object, you should grab a global object pointer
// with GlobalObjects_getGlobalObject first.
//
// You generally should NOT use this for Gfx pointers since their DL commands need to be handled.
//
// For a Gfx global pointer, use GlobalObjects_getGlobalGfxPtr instead.
#define SEGMENTED_TO_GLOBAL_PTR(globalObj, segmentedPtr) ((void *)((uintptr_t)globalObj + SEGMENT_OFFSET(segmentedPtr)))

// Converts segmented pointers in the passed in display list to the specified mappings in the
// segments parameter.
//
// For example, if segments[0x06] == 0x81010000, then a command with the pointer 0x06005000
// will be set to 0x81010000 + 0x00005000 = 0x81015000.
//
// If a segmented address is encountered and its entry in segments is set to NULL, the command will
// be left unchanged.
//
// If a gSPBranchList or gSPDisplayList command is encountered, this function will automatically jump to
// the DL pointed to by the respective command if there is a valid entry in segments for it.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, void GlobalObjects_rebaseDL(Gfx *globalPtr, GlobalObjectsSegmentMap segments));

// Wrapper for GlobalObjects_rebaseDL that takes a segmented pointer into the passed in object and uses
// address of the passed in object as the new base.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, void GlobalObjects_globalizeSegmentedDL(void *obj, Gfx *segmentedPtr));

// Converts a LodLimb skeleton's segmented limb pointers and limb display list pointers to global pointers.
//
// Objects that use LodLimb skeletons include Link's forms, Kafei, and so on.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, void GlobalObjects_globalizeLodLimbSkeleton(void *obj, FlexSkeletonHeader *skel));

// Converts a StandardLimb skeleton's segmented limb pointers and limb display list pointer to global pointers.
//
// Most NPCs use this skeleton type.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, void GlobalObjects_globalizeStandardLimbSkeleton(void *obj, FlexSkeletonHeader *skel));

// Gets a pointer to the start of the globally loaded object with the passed in ID.
//
// Returns NULL if an invalid ID is passed in.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, void *GlobalObjects_getGlobalObject(ObjectId id));

// Helper function for grabbing the object ID of the object stored at vromStart.
//
// Returns true if there is an object that starts at the given address, and the object id is written to out.
//
// Returns false if there is no object ID associated with the given vrom address and leaves out unmodified.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, bool GlobalObjects_getObjectIdFromVrom(uintptr_t vromStart, ObjectId *out));

// Gets the global object associated with a given vrom address.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, void *GlobalObjects_getGlobalObjectFromVrom(uintptr_t vromStart));

// Gets the global Gfx pointer to a display list. Repoints commands that share a segment with the passed in pointer.
RECOMP_IMPORT(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, Gfx *GlobalObjects_getGlobalGfxPtr(ObjectId id, Gfx *segmentedPtr));

// Helper define for declaring a callback to the onReady event
#define GLOBAL_OBJECTS_CALLBACK_ON_READY RECOMP_CALLBACK(YAZMT_Z64_GLOBAL_OBJECTS_MOD_NAME, onReady)

#endif
