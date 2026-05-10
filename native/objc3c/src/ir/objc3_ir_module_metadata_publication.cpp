#include "ir/objc3_ir_module_metadata_publication.h"

#include <cstddef>
#include <sstream>
#include <string>

#include "ir/objc3_ir_emission_prologue.h"
#include "ir/objc3_ir_emission_readiness_publication.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication.h"
#include "ir/objc3_ir_module_metadata_publication_advanced_profiles.h"
#include "ir/objc3_ir_module_metadata_publication_core_profiles.h"
#include "ir/objc3_ir_module_metadata_publication_runtime_gates.h"
#include "ir/objc3_ir_property_metadata_comment_emission.h"

namespace {

struct Objc3IRModuleMetadataPublicationProgramView {
  const std::string &module_name;
};

}  // namespace

void EmitObjc3IRModuleMetadataPublication(
    const Objc3IRModuleMetadataPublicationOptions &options,
    std::ostringstream &out) {
  const Objc3IRFrontendMetadata &frontend_metadata_ =
      options.frontend_metadata;
  const Objc3LoweringIRBoundary &lowering_ir_boundary_ =
      options.lowering_ir_boundary;
  const std::size_t synthesized_property_accessor_count_ =
      options.synthesized_property_accessor_count;
  const std::size_t vector_signature_function_count_ =
      options.vector_signature_function_count;
  const Objc3IRModuleMetadataPublicationProgramView program_{
      options.module_name};
    out << BuildObjc3IRModulePrologue(Objc3IRModulePrologue{
        Objc3LoweringIRBoundaryReplayKey(lowering_ir_boundary_),
        Objc3RuntimeDispatchDeclarationReplayKey(lowering_ir_boundary_),
        Objc3SimdVectorTypeLoweringReplayKey(),
    });
    EmitObjc3IRPropertyMetadataCommentEmission(
        Objc3IRPropertyMetadataCommentEmissionOptions{
            frontend_metadata_, lowering_ir_boundary_,
            synthesized_property_accessor_count_},
        out);
    if (!frontend_metadata_.runtime_metadata_source_ownership_contract_id.empty()) {
      out << "; runtime_metadata_source_ownership = "
          << frontend_metadata_.runtime_metadata_source_ownership_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_export_legality_contract_id.empty()) {
      out << "; runtime_export_legality = "
          << frontend_metadata_.runtime_export_legality_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_export_enforcement_contract_id.empty()) {
      out << "; runtime_export_enforcement = "
          << frontend_metadata_.runtime_export_enforcement_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_metadata_section_abi_contract_id.empty()) {
      out << "; runtime_metadata_section_abi = "
          << frontend_metadata_.runtime_metadata_section_abi_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_metadata_section_publication_contract_id.empty()) {
      out << "; runtime_metadata_section_publication = "
          << frontend_metadata_.runtime_metadata_section_publication_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_metadata_object_inspection_contract_id.empty()) {
      out << "; runtime_metadata_object_inspection = "
          << frontend_metadata_.runtime_metadata_object_inspection_contract_id
          << "\n";
    }
    out << "; runtime_selector_lookup_tables = contract="
        << kObjc3RuntimeSelectorLookupTablesContractId
        << ", interning_model="
        << kObjc3RuntimeSelectorLookupTablesInterningModel
        << ", merge_model="
        << kObjc3RuntimeSelectorLookupTablesMergeModel
        << ", dynamic_miss_model="
        << kObjc3RuntimeSelectorLookupTablesDynamicMissModel << "\n";
    out << "; runtime_method_cache_slow_path_lookup = contract="
        << kObjc3RuntimeMethodCacheSlowPathContractId
        << ", receiver_normalization_model="
        << kObjc3RuntimeMethodCacheSlowPathReceiverNormalizationModel
        << ", resolution_model="
        << kObjc3RuntimeMethodCacheSlowPathResolutionModel
        << ", cache_model="
        << kObjc3RuntimeMethodCacheSlowPathCacheModel
        << ", strict_error_model="
        << kObjc3RuntimeMethodCacheSlowPathStrictErrorModel << "\n";
    out << "; runtime_protocol_category_method_resolution = contract="
        << kObjc3RuntimeProtocolCategoryMethodResolutionContractId
        << ", category_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionCategoryModel
        << ", protocol_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionProtocolModel
        << ", strict_error_model="
        << kObjc3RuntimeProtocolCategoryMethodResolutionStrictErrorModel << "\n";
    if (!frontend_metadata_
             .executable_metadata_debug_projection_contract_id.empty()) {
      out << "; executable_metadata_debug_projection = "
          << frontend_metadata_.executable_metadata_debug_projection_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_support_library_contract_id.empty()) {
      out << "; runtime_support_library = "
          << frontend_metadata_.runtime_support_library_contract_id << "\n";
    }
    if (!frontend_metadata_.runtime_support_library_core_feature_contract_id
             .empty()) {
      out << "; runtime_support_library_core_feature = "
          << frontend_metadata_.runtime_support_library_core_feature_contract_id
          << "\n";
    }
    if (!frontend_metadata_.runtime_support_library_link_wiring_contract_id
             .empty()) {
      out << "; runtime_support_library_link_wiring = "
          << frontend_metadata_.runtime_support_library_link_wiring_contract_id
          << "\n";
    }
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
    EmitObjc3IRLoweringExtensionCommentPublication(frontend_metadata_, out);
    EmitObjc3IREmissionReadinessPublication(frontend_metadata_, out);
    out << "; simd_vector_function_signatures = " << vector_signature_function_count_ << "\n";
    EmitObjc3IRModuleMetadataCoreProfilePublication(
        frontend_metadata_, synthesized_property_accessor_count_, out);
    EmitObjc3IRModuleMetadataRuntimeSemanticsGatePublication(
        frontend_metadata_, out);
    out << "; frontend_objc_ownership_qualifier_lowering_profile = ownership_qualifier_sites="
        << frontend_metadata_.ownership_qualifier_lowering_ownership_qualifier_sites
        << ", invalid_ownership_qualifier_sites="
        << frontend_metadata_.ownership_qualifier_lowering_invalid_ownership_qualifier_sites
        << ", object_pointer_type_annotation_sites="
        << frontend_metadata_.ownership_qualifier_lowering_object_pointer_type_annotation_sites
        << ", deterministic_ownership_qualifier_lowering_handoff="
        << (frontend_metadata_.deterministic_ownership_qualifier_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_retain_release_operation_lowering_profile = ownership_qualified_sites="
        << frontend_metadata_.retain_release_operation_lowering_ownership_qualified_sites
        << ", retain_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_retain_insertion_sites
        << ", release_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_release_insertion_sites
        << ", autorelease_insertion_sites="
        << frontend_metadata_.retain_release_operation_lowering_autorelease_insertion_sites
        << ", contract_violation_sites="
        << frontend_metadata_.retain_release_operation_lowering_contract_violation_sites
        << ", deterministic_retain_release_operation_lowering_handoff="
        << (frontend_metadata_.deterministic_retain_release_operation_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_autoreleasepool_scope_lowering_profile = scope_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
        << ", scope_symbolized_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_symbolized_sites
        << ", max_scope_depth="
        << frontend_metadata_.autoreleasepool_scope_lowering_max_scope_depth
        << ", scope_entry_transition_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_entry_transition_sites
        << ", scope_exit_transition_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_scope_exit_transition_sites
        << ", contract_violation_sites="
        << frontend_metadata_.autoreleasepool_scope_lowering_contract_violation_sites
        << ", deterministic_autoreleasepool_scope_lowering_handoff="
        << (frontend_metadata_.deterministic_autoreleasepool_scope_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_weak_unowned_semantics_lowering_profile = ownership_candidate_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_ownership_candidate_sites
        << ", weak_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_weak_reference_sites
        << ", unowned_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_unowned_reference_sites
        << ", unowned_safe_reference_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_unowned_safe_reference_sites
        << ", weak_unowned_conflict_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_conflict_sites
        << ", contract_violation_sites="
        << frontend_metadata_.weak_unowned_semantics_lowering_contract_violation_sites
        << ", deterministic_weak_unowned_semantics_lowering_handoff="
        << (frontend_metadata_.deterministic_weak_unowned_semantics_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_arc_diagnostics_fixit_lowering_profile = ownership_arc_diagnostic_candidate_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_diagnostic_candidate_sites
        << ", ownership_arc_fixit_available_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_fixit_available_sites
        << ", ownership_arc_profiled_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_profiled_sites
        << ", ownership_arc_weak_unowned_conflict_diagnostic_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_weak_unowned_conflict_diagnostic_sites
        << ", ownership_arc_empty_fixit_hint_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_ownership_arc_empty_fixit_hint_sites
        << ", contract_violation_sites="
        << frontend_metadata_.arc_diagnostics_fixit_lowering_contract_violation_sites
        << ", deterministic_arc_diagnostics_fixit_lowering_handoff="
        << (frontend_metadata_.deterministic_arc_diagnostics_fixit_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_literal_capture_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_literal_sites
        << ", block_parameter_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_parameter_entries
        << ", block_capture_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_capture_entries
        << ", block_body_statement_entries="
        << frontend_metadata_.block_literal_capture_lowering_block_body_statement_entries
        << ", block_empty_capture_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_empty_capture_sites
        << ", block_nondeterministic_capture_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_nondeterministic_capture_sites
        << ", block_non_normalized_sites="
        << frontend_metadata_.block_literal_capture_lowering_block_non_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_literal_capture_lowering_contract_violation_sites
        << ", deterministic_block_literal_capture_lowering_handoff="
        << (frontend_metadata_.deterministic_block_literal_capture_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_abi_invoke_trampoline_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_block_literal_sites
        << ", invoke_argument_slots_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_argument_slots_total
        << ", capture_word_count_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_word_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_body_statement_entries_total
        << ", descriptor_symbolized_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_descriptor_symbolized_sites
        << ", invoke_trampoline_symbolized_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_invoke_symbolized_sites
        << ", missing_invoke_trampoline_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_missing_invoke_sites
        << ", non_normalized_layout_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_non_normalized_layout_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_abi_invoke_trampoline_lowering_contract_violation_sites
        << ", deterministic_block_abi_invoke_trampoline_lowering_handoff="
        << (frontend_metadata_.deterministic_block_abi_invoke_trampoline_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_storage_escape_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_storage_escape_lowering_block_literal_sites
        << ", mutable_capture_count_total="
        << frontend_metadata_.block_storage_escape_lowering_mutable_capture_count_total
        << ", byref_slot_count_total="
        << frontend_metadata_.block_storage_escape_lowering_byref_slot_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_storage_escape_lowering_body_statement_entries_total
        << ", requires_byref_cells_sites="
        << frontend_metadata_.block_storage_escape_lowering_requires_byref_cells_sites
        << ", escape_analysis_enabled_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_analysis_enabled_sites
        << ", escape_to_heap_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites
        << ", escape_profile_normalized_sites="
        << frontend_metadata_.block_storage_escape_lowering_escape_profile_normalized_sites
        << ", byref_layout_symbolized_sites="
        << frontend_metadata_.block_storage_escape_lowering_byref_layout_symbolized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_storage_escape_lowering_contract_violation_sites
        << ", deterministic_block_storage_escape_lowering_handoff="
        << (frontend_metadata_.deterministic_block_storage_escape_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_copy_dispose_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_copy_dispose_lowering_block_literal_sites
        << ", mutable_capture_count_total="
        << frontend_metadata_.block_copy_dispose_lowering_mutable_capture_count_total
        << ", byref_slot_count_total="
        << frontend_metadata_.block_copy_dispose_lowering_byref_slot_count_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_copy_dispose_lowering_body_statement_entries_total
        << ", copy_helper_required_sites="
        << frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites
        << ", dispose_helper_required_sites="
        << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites
        << ", profile_normalized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_profile_normalized_sites
        << ", copy_helper_symbolized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_copy_helper_symbolized_sites
        << ", dispose_helper_symbolized_sites="
        << frontend_metadata_.block_copy_dispose_lowering_dispose_helper_symbolized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_copy_dispose_lowering_contract_violation_sites
        << ", deterministic_block_copy_dispose_lowering_handoff="
        << (frontend_metadata_.deterministic_block_copy_dispose_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_block_determinism_perf_baseline_lowering_profile = block_literal_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_block_literal_sites
        << ", baseline_weight_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_baseline_weight_total
        << ", parameter_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_parameter_entries_total
        << ", capture_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_capture_entries_total
        << ", body_statement_entries_total="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_body_statement_entries_total
        << ", deterministic_capture_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_deterministic_capture_sites
        << ", heavy_tier_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_heavy_tier_sites
        << ", normalized_profile_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_normalized_profile_sites
        << ", contract_violation_sites="
        << frontend_metadata_.block_determinism_perf_baseline_lowering_contract_violation_sites
        << ", deterministic_block_determinism_perf_baseline_lowering_handoff="
        << (frontend_metadata_.deterministic_block_determinism_perf_baseline_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_lightweight_generic_constraint_lowering_profile = generic_constraint_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_generic_constraint_sites
        << ", generic_suffix_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_generic_suffix_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_object_pointer_type_sites
        << ", terminated_generic_suffix_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_terminated_generic_suffix_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_pointer_declarator_sites
        << ", normalized_constraint_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_normalized_constraint_sites
        << ", contract_violation_sites="
        << frontend_metadata_.lightweight_generic_constraint_lowering_contract_violation_sites
        << ", deterministic_lightweight_generic_constraint_lowering_handoff="
        << (frontend_metadata_.deterministic_lightweight_generic_constraint_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_nullability_flow_warning_precision_lowering_profile = nullability_flow_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_object_pointer_type_sites
        << ", nullability_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nullability_suffix_sites
        << ", nullable_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nullable_suffix_sites
        << ", nonnull_suffix_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_nonnull_suffix_sites
        << ", normalized_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.nullability_flow_warning_precision_lowering_contract_violation_sites
        << ", deterministic_nullability_flow_warning_precision_lowering_handoff="
        << (frontend_metadata_.deterministic_nullability_flow_warning_precision_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_protocol_qualified_object_type_lowering_profile = protocol_qualified_object_type_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_protocol_composition_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_object_pointer_type_sites
        << ", terminated_protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_terminated_protocol_composition_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_pointer_declarator_sites
        << ", normalized_protocol_composition_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_normalized_protocol_composition_sites
        << ", contract_violation_sites="
        << frontend_metadata_.protocol_qualified_object_type_lowering_contract_violation_sites
        << ", deterministic_protocol_qualified_object_type_lowering_handoff="
        << (frontend_metadata_.deterministic_protocol_qualified_object_type_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_variance_bridge_cast_lowering_profile = variance_bridge_cast_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_protocol_composition_sites
        << ", ownership_qualifier_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_ownership_qualifier_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.variance_bridge_cast_lowering_contract_violation_sites
        << ", deterministic_variance_bridge_cast_lowering_handoff="
        << (frontend_metadata_.deterministic_variance_bridge_cast_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_generic_metadata_abi_lowering_profile = generic_metadata_abi_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_sites
        << ", generic_suffix_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_generic_suffix_sites
        << ", protocol_composition_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_protocol_composition_sites
        << ", ownership_qualifier_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_ownership_qualifier_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.generic_metadata_abi_lowering_contract_violation_sites
        << ", deterministic_generic_metadata_abi_lowering_handoff="
        << (frontend_metadata_.deterministic_generic_metadata_abi_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_module_import_graph_lowering_profile = module_import_graph_sites="
        << frontend_metadata_.module_import_graph_lowering_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_.module_import_graph_lowering_import_edge_candidate_sites
        << ", namespace_segment_sites="
        << frontend_metadata_.module_import_graph_lowering_namespace_segment_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.module_import_graph_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.module_import_graph_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.module_import_graph_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_.module_import_graph_lowering_contract_violation_sites
        << ", deterministic_module_import_graph_lowering_handoff="
        << (frontend_metadata_.deterministic_module_import_graph_lowering_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_namespace_collision_shadowing_lowering_profile = namespace_collision_shadowing_sites="
        << frontend_metadata_.namespace_collision_shadowing_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .namespace_collision_shadowing_lowering_contract_violation_sites
        << ", deterministic_namespace_collision_shadowing_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_namespace_collision_shadowing_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_public_private_api_partition_lowering_profile = public_private_api_partition_sites="
        << frontend_metadata_.public_private_api_partition_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_normalized_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .public_private_api_partition_lowering_contract_violation_sites
        << ", deterministic_public_private_api_partition_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_public_private_api_partition_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_incremental_module_cache_invalidation_lowering_profile = incremental_module_cache_invalidation_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .incremental_module_cache_invalidation_lowering_contract_violation_sites
        << ", deterministic_incremental_module_cache_invalidation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_incremental_module_cache_invalidation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_cross_module_conformance_lowering_profile = cross_module_conformance_sites="
        << frontend_metadata_.cross_module_conformance_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.cross_module_conformance_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .cross_module_conformance_lowering_contract_violation_sites
        << ", deterministic_cross_module_conformance_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_cross_module_conformance_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_throws_propagation_lowering_profile = throws_propagation_sites="
        << frontend_metadata_.throws_propagation_lowering_sites
        << ", namespace_segment_sites="
        << frontend_metadata_.throws_propagation_lowering_namespace_segment_sites
        << ", import_edge_candidate_sites="
        << frontend_metadata_
               .throws_propagation_lowering_import_edge_candidate_sites
        << ", object_pointer_type_sites="
        << frontend_metadata_.throws_propagation_lowering_object_pointer_type_sites
        << ", pointer_declarator_sites="
        << frontend_metadata_.throws_propagation_lowering_pointer_declarator_sites
        << ", normalized_sites="
        << frontend_metadata_.throws_propagation_lowering_normalized_sites
        << ", cache_invalidation_candidate_sites="
        << frontend_metadata_
               .throws_propagation_lowering_cache_invalidation_candidate_sites
        << ", contract_violation_sites="
        << frontend_metadata_.throws_propagation_lowering_contract_violation_sites
        << ", deterministic_throws_propagation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_throws_propagation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_ns_error_bridging_lowering_profile = ns_error_bridging_sites="
        << frontend_metadata_.ns_error_bridging_lowering_sites
        << ", ns_error_parameter_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_parameter_sites
        << ", ns_error_out_parameter_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_out_parameter_sites
        << ", ns_error_bridge_path_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_ns_error_bridge_path_sites
        << ", failable_call_sites="
        << frontend_metadata_.ns_error_bridging_lowering_failable_call_sites
        << ", normalized_sites="
        << frontend_metadata_.ns_error_bridging_lowering_normalized_sites
        << ", bridge_boundary_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_bridge_boundary_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .ns_error_bridging_lowering_contract_violation_sites
        << ", deterministic_ns_error_bridging_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_ns_error_bridging_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_unwind_cleanup_lowering_profile = unwind_cleanup_sites="
        << frontend_metadata_.unwind_cleanup_lowering_sites
        << ", unwind_edge_sites="
        << frontend_metadata_.unwind_cleanup_lowering_unwind_edge_sites
        << ", cleanup_scope_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_scope_sites
        << ", cleanup_emit_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_emit_sites
        << ", landing_pad_sites="
        << frontend_metadata_.unwind_cleanup_lowering_landing_pad_sites
        << ", cleanup_resume_sites="
        << frontend_metadata_.unwind_cleanup_lowering_cleanup_resume_sites
        << ", normalized_sites="
        << frontend_metadata_.unwind_cleanup_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.unwind_cleanup_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.unwind_cleanup_lowering_contract_violation_sites
        << ", deterministic_unwind_cleanup_lowering_handoff="
        << (frontend_metadata_.deterministic_unwind_cleanup_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_error_diagnostics_recovery_lowering_profile = error_diagnostic_sites="
        << frontend_metadata_.error_diagnostics_recovery_lowering_sites
        << ", parser_diagnostic_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_parser_diagnostic_sites
        << ", semantic_diagnostic_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_semantic_diagnostic_sites
        << ", fixit_hint_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_fixit_hint_sites
        << ", recovery_candidate_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_recovery_candidate_sites
        << ", recovery_applied_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_recovery_applied_sites
        << ", normalized_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .error_diagnostics_recovery_lowering_contract_violation_sites
        << ", deterministic_error_diagnostics_recovery_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_error_diagnostics_recovery_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    EmitObjc3IRModuleMetadataAdvancedProfilePublication(frontend_metadata_, out);
    out << "; frontend_objc_object_pointer_nullability_generics_profile = object_pointer_type_spellings="
        << frontend_metadata_.object_pointer_type_spellings
        << ", pointer_declarator_entries=" << frontend_metadata_.pointer_declarator_entries
        << ", pointer_declarator_depth_total=" << frontend_metadata_.pointer_declarator_depth_total
        << ", pointer_declarator_token_entries=" << frontend_metadata_.pointer_declarator_token_entries
        << ", nullability_suffix_entries=" << frontend_metadata_.nullability_suffix_entries
        << ", generic_suffix_entries=" << frontend_metadata_.generic_suffix_entries
        << ", terminated_generic_suffix_entries=" << frontend_metadata_.terminated_generic_suffix_entries
        << ", unterminated_generic_suffix_entries=" << frontend_metadata_.unterminated_generic_suffix_entries
        << ", deterministic_object_pointer_nullability_generics_handoff="
        << (frontend_metadata_.deterministic_object_pointer_nullability_generics_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_symbol_graph_scope_resolution_profile = global_symbol_nodes="
        << frontend_metadata_.global_symbol_nodes
        << ", function_symbol_nodes=" << frontend_metadata_.function_symbol_nodes
        << ", interface_symbol_nodes=" << frontend_metadata_.interface_symbol_nodes
        << ", implementation_symbol_nodes=" << frontend_metadata_.implementation_symbol_nodes
        << ", interface_property_symbol_nodes=" << frontend_metadata_.interface_property_symbol_nodes
        << ", implementation_property_symbol_nodes=" << frontend_metadata_.implementation_property_symbol_nodes
        << ", interface_method_symbol_nodes=" << frontend_metadata_.interface_method_symbol_nodes
        << ", implementation_method_symbol_nodes=" << frontend_metadata_.implementation_method_symbol_nodes
        << ", top_level_scope_symbols=" << frontend_metadata_.top_level_scope_symbols
        << ", nested_scope_symbols=" << frontend_metadata_.nested_scope_symbols
        << ", scope_frames_total=" << frontend_metadata_.scope_frames_total
        << ", implementation_interface_resolution_sites="
        << frontend_metadata_.implementation_interface_resolution_sites
        << ", implementation_interface_resolution_hits="
        << frontend_metadata_.implementation_interface_resolution_hits
        << ", implementation_interface_resolution_misses="
        << frontend_metadata_.implementation_interface_resolution_misses
        << ", method_resolution_sites=" << frontend_metadata_.method_resolution_sites
        << ", method_resolution_hits=" << frontend_metadata_.method_resolution_hits
        << ", method_resolution_misses=" << frontend_metadata_.method_resolution_misses
        << ", deterministic_symbol_graph_handoff="
        << (frontend_metadata_.deterministic_symbol_graph_handoff ? "true" : "false")
        << ", deterministic_scope_resolution_handoff="
        << (frontend_metadata_.deterministic_scope_resolution_handoff ? "true" : "false")
        << ", deterministic_symbol_graph_scope_resolution_handoff_key="
        << frontend_metadata_.deterministic_symbol_graph_scope_resolution_handoff_key << "\n";
    out << "source_filename = \"" << program_.module_name << ".objc3\"\n\n";
}
