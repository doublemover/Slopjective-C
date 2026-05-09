#include "runtime/storage/current_property_context.h"

#include "runtime/memory/arc_debug_state.h"
#include "runtime/memory/autorelease_pool.h"
#include "runtime/objc3_runtime_bootstrap_internal.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"
#include "runtime/storage/current_property_binding_resolution.h"

#include <mutex>

namespace objc3c::runtime {

int BindCurrentPropertyContextForTesting(
    int receiver, const char *class_name, const char *property_name) {
  RuntimeState &state = ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const RuntimeCurrentPropertyBindingResolution resolution =
      ResolveCurrentPropertyBindingUnlocked(
          state, receiver, class_name, property_name);
  if (resolution.status != OBJC3_RUNTIME_REGISTRATION_STATUS_OK) {
    return resolution.status;
  }

  RuntimeDispatchFrame *testing_frame = SetRuntimeTestingDispatchFrame(
      receiver, resolution.base_identity, resolution.accessor);
  RecordRuntimeArcDebugPropertyContext(testing_frame);
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}

void ClearCurrentPropertyContextForTesting() {
  ClearRuntimeTestingDispatchFrame();
}

}  // namespace objc3c::runtime

extern "C" int objc3_runtime_bind_current_property_context_for_testing(
    int receiver, const char *class_name, const char *property_name) {
  return objc3c::runtime::BindCurrentPropertyContextForTesting(
      receiver, class_name, property_name);
}

extern "C" void objc3_runtime_clear_current_property_context_for_testing(void) {
  objc3c::runtime::ClearCurrentPropertyContextForTesting();
}
