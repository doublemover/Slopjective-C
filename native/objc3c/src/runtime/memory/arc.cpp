#include "runtime/memory/arc.h"

#include "runtime/memory/arc_debug_events.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>
#include <vector>

extern "C" int objc3_runtime_retain_i32(int value) {
  objc3c::runtime::RecordRuntimeArcRetainCall(value);
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::RetainRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_release_i32(int value) {
  objc3c::runtime::RecordRuntimeArcReleaseCall(value);
  objc3c::runtime::RuntimeState &state = objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  objc3c::runtime::ReleaseRuntimeValueUnlocked(state, value);
  return value;
}

extern "C" int objc3_runtime_autorelease_i32(int value) {
  objc3c::runtime::RecordRuntimeArcAutoreleaseCall(value);
  if (objc3c::runtime::RuntimeAutoreleasePoolCanEnqueue(value)) {
    objc3c::runtime::EnqueueAutoreleaseValue(value);
  }
  return value;
}

extern "C" void objc3_runtime_push_autoreleasepool_scope(void) {
  objc3c::runtime::PushRuntimeAutoreleasePoolFrame();
  objc3c::runtime::RecordRuntimeArcAutoreleasePoolPushCall();
}

extern "C" void objc3_runtime_pop_autoreleasepool_scope(void) {
  objc3c::runtime::RecordRuntimeArcAutoreleasePoolPopCall();
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
