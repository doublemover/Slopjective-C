#include "service_actor.h"

#include <cstdint>
#include <memory>
#include <limits>
#include <mutex>
#include <new>

// This archive member alone needs the Objective-C/Blocks runtime. The retained
// association is a copied native Block with a C++ owner; it never retains the
// actor object, so object destruction retires the mailbox without a cycle.
extern "C" id objc_getAssociatedObject(id, const void *);
extern "C" void objc_setAssociatedObject(id, const void *, id, uintptr_t);
extern "C" void *_Block_copy(const void *);
extern "C" void _Block_release(const void *);

namespace {
char actor_association_key;
char explicit_lifetime_key;
using LifetimeAssociation = uint64_t (^)(void);
struct ExplicitLifetime {
  const void *object{};
  uint64_t identity{};
  ~ExplicitLifetime() {
    objc3::services::forget_retired_actor_object(object, identity);
  }
};
using Association = objc3_actor_t *(^)(void);
objc3_actor_t *lookup_automatic(const void *object) {
  id association = objc_getAssociatedObject((id)object, &actor_association_key);
  return association ? objc3_actor_retain(((Association)association)()) : nullptr;
}
struct AutomaticActor {
  objc3_actor_t *actor{};
  explicit AutomaticActor(objc3_actor_t *value) : actor(value) {}
  ~AutomaticActor() {
    (void)objc3_actor_cancel(actor);
    objc3_actor_release(actor);
  }
};
objc3_service_status accept(objc3_actor_t *actor, objc3_executor_t *preferred,
                            objc3_actor_t **out_actor) {
  if (objc3::services::actor_is_closed(
          reinterpret_cast<objc3::services::Actor *>(actor)))
    return OBJC3_SERVICE_CLOSED;
  if (preferred && objc3_actor_executor_identity(actor) !=
                       objc3_executor_identity(preferred))
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_actor = objc3_actor_retain(actor);
  return *out_actor ? OBJC3_SERVICE_OK : OBJC3_SERVICE_NOT_READY;
}
} // namespace

namespace objc3::services {
uint64_t actor_object_lifetime(const void *object) {
  id association = objc_getAssociatedObject((id)object, &explicit_lifetime_key);
  return association ? ((LifetimeAssociation)association)() : 0;
}
objc3_service_status ensure_actor_object_lifetime(void *object, uint64_t *identity) {
  if ((*identity = actor_object_lifetime(object))) return OBJC3_SERVICE_OK;
  // The binding mutex serializes issuance. Never recycle a generation, even
  // when an old association is held by an outstanding autorelease lease.
  static uint64_t next = 0;
  if (next == std::numeric_limits<uint64_t>::max()) return OBJC3_SERVICE_NOT_READY;
  const auto generation = ++next;
  auto owner = std::make_shared<ExplicitLifetime>();
  owner->object = object;
  owner->identity = generation;
  LifetimeAssociation stack = ^{ return owner->identity; };
  struct BlockOwner {
    void *block;
    ~BlockOwner() { if (block) _Block_release(block); }
  } copied{_Block_copy((const void *)stack)};
  if (!copied.block) return OBJC3_SERVICE_OUT_OF_MEMORY;
  objc_setAssociatedObject((id)object, &explicit_lifetime_key, (id)copied.block, 0x301);
  *identity = generation;
  return OBJC3_SERVICE_OK;
}
} // namespace objc3::services

extern "C" objc3_service_status objc3_actor_get_or_create_for_object(
    void *object, objc3_executor_t *preferred, objc3_actor_t **out_actor) {
  if (!out_actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_actor = nullptr;
  if (!object) return OBJC3_SERVICE_INVALID_ARGUMENT;
  try {
  std::lock_guard<std::recursive_mutex> coordination(objc3::services::actor_object_binding_mutex());
  objc3::services::register_automatic_actor_lookup(lookup_automatic);
  objc3_actor_t *explicit_actor = nullptr;
  const auto explicit_status = objc3_actor_get_for_object(object, &explicit_actor);
  if (explicit_status == OBJC3_SERVICE_OK) {
    const auto status = accept(explicit_actor, preferred, out_actor);
    objc3_actor_release(explicit_actor);
    return status;
  }
  if (explicit_status != OBJC3_SERVICE_NOT_READY) return explicit_status;
    if (id association = objc_getAssociatedObject((id)object, &actor_association_key))
      return accept(((Association)association)(), preferred, out_actor);
    objc3_executor_t *destination = preferred;
    if (!destination) {
      const auto status = objc3_executor_get_global(&destination);
      if (status != OBJC3_SERVICE_OK) return status;
    }
    objc3_actor_t *actor = nullptr;
    const auto status = objc3_actor_create(destination, &actor);
    if (!preferred) objc3_executor_release(destination);
    if (status != OBJC3_SERVICE_OK) return status;
    std::shared_ptr<AutomaticActor> owner;
    try {
      owner = std::make_shared<AutomaticActor>(actor);
    } catch (...) {
      objc3_actor_release(actor);
      throw;
    }
    Association stack = ^{ return owner->actor; };
    auto *copied = _Block_copy((const void *)stack);
    if (!copied) return OBJC3_SERVICE_OUT_OF_MEMORY;
    // OBJC_ASSOCIATION_RETAIN: lookup keeps the association alive through its
    // normal autorelease lease; the source receiver must remain live per API.
    objc_setAssociatedObject((id)object, &actor_association_key, (id)copied, 0x301);
    _Block_release(copied);
    return accept(actor, preferred, out_actor);
  } catch (const std::bad_alloc &) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
}
