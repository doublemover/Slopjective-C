#include "ir/objc3_ir_module_metadata_publication_prelude_lowering_replay.h"

#include <sstream>

#include "ir/objc3_ir_frontend_metadata.h"
#include "lower/objc3_lowering_contract.h"

void EmitObjc3IRModuleMetadataPreludeLoweringReplayPublication(
    const Objc3IRFrontendMetadata &frontend_metadata_,
    std::ostringstream &out) {
  if (!frontend_metadata_.lowering_id_class_sel_object_pointer_typecheck_replay_key.empty()) {
    out << "; id_class_sel_object_pointer_typecheck_lowering = "
        << frontend_metadata_.lowering_id_class_sel_object_pointer_typecheck_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_message_send_selector_lowering_replay_key.empty()) {
    out << "; message_send_selector_lowering = "
        << frontend_metadata_.lowering_message_send_selector_lowering_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_dispatch_abi_marshalling_replay_key.empty()) {
    out << "; dispatch_abi_marshalling_lowering = "
        << frontend_metadata_.lowering_dispatch_abi_marshalling_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_nil_receiver_semantics_foldability_replay_key.empty()) {
    out << "; nil_receiver_semantics_foldability_lowering = "
        << frontend_metadata_.lowering_nil_receiver_semantics_foldability_replay_key << "\n";
  }
  out << "; type_system_optional_keypath_lowering = "
      << Objc3TypeSystemOptionalKeypathLoweringSummary() << "\n";
  // control-flow safety lowering freeze anchor: the frontend now
  // publishes one lowering-owned packet for admitted Part 5 control-flow
  // sites while native IR lowering still fails closed on guard/match/defer
  // execution until later lowering/runtime work materializes it.
  out << "; control_flow_control_flow_safety_lowering = "
      << Objc3ControlFlowControlFlowSafetyLoweringSummary() << "\n";
  out << "; type_system_optional_keypath_runtime_helper_contract = "
      << Objc3TypeSystemOptionalKeypathRuntimeHelperContractSummary() << "\n";
  if (!frontend_metadata_.lowering_super_dispatch_method_family_replay_key.empty()) {
    out << "; super_dispatch_method_family_lowering = "
        << frontend_metadata_.lowering_super_dispatch_method_family_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_runtime_link_host_link_replay_key.empty()) {
    out << "; runtime_link_host_link_lowering = "
        << frontend_metadata_.lowering_runtime_link_host_link_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_ownership_qualifier_replay_key.empty()) {
    out << "; ownership_qualifier_lowering = "
        << frontend_metadata_.lowering_ownership_qualifier_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_retain_release_operation_replay_key.empty()) {
    out << "; retain_release_operation_lowering = "
        << frontend_metadata_.lowering_retain_release_operation_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_autoreleasepool_scope_replay_key.empty()) {
    out << "; autoreleasepool_scope_lowering = "
        << frontend_metadata_.lowering_autoreleasepool_scope_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_weak_unowned_semantics_replay_key.empty()) {
    out << "; weak_unowned_semantics_lowering = "
        << frontend_metadata_.lowering_weak_unowned_semantics_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_arc_diagnostics_fixit_replay_key.empty()) {
    out << "; arc_diagnostics_fixit_lowering = "
        << frontend_metadata_.lowering_arc_diagnostics_fixit_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_block_literal_capture_replay_key.empty()) {
    out << "; block_literal_capture_lowering = "
        << frontend_metadata_.lowering_block_literal_capture_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_block_abi_invoke_trampoline_replay_key.empty()) {
    out << "; block_abi_invoke_trampoline_lowering = "
        << frontend_metadata_.lowering_block_abi_invoke_trampoline_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_block_storage_escape_replay_key.empty()) {
    out << "; block_storage_escape_lowering = "
        << frontend_metadata_.lowering_block_storage_escape_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_block_copy_dispose_replay_key.empty()) {
    out << "; block_copy_dispose_lowering = "
        << frontend_metadata_.lowering_block_copy_dispose_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_block_determinism_perf_baseline_replay_key.empty()) {
    out << "; block_determinism_perf_baseline_lowering = "
        << frontend_metadata_.lowering_block_determinism_perf_baseline_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_lightweight_generic_constraint_replay_key.empty()) {
    out << "; lightweight_generic_constraint_lowering = "
        << frontend_metadata_.lowering_lightweight_generic_constraint_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_nullability_flow_warning_precision_replay_key.empty()) {
    out << "; nullability_flow_warning_precision_lowering = "
        << frontend_metadata_.lowering_nullability_flow_warning_precision_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_protocol_qualified_object_type_replay_key.empty()) {
    out << "; protocol_qualified_object_type_lowering = "
        << frontend_metadata_.lowering_protocol_qualified_object_type_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_variance_bridge_cast_replay_key.empty()) {
    out << "; variance_bridge_cast_lowering = "
        << frontend_metadata_.lowering_variance_bridge_cast_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_generic_metadata_abi_replay_key.empty()) {
    out << "; generic_metadata_abi_lowering = "
        << frontend_metadata_.lowering_generic_metadata_abi_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_module_import_graph_replay_key.empty()) {
    // runtime-aware import/module surface anchor: the frontend
    // still preserves only the local translation-unit module-import graph
    // profile in emitted IR.
    // runtime-aware import/module surface anchor: the
    // frontend now emits a canonical runtime-import surface artifact for
    // later cross-translation-unit consumers.
    // cross-module semantic preservation anchor: imported runtime metadata semantics are not lowered into IR; the lane-B surface freezes
    // semantic preservation requirements without landing imported metadata
    // semantic equivalence yet.
    // imported metadata semantic rules anchor: imported runtime
    // surface artifacts may now be consumed and validated before IR
    // emission, but the imported runtime metadata payloads themselves still
    // are not lowered into IR in this lane.
    // serialized metadata import/lowering anchor: imported
    // runtime surface artifacts now freeze the semantic handoff boundary,
    // but serialized imported metadata payloads still are not rehydrated,
    // reused incrementally, or lowered into IR in this lane.
    // serialized metadata artifact reuse anchor: emitted
    // runtime-import-surface artifacts may now carry a transitive serialized
    // runtime-metadata payload for downstream frontend reuse, but imported
    // payloads still are not lowered directly into IR in this lane.
    // cross-module build/runtime orchestration anchor:
    // cross-module link-plan packaging and aggregated runtime-registration
    // orchestration remain outside the IR emitter; lane D only freezes the
    // boundary between emitted import-surface reuse payloads and the local
    // runtime registration manifest here.
    // cross-module runtime packaging anchor: lane D now consumes
    // those emitted object-local artifacts to publish an ordered cross-module
    // link plan and runtime-registration proof, but the IR emitter remains
    // object-local and does not directly orchestrate multi-image packaging.
    // cross-module object-model gate anchor: the truthful gate
    // still lives in the emitted evidence chain and not in any new
    // cross-module IR emitter surface.
    // runnable import/module execution-matrix anchor: the IR
    // emitter still stays object-local while lane-E proves the integrated
    // multi-image path above it.
    // Imported runtime-owned declarations and foreign metadata references
    // therefore
    // remain fail-closed in IR until the later lowering/runtime milestones.
    out << "; module_import_graph_lowering = "
        << frontend_metadata_.lowering_module_import_graph_replay_key << "\n";
  }
  if (!frontend_metadata_
           .lowering_namespace_collision_shadowing_replay_key.empty()) {
    out << "; namespace_collision_shadowing_lowering = "
        << frontend_metadata_
               .lowering_namespace_collision_shadowing_replay_key
        << "\n";
  }
  if (!frontend_metadata_
           .lowering_public_private_api_partition_replay_key.empty()) {
    out << "; public_private_api_partition_lowering = "
        << frontend_metadata_
               .lowering_public_private_api_partition_replay_key
        << "\n";
  }
  if (!frontend_metadata_
           .lowering_incremental_module_cache_invalidation_replay_key
           .empty()) {
    out << "; incremental_module_cache_invalidation_lowering = "
        << frontend_metadata_
               .lowering_incremental_module_cache_invalidation_replay_key
        << "\n";
  }
  if (!frontend_metadata_.lowering_cross_module_conformance_replay_key
           .empty()) {
    out << "; cross_module_conformance_lowering = "
        << frontend_metadata_.lowering_cross_module_conformance_replay_key
        << "\n";
  }
  if (!frontend_metadata_
           .lowering_error_handling_throws_abi_propagation_replay_key.empty()) {
    out << "; error_handling_throws_abi_propagation_lowering = "
        << frontend_metadata_
               .lowering_error_handling_throws_abi_propagation_replay_key
        << "\n";
  }
  if (!frontend_metadata_
           .lowering_error_handling_result_and_bridging_artifact_replay_key.empty()) {
    out << "; error_handling_result_and_bridging_artifact_replay = "
        << frontend_metadata_
               .lowering_error_handling_result_and_bridging_artifact_replay_key
        << "\n";
  }
  // error-runtime/bridge-helper anchor: publish the private
  // runtime helper ABI boundary consumed by the current runnable Part 6
  // lowering so the emitted IR truthfully advertises helper-backed storage,
  // bridge normalization, and catch dispatch instead of raw local-slot
  // traffic.
  out << "; error_handling_error_runtime_bridge_helper = "
      << Objc3ErrorHandlingErrorRuntimeBridgeHelperSummary() << "\n";
  // live catch/bridge/runtime integration anchor: publish the
  // executable Part 6 runtime-support boundary so linked object probes can
  // prove the private helper ABI is live rather than a contract-only marker.
  out << "; error_handling_live_error_runtime_integration = "
      << Objc3ErrorHandlingLiveErrorRuntimeIntegrationSummary() << "\n";
  // continuation/runtime-helper anchor: publish the first private
  // Part 7 helper ABI boundary for logical continuation allocation,
  // scheduler handoff, and resume traffic. The current async lowering slice
  // remains direct-call only, so this line freezes the helper contract
  // without claiming live suspension or executor scheduling yet.
  out << "; concurrency_continuation_runtime_helper = "
      << Objc3ConcurrencyContinuationRuntimeHelperSummary() << "\n";
  // live continuation/runtime integration anchor: publish the
  // runnable Part 7 helper-execution boundary so supported async fixtures can
  // prove the direct-call await path now executes through the helper cluster.
  out << "; concurrency_live_continuation_runtime_integration = "
      << Objc3ConcurrencyLiveContinuationRuntimeIntegrationSummary() << "\n";
  // ABI/artifact completion anchor: publish the dedicated Part 7
  // task-group/runtime ABI packet so later runtime freeze work consumes a
  // stable helper list and scheduler-visible proof surface.
  out << "; concurrency_task_runtime_abi_completion = contract="
      << kObjc3ConcurrencyTaskRuntimeAbiCompletionContractId
      << ";surface=" << kObjc3ConcurrencyTaskRuntimeAbiCompletionSurfacePath
      << ";helper_count=8;task_group_helper_count=4;runtime_snapshot="
      << "objc3_runtime_copy_task_runtime_state_for_testing"
      << ";follow_on_surface=objc3c.concurrency.taskruntime.helpersurface.v1\n";
  // scheduler/executor runtime freeze anchor: publish the private
  // task-runtime helper ABI and snapshot boundary as the truthful runtime
  // contract above the C003 ABI packet without claiming broader scheduler
  // execution semantics yet.
  out << "; concurrency_scheduler_executor_runtime_contract = "
      << Objc3ConcurrencySchedulerExecutorRuntimeSummary() << "\n";
  // live task runtime anchor: publish the runnable helper-backed
  // execution boundary so supported task-runtime probes can prove that the
  // existing private runtime cluster is live, while broader metadata-export
  // gating remains intentionally deferred.
  out << "; concurrency_live_task_runtime_integration = "
      << Objc3ConcurrencyLiveTaskRuntimeIntegrationSummary() << "\n";
  // hardening anchor: publish the reset/autorelease/cancellation
  // stability packet above the live task runtime boundary so issue-local
  // probes can prove deterministic replay across scope and reset edges.
  out << "; concurrency_task_runtime_hardening = "
      << Objc3ConcurrencyTaskRuntimeHardeningSummary() << "\n";
  if (!frontend_metadata_.lowering_throws_propagation_replay_key.empty()) {
    out << "; throws_propagation_lowering = "
        << frontend_metadata_.lowering_throws_propagation_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_ns_error_bridging_replay_key.empty()) {
    out << "; ns_error_bridging_lowering = "
        << frontend_metadata_.lowering_ns_error_bridging_replay_key << "\n";
  }
  if (!frontend_metadata_.lowering_unwind_cleanup_replay_key.empty()) {
    out << "; unwind_cleanup_lowering = "
        << frontend_metadata_.lowering_unwind_cleanup_replay_key << "\n";
  }
  if (!frontend_metadata_
           .lowering_error_diagnostics_recovery_replay_key.empty()) {
    out << "; error_diagnostics_recovery_lowering = "
        << frontend_metadata_
               .lowering_error_diagnostics_recovery_replay_key
        << "\n";
  }
  if (!frontend_metadata_.lowering_async_continuation_replay_key.empty()) {
    out << "; async_continuation_lowering = "
        << frontend_metadata_.lowering_async_continuation_replay_key << "\n";
  }
  if (!frontend_metadata_
           .lowering_await_lowering_suspension_state_replay_key.empty()) {
    out << "; await_lowering_suspension_state_lowering = "
        << frontend_metadata_
               .lowering_await_lowering_suspension_state_replay_key
        << "\n";
  }
  if (!frontend_metadata_
           .lowering_actor_isolation_sendability_replay_key.empty()) {
    out << "; actor_isolation_sendability_lowering = "
        << frontend_metadata_
               .lowering_actor_isolation_sendability_replay_key
        << "\n";
  }
}
