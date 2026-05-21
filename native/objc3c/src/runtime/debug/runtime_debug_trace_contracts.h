#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Private runtime debug trace source contract rows. These rows are intentionally
// not public ABI; tooling parses this table so debugger/inspector claims stay
// anchored to runtime-owned snapshot contracts instead of hand-maintained docs.
#define OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACTS(X)                         \
  X("runtime.task.snapshot", "task", "runtime-task-snapshot",             \
    "runtime/concurrency/runtime_concurrency_snapshot_contracts.h",        \
    "objc3_runtime_task_runtime_state_snapshot",                           \
    "objc3_runtime_copy_task_runtime_state_for_testing",                   \
    "spawn_call_count,scheduler_enqueue_count,last_executor_queue_depth,"   \
    "lifecycle_state,deadlock_guard_passed,race_guard_passed",             \
    "supported", "OBJ3-NEXT-023.runtime-debug-trace.task", 1, 0)          \
  X("runtime.actor.snapshot", "actor", "runtime-actor-snapshot",          \
    "runtime/concurrency/runtime_concurrency_snapshot_contracts.h",        \
    "objc3_runtime_actor_runtime_state_snapshot",                          \
    "objc3_runtime_copy_actor_runtime_state_for_testing",                  \
    "isolation_thunk_call_count,mailbox_enqueue_call_count,"               \
    "mailbox_drain_call_count,executor_binding_guard_passed,"              \
    "last_failure_code",                                                   \
    "supported", "OBJ3-NEXT-023.runtime-debug-trace.actor", 1, 0)         \
  X("runtime.dispatch.snapshot", "dispatch", "runtime-dispatch-snapshot", \
    "runtime/dispatch/dispatch_snapshot_contracts.h",                      \
    "objc3_runtime_dispatch_state_snapshot",                               \
    "objc3_runtime_copy_dispatch_state_for_testing",                       \
    "live_dispatch_count,last_selector,last_dispatch_path,"                \
    "last_implementation_kind,last_diagnostic_code",                       \
    "supported", "OBJ3-NEXT-023.runtime-debug-trace.dispatch", 1, 0)      \
  X("runtime.object.snapshot", "object", "runtime-object-snapshot",       \
    "runtime/classes/runtime_object_snapshot_contracts.h",                 \
    "objc3_runtime_object_model_query_state_snapshot",                     \
    "objc3_runtime_copy_object_model_query_state_for_testing",             \
    "realized_class_count,reflectable_property_count,"                     \
    "method_cache_entry_count,last_queried_class_name",                    \
    "supported", "OBJ3-NEXT-023.runtime-debug-trace.object", 1, 0)        \
  X("runtime.memory.snapshot", "memory", "runtime-memory-snapshot",       \
    "runtime/memory/arc_debug_snapshot_contracts.h",                       \
    "objc3_runtime_arc_debug_state_snapshot",                              \
    "objc3_runtime_copy_arc_debug_state_for_testing",                      \
    "retain_call_count,release_call_count,autorelease_call_count,"          \
    "autoreleasepool_push_count,autoreleasepool_pop_count",                \
    "supported", "OBJ3-NEXT-023.runtime-debug-trace.memory", 1, 0)        \
  X("runtime.error.snapshot", "error", "runtime-error-snapshot",          \
    "runtime/errors/error_bridge_snapshot_contracts.h",                    \
    "objc3_runtime_error_bridge_state_snapshot",                           \
    "objc3_runtime_copy_error_bridge_state_for_testing",                   \
    "store_call_count,load_call_count,status_bridge_call_count,"            \
    "foreign_exception_bridge_call_count,catch_match_call_count,"           \
    "last_foreign_exception_bridge_result,last_catch_match_result",         \
    "supported", "OBJ3-NEXT-023.runtime-debug-trace.error-unwind", 1, 0)

enum {
  OBJC3_RUNTIME_DEBUG_TRACE_LANE_CONTRACT_COUNT = 6,
};

typedef struct objc3_runtime_debug_trace_lane_contract {
  const char *lane_id;
  const char *trace_domain;
  const char *event_kind;
  const char *snapshot_header;
  const char *snapshot_type;
  const char *snapshot_symbol;
  const char *required_fields_csv;
  const char *status;
  const char *source_anchor;
  int deterministic;
  int public_abi;
} objc3_runtime_debug_trace_lane_contract;

uint64_t objc3_runtime_debug_trace_lane_contract_count_for_testing(void);
int objc3_runtime_copy_debug_trace_lane_contracts_for_testing(
    objc3_runtime_debug_trace_lane_contract *contracts, uint64_t capacity);

#ifdef __cplusplus
}
#endif
