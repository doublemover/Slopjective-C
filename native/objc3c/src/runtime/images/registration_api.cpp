#include "runtime/images/registration.h"

#include "runtime/state/runtime_reset.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

extern "C" void objc3_runtime_stage_registration_table_for_bootstrap(
    const objc3_runtime_registration_table *registration_table) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  state.staged_registration_table = registration_table;
}

extern "C" int objc3_runtime_register_image(
    const objc3_runtime_image_descriptor *image) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  const objc3_runtime_registration_table *const staged_registration_table =
      state.staged_registration_table;
  // runtime-bootstrap-table-consumption anchor: staging is one-shot and is
  // consumed by the next public registration call only.
  state.staged_registration_table = nullptr;
  return objc3c::runtime::RegisterImageUnlocked(
      state, image, staged_registration_table, true, false);
}

extern "C" int objc3_runtime_replay_registered_images_for_testing(void) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  return objc3c::runtime::ReplayRegisteredImagesForTestingUnlocked(
      state, objc3c::runtime::RegisterImageUnlocked);
}
