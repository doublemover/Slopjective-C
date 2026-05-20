#include "runtime/objc3_runtime_bootstrap_internal.h"

#include "runtime/blocks/block_runtime_state.h"
#include "runtime/memory/arc_debug_state.h"
#include "runtime/state/runtime_state_records.h"
#include "runtime/state/runtime_state_store.h"

#include <cstdint>
#include <mutex>

extern "C" int objc3_runtime_copy_block_arc_runtime_abi_snapshot_for_testing(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot) {
  if (snapshot == nullptr) {
    return OBJC3_RUNTIME_REGISTRATION_STATUS_INVALID_DESCRIPTOR;
  }

  snapshot->private_runtime_abi_ready = 1;
  snapshot->public_runtime_header_unchanged = 1;
  snapshot->deterministic = 1;
  snapshot->live_runtime_block_handle_count = 0;
  objc3c::runtime::CopyRuntimeBlockFieldsToBlockArcSnapshot(snapshot);
  objc3c::runtime::CopyRuntimeArcFieldsToBlockArcSnapshot(snapshot);
  snapshot->block_promote_symbol = "objc3_runtime_promote_block_i32";
  snapshot->block_invoke_symbol = "objc3_runtime_invoke_block_i32";
  snapshot->retain_symbol = "objc3_runtime_retain_i32";
  snapshot->release_symbol = "objc3_runtime_release_i32";
  snapshot->autorelease_symbol = "objc3_runtime_autorelease_i32";
  snapshot->autoreleasepool_push_symbol =
      "objc3_runtime_push_autoreleasepool_scope";
  snapshot->autoreleasepool_pop_symbol =
      "objc3_runtime_pop_autoreleasepool_scope";
  snapshot->current_property_read_symbol =
      "objc3_runtime_read_current_property_i32";
  snapshot->current_property_write_symbol =
      "objc3_runtime_write_current_property_i32";
  snapshot->current_property_exchange_symbol =
      "objc3_runtime_exchange_current_property_i32";
  snapshot->bind_current_property_context_symbol =
      "objc3_runtime_bind_current_property_context_for_testing";
  snapshot->clear_current_property_context_symbol =
      "objc3_runtime_clear_current_property_context_for_testing";
  snapshot->weak_current_property_load_symbol =
      "objc3_runtime_load_weak_current_property_i32";
  snapshot->weak_current_property_store_symbol =
      "objc3_runtime_store_weak_current_property_i32";
  snapshot->arc_debug_state_snapshot_symbol =
      "objc3_runtime_copy_arc_debug_state_for_testing";
  snapshot->runtime_abi_boundary_model =
      "private-block-and-arc-helper-entrypoints-plus-testing-snapshots-define-the-live-runtime-abi-without-widening-the-public-runtime-header";
  snapshot->block_runtime_model =
      "descriptor-backed-promote-invoke-and-handle-lifetime-for-supported-block-records-stay-on-bootstrap-internal-runtime-entrypoints";
  snapshot->block_descriptor_model =
      "storage-slot-zero-carries-an-internal-descriptor-pointer-whose-record-preserves-size-captures-flags-arity-and-invoke-thunk";
  snapshot->block_invoke_thunk_model =
      "runtime-invocation-plans-call-the-descriptor-owned-i32-invoke-thunk-with-copied-runtime-owned-storage";
  snapshot->arc_runtime_model =
      "retain-release-autorelease-autoreleasepool-and-current-property-weak-helper-traffic-stays-on-bootstrap-internal-runtime-entrypoints";
  snapshot->fail_closed_model =
      "public-runtime-header-remains-registration-lookup-dispatch-only-until-deliberate-runtime-abi-widening";

  objc3c::runtime::RuntimeState &state =
      objc3c::runtime::ProcessRuntimeState();
  std::lock_guard<std::mutex> lock(state.mutex);
  snapshot->live_runtime_block_handle_count =
      static_cast<std::uint64_t>(state.runtime_blocks_by_handle.size());
  return OBJC3_RUNTIME_REGISTRATION_STATUS_OK;
}
