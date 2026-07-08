#ifndef __Z64RECOMP_API_H__
#define __Z64RECOMP_API_H__

#include "z64actor.h"
#include "modding.h"

typedef u32 ActorExtensionId;

// Registers an actor data extension of a given size that applies to a single type of actor specified by `actor_id`.
// Returns a handle that can be passed to `z64recomp_get_extended_actor_data` along with an Actor* to get the data.
// This must be called before any actors have spawned. It is recommended to call this from a `recomp_on_init` callback.
RECOMP_IMPORT("*", ActorExtensionId z64recomp_extend_actor(s16 actor_id, u32 size));

// Registers an actor data extension of the given size that applies to all actors in the game.
// Returns a handle that can be passed to `z64recomp_get_extended_actor_data` along with an Actor* to get the data.
// This must be called before any actors have spawned. It is recommended to call this from a `recomp_on_init` callback.
RECOMP_IMPORT("*", ActorExtensionId z64recomp_extend_actor_all(u32 size));

// Returns a pointer to the extended actor data associated with a given extension for the provided Actor*.
RECOMP_IMPORT("*", void* z64recomp_get_extended_actor_data(Actor* actor, ActorExtensionId extension));

// Returns the spawn index for a given actor in the current scene. This is an incremental value that starts at 0 when a scene
// is loaded and counts up by one for every actor spawned.
// Note that this may not be deterministic for scene spawn list actors, as other mods could potentially spawn additional actors
// before the scene's spawn list is processed.
RECOMP_IMPORT("*", u32 z64recomp_get_actor_spawn_index(Actor* actor));

#endif
