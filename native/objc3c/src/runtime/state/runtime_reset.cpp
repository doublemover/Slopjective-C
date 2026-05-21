#include "runtime/state/runtime_reset.h"

#include "runtime/stdlib/collections_runtime_contract.h"
#include "runtime/stdlib/core_runtime_contract.h"
#include "runtime/stdlib/text_runtime_contract.h"
#include "runtime/state/runtime_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <mutex>

namespace objc3c::runtime {

}  // namespace objc3c::runtime

extern "C" void objc3_runtime_reset_for_testing(void) {
  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  (void)objc3c::runtime::RuntimeStateLockDiscipline();
  (void)objc3c::runtime::RuntimeStateOwnershipModel();
  if (objc3c::runtime::RuntimeResetClearsLiveExecutionState()) {
    objc3c::runtime::ClearLiveRegistrationStateUnlocked(state);
  }
  state.last_reset_cleared_image_local_init_state_count =
      objc3c::runtime::RuntimeResetPreservesBootstrapCatalog()
          ? objc3c::runtime::ZeroRetainedBootstrapImageLocalInitStatesUnlocked(
                state)
          : 0;
  ++state.reset_generation;
  state.last_replay_status = OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
  state.last_replayed_image_count = 0;
  state.last_replayed_module_name.clear();
  state.last_replayed_translation_unit_identity_key.clear();
  objc3c::runtime::ResetRuntimeStdlibCollectionsStateForTesting();
  objc3c::runtime::ResetRuntimeStdlibCoreStateForTesting();
  objc3c::runtime::ResetRuntimeStdlibTextStateForTesting();
  objc3c::runtime::ResetRuntimeThreadLocalDebugStateForTesting();
}
