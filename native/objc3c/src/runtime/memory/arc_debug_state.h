#pragma once

#include "runtime/memory/arc_debug_snapshot_contracts.h"
#include "runtime/memory/block_arc_runtime_abi_snapshot_contracts.h"
#include "runtime/state/runtime_thread_records.h"

#include <cstdint>
#include <string>

namespace objc3c::runtime {

struct RuntimeArcDebugState {
  std::uint64_t retain_call_count = 0;
  std::uint64_t release_call_count = 0;
  std::uint64_t autorelease_call_count = 0;
  std::uint64_t autoreleasepool_push_count = 0;
  std::uint64_t autoreleasepool_pop_count = 0;
  std::uint64_t current_property_read_count = 0;
  std::uint64_t current_property_write_count = 0;
  std::uint64_t current_property_exchange_count = 0;
  std::uint64_t weak_current_property_load_count = 0;
  std::uint64_t weak_current_property_store_count = 0;
  int last_retain_value = 0;
  int last_release_value = 0;
  int last_autorelease_value = 0;
  int last_property_read_value = 0;
  int last_property_written_value = 0;
  int last_property_exchange_previous_value = 0;
  int last_property_exchange_new_value = 0;
  int last_weak_loaded_value = 0;
  int last_weak_stored_value = 0;
  int last_property_receiver = 0;
  std::string last_property_name;
  std::string last_property_owner_identity;
};

RuntimeArcDebugState &RuntimeArcDebugStateForCurrentThread();
void ResetRuntimeArcDebugStateForTesting();
void RecordRuntimeArcDebugPropertyContext(const RuntimeDispatchFrame *frame);
void CopyRuntimeArcDebugStateForTesting(
    objc3_runtime_arc_debug_state_snapshot *snapshot);
void CopyRuntimeArcFieldsToBlockArcSnapshot(
    objc3_runtime_block_arc_runtime_abi_snapshot *snapshot);

}  // namespace objc3c::runtime
