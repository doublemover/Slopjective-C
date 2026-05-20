#include "runtime/storage/property_value_storage.h"

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/instance_storage.h"
#include "runtime/storage/ivar_storage_span.h"
#include "runtime/storage/property_accessor_profiles.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/storage/weak_slots.h"

#include <algorithm>
#include <cstdint>
#include <cstring>

namespace objc3c::runtime {

namespace {

bool IsRuntimeManagedReceiverValueUnlocked(const RuntimeState &state,
                                           int value) {
  return RuntimeInstanceReceiverIsManaged(value) &&
         state.runtime_instances_by_receiver.find(value) !=
             state.runtime_instances_by_receiver.end();
}

int NormalizeWeakRuntimeManagedPropertyValueUnlocked(
    const RuntimeState &state, int value) {
  return IsRuntimeManagedReceiverValueUnlocked(state, value) ? value : 0;
}

bool WriteWeakRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  const RuntimeIvarStorageSpan span =
      ResolveRuntimeIvarStorageSpan(instance, accessor);
  if (!span.addressable) {
    return false;
  }
  int previous = 0;
  (void)ReadRuntimeManagedPropertyValueRaw(instance, accessor, previous);
  RemoveWeakSlotRefUnlocked(state, previous,
                            static_cast<int>(instance.receiver_identity),
                            span.offset, span.size);
  const int stored_value =
      NormalizeWeakRuntimeManagedPropertyValueUnlocked(state, value);
  if (!WriteRuntimeManagedPropertyValueRaw(instance, accessor, stored_value)) {
    return false;
  }
  if (stored_value != 0) {
    RegisterWeakSlotRefUnlocked(state, stored_value,
                                static_cast<int>(instance.receiver_identity),
                                span.offset, span.size);
  }
  return true;
}

}  // namespace

bool ReadRuntimeManagedPropertyValueRaw(
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value) {
  const RuntimeIvarStorageSpan span =
      ResolveRuntimeIvarStorageSpan(instance, accessor);
  if (!span.addressable) {
    return false;
  }
  std::uint64_t raw = 0;
  std::memcpy(&raw, instance.storage_bytes.data() + span.offset,
              std::min<std::size_t>(span.size, sizeof(raw)));
  if (accessor.getter_return_kind == RuntimeMethodReturnKind::Bool) {
    value = raw != 0u ? 1 : 0;
    return true;
  }
  value = static_cast<int>(raw & 0xffffffffu);
  return true;
}

bool WriteRuntimeManagedPropertyValueRaw(
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  const RuntimeIvarStorageSpan span =
      ResolveRuntimeIvarStorageSpan(instance, accessor);
  if (!span.addressable) {
    return false;
  }
  std::uint64_t raw = accessor.getter_return_kind == RuntimeMethodReturnKind::Bool
                          ? static_cast<std::uint64_t>(value != 0 ? 1 : 0)
                          : static_cast<std::uint64_t>(
                                static_cast<std::uint32_t>(value));
  std::memcpy(instance.storage_bytes.data() + span.offset, &raw,
              std::min<std::size_t>(span.size, sizeof(raw)));
  if (span.size > sizeof(raw)) {
    std::fill(
        instance.storage_bytes.begin() +
            static_cast<std::ptrdiff_t>(span.offset + sizeof(raw)),
        instance.storage_bytes.begin() +
            static_cast<std::ptrdiff_t>(span.offset + span.size),
        0);
  }
  return true;
}

bool ReadRuntimeManagedPropertyValueUnlocked(
    const RuntimeState &state,
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value) {
  if (!ReadRuntimeManagedPropertyValueRaw(instance, accessor, value)) {
    return false;
  }
  if (!UsesSafeUnownedRuntimeHooks(accessor) || value == 0) {
    return true;
  }
  if (!IsRuntimeManagedReceiverValueUnlocked(state, value)) {
    value = 0;
  }
  return true;
}

bool WriteRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  if (UsesWeakRuntimeHooks(accessor)) {
    return WriteWeakRuntimeManagedPropertyValueUnlocked(state, instance,
                                                        accessor, value);
  }
  return WriteRuntimeManagedPropertyValueRaw(instance, accessor, value);
}

bool ExchangeRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value,
    int &previous_value) {
  if (!ReadRuntimeManagedPropertyValueUnlocked(state, instance, accessor,
                                               previous_value)) {
    previous_value = 0;
    return false;
  }
  return WriteRuntimeManagedPropertyValueUnlocked(state, instance, accessor,
                                                  value);
}

}  // namespace objc3c::runtime
