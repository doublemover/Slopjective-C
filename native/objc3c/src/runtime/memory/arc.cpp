#include "runtime/memory/arc.h"

#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>
#include <vector>

extern "C" int objc3_runtime_load_weak_current_property_i32(void) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.weak_current_property_load_count;
  const int result = objc3_runtime_read_current_property_i32();
  arc_debug.last_weak_loaded_value = result;
  return result;
}

extern "C" void objc3_runtime_store_weak_current_property_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.weak_current_property_store_count;
  arc_debug.last_weak_stored_value = value;
  objc3_runtime_write_current_property_i32(value);
}

extern "C" int objc3_runtime_retain_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.retain_call_count;
  arc_debug.last_retain_value = value;
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::RetainRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_release_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.release_call_count;
  arc_debug.last_release_value = value;
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::ReleaseRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_autorelease_i32(int value) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.autorelease_call_count;
  arc_debug.last_autorelease_value = value;
  if (objc3c::runtime::RuntimeAutoreleasePoolCanEnqueue(value)) {
    objc3c::runtime::EnqueueAutoreleaseValue(value);
  }
  return value;
}

extern "C" void objc3_runtime_push_autoreleasepool_scope(void) {
  objc3c::runtime::PushRuntimeAutoreleasePoolFrame();
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.autoreleasepool_push_count;
}

extern "C" void objc3_runtime_pop_autoreleasepool_scope(void) {
  objc3c::runtime::RuntimeArcDebugState &arc_debug =
      objc3c::runtime::RuntimeArcDebugStateForCurrentThread();
  ++arc_debug.autoreleasepool_pop_count;
  const std::vector<int> values =
      objc3c::runtime::PopRuntimeAutoreleasePoolFrameValues();
  if (values.empty()) {
    return;
  }
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  for (auto it = values.rbegin(); it != values.rend(); ++it) {
    objc3c::runtime::RecordRuntimeAutoreleasePoolDrainedValue(*it);
    objc3c::runtime::ReleaseRuntimeValueUnlocked(state, *it);
  }
}
