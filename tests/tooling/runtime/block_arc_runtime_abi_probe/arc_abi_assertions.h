#pragma once

#include "probe_state.h"

#include "runtime/public/objc3_runtime_api.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

inline bool BlockArcRuntimeAbiAssertionsPassed(const ProbeResult &result) {
  const RuntimeInvocationResult &runtime = result.runtime;
  const ::objc3_runtime_block_arc_runtime_abi_snapshot &abi = result.abi;
  const ::objc3_runtime_arc_debug_state_snapshot &arc = result.arc;

  return result.abi_status == OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
         result.arc_status == OBJC3_RUNTIME_REGISTRATION_STATUS_OK &&
         runtime.retained == 77 && runtime.autoreleased == 77 &&
         runtime.released == 77 && runtime.handle > 0 &&
         runtime.invoke_result == 17 &&
         runtime.retain_handle_result == runtime.handle &&
         runtime.release_handle_result == runtime.handle &&
         runtime.final_release_result == runtime.handle &&
         abi.private_runtime_abi_ready == 1 &&
         abi.public_runtime_header_unchanged == 1 && abi.deterministic == 1 &&
         abi.live_runtime_block_handle_count == 0 &&
         abi.block_promote_call_count == 1 && abi.block_invoke_call_count == 1 &&
         abi.retain_call_count == 2 && abi.release_call_count == 3 &&
         abi.autorelease_call_count == 1 &&
         abi.autoreleasepool_push_count == 1 &&
         abi.autoreleasepool_pop_count == 1 &&
         abi.current_property_read_count == 0 &&
         abi.current_property_write_count == 0 &&
         abi.current_property_exchange_count == 0 &&
         abi.weak_current_property_load_count == 0 &&
         abi.weak_current_property_store_count == 0 &&
         abi.last_promoted_block_handle == runtime.handle &&
         abi.last_promote_has_pointer_capture_storage == 1 &&
         abi.last_invoked_block_handle == runtime.handle &&
         abi.last_block_invoke_result == 17 &&
         abi.last_retain_value == runtime.handle &&
         abi.last_release_value == runtime.handle &&
         abi.last_autorelease_value == 77 &&
         arc.retain_call_count == abi.retain_call_count &&
         arc.release_call_count == abi.release_call_count &&
         arc.autorelease_call_count == abi.autorelease_call_count &&
         arc.autoreleasepool_push_count == abi.autoreleasepool_push_count &&
         arc.autoreleasepool_pop_count == abi.autoreleasepool_pop_count;
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
