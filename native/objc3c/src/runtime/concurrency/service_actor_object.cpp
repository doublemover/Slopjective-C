#include "service_actor.h"

#include <map>
#include <mutex>
#include <new>
#include <utility>

// Keep object pins in their own archive member. Value-only task/actor clients
// continue to link without the Objective-C runtime.
extern "C" void *objc3_actor_object_retain(void *) __asm__("objc_retain");
extern "C" void objc3_actor_object_release(void *) __asm__("objc_release");

namespace {
struct ObjectBinding {
  void *object{};
  objc3_actor_t *actor{};
  bool retired{false};
  uint64_t lifetime{};
};
struct ObjectRegistry {
  std::mutex mutex;
  std::map<const void *, ObjectBinding, std::less<>> bindings;
};

ObjectRegistry &object_registry() {
  // Explicit unbind owns the pins; object-owned generation witnesses reclaim
  // retirement metadata. The registry itself outlives atexit callbacks.
  static auto *registry = new ObjectRegistry;
  return *registry;
}

struct BindingOwners {
  void *object{};
  objc3_actor_t *actor{};
  ~BindingOwners() {
    // Object deallocation may reenter binding lookup or actor APIs. Keep its
    // actor alive through that call, with no runtime mutex held.
    if (object) objc3_actor_object_release(object);
    objc3_actor_release(actor);
  }
};

objc3::services::Actor *implementation(objc3_actor_t *actor) noexcept {
  return reinterpret_cast<objc3::services::Actor *>(actor);
}
} // namespace

namespace objc3::services {
std::recursive_mutex &actor_object_binding_mutex() {
  static auto *mutex = new std::recursive_mutex;
  return *mutex;
}
namespace { AutomaticActorLookup automatic_lookup = nullptr; }
void register_automatic_actor_lookup(AutomaticActorLookup lookup) {
  std::lock_guard<std::recursive_mutex> lock(actor_object_binding_mutex());
  automatic_lookup = lookup;
}
void forget_retired_actor_object(const void *object, uint64_t identity) noexcept {
  // GNUstep may invoke this destructor while holding its class-runtime lock.
  // Never acquire the binding coordination mutex here: binding calls enter
  // GNUstep while holding that mutex. Registry sections call no ObjC code.
  auto &registry = object_registry();
  std::lock_guard<std::mutex> lock(registry.mutex);
  const auto found = registry.bindings.find(object);
  if (found != registry.bindings.end() && found->second.retired &&
      found->second.lifetime == identity)
    registry.bindings.erase(found);
}
} // namespace objc3::services

extern "C" objc3_service_status objc3_actor_bind_object(
    void *object, objc3_actor_t *actor) {
  if (!object || !actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  BindingOwners owners;
  try {
    owners.actor = objc3_actor_retain(actor);
    if (!owners.actor) return OBJC3_SERVICE_NOT_READY;
    owners.object = objc3_actor_object_retain(object);
    if (!owners.object) return OBJC3_SERVICE_NOT_READY;
    std::lock_guard<std::recursive_mutex> coordination(objc3::services::actor_object_binding_mutex());
    auto &registry = object_registry();
    {
      std::lock_guard<std::mutex> lock(registry.mutex);
      const auto found = registry.bindings.find(object);
      if (found != registry.bindings.end() && !found->second.retired)
        return found->second.actor == actor ? OBJC3_SERVICE_OK
                                            : OBJC3_SERVICE_INVALID_ARGUMENT;
    }
    if (objc3::services::actor_is_closed(implementation(actor)))
      return OBJC3_SERVICE_CLOSED;
    if (objc3::services::automatic_lookup) {
      if (auto *automatic = objc3::services::automatic_lookup(object)) {
        const bool same = automatic == actor;
        objc3_actor_release(automatic);
        if (!same) return OBJC3_SERVICE_INVALID_ARGUMENT;
      }
    }
    uint64_t lifetime = 0;
    const auto status = objc3::services::ensure_actor_object_lifetime(object, &lifetime);
    if (status != OBJC3_SERVICE_OK) return status;
    std::lock_guard<std::mutex> lock(registry.mutex);
    auto found = registry.bindings.find(object);
    if (found == registry.bindings.end())
      found = registry.bindings.emplace(object, ObjectBinding{}).first;
    found->second = {std::exchange(owners.object, nullptr),
                     std::exchange(owners.actor, nullptr), false, lifetime};
    return OBJC3_SERVICE_OK;
  } catch (const std::bad_alloc &) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
}

extern "C" objc3_service_status objc3_actor_get_for_object(
    const void *object, objc3_actor_t **out_actor) {
  if (!out_actor) return OBJC3_SERVICE_INVALID_ARGUMENT;
  *out_actor = nullptr;
  if (!object) return OBJC3_SERVICE_INVALID_ARGUMENT;
  try {
    std::lock_guard<std::recursive_mutex> coordination(objc3::services::actor_object_binding_mutex());
    const auto lifetime = objc3::services::actor_object_lifetime(object);
    auto &registry = object_registry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    const auto found = registry.bindings.find(object);
    if (found == registry.bindings.end()) return OBJC3_SERVICE_NOT_READY;
    if (found->second.retired) {
      if (found->second.lifetime == lifetime) return OBJC3_SERVICE_CLOSED;
      registry.bindings.erase(found);
      return OBJC3_SERVICE_NOT_READY;
    }
    *out_actor = objc3_actor_retain(found->second.actor);
    return *out_actor ? OBJC3_SERVICE_OK : OBJC3_SERVICE_NOT_READY;
  } catch (const std::bad_alloc &) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
}

extern "C" objc3_service_status objc3_actor_unbind_object(void *object) {
  if (!object) return OBJC3_SERVICE_INVALID_ARGUMENT;
  BindingOwners owners;
  try {
    std::lock_guard<std::recursive_mutex> coordination(objc3::services::actor_object_binding_mutex());
    const auto lifetime = objc3::services::actor_object_lifetime(object);
    auto &registry = object_registry();
    std::lock_guard<std::mutex> lock(registry.mutex);
    const auto found = registry.bindings.find(object);
    if (found == registry.bindings.end()) return OBJC3_SERVICE_NOT_READY;
    if (found->second.retired) {
      if (found->second.lifetime == lifetime) return OBJC3_SERVICE_CLOSED;
      registry.bindings.erase(found);
      return OBJC3_SERVICE_NOT_READY;
    }
    if (!objc3::services::actor_is_closed_and_quiescent(
            implementation(found->second.actor)))
      return OBJC3_SERVICE_NOT_READY;
    // Closing is monotonic: no old mailbox continuation can enter after this
    // observation, even if this same live object is later bound elsewhere.
    found->second.retired = true;
    owners.object = std::exchange(found->second.object, nullptr);
    owners.actor = std::exchange(found->second.actor, nullptr);
    return OBJC3_SERVICE_OK;
  } catch (const std::bad_alloc &) {
    return OBJC3_SERVICE_OUT_OF_MEMORY;
  } catch (...) {
    return OBJC3_SERVICE_INVALID_ARGUMENT;
  }
}
