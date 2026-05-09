#include "runtime/storage/property_value_storage.h"

#include "runtime/metadata/runtime_realized_records.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/storage/instance_storage.h"
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

bool WriteWeakRuntimeManagedPropertyValueUnlocked(
    RuntimeState &state,
    RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int value) {
  const std::size_t offset = EffectiveIvarOffset(accessor);
  const std::size_t size = EffectiveIvarSize(accessor);
  if (size == 0u || offset + size > instance.storage_bytes.size()) {
    return false;
  }
  int previous = 0;
  (void)ReadRuntimeManagedPropertyValueRaw(instance, accessor, previous);
  RemoveWeakSlotRefUnlocked(state, previous,
                            static_cast<int>(instance.receiver_identity),
                            offset, size);
  if (!WriteRuntimeManagedPropertyValueRaw(instance, accessor, value)) {
    return false;
  }
  if (IsRuntimeManagedReceiverValueUnlocked(state, value)) {
    RegisterWeakSlotRefUnlocked(state, value,
                                static_cast<int>(instance.receiver_identity),
                                offset, size);
  }
  return true;
}

}  // namespace

std::size_t EffectiveIvarOffset(const RealizedPropertyAccessor &accessor) {
  if (accessor.ivar_descriptor == nullptr) {
    return 0u;
  }
  if (accessor.ivar_descriptor->offset_global != nullptr) {
    return static_cast<std::size_t>(*accessor.ivar_descriptor->offset_global);
  }
  return static_cast<std::size_t>(accessor.ivar_descriptor->offset_bytes);
}

std::size_t EffectiveIvarSize(const RealizedPropertyAccessor &accessor) {
  return accessor.ivar_descriptor != nullptr
             ? static_cast<std::size_t>(accessor.ivar_descriptor->size_bytes)
             : 0u;
}

bool ReadRuntimeManagedPropertyValueRaw(
    const RuntimeInstanceRecord &instance,
    const RealizedPropertyAccessor &accessor,
    int &value) {
  const std::size_t offset = EffectiveIvarOffset(accessor);
  const std::size_t size = EffectiveIvarSize(accessor);
  if (size == 0u || offset + size > instance.storage_bytes.size()) {
    return false;
  }
  std::uint64_t raw = 0;
  std::memcpy(&raw, instance.storage_bytes.data() + offset,
              std::min<std::size_t>(size, sizeof(raw)));
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
  const std::size_t offset = EffectiveIvarOffset(accessor);
  const std::size_t size = EffectiveIvarSize(accessor);
  if (size == 0u || offset + size > instance.storage_bytes.size()) {
    return false;
  }
  std::uint64_t raw = accessor.getter_return_kind == RuntimeMethodReturnKind::Bool
                          ? static_cast<std::uint64_t>(value != 0 ? 1 : 0)
                          : static_cast<std::uint64_t>(
                                static_cast<std::uint32_t>(value));
  std::memcpy(instance.storage_bytes.data() + offset, &raw,
              std::min<std::size_t>(size, sizeof(raw)));
  if (size > sizeof(raw)) {
    std::fill(
        instance.storage_bytes.begin() +
            static_cast<std::ptrdiff_t>(offset + sizeof(raw)),
        instance.storage_bytes.begin() +
            static_cast<std::ptrdiff_t>(offset + size),
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
