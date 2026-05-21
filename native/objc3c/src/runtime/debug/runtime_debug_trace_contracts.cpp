#include "runtime/debug/runtime_debug_trace_contracts.h"

#include <stddef.h>

namespace {

#define OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACT_ENTRY(                     \
    lane_id, trace_domain, event_kind, snapshot_header, snapshot_type,     \
    snapshot_symbol, required_fields_csv, status, source_anchor,           \
    deterministic, public_abi)                                             \
  {lane_id, trace_domain, event_kind, snapshot_header, snapshot_type,       \
   snapshot_symbol, required_fields_csv, status, source_anchor,             \
   deterministic, public_abi},

const objc3_runtime_debug_trace_lane_contract kRuntimeDebugTraceContracts[] = {
    OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACTS(
        OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACT_ENTRY)};

#undef OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACT_ENTRY

}  // namespace

uint64_t objc3_runtime_debug_trace_lane_contract_count_for_testing(void) {
  return static_cast<uint64_t>(
      sizeof(kRuntimeDebugTraceContracts) / sizeof(kRuntimeDebugTraceContracts[0]));
}

int objc3_runtime_copy_debug_trace_lane_contracts_for_testing(
    objc3_runtime_debug_trace_lane_contract *contracts, uint64_t capacity) {
  const uint64_t count =
      objc3_runtime_debug_trace_lane_contract_count_for_testing();
  if (contracts == nullptr || capacity < count) {
    return 0;
  }

  for (uint64_t index = 0; index < count; ++index) {
    contracts[index] = kRuntimeDebugTraceContracts[index];
  }
  return static_cast<int>(count);
}

