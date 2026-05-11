#pragma once

#include "fixture_block_setup.h"
#include "probe_state.h"

#include "runtime/objc3_runtime_bootstrap_internal.h"

namespace objc3c {
namespace runtime {
namespace probe {
namespace block_arc_runtime_abi {

inline RuntimeInvocationResult InvokeRuntimeHelpersForProbe() {
  RuntimeInvocationResult result;

  ::objc3_runtime_push_autoreleasepool_scope();
  result.retained = ::objc3_runtime_retain_i32(77);
  result.autoreleased = ::objc3_runtime_autorelease_i32(result.retained);
  result.released = ::objc3_runtime_release_i32(result.retained);

  result.capture = SetUpProbeCaptureState();
  ProbeBlockStorage block = SetUpProbeBlockStorage(&result.capture);
  result.handle =
      ::objc3_runtime_promote_block_i32(&block, sizeof(block), 1);
  result.invoke_result =
      result.handle > 0
          ? ::objc3_runtime_invoke_block_i32(result.handle, 1, 2, 3, 4)
          : 0;
  result.retain_handle_result =
      result.handle > 0 ? ::objc3_runtime_retain_i32(result.handle) : 0;
  result.release_handle_result =
      result.handle > 0 ? ::objc3_runtime_release_i32(result.handle) : 0;
  result.final_release_result =
      result.handle > 0 ? ::objc3_runtime_release_i32(result.handle) : 0;
  ::objc3_runtime_pop_autoreleasepool_scope();

  return result;
}

inline void CaptureRuntimeAbiSnapshots(ProbeResult *result) {
  result->abi_status =
      ::objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing(
          &result->abi);
  result->arc_status =
      ::objc3_runtime_copy_arc_debug_state_for_testing(&result->arc);
}

} // namespace block_arc_runtime_abi
} // namespace probe
} // namespace runtime
} // namespace objc3c
