#include "runtime/memory/dispatch_frame_state.h"
#include "runtime/memory/arc_debug_events.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/storage/instance_storage.h"
#include "runtime/storage/ivar_storage_span.h"
#include "runtime/storage/property_accessor_profiles.h"
#include "runtime/storage/property_value_storage.h"
#include "runtime/storage/runtime_instance_records.h"
#include "runtime/storage/weak_slots.h"

#include <mutex>

namespace objc3c::runtime {

namespace {

bool CurrentPropertyFrameTargetsWeakStorage(const RuntimeDispatchFrame *frame) {
  return frame != nullptr && frame->runtime_property_accessor != nullptr &&
         frame->receiver != 0 &&
         UsesWeakRuntimeHooks(*frame->runtime_property_accessor);
}

bool WeakTargetIsLiveRuntimeInstanceUnlocked(const RuntimeState &state,
                                             int value) {
  return value == 0 ||
         (RuntimeInstanceReceiverIsManaged(value) &&
          state.runtime_instances_by_receiver.find(value) !=
              state.runtime_instances_by_receiver.end());
}

int LoadWeakCurrentPropertyValueI32() {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  RecordRuntimeArcCurrentPropertyReadCall(frame);
  if (!CurrentPropertyFrameTargetsWeakStorage(frame)) {
    RecordRuntimeArcLastPropertyReadValue(0);
    return 0;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it =
      state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    RecordRuntimeArcLastPropertyReadValue(0);
    return 0;
  }

  RuntimeInstanceRecord &instance = instance_it->second;
  const RealizedPropertyAccessor &accessor =
      *frame->runtime_property_accessor;
  int value = 0;
  if (!ReadRuntimeManagedPropertyValueRaw(instance, accessor, value)) {
    RecordRuntimeArcLastPropertyReadValue(0);
    return 0;
  }
  if (!WeakTargetIsLiveRuntimeInstanceUnlocked(state, value)) {
    const RuntimeIvarStorageSpan span =
        ResolveRuntimeIvarStorageSpan(instance, accessor);
    if (span.addressable) {
      RemoveWeakSlotRefUnlocked(state, value, frame->receiver, span.offset,
                                span.size);
      (void)WriteRuntimeManagedPropertyValueRaw(instance, accessor, 0);
    }
    value = 0;
  }
  RecordRuntimeArcLastPropertyReadValue(value);
  return value;
}

void StoreWeakCurrentPropertyValueI32(int value) {
  RuntimeDispatchFrame *frame = CurrentRuntimeDispatchFrame();
  RecordRuntimeArcCurrentPropertyWriteCall(frame, value);
  if (!CurrentPropertyFrameTargetsWeakStorage(frame)) {
    return;
  }

  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const auto instance_it =
      state.runtime_instances_by_receiver.find(frame->receiver);
  if (instance_it == state.runtime_instances_by_receiver.end()) {
    return;
  }

  RuntimeInstanceRecord &instance = instance_it->second;
  const RealizedPropertyAccessor &accessor =
      *frame->runtime_property_accessor;
  const RuntimeIvarStorageSpan span =
      ResolveRuntimeIvarStorageSpan(instance, accessor);
  if (!span.addressable) {
    return;
  }

  int previous = 0;
  (void)ReadRuntimeManagedPropertyValueRaw(instance, accessor, previous);
  RemoveWeakSlotRefUnlocked(state, previous, frame->receiver, span.offset,
                            span.size);

  const int stored_value =
      WeakTargetIsLiveRuntimeInstanceUnlocked(state, value) ? value : 0;
  if (!WriteRuntimeManagedPropertyValueRaw(instance, accessor, stored_value)) {
    return;
  }
  if (stored_value != 0) {
    RegisterWeakSlotRefUnlocked(state, stored_value, frame->receiver,
                                span.offset, span.size);
  }
}

}  // namespace

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_load_weak_current_property_i32(void) {
  objc3c::runtime::RecordRuntimeArcWeakCurrentPropertyLoadCall();
  const int result = objc3c::runtime::LoadWeakCurrentPropertyValueI32();
  objc3c::runtime::RecordRuntimeArcLastWeakCurrentPropertyLoadedValue(result);
  return result;
}

extern "C" void objc3_runtime_store_weak_current_property_i32(int value) {
  objc3c::runtime::RecordRuntimeArcWeakCurrentPropertyStoreCall(value);
  objc3c::runtime::StoreWeakCurrentPropertyValueI32(value);
}
