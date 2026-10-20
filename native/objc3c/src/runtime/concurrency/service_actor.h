#ifndef OBJC3_RUNTIME_SERVICE_ACTOR_H
#define OBJC3_RUNTIME_SERVICE_ACTOR_H

#include "service_internal.h"

#include <cstdint>

namespace objc3::services {

struct Actor;

using ActorJob = objc3_actor_job;
using ActorContextDestroy = objc3_actor_context_destroy;

constexpr uint32_t kActorServiceVersion = OBJC3_ACTOR_SERVICE_VERSION;

// Facts describe logical identities, not the physical worker thread. A
// different actor on the same executor still requires an actor hop.
using ActorHopFacts = objc3_actor_hop_facts;
using CurrentActorFacts = objc3_current_actor_facts;

objc3_service_status actor_create(objc3_executor_t *executor, Actor **out_actor);
Actor *actor_retain(Actor *actor);
void actor_release(Actor *actor);
uint64_t actor_identity(const Actor *actor) noexcept;
uint64_t actor_executor_identity(const Actor *actor) noexcept;
// Borrowed while the caller keeps the actor alive.
objc3_executor_t *actor_executor(Actor *actor) noexcept;

// On success the actor owns context and calls destroy exactly once, after the
// job returns or when cancellation discards the queued job. On failure the
// caller retains ownership. The result reports logical actor and executor hops
// at enqueue time.
objc3_service_status actor_enqueue(Actor *actor, ActorJob job, void *context,
                                   ActorContextDestroy destroy,
                                   ActorHopFacts *out_facts);

// close prevents new jobs; cancel also destroys all queued jobs. An active job
// finishes cooperatively. join waits for active work and all context cleanup.
objc3_service_status actor_close(Actor *actor);
objc3_service_status actor_cancel(Actor *actor);
objc3_service_status actor_join(Actor *actor);
objc3_service_status actor_current_facts(CurrentActorFacts *out_facts) noexcept;

} // namespace objc3::services

#endif
