#include "runtime/dispatch/builtin_methods.h"

#include "runtime/classes/receiver_identity.h"
#include "runtime/dispatch/runtime_method_return.h"
#include "runtime/dispatch/typed_dispatch_result.h"
#include "runtime/memory/arc.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/public/objc3_runtime_result.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/instance_storage.h"
#include "runtime/storage/property_accessors.h"
#include "runtime/storage/runtime_instance_records.h"

#include <mutex>
#include <utility>

namespace objc3c::runtime {

const char *DescribeResolvedImplementationKind(
    RuntimeBuiltinKind builtin_kind, const void *implementation) {
  switch (builtin_kind) {
    case RuntimeBuiltinKind::Alloc:
      return "builtin-alloc";
    case RuntimeBuiltinKind::Init:
      return "builtin-init";
    case RuntimeBuiltinKind::New:
      return "builtin-new";
    case RuntimeBuiltinKind::PropertyGetter:
      return "builtin-property-getter";
    case RuntimeBuiltinKind::PropertySetter:
      return "builtin-property-setter";
    case RuntimeBuiltinKind::None:
      break;
  }
  if (implementation != nullptr) {
    return "emitted-method-body";
  }
  return "";
}

RuntimeTypedDispatchResult InvokeRuntimeBuiltinMethod(
    RuntimeState &state, RuntimeBuiltinKind builtin_kind, int receiver,
    std::uint64_t base_identity,
    const RealizedPropertyAccessor *runtime_property_accessor, int a0, int a1,
    int a2, int a3) {
  (void)a1;
  (void)a2;
  (void)a3;
  switch (builtin_kind) {
    case RuntimeBuiltinKind::Alloc:
    case RuntimeBuiltinKind::New: {
      std::lock_guard<std::mutex> lock(state.mutex);
      const RealizedClassNode *node =
          FindRealizedClassNodeByBaseIdentityUnlocked(state, base_identity);
      if (node == nullptr) {
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
            RuntimeMethodReturnKind::ObjectReference);
      }
      int receiver_identity = state.next_runtime_instance_receiver;
      while (receiver_identity <= 0 ||
             state.runtime_instances_by_receiver.find(receiver_identity) !=
                 state.runtime_instances_by_receiver.end()) {
        ++receiver_identity;
      }
      state.next_runtime_instance_receiver = receiver_identity + 1;
      RuntimeInstanceRecord instance;
      instance.receiver_identity = static_cast<std::uint64_t>(receiver_identity);
      instance.base_identity = base_identity;
      instance.class_name = node->class_name;
      instance.instance_size_bytes =
          RuntimeInstanceStorageSize(node->runtime_instance_size_bytes);
      const std::uint64_t instance_size_bytes =
          static_cast<std::uint64_t>(instance.instance_size_bytes);
      instance.storage_bytes.assign(instance.instance_size_bytes, 0u);
      instance.retain_count = 1u;
      state.runtime_instances_by_receiver.emplace(receiver_identity,
                                                 std::move(instance));
      state.live_runtime_instance_count =
          static_cast<std::uint64_t>(state.runtime_instances_by_receiver.size());
      state.last_allocated_runtime_instance_receiver =
          static_cast<std::uint64_t>(receiver_identity);
      state.last_allocated_runtime_instance_base_identity = base_identity;
      state.last_allocated_runtime_instance_size_bytes = instance_size_bytes;
      state.last_allocated_runtime_instance_class_name = node->class_name;
      return RuntimeTypedDispatchSuccess(RuntimeMethodReturnKind::ObjectReference,
                                         receiver_identity);
    }
    case RuntimeBuiltinKind::Init:
      return RuntimeTypedDispatchSuccess(RuntimeMethodReturnKind::ObjectReference,
                                         receiver);
    case RuntimeBuiltinKind::PropertyGetter: {
      if (runtime_property_accessor == nullptr) {
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
            RuntimeMethodReturnKind::Unsupported);
      }
      std::lock_guard<std::mutex> lock(state.mutex);
      const auto instance_it =
          state.runtime_instances_by_receiver.find(receiver);
      if (instance_it == state.runtime_instances_by_receiver.end()) {
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
            runtime_property_accessor->getter_return_kind);
      }
      int value = 0;
      if (!ReadRuntimeManagedPropertyValueUnlocked(state, instance_it->second,
                                                   *runtime_property_accessor,
                                                   value)) {
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
            runtime_property_accessor->getter_return_kind);
      }
      if (UsesStrongOwnedRuntimeHooks(*runtime_property_accessor) &&
          value != 0) {
        RetainRuntimeValueUnlocked(state, value);
        EnqueueAutoreleaseValue(value);
      }
      return RuntimeTypedDispatchSuccess(
          runtime_property_accessor->getter_return_kind, value);
    }
    case RuntimeBuiltinKind::PropertySetter: {
      if (runtime_property_accessor == nullptr) {
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
            RuntimeMethodReturnKind::Void);
      }
      std::lock_guard<std::mutex> lock(state.mutex);
      const auto instance_it =
          state.runtime_instances_by_receiver.find(receiver);
      if (instance_it == state.runtime_instances_by_receiver.end()) {
        return RuntimeTypedDispatchFailure(
            OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_RECEIVER_CLASS,
            RuntimeMethodReturnKind::Void);
      }
      if (UsesStrongOwnedRuntimeHooks(*runtime_property_accessor)) {
        if (a0 != 0) {
          RetainRuntimeValueUnlocked(state, a0);
        }
        int previous_value = 0;
        if (!ExchangeRuntimeManagedPropertyValueUnlocked(
                state, instance_it->second, *runtime_property_accessor, a0,
                previous_value)) {
          if (a0 != 0) {
            ReleaseRuntimeValueUnlocked(state, a0);
          }
          return RuntimeTypedDispatchFailure(
              OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
              RuntimeMethodReturnKind::Void);
        }
        if (previous_value != 0) {
          ReleaseRuntimeValueUnlocked(state, previous_value);
        }
      } else {
        if (!WriteRuntimeManagedPropertyValueUnlocked(
                state, instance_it->second, *runtime_property_accessor, a0)) {
          return RuntimeTypedDispatchFailure(
              OBJC3_RUNTIME_DISPATCH_STATUS_MALFORMED_METADATA,
              RuntimeMethodReturnKind::Void);
        }
      }
      return RuntimeTypedDispatchSuccess(RuntimeMethodReturnKind::Void, 0);
    }
    case RuntimeBuiltinKind::None:
      return RuntimeTypedDispatchFailure(
          OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
          RuntimeMethodReturnKind::Unsupported);
  }
  return RuntimeTypedDispatchFailure(
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR,
      RuntimeMethodReturnKind::Unsupported);
}

}  // namespace objc3c::runtime
