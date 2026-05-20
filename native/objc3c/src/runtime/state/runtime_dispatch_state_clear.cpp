#include "runtime/state/runtime_dispatch_state_clear.h"

#include "runtime/metadata/runtime_ownership_contracts.h"
#include "runtime/state/runtime_state_records.h"

namespace objc3c::runtime {

void ClearMethodCacheStateUnlocked(RuntimeState &state) {
  state.dispatch_frame_state_owner = kObjc3RuntimeDispatchFrameStateOwner;
  state.public_dispatch_diagnostics_owner =
      kObjc3RuntimePublicDispatchDiagnosticsOwner;
  state.method_cache.clear();
  state.method_cache_hit_count = 0;
  state.method_cache_miss_count = 0;
  state.slow_path_lookup_count = 0;
  state.stale_method_cache_entry_count = 0;
  state.live_dispatch_count = 0;
  state.strict_dispatch_error_count = 0;
  state.fast_path_seed_count = 0;
  state.fast_path_hit_count = 0;
  state.last_dispatch_selector.clear();
  state.last_dispatch_selector_stable_id = 0;
  state.last_dispatch_normalized_receiver_identity = 0;
  state.last_category_probe_count = 0;
  state.last_protocol_probe_count = 0;
  state.last_dispatch_used_cache = false;
  state.last_dispatch_used_fast_path = false;
  state.last_dispatch_resolved_live_method = false;
  state.last_dispatch_strict_error = false;
  state.last_dispatch_effective_direct_dispatch = false;
  state.last_dispatch_used_builtin = false;
  state.last_dispatch_status_code =
      OBJC3_RUNTIME_DISPATCH_STATUS_UNKNOWN_SELECTOR;
  state.last_fast_path_reason.clear();
  state.last_dispatch_path.clear();
  state.last_dispatch_implementation_kind.clear();
  state.last_dispatch_return_kind.clear();
  state.last_dispatch_diagnostic_code.clear();
  state.last_dispatch_diagnostic_message.clear();
  state.last_dispatch_result_contract.clear();
  state.last_dispatch_property_name.clear();
  state.last_resolved_class_name.clear();
  state.last_resolved_owner_identity.clear();
  state.last_dispatch_parameter_count = 0;
  state.last_dispatch_property_base_identity = 0;
  state.last_dispatch_property_slot_index = 0;
}

}  // namespace objc3c::runtime
