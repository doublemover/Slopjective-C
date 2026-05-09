#include "ir/objc3_ir_emitter.h"

#include <algorithm>
#include <array>
#include <cstdint>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ast/objc3_ast.h"
#include "ir/objc3_ir_block_runtime_contracts.h"
#include "ir/objc3_ir_canonical_literal_pools.h"
#include "ir/objc3_ir_concurrency_identity.h"
#include "ir/objc3_ir_concurrency_runtime_call_emission.h"
#include "ir/objc3_ir_control_flow_ops.h"
#include "ir/objc3_ir_direct_call_emission.h"
#include "ir/objc3_ir_emission_helpers.h"
#include "ir/objc3_ir_emission_prologue.h"
#include "ir/objc3_ir_emission_readiness_publication.h"
#include "ir/objc3_ir_entry_point_emission.h"
#include "ir/objc3_ir_emitter_context.h"
#include "ir/objc3_ir_expression_emission.h"
#include "ir/objc3_ir_frontend_metadata_publication.h"
#include "ir/objc3_ir_function_effect_analysis.h"
#include "ir/objc3_ir_function_definition_emission.h"
#include "ir/objc3_ir_message_send_emission.h"
#include "ir/objc3_ir_lowering_extension_metadata_publication.h"
#include "ir/objc3_ir_message_send_lowering.h"
#include "ir/objc3_ir_message_send_validation.h"
#include "ir/objc3_ir_method_definition_plan.h"
#include "ir/objc3_ir_module_emission_surface.h"
#include "ir/objc3_ir_property_metadata_comment_emission.h"
#include "ir/objc3_ir_prototype_declarations.h"
#include "ir/objc3_ir_receiver_dispatch_policy.h"
#include "ir/objc3_ir_receiver_identity_contracts.h"
#include "ir/objc3_ir_runtime_dispatch_calls.h"
#include "ir/objc3_ir_runtime_dispatch_declarations.h"
#include "ir/objc3_ir_runtime_dispatch_state.h"
#include "ir/objc3_ir_runtime_bootstrap_global_emission.h"
#include "ir/objc3_ir_runtime_helper_calls.h"
#include "ir/objc3_ir_runtime_metadata_emission.h"
#include "ir/objc3_ir_runtime_metadata_scaffold_emission.h"
#include "ir/objc3_ir_scope_cleanup_emission.h"
#include "ir/objc3_ir_statement_emission.h"
#include "ir/objc3_ir_static_data_emission.h"
#include "ir/objc3_ir_synthesized_property_accessors.h"
#include "parse/objc3_parse_support.h"

class Objc3IREmitter {
 public:
  Objc3IREmitter(const Objc3Program &program,
                 const Objc3LoweringContract &lowering_contract,
                 const Objc3IRFrontendMetadata &frontend_metadata)
      : program_(program),
        frontend_metadata_(frontend_metadata),
        runtime_metadata_symbols_(
            BuildObjc3IRRuntimeMetadataSymbols(program.module_name,
                                               frontend_metadata)) {
    if (!TryBuildObjc3LoweringIRBoundary(lowering_contract, lowering_ir_boundary_, boundary_error_)) {
      return;
    }
    vector_signature_function_count_ = CountVectorSignatureFunctions(program_);
    for (const auto &global : program_.globals) {
      globals_.insert(global.name);
    }
    for (const auto &fn : program_.functions) {
      function_arity_[fn.name] = fn.params.size();
      if (fn.is_pure) {
        declared_pure_functions_.insert(fn.name);
      }
      if (!fn.is_prototype && defined_functions_.insert(fn.name).second) {
        function_definitions_.push_back(&fn);
      }
    }
    Objc3IRMethodDefinitionPlan method_definition_plan =
        BuildObjc3IRMethodDefinitionPlan(program_, frontend_metadata_);
    if (!method_definition_plan.error.empty()) {
      boundary_error_ = method_definition_plan.error;
      return;
    }
    method_definitions_ = method_definition_plan.method_definitions;
    direct_dispatch_symbols_by_key_ =
        method_definition_plan.direct_dispatch_symbols_by_key;
    metaprogramming_global_artifacts_ =
        method_definition_plan.metaprogramming_global_artifacts;
    synthesized_property_accessor_count_ =
        method_definition_plan.synthesized_property_accessor_count;
    metaprogramming_derived_method_count_ =
        method_definition_plan.metaprogramming_derived_method_count;
    function_signatures_ = BuildLoweredFunctionSignatures(program_);
    CollectKnownClassReceiverConstants();
    Objc3IRCanonicalLiteralPools canonical_literal_pools =
        BuildObjc3IRCanonicalLiteralPools(program_, frontend_metadata_);
    selector_pool_globals_ =
        std::move(canonical_literal_pools.selector_pool_globals);
    runtime_string_pool_globals_ =
        std::move(canonical_literal_pools.runtime_string_pool_globals);
    typed_keypath_artifacts_ =
        std::move(canonical_literal_pools.typed_keypath_artifacts);
    Objc3IRFunctionEffectAnalysis function_effect_analysis =
        BuildObjc3IRFunctionEffectAnalysis(
            Objc3IRFunctionEffectAnalysisOptions{
                function_definitions_, globals_, defined_functions_,
                declared_pure_functions_});
    mutable_global_symbols_ =
        std::move(function_effect_analysis.mutable_global_symbols);
    function_effects_ = std::move(function_effect_analysis.function_effects);
    impure_functions_ = std::move(function_effect_analysis.impure_functions);
  }

  bool Emit(std::string &ir, std::string &error) {
    runtime_dispatch_call_state_.Reset();
    synthesized_getter_definition_count_ = 0;
    synthesized_setter_definition_count_ = 0;
    current_property_read_helper_call_count_ = 0;
    current_property_write_helper_call_count_ = 0;
    current_property_exchange_helper_call_count_ = 0;
    weak_current_property_load_helper_call_count_ = 0;
    weak_current_property_store_helper_call_count_ = 0;
    retain_helper_call_count_ = 0;
    release_helper_call_count_ = 0;
    autorelease_helper_call_count_ = 0;
    unsupported_fail_closed_path_triggered_ = false;
    unsupported_fail_closed_path_reason_.clear();
    block_function_definitions_.clear();
    emitted_block_invoke_symbols_.clear();
    emitted_block_copy_helper_symbols_.clear();
    emitted_block_dispose_helper_symbols_.clear();

    if (!boundary_error_.empty()) {
      error = boundary_error_;
      return false;
    }
    if (!ValidateObjc3IRMessageSendArityContract(
            program_, lowering_ir_boundary_.runtime_dispatch_arg_slots,
            error)) {
      return false;
    }

    std::ostringstream body;

    if (!EmitObjc3IRStaticData(
            Objc3IRStaticDataEmissionOptions{
                program_.globals, mutable_global_symbols_,
                metaprogramming_global_artifacts_, global_const_values_,
                global_nil_proven_symbols_,
                [this](const Expr *expr) {
                  return IsCompileTimeGlobalNilExpr(expr);
                }},
            body, error)) {
      return false;
    }

    EmitRuntimeMetadataSectionScaffold(body);

    EmitPrototypeDeclarations(body);

    EmitRuntimeBootstrapLoweringFunctions(body);

    for (const FunctionDecl *fn : function_definitions_) {
      EmitFunction(*fn, body);
      body << "\n";
    }
    for (const Objc3IRMethodDefinition &method_def : method_definitions_) {
      EmitMethod(method_def, body);
      body << "\n";
    }
    for (const std::string &definition : block_function_definitions_) {
      body << definition << "\n";
    }

    EmitEntryPoint(body);
    EmitRuntimeDispatchDeclarations(body);

    if (unsupported_fail_closed_path_triggered_) {
      error = "lowering encountered unsupported fail-closed path: " + unsupported_fail_closed_path_reason_;
      return false;
    }

    std::ostringstream out;
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
    out << BuildObjc3IRFrontendProfileComment(frontend_metadata_) << "\n";
    out << "; frontend_objc_interface_implementation_profile = declared_interfaces="
        << frontend_metadata_.declared_interfaces
        << ", declared_implementations=" << frontend_metadata_.declared_implementations
        << ", resolved_interface_symbols=" << frontend_metadata_.resolved_interface_symbols
        << ", resolved_implementation_symbols=" << frontend_metadata_.resolved_implementation_symbols
        << ", interface_method_symbols=" << frontend_metadata_.interface_method_symbols
        << ", implementation_method_symbols=" << frontend_metadata_.implementation_method_symbols
        << ", linked_implementation_symbols=" << frontend_metadata_.linked_implementation_symbols
        << ", deterministic_interface_implementation_handoff="
        << (frontend_metadata_.deterministic_interface_implementation_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_protocol_category_profile = declared_protocols="
        << frontend_metadata_.declared_protocols
        << ", declared_categories=" << frontend_metadata_.declared_categories
        << ", resolved_protocol_symbols=" << frontend_metadata_.resolved_protocol_symbols
        << ", resolved_category_symbols=" << frontend_metadata_.resolved_category_symbols
        << ", protocol_method_symbols=" << frontend_metadata_.protocol_method_symbols
        << ", category_method_symbols=" << frontend_metadata_.category_method_symbols
        << ", linked_category_symbols=" << frontend_metadata_.linked_category_symbols
        << ", deterministic_protocol_category_handoff="
        << (frontend_metadata_.deterministic_protocol_category_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_class_protocol_category_linking_profile = declared_class_interfaces="
        << frontend_metadata_.declared_class_interfaces
        << ", declared_class_implementations=" << frontend_metadata_.declared_class_implementations
        << ", resolved_class_interfaces=" << frontend_metadata_.resolved_class_interfaces
        << ", resolved_class_implementations=" << frontend_metadata_.resolved_class_implementations
        << ", linked_class_method_symbols=" << frontend_metadata_.linked_class_method_symbols
        << ", linked_category_method_symbols=" << frontend_metadata_.linked_category_method_symbols
        << ", protocol_composition_sites=" << frontend_metadata_.protocol_composition_sites
        << ", protocol_composition_symbols=" << frontend_metadata_.protocol_composition_symbols
        << ", category_composition_sites=" << frontend_metadata_.category_composition_sites
        << ", category_composition_symbols=" << frontend_metadata_.category_composition_symbols
        << ", invalid_protocol_composition_sites=" << frontend_metadata_.invalid_protocol_composition_sites
        << ", deterministic_class_protocol_category_linking_handoff="
        << (frontend_metadata_.deterministic_class_protocol_category_linking_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_selector_normalization_profile = method_declaration_entries="
        << frontend_metadata_.selector_method_declaration_entries
        << ", normalized_method_declarations=" << frontend_metadata_.selector_normalized_method_declarations
        << ", selector_piece_entries=" << frontend_metadata_.selector_piece_entries
        << ", selector_piece_parameter_links=" << frontend_metadata_.selector_piece_parameter_links
        << ", deterministic_selector_normalization_handoff="
        << (frontend_metadata_.deterministic_selector_normalization_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_property_attribute_profile = property_declaration_entries="
        << frontend_metadata_.property_declaration_entries
        << ", property_attribute_entries=" << frontend_metadata_.property_attribute_entries
        << ", property_attribute_value_entries=" << frontend_metadata_.property_attribute_value_entries
        << ", property_accessor_modifier_entries=" << frontend_metadata_.property_accessor_modifier_entries
        << ", property_getter_selector_entries=" << frontend_metadata_.property_getter_selector_entries
        << ", property_setter_selector_entries=" << frontend_metadata_.property_setter_selector_entries
        << ", deterministic_property_attribute_handoff="
        << (frontend_metadata_.deterministic_property_attribute_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_id_class_sel_object_pointer_typecheck_profile = id_typecheck_sites="
        << frontend_metadata_.id_typecheck_sites
        << ", class_typecheck_sites=" << frontend_metadata_.class_typecheck_sites
        << ", sel_typecheck_sites=" << frontend_metadata_.sel_typecheck_sites
        << ", object_pointer_typecheck_sites=" << frontend_metadata_.object_pointer_typecheck_sites
        << ", total_typecheck_sites=" << frontend_metadata_.id_class_sel_object_pointer_typecheck_sites_total
        << ", deterministic_id_class_sel_object_pointer_typecheck_handoff="
        << (frontend_metadata_.deterministic_id_class_sel_object_pointer_typecheck_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_dispatch_surface_classification_profile = instance_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_instance_sites
        << ", class_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_class_sites
        << ", super_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_super_sites
        << ", direct_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_direct_sites
        << ", dynamic_dispatch_sites="
        << frontend_metadata_.dispatch_surface_classification_dynamic_sites
        << ", instance_entrypoint_family="
        << frontend_metadata_
               .dispatch_surface_classification_instance_entrypoint_family
        << ", class_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_class_entrypoint_family
        << ", super_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_super_entrypoint_family
        << ", direct_entrypoint_family="
        << frontend_metadata_.dispatch_surface_classification_direct_entrypoint_family
        << ", dynamic_entrypoint_family="
        << frontend_metadata_
               .dispatch_surface_classification_dynamic_entrypoint_family
        << ", deterministic_dispatch_surface_classification_handoff="
        << (frontend_metadata_.deterministic_dispatch_surface_classification_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_message_send_selector_lowering_profile = message_send_sites="
        << frontend_metadata_.message_send_selector_lowering_sites
        << ", unary_selector_sites=" << frontend_metadata_.message_send_selector_lowering_unary_sites
        << ", keyword_selector_sites=" << frontend_metadata_.message_send_selector_lowering_keyword_sites
        << ", selector_piece_sites=" << frontend_metadata_.message_send_selector_lowering_selector_piece_sites
        << ", argument_expression_sites="
        << frontend_metadata_.message_send_selector_lowering_argument_expression_sites
        << ", receiver_expression_sites=" << frontend_metadata_.message_send_selector_lowering_receiver_sites
        << ", selector_literal_entries="
        << frontend_metadata_.message_send_selector_lowering_selector_literal_entries
        << ", selector_literal_characters="
        << frontend_metadata_.message_send_selector_lowering_selector_literal_characters
        << ", deterministic_message_send_selector_lowering_handoff="
        << (frontend_metadata_.deterministic_message_send_selector_lowering_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_dispatch_abi_marshalling_profile = message_send_sites="
        << frontend_metadata_.dispatch_abi_marshalling_message_send_sites
        << ", receiver_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_receiver_slots_marshaled
        << ", selector_slots_marshaled=" << frontend_metadata_.dispatch_abi_marshalling_selector_slots_marshaled
        << ", argument_value_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_value_slots_marshaled
        << ", argument_padding_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_padding_slots_marshaled
        << ", argument_total_slots_marshaled="
        << frontend_metadata_.dispatch_abi_marshalling_argument_total_slots_marshaled
        << ", total_marshaled_slots=" << frontend_metadata_.dispatch_abi_marshalling_total_marshaled_slots
        << ", runtime_dispatch_arg_slots="
        << frontend_metadata_.dispatch_abi_marshalling_runtime_dispatch_arg_slots
        << ", deterministic_dispatch_abi_marshalling_handoff="
        << (frontend_metadata_.deterministic_dispatch_abi_marshalling_handoff ? "true" : "false") << "\n";
    out << "; frontend_objc_nil_receiver_semantics_foldability_profile = message_send_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_message_send_sites
        << ", receiver_nil_literal_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_receiver_nil_literal_sites
        << ", nil_receiver_semantics_enabled_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_enabled_sites
        << ", nil_receiver_foldable_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_foldable_sites
        << ", nil_receiver_runtime_dispatch_required_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_runtime_dispatch_required_sites
        << ", non_nil_receiver_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_non_nil_receiver_sites
        << ", contract_violation_sites="
        << frontend_metadata_.nil_receiver_semantics_foldability_contract_violation_sites
        << ", deterministic_nil_receiver_semantics_foldability_handoff="
        << (frontend_metadata_.deterministic_nil_receiver_semantics_foldability_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_super_dispatch_method_family_profile = message_send_sites="
        << frontend_metadata_.super_dispatch_method_family_message_send_sites
        << ", receiver_super_identifier_sites="
        << frontend_metadata_.super_dispatch_method_family_receiver_super_identifier_sites
        << ", super_dispatch_enabled_sites=" << frontend_metadata_.super_dispatch_method_family_enabled_sites
        << ", super_dispatch_requires_class_context_sites="
        << frontend_metadata_.super_dispatch_method_family_requires_class_context_sites
        << ", method_family_init_sites=" << frontend_metadata_.super_dispatch_method_family_init_sites
        << ", method_family_copy_sites=" << frontend_metadata_.super_dispatch_method_family_copy_sites
        << ", method_family_mutable_copy_sites="
        << frontend_metadata_.super_dispatch_method_family_mutable_copy_sites
        << ", method_family_new_sites=" << frontend_metadata_.super_dispatch_method_family_new_sites
        << ", method_family_none_sites=" << frontend_metadata_.super_dispatch_method_family_none_sites
        << ", method_family_returns_retained_result_sites="
        << frontend_metadata_.super_dispatch_method_family_returns_retained_result_sites
        << ", method_family_returns_related_result_sites="
        << frontend_metadata_.super_dispatch_method_family_returns_related_result_sites
        << ", contract_violation_sites="
        << frontend_metadata_.super_dispatch_method_family_contract_violation_sites
        << ", deterministic_super_dispatch_method_family_handoff="
        << (frontend_metadata_.deterministic_super_dispatch_method_family_handoff ? "true" : "false")
        << "\n";
    out << "; frontend_objc_runtime_link_host_link_profile = message_send_sites="
        << frontend_metadata_.runtime_link_host_link_message_send_sites
        << ", runtime_link_required_sites=" << frontend_metadata_.runtime_link_host_link_required_sites
        << ", runtime_link_elided_sites=" << frontend_metadata_.runtime_link_host_link_elided_sites
        << ", runtime_dispatch_arg_slots="
        << frontend_metadata_.runtime_link_host_link_runtime_dispatch_arg_slots
        << ", runtime_dispatch_declaration_parameter_count="
        << frontend_metadata_.runtime_link_host_link_runtime_dispatch_declaration_parameter_count
        << ", runtime_dispatch_symbol=" << frontend_metadata_.runtime_link_host_link_runtime_dispatch_symbol
        << ", default_runtime_dispatch_symbol_binding="
        << (frontend_metadata_.runtime_link_host_link_default_runtime_dispatch_symbol_binding ? "true" : "false")
        << ", contract_violation_sites="
        << frontend_metadata_.runtime_link_host_link_contract_violation_sites
        << ", deterministic_runtime_link_host_link_handoff="
        << (frontend_metadata_.deterministic_runtime_link_host_link_handoff ? "true" : "false")
        << "\n";
    // runtime-backed-object-ownership freeze anchor: the current
    // runnable object slice preserves ownership through property/accessor
    // metadata profiles plus these source-side ownership lowering summaries. No
    // live ARC runtime retain/release/autorelease execution hooks are emitted
    // here yet.
    // retainable-object semantic-rule freeze anchor: retain/release,
    // autoreleasepool, and destruction-order behavior are still represented by
    // deterministic summary lanes only; runtime-backed property/member
    // ownership metadata and storage legality are now live sema-enforced
    // surfaces before metadata emission.
    out << "; retainable_object_semantic_rules_freeze = "
        << Objc3RetainableObjectSemanticRulesFreezeSummary() << "\n";
    // runtime-backed storage ownership legality anchor: explicit
    // object-property ownership qualifiers now participate in live semantic
    // legality, so the IR closeout surface publishes the exact owned/weak/
    // unowned contract now enforced before metadata emission.
    out << "; runtime_backed_storage_ownership_legality = "
        << Objc3RuntimeBackedStorageOwnershipLegalitySummary() << "\n";
    // autoreleasepool/destruction-order semantic expansion anchor:
    // lane-B still fail-closes autoreleasepool, but the IR closeout surface
    // now publishes the ownership-sensitive destruction-order contract that
    // distinguishes plain pool rejection from owned runtime-backed object
    // storage edges.
    out << "; runtime_backed_autoreleasepool_destruction_order = "
        << Objc3RuntimeBackedAutoreleasepoolDestructionOrderSummary() << "\n";
    // ownership-lowering baseline freeze anchor: the current IR
    // surface keeps ownership qualifier, retain/release, autoreleasepool, and
    // weak/unowned lowering on deterministic summary lanes only.
    // No live runtime ownership hooks are emitted here before the next runtime step.
    out << "; ownership_lowering_baseline = "
        << Objc3OwnershipLoweringBaselineSummary() << "\n";
    if (synthesized_property_accessor_count_ > 0u) {
      // runtime hook emission anchor: synthesized accessors now
      // execute through runtime-owned helper entrypoints that consume the
      // current dispatch-frame property context rather than the old summary-only
      // ownership lane, and owned and weak execution paths use the live runtime
      // hooks below.
      out << "; ownership_runtime_hook_emission = "
          << Objc3OwnershipRuntimeHookEmissionSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_ << "\n";
      // runtime memory-management API freeze anchor: the public
      // runtime ABI remains narrow while lane-C emits the private helper
      // surface consumed by synthesized ownership accessors.
      out << "; runtime_memory_management_api = "
          << Objc3RuntimeMemoryManagementApiSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_ << "\n";
    }
    if (synthesized_property_accessor_count_ > 0u ||
        frontend_metadata_.autoreleasepool_scope_lowering_scope_sites > 0u ||
        frontend_metadata_.block_storage_escape_lowering_escape_to_heap_sites >
            0u ||
        frontend_metadata_.block_copy_dispose_lowering_copy_helper_required_sites > 0u ||
        frontend_metadata_.block_copy_dispose_lowering_dispose_helper_required_sites > 0u) {
      // runtime memory-management implementation anchor: emitted IR
      // now carries the private autoreleasepool push/pop runtime surface and
      // the live refcount/weak/autoreleasepool execution model that native mode
      // consumes for runtime-backed object programs.
      out << "; runtime_memory_management_implementation = "
          << Objc3RuntimeMemoryManagementImplementationSummary()
          << ";synthesized_accessor_entries="
          << synthesized_property_accessor_count_
          << ";autoreleasepool_scope_sites="
          << frontend_metadata_.autoreleasepool_scope_lowering_scope_sites
          << "\n";
    }
    // ownership-runtime-gate freeze anchor: lane-E freezes the
    // supported ownership runtime slice and its explicit non-goals as a
    // dedicated emitted boundary so later smoke/docs work can validate the
    // correct baseline without rediscovering it from prose alone.
    // ownership-smoke closeout anchor: the runnable smoke matrix
    // consumes this exact emitted gate boundary as the integrated ownership
    // proof surface for closeout.
    out << "; ownership_runtime_gate = "
        << Objc3OwnershipRuntimeGateSummary() << "\n";
    // executable-block-source-closure anchor: emit the truthful
    // parser/AST boundary so lane-A can prove block literals entered the source
    // closure before runtime lowering remains fail closed.
    out << "; executable_block_source_closure = "
        << Objc3ExecutableBlockSourceClosureSummary() << "\n";
    // block-source-model-completion anchor: emit the completed
    // source-model replay boundary so source-only frontend runs and later
    // lowering work can agree on typed parameter signatures, capture storage
    // inventories, and invoke-surface symbols without reconstructing them.
    out << "; executable_block_source_model_completion = "
        << Objc3ExecutableBlockSourceModelCompletionSummary()
        << ";replay_key="
        << frontend_metadata_.lowering_block_source_model_completion_replay_key
        << "\n";
    // block-source-storage-annotation anchor: emit the truthful
    // byref/helper/escape-shape replay boundary so source-only manifests and
    // later runnable block lowering consume the same deterministic source
    // annotations while native block execution remains fail closed.
    out << "; executable_block_source_storage_annotations = "
        << Objc3ExecutableBlockSourceStorageAnnotationSummary()
        << ";replay_key="
        << frontend_metadata_
               .lowering_block_source_storage_annotation_replay_key
        << "\n";
    // block-runtime-semantic-rules freeze anchor: emit the current
    // semantic-rule boundary so block runtime follow-on work can preserve the
    // source-only admission/native fail-closed split without rediscovering it
    // from prose or historical issue packets.
    // capture-legality/escape/invocation implementation anchor:
    // the emitted summary line stays on the frozen runtime boundary while the
    // source-only sema path now enforces live capture legality and local
    // block-call typing ahead of runnable block-object lowering.
    // byref/copy-dispose/object-ownership anchor: helper-eligibility
    // totals in this emitted surface now reflect owned object captures as well as byref cells,
    // while native block execution still remains fail-closed.
    out << "; executable_block_runtime_semantic_rules = "
        << Objc3ExecutableBlockRuntimeSemanticRulesSummary() << "\n";
    // block-lowering-ABI/artifact-boundary freeze anchor: lane-C
    // now publishes the truthful lowering boundary required for runnable block
    // execution, while native emit still fails closed before any emitted block
    // object records, invoke thunks, byref cells, or helper bodies land.
    out << "; executable_block_lowering_abi_artifact_boundary = "
        << Objc3ExecutableBlockLoweringAbiArtifactBoundarySummary() << "\n";
    // executable-block-object/invoke-thunk anchor: native lowering
    // now emits stack block objects plus internal invoke thunks for the narrow
    // readonly-scalar capture slice, while byref/helper/ownership-sensitive
    // cases remain explicitly deferred to C003.
    out << "; executable_block_object_invoke_thunk_lowering = "
        << Objc3ExecutableBlockObjectInvokeThunkLoweringSummary() << "\n";
    // byref-cell/copy-helper/dispose-helper anchor: native lowering
    // now widens the runnable block slice to non-escaping byref and owned
    // capture cases through stack helper emission and helper call sites, while
    // escaping heap promotion remains deferred to later work.
    out << "; executable_block_byref_helper_lowering = "
        << Objc3ExecutableBlockByrefHelperLoweringSummary() << "\n";
    // escaping-block runtime-hook anchor: lane-C now publishes the
    // escaping readonly-scalar block slice that lowers through runtime
    // promotion/invoke hooks while pointer-managed escaping captures remain
    // explicitly deferred to later runtime issues.
    out << "; executable_block_escape_runtime_hook_lowering = "
        << Objc3ExecutableBlockEscapeRuntimeHookLoweringSummary() << "\n";
    // block-runtime API/object-layout freeze anchor: emitted IR now
    // republishes the current private helper ABI and private runtime layout
    // boundary so later runtime implementation issues preserve this exact
    // contract instead of widening it ad hoc.
    out << "; runtime_block_api_object_layout = "
        << Objc3RuntimeBlockApiObjectLayoutSummary() << "\n";
    // block-runtime allocation/copy-dispose/invoke anchor: emitted
    // IR now republishes the live runtime capability boundary for promoted
    // block records with helper-mediated copy/dispose support while byref and
    // ownership-interoperating escape paths remain deferred.
    out << "; runtime_block_allocation_copy_dispose_invoke_support = "
        << Objc3RuntimeBlockAllocationCopyDisposeInvokeSupportSummary() << "\n";
    // byref-forwarding/heap-promotion/ownership-interop anchor:
    // emitted IR now republishes that escaping pointer-capture block handles
    // rewrite capture slots onto runtime-owned forwarding cells before helper
    // execution, keeping byref mutation and owned capture lifetimes live after
    // the source frame returns.
    out << "; runtime_block_byref_forwarding_heap_promotion_ownership_interop = "
        << Objc3RuntimeBlockByrefForwardingHeapPromotionInteropSummary() << "\n";
    // runnable-block-runtime gate anchor: lane-E now freezes the
    // integrated block-runtime gate above the retained A003/B003/C004/D003
    // source, sema, lowering, and runtime proofs so later closeout work
    // cannot substitute metadata-only evidence.
    out << "; runnable_block_runtime_gate = "
        << Objc3RunnableBlockRuntimeGateSummary() << "\n";
    // runnable-block execution-matrix anchor: lane-E now closes the
    // current current slice by requiring integrated executable block probes above
    // the retained gate, without widening the public block ABI or helper
    // surface.
    out << "; runnable_block_execution_matrix = "
        << Objc3RunnableBlockExecutionMatrixSummary() << "\n";
    // ARC source-surface/mode-boundary anchor: emit the truthful
    // ARC-adjacent frontend/mode boundary so later ARC automation work cannot
    // silently claim a runnable `-fobjc-arc` mode before the driver and
    // executable ownership-qualified function/method path are actually live.
    out << "; arc_source_mode_boundary = "
        << Objc3ArcSourceModeBoundarySummary() << "\n";
    // ARC mode-handling core implementation anchor: publish the
    // explicit ARC-mode execution boundary so manifests and emitted IR stay
    // aligned on when ownership-qualified executable signatures are runnable.
    out << "; arc_mode_handling = "
        << Objc3ArcModeHandlingSummary(frontend_metadata_.arc_mode_enabled) << "\n";
    // ARC semantic-rule freeze anchor: publish the semantic
    // fail-closed boundary for property conflicts and deferred inference so IR
    // evidence stays aligned with semantic validation.
    out << "; arc_semantic_rules = "
        << Objc3ArcSemanticRulesSummary() << "\n";
    // ARC inference/lifetime implementation anchor: publish the
    // truthful semantic-upgrade boundary so emitted IR and manifests agree
    // when ARC mode has widened the supported slice from explicit-only
    // ownership spelling into inferred strong-owned retain/release activity.
    out << "; arc_inference_lifetime = "
        << Objc3ArcInferenceLifetimeSummary() << "\n";
    // ARC interaction-semantics expansion anchor: publish the
    // supported weak/autorelease-return/property-synthesis/block-interaction
    // semantic boundary so emitted IR stays aligned with the live ARC slice
    // instead of forcing later issues to reconstruct it out of older packets.
    out << "; arc_interaction_semantics = "
        << Objc3ArcInteractionSemanticsSummary() << "\n";
    // ARC lowering ABI/cleanup freeze anchor: publish the current
    // lowering/helper boundary directly into IR so later retain/release,
    // cleanup-scheduling, weak lowering, and autorelease-return work must
    // preserve one explicit contract instead of inferring it from older ARC
    // semantic packets.
    out << "; arc_lowering_abi_cleanup_model = "
        << Objc3ArcLoweringAbiCleanupModelSummary() << "\n";
    // ARC automatic-insertion implementation anchor: publish the
    // live param/return helper-insertion boundary so later ARC work extends a
    // real lowering surface instead of a summary-only semantic contract.
    out << "; arc_automatic_insertions = "
        << Objc3ArcAutomaticInsertionSummary() << "\n";
    // ARC cleanup/weak/lifetime implementation anchor: publish the
    // supported scope-exit cleanup, weak current-property helper, and block
    // lifetime cleanup boundary so later ARC/block widening extends a real
    // lowering/runtime surface rather than inferred behavior.
    out << "; arc_cleanup_weak_lifetime_hooks = "
        << Objc3ArcCleanupWeakLifetimeHooksSummary() << "\n";
    // ARC/block autorelease-return implementation anchor: publish
    // the supported escaping-block plus autoreleasing-return edge inventory so
    // later runtime ARC work extends a real branch-stable lowering surface
    // rather than re-deriving cleanup ordering from semantic summaries.
    out << "; arc_block_autorelease_return_lowering = "
        << Objc3ArcBlockAutoreleaseReturnLoweringSummary() << "\n";
    // runtime ARC helper API surface anchor: publish the private
    // helper ABI boundary that current ARC lowering already consumes so later
    // runtime implementation work extends a truthful runtime contract instead
    // of inferring helper availability from emitted calls alone.
    out << "; runtime_arc_helper_api_surface = "
        << Objc3RuntimeArcHelperApiSurfaceSummary() << "\n";
    // runtime ARC helper implementation anchor: publish the live
    // executable helper-runtime support boundary so later diagnostics or debug
    // instrumentation work extends a truthful runtime capability rather than
    // another summary-only marker.
    out << "; runtime_arc_helper_runtime_support = "
        << Objc3RuntimeArcHelperRuntimeSupportSummary() << "\n";
    // ownership-debug/runtime-validation anchor: publish the
    // private ARC debug snapshot boundary so lane-D validation can prove live
    // helper traffic without widening the public runtime ABI.
    out << "; runtime_arc_debug_instrumentation = "
        << Objc3RuntimeArcDebugInstrumentationSummary() << "\n";
    // runnable-arc-runtime gate anchor: lane-E freezes the supported
    // ARC slice above the existing mode-handling, interaction, lowering, and
    // runtime proof chain without claiming closeout-matrix coverage yet.
    out << "; runnable_arc_runtime_gate = "
        << Objc3RunnableArcRuntimeGateSummary() << "\n";
    // runnable-arc-closeout anchor: lane-E consumes the already-live
    // ARC proof chain plus integrated execution smoke as the closeout surface
    // without widening the supported ARC semantics or runtime ABI.
    out << "; runnable_arc_closeout = " << Objc3RunnableArcCloseoutSummary()
        << "\n";
    // runtime-backed semantics closure anchor: issue #8017 consumes the
    // existing block, ARC, error, async/task, and actor runtime-backed slices
    // through one emitted contract line plus a durable claimability report.
    out << "; runtime_backed_semantics_closure = "
        << Objc3RuntimeBackedSemanticsClosureSummary() << "\n";
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
    out << "; frontend_objc_async_continuation_lowering_profile = async_continuation_sites="
        << frontend_metadata_.async_continuation_lowering_sites
        << ", async_keyword_sites="
        << frontend_metadata_.async_continuation_lowering_async_keyword_sites
        << ", async_function_sites="
        << frontend_metadata_.async_continuation_lowering_async_function_sites
        << ", continuation_allocation_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_allocation_sites
        << ", continuation_resume_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_resume_sites
        << ", continuation_suspend_sites="
        << frontend_metadata_
               .async_continuation_lowering_continuation_suspend_sites
        << ", async_state_machine_sites="
        << frontend_metadata_
               .async_continuation_lowering_async_state_machine_sites
        << ", normalized_sites="
        << frontend_metadata_.async_continuation_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_.async_continuation_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .async_continuation_lowering_contract_violation_sites
        << ", deterministic_async_continuation_lowering_handoff="
        << (frontend_metadata_.deterministic_async_continuation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_await_lowering_suspension_state_lowering_profile = await_suspension_sites="
        << frontend_metadata_.await_lowering_suspension_state_lowering_sites
        << ", await_keyword_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_keyword_sites
        << ", await_suspension_point_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_suspension_point_sites
        << ", await_resume_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_resume_sites
        << ", await_state_machine_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_state_machine_sites
        << ", await_continuation_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_await_continuation_sites
        << ", normalized_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .await_lowering_suspension_state_lowering_contract_violation_sites
        << ", deterministic_await_lowering_suspension_state_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_await_lowering_suspension_state_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_actor_isolation_sendability_lowering_profile = actor_isolation_sites="
        << frontend_metadata_.actor_isolation_sendability_lowering_sites
        << ", sendability_check_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_sendability_check_sites
        << ", cross_actor_hop_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_cross_actor_hop_sites
        << ", non_sendable_capture_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_non_sendable_capture_sites
        << ", sendable_transfer_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_sendable_transfer_sites
        << ", isolation_boundary_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_isolation_boundary_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .actor_isolation_sendability_lowering_contract_violation_sites
        << ", deterministic_actor_isolation_sendability_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_actor_isolation_sendability_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_actor_lowering_metadata_profile = actor_interface_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_interface_sites
        << ", actor_method_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_method_sites
        << ", actor_metadata_record_sites="
        << frontend_metadata_
               .actor_lowering_metadata_actor_metadata_record_sites
        << ", nonisolated_entry_sites="
        << frontend_metadata_.actor_lowering_metadata_nonisolated_entry_sites
        << ", executor_affinity_sites="
        << frontend_metadata_.actor_lowering_metadata_executor_affinity_sites
        << ", actor_hop_artifact_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_hop_artifact_sites
        << ", actor_isolation_thunk_sites="
        << frontend_metadata_.actor_lowering_metadata_actor_isolation_thunk_sites
        << ", replay_proof_dependency_sites="
        << frontend_metadata_
               .actor_lowering_metadata_replay_proof_dependency_sites
        << ", race_guard_dependency_sites="
        << frontend_metadata_
               .actor_lowering_metadata_race_guard_dependency_sites
        << ", task_handoff_sites="
        << frontend_metadata_.actor_lowering_metadata_task_handoff_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.actor_lowering_metadata_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.actor_lowering_metadata_contract_violation_sites
        << ", deterministic_actor_lowering_metadata_handoff="
        << (frontend_metadata_.deterministic_actor_lowering_metadata_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_dispatch_control_lowering_profile = direct_call_candidate_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_direct_call_candidate_sites
        << ", direct_members_defaulted_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_direct_members_defaulted_sites
        << ", dynamic_opt_out_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_dynamic_opt_out_sites
        << ", final_container_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_final_container_sites
        << ", sealed_container_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_sealed_container_sites
        << ", override_legality_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_override_legality_sites
        << ", metadata_preserved_callable_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_metadata_preserved_callable_sites
        << ", metadata_preserved_container_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_metadata_preserved_container_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.dispatch_dispatch_control_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .dispatch_dispatch_control_lowering_contract_violation_sites
        << ", deterministic_dispatch_dispatch_control_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_dispatch_dispatch_control_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_interop_interop_lowering_profile = foreign_callable_sites="
        << frontend_metadata_.interop_interop_lowering_foreign_callable_sites
        << ", c_foreign_callable_sites="
        << frontend_metadata_.interop_interop_lowering_c_foreign_callable_sites
        << ", objc_runtime_parity_callable_sites="
        << frontend_metadata_.interop_interop_lowering_objc_runtime_parity_callable_sites
        << ", ownership_bridge_callable_sites="
        << frontend_metadata_.interop_interop_lowering_ownership_bridge_callable_sites
        << ", error_surface_sites="
        << frontend_metadata_.interop_interop_lowering_error_surface_sites
        << ", async_boundary_sites="
        << frontend_metadata_.interop_interop_lowering_async_boundary_sites
        << ", swift_concurrency_metadata_sites="
        << frontend_metadata_.interop_interop_lowering_swift_concurrency_metadata_sites
        << ", interface_preserved_foreign_callable_sites="
        << frontend_metadata_.interop_interop_lowering_interface_preserved_foreign_callable_sites
        << ", interface_preserved_metadata_annotation_sites="
        << frontend_metadata_.interop_interop_lowering_interface_preserved_metadata_annotation_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.interop_interop_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.interop_interop_lowering_contract_violation_sites
        << ", deterministic_interop_interop_lowering_handoff="
        << (frontend_metadata_.deterministic_interop_interop_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_metaprogramming_expansion_lowering_profile = derive_inventory_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_derive_inventory_sites
        << ", derived_selector_artifact_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_derived_selector_artifact_sites
        << ", macro_replay_visible_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_macro_replay_visible_sites
        << ", property_behavior_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_property_behavior_sites
        << ", synthesized_binding_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_binding_sites
        << ", synthesized_getter_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_getter_sites
        << ", synthesized_setter_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_synthesized_setter_sites
        << ", replay_visible_metadata_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_replay_visible_metadata_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.metaprogramming_expansion_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .metaprogramming_expansion_lowering_contract_violation_sites
        << ", deterministic_metaprogramming_expansion_lowering_handoff="
        << (frontend_metadata_.deterministic_metaprogramming_expansion_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_metaprogramming_synthesized_emission_profile = emitted_derive_method_sites="
        << frontend_metadata_.metaprogramming_synthesized_emitted_derive_method_sites
        << ", emitted_macro_artifact_sites="
        << frontend_metadata_.metaprogramming_synthesized_emitted_macro_artifact_sites
        << ", emitted_property_behavior_artifact_sites="
        << frontend_metadata_
               .metaprogramming_synthesized_emitted_property_behavior_artifact_sites
        << ", emitted_global_artifact_sites="
        << frontend_metadata_.metaprogramming_synthesized_emitted_global_artifact_sites
        << ", emitted_runtime_method_list_sites="
        << frontend_metadata_
               .metaprogramming_synthesized_emitted_runtime_method_list_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.metaprogramming_synthesized_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_.metaprogramming_synthesized_contract_violation_sites
        << ", deterministic_metaprogramming_synthesized_emission_handoff="
        << (frontend_metadata_.deterministic_metaprogramming_synthesized_emission_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_dispatch_metadata_interface_profile = local_direct_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_direct_callable_record_count
        << ", local_final_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_final_callable_record_count
        << ", local_final_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_final_container_record_count
        << ", local_sealed_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_local_sealed_container_record_count
        << ", imported_module_count="
        << frontend_metadata_.dispatch_dispatch_metadata_imported_module_count
        << ", imported_direct_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_direct_callable_record_count
        << ", imported_final_callable_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_final_callable_record_count
        << ", imported_final_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_final_container_record_count
        << ", imported_sealed_container_record_count="
        << frontend_metadata_
               .dispatch_dispatch_metadata_imported_sealed_container_record_count
        << ", runtime_import_artifact_ready="
        << (frontend_metadata_
                    .dispatch_dispatch_metadata_runtime_import_artifact_ready
                ? "true"
                : "false")
        << ", separate_compilation_preservation_ready="
        << (frontend_metadata_
                    .dispatch_dispatch_metadata_separate_compilation_preservation_ready
                ? "true"
                : "false")
        << ", deterministic_dispatch_dispatch_metadata_interface_handoff="
        << (frontend_metadata_
                    .deterministic_dispatch_dispatch_metadata_interface_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_system_extension_lowering_profile = cleanup_hook_sites="
        << frontend_metadata_.ownership_system_extension_lowering_cleanup_hook_sites
        << ", resource_local_sites="
        << frontend_metadata_.ownership_system_extension_lowering_resource_local_sites
        << ", cleanup_owned_local_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_cleanup_owned_local_sites
        << ", resource_move_capture_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_resource_move_capture_sites
        << ", borrowed_parameter_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_borrowed_parameter_sites
        << ", borrowed_return_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_borrowed_return_callable_sites
        << ", borrowed_escape_candidate_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_borrowed_escape_candidate_sites
        << ", explicit_capture_item_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_explicit_capture_item_sites
        << ", retainable_family_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_retainable_family_callable_sites
        << ", retainable_family_operation_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_retainable_family_operation_callable_sites
        << ", retainable_family_alias_callable_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_retainable_family_alias_callable_sites
        << ", guard_blocked_sites="
        << frontend_metadata_.ownership_system_extension_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .ownership_system_extension_lowering_contract_violation_sites
        << ", deterministic_ownership_system_extension_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_ownership_system_extension_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_ownership_borrowed_retainable_abi_profile = returns_borrowed_attribute_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_returns_borrowed_attribute_sites
        << ", family_retain_sites="
        << frontend_metadata_.ownership_borrowed_retainable_family_retain_sites
        << ", family_release_sites="
        << frontend_metadata_.ownership_borrowed_retainable_family_release_sites
        << ", family_autorelease_sites="
        << frontend_metadata_.ownership_borrowed_retainable_family_autorelease_sites
        << ", compatibility_returns_retained_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_compatibility_returns_retained_sites
        << ", compatibility_returns_not_retained_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_compatibility_returns_not_retained_sites
        << ", compatibility_consumed_sites="
        << frontend_metadata_
               .ownership_borrowed_retainable_compatibility_consumed_sites
        << ", deterministic_ownership_borrowed_retainable_abi_completion_handoff="
        << (frontend_metadata_
                    .deterministic_ownership_borrowed_retainable_abi_completion_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_task_runtime_interop_cancellation_lowering_profile = task_runtime_sites="
        << frontend_metadata_.task_runtime_interop_cancellation_lowering_sites
        << ", task_runtime_interop_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_interop_sites
        << ", cancellation_probe_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_cancellation_probe_sites
        << ", cancellation_handler_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_cancellation_handler_sites
        << ", runtime_resume_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_resume_sites
        << ", runtime_cancel_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_runtime_cancel_sites
        << ", normalized_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_normalized_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .task_runtime_interop_cancellation_lowering_contract_violation_sites
        << ", deterministic_task_runtime_interop_cancellation_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_task_runtime_interop_cancellation_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_concurrency_replay_race_guard_lowering_profile = concurrency_replay_sites="
        << frontend_metadata_.concurrency_replay_race_guard_lowering_sites
        << ", replay_proof_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_replay_proof_sites
        << ", race_guard_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_race_guard_sites
        << ", task_handoff_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_task_handoff_sites
        << ", actor_isolation_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_actor_isolation_sites
        << ", deterministic_schedule_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_deterministic_schedule_sites
        << ", guard_blocked_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_guard_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .concurrency_replay_race_guard_lowering_contract_violation_sites
        << ", deterministic_concurrency_replay_race_guard_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_concurrency_replay_race_guard_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_unsafe_pointer_extension_lowering_profile = unsafe_pointer_extension_sites="
        << frontend_metadata_.unsafe_pointer_extension_lowering_sites
        << ", unsafe_keyword_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_unsafe_keyword_sites
        << ", pointer_arithmetic_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_pointer_arithmetic_sites
        << ", raw_pointer_type_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_raw_pointer_type_sites
        << ", unsafe_operation_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_unsafe_operation_sites
        << ", normalized_sites="
        << frontend_metadata_.unsafe_pointer_extension_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .unsafe_pointer_extension_lowering_contract_violation_sites
        << ", deterministic_unsafe_pointer_extension_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_unsafe_pointer_extension_lowering_handoff
                ? "true"
                : "false")
        << "\n";
    out << "; frontend_objc_inline_asm_intrinsic_governance_lowering_profile = inline_asm_intrinsic_sites="
        << frontend_metadata_.inline_asm_intrinsic_governance_lowering_sites
        << ", inline_asm_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_inline_asm_sites
        << ", intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_intrinsic_sites
        << ", governed_intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_governed_intrinsic_sites
        << ", privileged_intrinsic_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_privileged_intrinsic_sites
        << ", normalized_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_normalized_sites
        << ", gate_blocked_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_gate_blocked_sites
        << ", contract_violation_sites="
        << frontend_metadata_
               .inline_asm_intrinsic_governance_lowering_contract_violation_sites
        << ", deterministic_inline_asm_intrinsic_governance_lowering_handoff="
        << (frontend_metadata_
                    .deterministic_inline_asm_intrinsic_governance_lowering_handoff
                ? "true"
                : "false")
        << "\n";
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
    EmitFrontendMetadata(out);
    // Historical extraction contract markers retained for fail-closed tooling:
    // out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";
    // for (std::size_t i = 0; i < lowering_ir_boundary_.runtime_dispatch_arg_slots; ++i) {
    //   out << ", i32";
    // }
    // out << ")\n\n";
    EmitObjc3IRRuntimeDispatchDeclarationSurface(
        lowering_ir_boundary_, runtime_dispatch_call_state_, out);
    EmitObjc3IRSynthesizedAccessorEmissionSurface(
        Objc3IRSynthesizedAccessorEmissionStats{
            synthesized_getter_definition_count_,
            synthesized_setter_definition_count_,
            current_property_read_helper_call_count_,
            current_property_write_helper_call_count_,
            current_property_exchange_helper_call_count_,
            weak_current_property_load_helper_call_count_,
            weak_current_property_store_helper_call_count_,
            retain_helper_call_count_,
            release_helper_call_count_,
            autorelease_helper_call_count_},
        out);
    EmitObjc3IRMethodDispatchEmissionSurface(
        lowering_ir_boundary_, runtime_dispatch_call_state_,
        Objc3IRMethodDispatchEmissionStats{
            selector_pool_globals_.size(),
            frontend_metadata_
                .dispatch_dispatch_control_lowering_direct_call_candidate_sites,
            frontend_metadata_
                .dispatch_dispatch_control_lowering_dynamic_opt_out_sites,
            !selector_pool_globals_.empty()},
        out);
    out << body.str();
    ir = out.str();
    return true;
  }

 private:
  bool IsActorImplementation(const std::string &name) const {
    if (name.empty()) {
      return false;
    }
    for (const auto &interface_decl : program_.interfaces) {
      if (!interface_decl.has_category && interface_decl.name == name) {
        return interface_decl.is_actor;
      }
    }
    return false;
  }

  bool TryEmitConcurrencyTaskRuntimeLoweringCall(const Expr *expr, FunctionContext &ctx,
                                           std::string &result_out) const {
    Objc3IRConcurrencyRuntimeCallEmissionCallbacks callbacks{
        [this](FunctionContext &callback_ctx) { return NewTemp(callback_ctx); },
        [this, &ctx](const Expr *arg) { return EmitExpr(arg, ctx); },
        [this](FunctionContext &callback_ctx) {
          InvalidateGlobalProofState(callback_ctx);
        }};
    return TryEmitObjc3IRConcurrencyTaskRuntimeLoweringCall(
        expr, ctx, callbacks, result_out);
  }

  bool TryEmitConcurrencyActorLoweringCall(const Expr *expr, FunctionContext &ctx,
                                     std::string &result_out) const {
    Objc3IRConcurrencyRuntimeCallEmissionCallbacks callbacks{
        [this](FunctionContext &callback_ctx) { return NewTemp(callback_ctx); },
        [this, &ctx](const Expr *arg) { return EmitExpr(arg, ctx); },
        [this](FunctionContext &callback_ctx) {
          InvalidateGlobalProofState(callback_ctx);
        }};
    return TryEmitObjc3IRConcurrencyActorLoweringCall(
        expr, ctx, callbacks, result_out);
  }

  bool ShouldEmitRuntimeBootstrapLowering() const {
    return Objc3IRRuntimeBootstrapLoweringReady(frontend_metadata_);
  }

  bool ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering() const {
    return Objc3IRRuntimeBootstrapRegistrationDescriptorImageRootLoweringReady(
        frontend_metadata_);
  }

  void EmitFrontendMetadata(std::ostringstream &out) const {
    EmitObjc3IRFrontendMetadataPublication(
        frontend_metadata_, runtime_metadata_symbols_, selector_pool_globals_.size(),
        runtime_string_pool_globals_.size(),
        synthesized_property_accessor_count_, out);
  }

  void EmitRuntimeMetadataSectionScaffold(std::ostringstream &out) const {
    std::string scaffold_error;
    if (!EmitObjc3IRRuntimeMetadataSectionScaffold(
            Objc3IRRuntimeMetadataScaffoldEmissionOptions{
                program_.module_name,
                frontend_metadata_,
                runtime_metadata_symbols_,
                selector_pool_globals_,
                runtime_string_pool_globals_,
                typed_keypath_artifacts_,
                method_definitions_,
                ShouldEmitRuntimeBootstrapLowering(),
                ShouldEmitRuntimeBootstrapRegistrationDescriptorImageRootLowering()},
            out, scaffold_error)) {
      if (!unsupported_fail_closed_path_triggered_) {
        unsupported_fail_closed_path_triggered_ = true;
        unsupported_fail_closed_path_reason_ =
            scaffold_error.empty()
                ? "runtime metadata scaffold emission failed"
                : scaffold_error;
      }
      return;
    }
  }

  std::string NewTemp(FunctionContext &ctx) const { return "%t" + std::to_string(ctx.temp_counter++); }

  std::string NewLabel(FunctionContext &ctx, const std::string &prefix) const {
    return prefix + std::to_string(ctx.label_counter++);
  }

  Objc3IRScopeCleanupEmissionCallbacks ScopeCleanupCallbacks() const {
    return Objc3IRScopeCleanupEmissionCallbacks{
        [this](FunctionContext &callback_ctx) {
          return NewTemp(callback_ctx);
        },
        [this](FunctionContext &callback_ctx, const std::string &prefix) {
          return NewLabel(callback_ctx, prefix);
        },
        [this](const Stmt *stmt, FunctionContext &callback_ctx) {
          EmitStatement(stmt, callback_ctx);
        }};
  }

  void EmitAutoreleasepoolUnwindToDepth(FunctionContext &ctx,
                                        std::size_t target_depth) const {
    EmitObjc3IRAutoreleasepoolUnwindToDepth(ctx, target_depth);
  }

  void PushScope(FunctionContext &ctx) const {
    PushObjc3IRScope(ctx);
  }

  void EmitDeferredCleanupTerminalToDepth(FunctionContext &ctx,
                                          std::size_t target_scope_depth) const {
    EmitObjc3IRDeferredCleanupTerminalToDepth(
        ctx, target_scope_depth, ScopeCleanupCallbacks());
  }

  void EmitPendingBlockDisposeUnwindToDepth(FunctionContext &ctx,
                                            std::size_t target_depth) const {
    EmitObjc3IRPendingBlockDisposeUnwindToDepth(ctx, target_depth);
  }

  void EmitPendingBlockDisposeTerminalCleanupToDepth(
      const FunctionContext &ctx, std::size_t target_depth,
      std::vector<std::string> &out_lines) const {
    EmitObjc3IRPendingBlockDisposeTerminalCleanupToDepth(
        ctx, target_depth, out_lines);
  }

  void EmitOwnershipCleanupUnwindToDepth(FunctionContext &ctx,
                                     std::size_t target_depth) const {
    EmitObjc3IROwnershipCleanupUnwindToDepth(
        ctx, target_depth, ScopeCleanupCallbacks());
  }

  void EmitOwnershipCleanupTerminalCleanupToDepth(const FunctionContext &ctx,
                                              std::size_t target_depth,
                                              std::vector<std::string> &out_lines,
                                              int &temp_counter) const {
    EmitObjc3IROwnershipCleanupTerminalCleanupToDepth(
        ctx, target_depth, out_lines, temp_counter);
  }

  void PopScope(FunctionContext &ctx, bool emit_cleanup) const {
    PopObjc3IRScope(ctx, emit_cleanup, ScopeCleanupCallbacks());
  }

  std::string LookupVarPtr(const FunctionContext &ctx, const std::string &name) const {
    for (auto it = ctx.scopes.rbegin(); it != ctx.scopes.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second;
      }
    }
    if (globals_.find(name) != globals_.end()) {
      return "@" + name;
    }
    return "";
  }

  int LookupImmediateIdentifierValue(const FunctionContext &ctx,
                                     const std::string &name) const {
    const auto value_it = ctx.immediate_identifiers.find(name);
    if (value_it == ctx.immediate_identifiers.end()) {
      return 0;
    }
    return value_it->second;
  }

  static bool SortedStringListContains(const std::vector<std::string> &entries,
                                       const std::string &needle) {
    return std::binary_search(entries.begin(), entries.end(), needle);
  }

  void EmitPendingBlockDisposeHelpers(FunctionContext &ctx) const {
    EmitPendingBlockDisposeUnwindToDepth(ctx, 0u);
  }

  void EmitArcOwnedCleanupUnwindToDepth(FunctionContext &ctx,
                                        std::size_t target_depth) const {
    EmitObjc3IRArcOwnedCleanupUnwindToDepth(
        ctx, target_depth, ScopeCleanupCallbacks());
  }

  void EmitArcOwnedTerminalCleanupToDepth(const FunctionContext &ctx,
                                          std::size_t target_depth,
                                          std::vector<std::string> &out_lines,
                                          int &temp_counter) const {
    EmitObjc3IRArcOwnedTerminalCleanupToDepth(
        ctx, target_depth, out_lines, temp_counter);
  }

  void EmitBlockCopyHelper(const Expr &expr) const {
    if (!BlockLiteralUsesPointerCaptureStorage(expr) ||
        !expr.block_runtime_copy_helper_required) {
      return;
    }
    const std::string symbol = BuildBlockCopyHelperSymbol(expr);
    if (symbol.empty() ||
        !emitted_block_copy_helper_symbols_.insert(symbol).second) {
      return;
    }

    const std::string storage_type = BuildBlockStorageType(expr);
    std::ostringstream out;
    out << "define internal void @" << symbol << "(ptr %block) {\n";
    out << "entry:\n";
    int temp_counter = 0;
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
         ++i) {
      const std::string &capture_name =
          expr.block_capture_names_lexicographic[i];
      if (!SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name)) {
        continue;
      }
      const std::string slot_ptr =
          "%block.copy.slot." + std::to_string(temp_counter++);
      const std::string capture_ptr =
          "%block.copy.capture." + std::to_string(temp_counter++);
      const std::string loaded_value =
          "%block.copy.value." + std::to_string(temp_counter++);
      const std::string retained_value =
          "%block.copy.retained." + std::to_string(temp_counter++);
      out << "  " << slot_ptr << " = getelementptr inbounds " << storage_type
          << ", ptr %block, i32 0, i32 3, i32 " << i << "\n";
      out << "  " << capture_ptr << " = load ptr, ptr " << slot_ptr
          << ", align 8\n";
      out << "  " << loaded_value << " = load i32, ptr " << capture_ptr
          << ", align 4\n";
      out << "  " << retained_value << " = call i32 @"
          << kObjc3RuntimeRetainI32Symbol << "(i32 " << loaded_value << ")\n";
      out << "  store i32 " << retained_value << ", ptr " << capture_ptr
          << ", align 4\n";
    }
    out << "  ret void\n";
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  void EmitBlockDisposeHelper(const Expr &expr,
                              const FunctionContext &ctx) const {
    if (!BlockLiteralUsesPointerCaptureStorage(expr) ||
        !expr.block_runtime_dispose_helper_required) {
      return;
    }
    const std::string symbol = BuildBlockDisposeHelperSymbol(expr);
    if (symbol.empty() ||
        !emitted_block_dispose_helper_symbols_.insert(symbol).second) {
      return;
    }

    const std::string storage_type = BuildBlockStorageType(expr);
    std::ostringstream out;
    out << "define internal void @" << symbol << "(ptr %block) {\n";
    out << "entry:\n";
    int temp_counter = 0;
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size();
         ++i) {
      const std::string &capture_name =
          expr.block_capture_names_lexicographic[i];
      const auto moved_capture_it = std::find_if(
          expr.block_explicit_capture_items_source_order.begin(),
          expr.block_explicit_capture_items_source_order.end(),
          [&capture_name](const Expr::ExplicitBlockCaptureItem &item) {
            return item.name == capture_name && item.mode == "move";
          });
      const bool moved_capture =
          moved_capture_it != expr.block_explicit_capture_items_source_order.end();
      const auto cleanup_it = ctx.ownership_cleanup_call_indices.find(capture_name);
      if (!SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name) &&
          (!moved_capture || cleanup_it == ctx.ownership_cleanup_call_indices.end())) {
        continue;
      }
      const std::string slot_ptr =
          "%block.dispose.slot." + std::to_string(temp_counter++);
      const std::string capture_ptr =
          "%block.dispose.capture." + std::to_string(temp_counter++);
      const std::string loaded_value =
          "%block.dispose.value." + std::to_string(temp_counter++);
      const std::string released_value =
          "%block.dispose.released." + std::to_string(temp_counter++);
      out << "  " << slot_ptr << " = getelementptr inbounds " << storage_type
          << ", ptr %block, i32 0, i32 3, i32 " << i << "\n";
      out << "  " << capture_ptr << " = load ptr, ptr " << slot_ptr
          << ", align 8\n";
      out << "  " << loaded_value << " = load i32, ptr " << capture_ptr
          << ", align 4\n";
      if (SortedStringListContains(
              expr.block_runtime_owned_object_capture_names_lexicographic,
              capture_name)) {
        out << "  " << released_value << " = call i32 @"
            << kObjc3RuntimeReleaseI32Symbol << "(i32 " << loaded_value
            << ")\n";
      }
      if (moved_capture && cleanup_it != ctx.ownership_cleanup_call_indices.end()) {
        const PendingOwnershipCleanupCall &cleanup_call =
            ctx.pending_ownership_cleanup_calls[cleanup_it->second];
        if (!cleanup_call.resource_close_symbol.empty()) {
          if (cleanup_call.has_resource_invalid_value) {
            const std::string resource_live =
                "%block.dispose.resource.live." + std::to_string(temp_counter++);
            const std::string resource_close_label =
                "block.dispose.resource.close." + std::to_string(temp_counter++);
            const std::string resource_skip_label =
                "block.dispose.resource.skip." + std::to_string(temp_counter++);
            out << "  " << resource_live << " = icmp ne i32 " << loaded_value
                << ", " << cleanup_call.resource_invalid_value << "\n";
            out << "  br i1 " << resource_live << ", label %"
                << resource_close_label << ", label %" << resource_skip_label
                << "\n";
            out << resource_close_label << ":\n";
            out << "  call void @" << cleanup_call.resource_close_symbol
                << "(i32 " << loaded_value << ")\n";
            out << "  br label %" << resource_skip_label << "\n";
            out << resource_skip_label << ":\n";
          } else {
            out << "  call void @" << cleanup_call.resource_close_symbol
                << "(i32 " << loaded_value << ")\n";
          }
        }
        if (!cleanup_call.cleanup_function_symbol.empty()) {
          out << "  call void @" << cleanup_call.cleanup_function_symbol
              << "(i32 " << loaded_value << ")\n";
        }
      }
      (void)released_value;
    }
    out << "  ret void\n";
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  std::string EmitPromotedBlockHandle(const Expr &expr,
                                      const std::string &storage_ptr,
                                      FunctionContext &ctx,
                                      bool allow_nonescaping_scalar_promotion =
                                          false) const {
    // escaping-block runtime-hook anchor: readonly-scalar escaping
    // block values now lower through a private runtime promotion hook instead
    // of failing closed at the first escaping expression use.
    const bool supported = allow_nonescaping_scalar_promotion
                               ? BlockLiteralSupportsScalarRuntimePromotion(expr)
                               : BlockLiteralSupportsEscapingRuntimeHookLowering(
                                     expr);
    if (!supported) {
      return EmitUnsupportedI32Value(
          "escaping block value still requires normalized block runtime helper metadata that lands in later runtime work");
    }
    // lowering-implementation anchor: actual promotion of move-based
    // cleanup/resource captures remains fail-closed until runtime ownership
    // transfer exists. Plain stack/local helper lowering is implemented now.
    if (expr.block_explicit_capture_move_count > 0u) {
      return EmitUnsupportedI32Value(
          "escaping move captures for cleanup/resource-backed locals still require later Part 8 runtime ownership transfer support");
    }
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    const std::string promoted = NewTemp(ctx);
    ctx.code_lines.push_back(
        "  " + promoted + " = call i32 @" +
        std::string(kObjc3RuntimePromoteBlockI32Symbol) + "(ptr " + storage_ptr +
        ", i64 " + std::to_string(BlockStorageStaticSizeBytes(expr)) +
        ", i32 " + std::string(pointer_capture_storage ? "1" : "0") + ")");
    return promoted;
  }

  std::string EmitPromotedBlockHandleLoad(BlockBinding &binding,
                                          FunctionContext &ctx) const {
    if (!binding.promoted_handle_ptr.empty()) {
      const std::string loaded = NewTemp(ctx);
      ctx.code_lines.push_back("  " + loaded + " = load i32, ptr " +
                               binding.promoted_handle_ptr + ", align 4");
      return loaded;
    }
    if (binding.literal == nullptr || binding.storage_ptr.empty()) {
      return EmitUnsupportedI32Value(
          "missing block literal metadata for escaping block-handle lowering");
    }
    const std::string promoted =
        EmitPromotedBlockHandle(*binding.literal, binding.storage_ptr, ctx,
                                true);
    if (promoted == "poison") {
      return promoted;
    }
    binding.promoted_handle_ptr =
        "%block.promoted.addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + binding.promoted_handle_ptr +
                              " = alloca i32, align 4");
    ctx.code_lines.push_back("  store i32 " + promoted + ", ptr " +
                             binding.promoted_handle_ptr + ", align 4");
    return promoted;
  }

  std::string EmitIdentifierValue(const std::string &name,
                                  FunctionContext &ctx) const {
    auto block_it = ctx.block_bindings.find(name);
    if (block_it != ctx.block_bindings.end()) {
      return EmitPromotedBlockHandleLoad(block_it->second, ctx);
    }
    const std::string ptr = LookupVarPtr(ctx, name);
    if (!ptr.empty()) {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = load i32, ptr " + ptr + ", align 4");
      return tmp;
    }
    if (globals_.find(name) != globals_.end()) {
      const std::string tmp = NewTemp(ctx);
      ctx.code_lines.push_back("  " + tmp + " = load i32, ptr @" + name + ", align 4");
      return tmp;
    }
    const auto immediate_it = ctx.immediate_identifiers.find(name);
    if (immediate_it != ctx.immediate_identifiers.end()) {
      return std::to_string(immediate_it->second);
    }
    // Match bindings are now materialized by the executable Part 5 lowering
    // path when a live match arm captures the condition value. Remaining
    // unresolved identifiers still fail closed here.
    return EmitUnsupportedI32Value("unresolved identifier '" + name + "' during IR lowering");
  }

  std::string EmitTypedKeyPathLiteralValue(const Expr &expr) const {
    const std::string profile =
        expr.typed_keypath_literal_profile.empty()
            ? std::string("typed-keypath:root=") + expr.typed_keypath_root_name
            : expr.typed_keypath_literal_profile;
    const auto artifact_it = typed_keypath_artifacts_.find(profile);
    if (artifact_it == typed_keypath_artifacts_.end()) {
      return EmitUnsupportedI32Value(
          "typed key-path artifact '" + profile +
          "' was not registered before IR lowering");
    }
    return std::to_string(
        static_cast<unsigned long long>(artifact_it->second.ordinal + 1u));
  }

  void EmitBlockInvokeThunk(const Expr &expr) const {
    // byref-cell/copy-helper/dispose-helper anchor: each runnable
    // local block literal now receives one internal invoke thunk definition
    // that rehydrates readonly captures from snapshot cells and mutated captures
    // from stack byref-cell references.
    const std::string symbol = BuildBlockInvokeSymbol(expr);
    if (symbol.empty() || !emitted_block_invoke_symbols_.insert(symbol).second) {
      return;
    }

    std::ostringstream out;
    out << "define internal i32 @" << symbol
        << "(ptr %block, i32 %arg0, i32 %arg1, i32 %arg2, i32 %arg3) {\n";
    out << "entry:\n";

    FunctionContext ctx;
    ctx.return_type = ValueType::I32;
    PushScope(ctx);

    const std::string block_storage_type = BuildBlockStorageType(expr);
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size(); ++i) {
      const std::string &capture_name = expr.block_capture_names_lexicographic[i];
      const std::string slot_ptr = NewTemp(ctx);
      const std::size_t capture_field_index =
          pointer_capture_storage ? 3u : 1u;
      ctx.entry_lines.push_back("  " + slot_ptr + " = getelementptr inbounds " +
                                block_storage_type +
                                ", ptr %block, i32 0, i32 " +
                                std::to_string(capture_field_index) +
                                ", i32 " + std::to_string(i));
      if (pointer_capture_storage) {
        const std::string capture_ptr = NewTemp(ctx);
        ctx.entry_lines.push_back("  " + capture_ptr + " = load ptr, ptr " +
                                  slot_ptr + ", align 8");
        ctx.scopes.back()[capture_name] = capture_ptr;
        continue;
      }
      const std::string ptr =
          "%" + capture_name + ".addr." + std::to_string(ctx.temp_counter++);
      const std::string value = NewTemp(ctx);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.scopes.back()[capture_name] = ptr;
      ctx.entry_lines.push_back("  " + value + " = load i32, ptr " + slot_ptr +
                                ", align 4");
      ctx.entry_lines.push_back("  store i32 " + value + ", ptr " + ptr +
                                ", align 4");
    }

    for (std::size_t i = 0; i < expr.block_parameters_source_order.size() && i < 4u; ++i) {
      const auto &parameter = expr.block_parameters_source_order[i];
      const std::string ptr =
          "%" + parameter.name + ".addr." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + ptr + " = alloca i32, align 4");
      ctx.entry_lines.push_back("  store i32 %arg" + std::to_string(i) + ", ptr " + ptr + ", align 4");
      ctx.scopes.back()[parameter.name] = ptr;
    }

    for (const auto &stmt : expr.block_body) {
      EmitStatement(stmt.get(), ctx);
      if (ctx.terminated) {
        break;
      }
    }

    if (!ctx.terminated) {
      EmitAutoreleasepoolUnwindToDepth(ctx, 0u);
      EmitOwnershipCleanupUnwindToDepth(ctx, 0u);
      EmitPendingBlockDisposeHelpers(ctx);
      EmitArcOwnedCleanupReleases(ctx);
      ctx.code_lines.push_back("  ret i32 0");
    }

    for (const auto &line : ctx.entry_lines) {
      out << line << "\n";
    }
    for (const auto &line : ctx.code_lines) {
      out << line << "\n";
    }
    out << "}\n";
    block_function_definitions_.push_back(out.str());
  }

  std::string EmitBlockLiteralStorage(const Expr &expr,
                                      FunctionContext &ctx) const {
    // byref-cell/copy-helper/dispose-helper anchor: the current live
    // lowering slice now supports non-escaping byref and owned-capture block
    // objects through stack snapshot/byref cells plus emitted helper bodies.
    // lowering-implementation anchor: cleanup/resource-backed move
    // captures now lower through the same stack/local helper path. The
    // unsupported boundary is promotion, not local helper materialization.
    if (BlockLiteralRequiresFutureRuntimeLanes(expr)) {
      return EmitUnsupportedI32Value(
          "block literal requires escaping heap-promotion or runtime-managed copy/dispose lowering that lands in later runtime work");
    }
    if (expr.block_parameter_count > 4u) {
      return EmitUnsupportedI32Value(
          "block literal exceeds current runnable invoke-thunk arity limit of 4");
    }

    EmitBlockInvokeThunk(expr);
    EmitBlockCopyHelper(expr);
    EmitBlockDisposeHelper(expr, ctx);

    const std::string storage_type = BuildBlockStorageType(expr);
    const bool pointer_capture_storage =
        BlockLiteralUsesPointerCaptureStorage(expr);
    const std::string storage_ptr =
        "%block.literal.addr." + std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + storage_ptr + " = alloca " + storage_type + ", align 8");

    const std::string invoke_ptr_slot = NewTemp(ctx);
    ctx.code_lines.push_back("  " + invoke_ptr_slot + " = getelementptr inbounds " +
                             storage_type + ", ptr " + storage_ptr + ", i32 0, i32 0");
    ctx.code_lines.push_back("  store ptr @" + BuildBlockInvokeSymbol(expr) + ", ptr " +
                             invoke_ptr_slot + ", align 8");

    if (pointer_capture_storage) {
      const std::string copy_helper_slot = NewTemp(ctx);
      const std::string dispose_helper_slot = NewTemp(ctx);
      ctx.code_lines.push_back("  " + copy_helper_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr + ", i32 0, i32 1");
      ctx.code_lines.push_back("  store ptr " +
                               std::string(expr.block_runtime_copy_helper_required
                                               ? "@" + BuildBlockCopyHelperSymbol(expr)
                                               : "null") +
                               ", ptr " + copy_helper_slot + ", align 8");
      ctx.code_lines.push_back("  " + dispose_helper_slot +
                               " = getelementptr inbounds " + storage_type +
                               ", ptr " + storage_ptr + ", i32 0, i32 2");
      ctx.code_lines.push_back("  store ptr " +
                               std::string(expr.block_runtime_dispose_helper_required
                                               ? "@" + BuildBlockDisposeHelperSymbol(expr)
                                               : "null") +
                               ", ptr " + dispose_helper_slot + ", align 8");
    }

    for (std::size_t i = 0; i < expr.block_capture_names_lexicographic.size(); ++i) {
      const std::string &capture_name = expr.block_capture_names_lexicographic[i];
      const std::string capture_slot = NewTemp(ctx);
      if (pointer_capture_storage) {
        std::string capture_cell_ptr;
        const auto moved_capture_it = std::find_if(
            expr.block_explicit_capture_items_source_order.begin(),
            expr.block_explicit_capture_items_source_order.end(),
            [&capture_name](const Expr::ExplicitBlockCaptureItem &item) {
              return item.name == capture_name && item.mode == "move";
            });
        if (moved_capture_it != expr.block_explicit_capture_items_source_order.end()) {
          const auto cleanup_it = ctx.ownership_cleanup_call_indices.find(capture_name);
          if (cleanup_it == ctx.ownership_cleanup_call_indices.end()) {
            return EmitUnsupportedI32Value(
                "move capture '" + capture_name +
                "' was not registered as a cleanup/resource-backed local");
          }
          PendingOwnershipCleanupCall &cleanup_call =
              ctx.pending_ownership_cleanup_calls[cleanup_it->second];
          cleanup_call.active = false;
          capture_cell_ptr = cleanup_call.storage_ptr;
        } else if (SortedStringListContains(
                expr.block_byref_capture_names_lexicographic, capture_name)) {
          capture_cell_ptr = LookupVarPtr(ctx, capture_name);
          if (capture_cell_ptr.empty()) {
            return EmitUnsupportedI32Value(
                "block literal byref capture '" + capture_name +
                "' could not be resolved during IR lowering");
          }
        } else {
          capture_cell_ptr = "%" + capture_name + ".capture.addr." +
                             std::to_string(ctx.temp_counter++);
          ctx.entry_lines.push_back("  " + capture_cell_ptr +
                                    " = alloca i32, align 4");
          const std::string capture_value = EmitIdentifierValue(capture_name, ctx);
          ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " +
                                   capture_cell_ptr + ", align 4");
        }
        ctx.code_lines.push_back("  " + capture_slot +
                                 " = getelementptr inbounds " + storage_type +
                                 ", ptr " + storage_ptr +
                                 ", i32 0, i32 3, i32 " + std::to_string(i));
        ctx.code_lines.push_back("  store ptr " + capture_cell_ptr + ", ptr " +
                                 capture_slot + ", align 8");
        continue;
      }
      const std::string capture_value = EmitIdentifierValue(capture_name, ctx);
      ctx.code_lines.push_back("  " + capture_slot + " = getelementptr inbounds " +
                               storage_type + ", ptr " + storage_ptr + ", i32 0, i32 1, i32 " +
                               std::to_string(i));
      ctx.code_lines.push_back("  store i32 " + capture_value + ", ptr " + capture_slot +
                               ", align 4");
    }

    if (pointer_capture_storage && expr.block_runtime_copy_helper_required) {
      ctx.code_lines.push_back("  call void @" + BuildBlockCopyHelperSymbol(expr) +
                               "(ptr " + storage_ptr + ")");
    }
    if (pointer_capture_storage && expr.block_runtime_dispose_helper_required) {
      ctx.pending_block_dispose_calls.push_back(
          PendingBlockDisposeCall{BuildBlockDisposeHelperSymbol(expr),
                                  storage_ptr});
    }

    return storage_ptr;
  }

  std::string EmitBlockInvokeCall(const BlockBinding &binding,
                                  const Expr *call_expr,
                                  FunctionContext &ctx) const {
    if (binding.literal == nullptr) {
      return EmitUnsupportedI32Value(
          "missing block literal metadata for local callable invocation");
    }

    if (!binding.promoted_handle_ptr.empty()) {
      std::array<std::string, 4> args{"0", "0", "0", "0"};
      for (std::size_t i = 0; i < call_expr->args.size() && i < args.size();
           ++i) {
        args[i] = EmitExpr(call_expr->args[i].get(), ctx);
      }
      const std::string handle = NewTemp(ctx);
      ctx.code_lines.push_back("  " + handle + " = load i32, ptr " +
                               binding.promoted_handle_ptr + ", align 4");
      const std::string out = NewTemp(ctx);
      ctx.code_lines.push_back(
          "  " + out + " = call i32 @" +
          std::string(kObjc3RuntimeInvokeBlockI32Symbol) + "(i32 " + handle +
          ", i32 " + args[0] + ", i32 " + args[1] + ", i32 " + args[2] +
          ", i32 " + args[3] + ")");
      return out;
    }

    const std::string storage_type = BuildBlockStorageType(*binding.literal);
    const std::string invoke_ptr_slot = NewTemp(ctx);
    const std::string invoke_ptr = NewTemp(ctx);
    ctx.code_lines.push_back("  " + invoke_ptr_slot + " = getelementptr inbounds " +
                             storage_type + ", ptr " + binding.storage_ptr + ", i32 0, i32 0");
    ctx.code_lines.push_back("  " + invoke_ptr + " = load ptr, ptr " + invoke_ptr_slot +
                             ", align 8");

    std::array<std::string, 4> args{"0", "0", "0", "0"};
    for (std::size_t i = 0; i < call_expr->args.size() && i < args.size(); ++i) {
      args[i] = EmitExpr(call_expr->args[i].get(), ctx);
    }

    const std::string out = NewTemp(ctx);
    ctx.code_lines.push_back("  " + out + " = call i32 " + invoke_ptr + "(ptr " +
                             binding.storage_ptr + ", i32 " + args[0] + ", i32 " + args[1] +
                             ", i32 " + args[2] + ", i32 " + args[3] + ")");
    return out;
  }

  void CollectKnownClassReceiverConstants() {
    std::set<std::string> class_names;
    for (const auto &interface_decl : program_.interfaces) {
      if (!interface_decl.has_category && !interface_decl.name.empty()) {
        class_names.insert(interface_decl.name);
      }
    }
    for (const auto &implementation : program_.implementations) {
      if (!implementation.has_category && !implementation.name.empty()) {
        class_names.insert(implementation.name);
      }
    }
    std::size_t ordinal = 0;
    for (const std::string &class_name : class_names) {
      class_receiver_constants_[class_name] =
          NextNonZeroReceiverIdentityValue(ordinal++, 0);
    }
  }

  int LookupClassReceiverIdentityValue(const std::string &class_name) const {
    const auto value_it = class_receiver_constants_.find(class_name);
    if (value_it == class_receiver_constants_.end()) {
      return 0;
    }
    return value_it->second;
  }

  void SeedKnownClassReceiverBindings(FunctionContext &ctx) const {
    for (const auto &entry : class_receiver_constants_) {
      ctx.immediate_identifiers.emplace(entry.first, entry.second);
    }
  }

  std::string CoerceI32ToBoolI1(const std::string &i32_value, FunctionContext &ctx) const {
    const std::string bool_i1 = NewTemp(ctx);
    ctx.code_lines.push_back("  " + bool_i1 + " = icmp ne i32 " + i32_value + ", 0");
    return bool_i1;
  }

  std::string CoerceValueToI32(const std::string &value, ValueType value_type, FunctionContext &ctx) const {
    if (value_type != ValueType::Bool) {
      return value;
    }
    const std::string widened = NewTemp(ctx);
    ctx.code_lines.push_back("  " + widened + " = zext i1 " + value + " to i32");
    return widened;
  }

  const LoweredFunctionSignature *LookupFunctionSignature(const std::string &name) const {
    auto signature_it = function_signatures_.find(name);
    if (signature_it == function_signatures_.end()) {
      return nullptr;
    }
    return &signature_it->second;
  }

  std::string BuildThrowsErrorSlotAlloca(FunctionContext &ctx,
                                         const std::string &prefix) const {
    const std::string slot = "%" + prefix + ".error.addr." +
                             std::to_string(ctx.temp_counter++);
    ctx.entry_lines.push_back("  " + slot + " = alloca i32, align 4");
    return slot;
  }

  void EmitStoreThrownError(const std::string &error_value,
                            const std::string &slot,
                            FunctionContext &ctx) const {
    if (slot.empty()) {
      return;
    }
    ctx.code_lines.push_back("  call void @" +
                             std::string(kObjc3RuntimeStoreThrownErrorI32Symbol) +
                             "(ptr " + slot + ", i32 " + error_value + ")");
  }

  std::string EmitLoadThrownError(const std::string &slot,
                                  FunctionContext &ctx) const {
    if (slot.empty()) {
      return "0";
    }
    const std::string loaded = NewTemp(ctx);
    ctx.code_lines.push_back("  " + loaded + " = call i32 @" +
                             std::string(kObjc3RuntimeLoadThrownErrorI32Symbol) +
                             "(ptr " + slot + ")");
    return loaded;
  }

  void EmitPropagateThrownError(const std::string &error_value,
                                FunctionContext &ctx) const {
    if (!ctx.error_handler_stack.empty()) {
      const auto &handler = ctx.error_handler_stack.back();
      EmitStoreThrownError(error_value, handler.error_slot_ptr, ctx);
      EmitTerminalCleanupToDepth(ctx, handler.scope_depth,
                                 handler.autoreleasepool_depth,
                                 handler.pending_block_dispose_depth,
                                 handler.ownership_cleanup_depth,
                                 handler.arc_cleanup_depth);
      ctx.code_lines.push_back("  br label %" + handler.dispatch_label);
      ctx.terminated = true;
      return;
    }
    if (!ctx.function_error_out_param.empty()) {
      EmitStoreThrownError(error_value, ctx.function_error_out_param, ctx);
      EmitTypedReturn("0", ctx);
      ctx.terminated = true;
      return;
    }
    ctx.code_lines.push_back("  call void @abort()");
    ctx.code_lines.push_back("  unreachable");
    ctx.terminated = true;
  }

  std::string EmitDirectFunctionCall(const Expr *expr,
                                     const LoweredFunctionSignature *signature,
                                     FunctionContext &ctx,
                                     const std::string &throws_error_slot_ptr,
                                     bool *bridge_failed_out = nullptr,
                                     std::string *bridge_error_value_out = nullptr) const {
    return EmitObjc3IRDirectFunctionCall(
        expr, signature, ctx,
        Objc3IRDirectCallEmissionCallbacks{
            [this, &ctx](const Expr *arg_expr) {
              return EmitExpr(arg_expr, ctx);
            },
            [this](FunctionContext &callback_ctx) {
              return NewTemp(callback_ctx);
            },
            [this](const std::string &value, FunctionContext &callback_ctx) {
              return CoerceI32ToBoolI1(value, callback_ctx);
            },
            [this](const std::string &value, ValueType value_type,
                   FunctionContext &callback_ctx) {
              return CoerceValueToI32(value, value_type, callback_ctx);
            },
            [this](const std::string &function_name) {
              return Objc3IRFunctionMayHaveGlobalSideEffects(
                  function_name, defined_functions_, declared_pure_functions_,
                  impure_functions_);
            },
            [this](const Expr *call_expr, FunctionContext &callback_ctx,
                   std::string &result_out) {
              return TryEmitConcurrencyActorLoweringCall(
                  call_expr, callback_ctx, result_out);
            },
            [this](const Expr *call_expr, FunctionContext &callback_ctx,
                   std::string &result_out) {
              return TryEmitConcurrencyTaskRuntimeLoweringCall(
                  call_expr, callback_ctx, result_out);
            },
            [this](FunctionContext &callback_ctx) {
              InvalidateGlobalProofState(callback_ctx);
            },
            [this](const std::string &name) {
              return LookupFunctionSignature(name);
            }},
        throws_error_slot_ptr, bridge_failed_out, bridge_error_value_out);
  }

  void RegisterArcOwnedCleanupPtr(const std::string &ptr,
                                  FunctionContext &ctx) const {
    RegisterObjc3IRArcOwnedCleanupPtr(ptr, ctx);
  }

  void EmitArcOwnedCleanupReleases(FunctionContext &ctx) const {
    EmitObjc3IRArcOwnedCleanupReleases(ctx, ScopeCleanupCallbacks());
  }

  void EmitTerminalCleanupToDepth(FunctionContext &ctx, std::size_t scope_depth,
                                  std::size_t autoreleasepool_depth,
                                  std::size_t pending_block_dispose_depth,
                                  std::size_t ownership_cleanup_depth,
                                  std::size_t arc_cleanup_depth) const {
    EmitObjc3IRTerminalCleanupToDepth(
        ctx, scope_depth, autoreleasepool_depth, pending_block_dispose_depth,
        ownership_cleanup_depth, arc_cleanup_depth, ScopeCleanupCallbacks());
  }

  void EmitTypedReturn(const std::string &i32_value, FunctionContext &ctx) const {
    if (ctx.return_type == ValueType::Void) {
      EmitDeferredCleanupTerminalToDepth(ctx, 0u);
      EmitOwnershipCleanupTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      EmitPendingBlockDisposeTerminalCleanupToDepth(ctx, 0u, ctx.code_lines);
      EmitArcOwnedTerminalCleanupToDepth(
          ctx, 0u, ctx.code_lines, ctx.temp_counter);
      ctx.code_lines.push_back("  ret void");
      return;
    }
    std::string returned_value = i32_value;
    if (ctx.arc_return_insert_retain) {
      const std::string retained_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + retained_value + " = call i32 @" +
                               std::string(kObjc3RuntimeRetainI32Symbol) +
                               "(i32 " + returned_value + ")");
      returned_value = retained_value;
    }
    if (ctx.arc_return_insert_autorelease) {
      const std::string autoreleased_value = NewTemp(ctx);
      ctx.code_lines.push_back("  " + autoreleased_value + " = call i32 @" +
                               std::string(kObjc3RuntimeAutoreleaseI32Symbol) +
                               "(i32 " + returned_value + ")");
      returned_value = autoreleased_value;
    }
    EmitDeferredCleanupTerminalToDepth(ctx, 0u);
    EmitOwnershipCleanupTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    EmitPendingBlockDisposeTerminalCleanupToDepth(ctx, 0u, ctx.code_lines);
    EmitArcOwnedTerminalCleanupToDepth(
        ctx, 0u, ctx.code_lines, ctx.temp_counter);
    if (ctx.return_type == ValueType::Bool) {
      const std::string bool_i1 = CoerceI32ToBoolI1(returned_value, ctx);
      ctx.code_lines.push_back("  ret i1 " + bool_i1);
      return;
    }
    ctx.code_lines.push_back("  ret i32 " + returned_value);
  }

  void EmitTypedParamStore(const FuncParam &param, std::size_t index, const std::string &ptr, FunctionContext &ctx) const {
    if (param.type == ValueType::Bool) {
      const std::string widened = "%arg" + std::to_string(index) + ".zext." + std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + widened + " = zext i1 %arg" + std::to_string(index) + " to i32");
      ctx.entry_lines.push_back("  store i32 " + widened + ", ptr " + ptr + ", align 4");
      return;
    }
    std::string stored_value = "%arg" + std::to_string(index);
    if (EffectiveArcParamInsertRetain(param, frontend_metadata_.arc_mode_enabled)) {
      const std::string retained_value =
          "%arg" + std::to_string(index) + ".retained." +
          std::to_string(ctx.temp_counter++);
      ctx.entry_lines.push_back("  " + retained_value + " = call i32 @" +
                                std::string(kObjc3RuntimeRetainI32Symbol) +
                                "(i32 " + stored_value + ")");
      stored_value = retained_value;
    }
    ctx.entry_lines.push_back("  store i32 " + stored_value + ", ptr " + ptr +
                              ", align 4");
    if (EffectiveArcParamInsertRelease(param, frontend_metadata_.arc_mode_enabled)) {
      RegisterArcOwnedCleanupPtr(ptr, ctx);
    }
  }

  bool IsCompileTimeNilReceiverExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeNilReceiverExprInContext(expr->right.get(), ctx);
      }
      return IsCompileTimeNilReceiverExprInContext(expr->third.get(), ctx);
    }
    if (expr->kind != Expr::Kind::Identifier) {
      return false;
    }
    const std::string ptr = LookupVarPtr(ctx, expr->ident);
    if (ptr.empty()) {
      return LookupImmediateIdentifierValue(ctx, expr->ident) == 0;
    }
    if (ctx.nil_bound_ptrs.find(ptr) != ctx.nil_bound_ptrs.end()) {
      return true;
    }
    if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    return false;
  }

  bool IsCompileTimeGlobalNilExpr(const Expr *expr) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      return global_nil_proven_symbols_.find(expr->ident) != global_nil_proven_symbols_.end();
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      const FunctionContext global_eval_ctx;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), global_eval_ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return IsCompileTimeGlobalNilExpr(expr->right.get());
      }
      return IsCompileTimeGlobalNilExpr(expr->third.get());
    }
    return false;
  }

  bool TryGetCompileTimeI32ExprInContext(const Expr *expr, const FunctionContext &ctx, int &value) const {
    if (expr == nullptr) {
      return false;
    }
    if (expr->kind == Expr::Kind::Number) {
      value = expr->number;
      return true;
    }
    if (expr->kind == Expr::Kind::BoolLiteral) {
      value = expr->bool_value ? 1 : 0;
      return true;
    }
    if (expr->kind == Expr::Kind::NilLiteral) {
      value = 0;
      return true;
    }
    if (expr->kind == Expr::Kind::Identifier) {
      const std::string ptr = LookupVarPtr(ctx, expr->ident);
      if (ptr.empty()) {
        const int immediate_value =
            LookupImmediateIdentifierValue(ctx, expr->ident);
        if (immediate_value != 0) {
          value = immediate_value;
          return true;
        }
        return false;
      }
      auto value_it = ctx.const_value_ptrs.find(ptr);
      if (value_it != ctx.const_value_ptrs.end()) {
        value = value_it->second;
        return true;
      }
      if (ptr.rfind("@", 0) == 0 && !ctx.global_proofs_invalidated) {
        auto global_it = global_const_values_.find(expr->ident);
        if (global_it != global_const_values_.end()) {
          value = global_it->second;
          return true;
        }
      }
      return false;
    }
    if (expr->kind == Expr::Kind::Conditional) {
      if (expr->left == nullptr || expr->right == nullptr || expr->third == nullptr) {
        return false;
      }
      int cond_value = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, cond_value)) {
        return false;
      }
      if (cond_value != 0) {
        return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
      }
      return TryGetCompileTimeI32ExprInContext(expr->third.get(), ctx, value);
    }
    if (expr->kind != Expr::Kind::Binary || expr->left == nullptr || expr->right == nullptr) {
      return false;
    }
    if (expr->op == "&&" || expr->op == "||") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (expr->op == "&&") {
        if (lhs == 0) {
          value = 0;
          return true;
        }
        int rhs = 0;
        if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
          return false;
        }
        value = rhs != 0 ? 1 : 0;
        return true;
      }
      if (lhs != 0) {
        value = 1;
        return true;
      }
      int rhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
        return false;
      }
      value = rhs != 0 ? 1 : 0;
      return true;
    }
    if (expr->op == "??") {
      int lhs = 0;
      if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs)) {
        return false;
      }
      if (lhs != 0) {
        value = lhs;
        return true;
      }
      return TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, value);
    }
    int lhs = 0;
    int rhs = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr->left.get(), ctx, lhs) ||
        !TryGetCompileTimeI32ExprInContext(expr->right.get(), ctx, rhs)) {
      return false;
    }
    if (expr->op == "+") {
      value = lhs + rhs;
      return true;
    }
    if (expr->op == "-") {
      value = lhs - rhs;
      return true;
    }
    if (expr->op == "*") {
      value = lhs * rhs;
      return true;
    }
    if (expr->op == "/") {
      if (rhs == 0) {
        return false;
      }
      value = lhs / rhs;
      return true;
    }
    if (expr->op == "%") {
      if (rhs == 0) {
        return false;
      }
      value = lhs % rhs;
      return true;
    }
    if (expr->op == "&") {
      value = lhs & rhs;
      return true;
    }
    if (expr->op == "|") {
      value = lhs | rhs;
      return true;
    }
    if (expr->op == "^") {
      value = lhs ^ rhs;
      return true;
    }
    if (expr->op == "<<" || expr->op == ">>") {
      if (rhs < 0 || rhs > 31) {
        return false;
      }
      value = expr->op == "<<" ? (lhs << rhs) : (lhs >> rhs);
      return true;
    }
    if (expr->op == "==") {
      value = lhs == rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "!=") {
      value = lhs != rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<") {
      value = lhs < rhs ? 1 : 0;
      return true;
    }
    if (expr->op == "<=") {
      value = lhs <= rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">") {
      value = lhs > rhs ? 1 : 0;
      return true;
    }
    if (expr->op == ">=") {
      value = lhs >= rhs ? 1 : 0;
      return true;
    }
    return false;
  }

  bool IsCompileTimeKnownNonNilExprInContext(const Expr *expr, const FunctionContext &ctx) const {
    int const_value = 0;
    if (!TryGetCompileTimeI32ExprInContext(expr, ctx, const_value)) {
      return false;
    }
    return const_value != 0;
  }

  std::string EmitMessageSendExpr(const Expr *expr, FunctionContext &ctx) const {
    return EmitObjc3IRMessageSendExpr(
        expr, ctx,
        Objc3IRMessageSendEmissionOptions{
            selector_pool_globals_, class_receiver_constants_,
            direct_dispatch_symbols_by_key_,
            lowering_ir_boundary_.runtime_dispatch_arg_slots,
            lowering_ir_boundary_.runtime_dispatch_symbol,
            runtime_dispatch_call_state_},
        Objc3IRMessageSendEmissionCallbacks{
            [this](const Expr *arg_expr, FunctionContext &callback_ctx) {
              return EmitExpr(arg_expr, callback_ctx);
            },
            [this](FunctionContext &callback_ctx) {
              return NewTemp(callback_ctx);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return NewLabel(callback_ctx, prefix);
            },
            [this](const Expr *receiver_expr,
                   const FunctionContext &callback_ctx) {
              return IsCompileTimeNilReceiverExprInContext(receiver_expr,
                                                           callback_ctx);
            },
            [this](const Expr *receiver_expr,
                   const FunctionContext &callback_ctx) {
              return IsCompileTimeKnownNonNilExprInContext(receiver_expr,
                                                           callback_ctx);
            },
            [this](const std::string &reason) {
              return EmitUnsupportedI32Value(reason);
            },
            [this](FunctionContext &callback_ctx) {
              InvalidateGlobalProofState(callback_ctx);
            }});
  }

  std::string EmitExpr(const Expr *expr, FunctionContext &ctx) const {
    return EmitObjc3IRExpr(
        expr, ctx,
        Objc3IRExpressionEmissionCallbacks{
            [this](FunctionContext &callback_ctx) {
              return NewTemp(callback_ctx);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return NewLabel(callback_ctx, prefix);
            },
            [this](const std::string &reason) {
              return EmitUnsupportedI32Value(reason);
            },
            [this](const std::string &name, FunctionContext &callback_ctx) {
              return EmitIdentifierValue(name, callback_ctx);
            },
            [this](const Expr &callback_expr) {
              return EmitTypedKeyPathLiteralValue(callback_expr);
            },
            [this](const Expr &callback_expr, FunctionContext &callback_ctx) {
              return EmitBlockLiteralStorage(callback_expr, callback_ctx);
            },
            [this](const Expr &callback_expr,
                   const std::string &storage_ptr,
                   FunctionContext &callback_ctx) {
              return EmitPromotedBlockHandle(callback_expr, storage_ptr,
                                             callback_ctx);
            },
            [this](const BlockBinding &binding, const Expr *call_expr,
                   FunctionContext &callback_ctx) {
              return EmitBlockInvokeCall(binding, call_expr, callback_ctx);
            },
            [this](const std::string &name) {
              return LookupFunctionSignature(name);
            },
            [this](const Expr *call_expr,
                   const LoweredFunctionSignature *signature,
                   FunctionContext &callback_ctx,
                   const std::string &throws_error_slot_ptr,
                   bool *bridge_failed_out,
                   std::string *bridge_error_value_out) {
              return EmitDirectFunctionCall(
                  call_expr, signature, callback_ctx, throws_error_slot_ptr,
                  bridge_failed_out, bridge_error_value_out);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return BuildThrowsErrorSlotAlloca(callback_ctx, prefix);
            },
            [this](const std::string &error_value,
                   FunctionContext &callback_ctx) {
              EmitPropagateThrownError(error_value, callback_ctx);
            },
            [this](const Expr *message_expr, FunctionContext &callback_ctx) {
              return EmitMessageSendExpr(message_expr, callback_ctx);
            }});
  }

  std::string EmitUnsupportedI32Value(const std::string &reason) const {
    if (!unsupported_fail_closed_path_triggered_) {
      unsupported_fail_closed_path_triggered_ = true;
      unsupported_fail_closed_path_reason_ = reason;
    }
    return "poison";
  }

  void EmitStatement(const Stmt *stmt, FunctionContext &ctx) const {
    EmitObjc3IRStatement(
        stmt, ctx,
        Objc3IRStatementEmissionCallbacks{
            [this](const Expr *expr, FunctionContext &callback_ctx) {
              return EmitExpr(expr, callback_ctx);
            },
            [this](const Expr &expr, FunctionContext &callback_ctx) {
              return EmitBlockLiteralStorage(expr, callback_ctx);
            },
            [this](FunctionContext &callback_ctx) {
              return NewTemp(callback_ctx);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return NewLabel(callback_ctx, prefix);
            },
            [this](const FunctionContext &callback_ctx,
                   const std::string &name) {
              return LookupVarPtr(callback_ctx, name);
            },
            [this](const Expr *expr, const FunctionContext &callback_ctx,
                   int &value) {
              return TryGetCompileTimeI32ExprInContext(
                  expr, callback_ctx, value);
            },
            [this](const Expr *expr, const FunctionContext &callback_ctx) {
              return IsCompileTimeNilReceiverExprInContext(expr,
                                                           callback_ctx);
            },
            [this](const std::string &reason) {
              return EmitUnsupportedI32Value(reason);
            },
            [this](FunctionContext &callback_ctx) {
              PushScope(callback_ctx);
            },
            [this](FunctionContext &callback_ctx, bool emit_cleanup) {
              PopScope(callback_ctx, emit_cleanup);
            },
            [this](FunctionContext &callback_ctx, std::size_t target_depth) {
              EmitAutoreleasepoolUnwindToDepth(callback_ctx, target_depth);
            },
            [this](const std::string &i32_value,
                   FunctionContext &callback_ctx) {
              EmitTypedReturn(i32_value, callback_ctx);
            },
            [this](FunctionContext &callback_ctx, std::size_t scope_depth,
                   std::size_t autoreleasepool_depth,
                   std::size_t pending_block_dispose_depth,
                   std::size_t ownership_cleanup_depth,
                   std::size_t arc_cleanup_depth) {
              EmitTerminalCleanupToDepth(
                  callback_ctx, scope_depth, autoreleasepool_depth,
                  pending_block_dispose_depth, ownership_cleanup_depth,
                  arc_cleanup_depth);
            },
            [this](FunctionContext &callback_ctx,
                   const std::string &prefix) {
              return BuildThrowsErrorSlotAlloca(callback_ctx, prefix);
            },
            [this](const std::string &slot, FunctionContext &callback_ctx) {
              return EmitLoadThrownError(slot, callback_ctx);
            },
            [this](const std::string &error_value,
                   FunctionContext &callback_ctx) {
              EmitPropagateThrownError(error_value, callback_ctx);
            }});
  }

  void InvalidateGlobalProofState(FunctionContext &ctx) const {
    ctx.global_proofs_invalidated = true;
    for (auto it = ctx.nil_bound_ptrs.begin(); it != ctx.nil_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nil_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.nonzero_bound_ptrs.begin(); it != ctx.nonzero_bound_ptrs.end();) {
      if (it->rfind("@", 0) == 0) {
        it = ctx.nonzero_bound_ptrs.erase(it);
      } else {
        ++it;
      }
    }
    for (auto it = ctx.const_value_ptrs.begin(); it != ctx.const_value_ptrs.end();) {
      if (it->first.rfind("@", 0) == 0) {
        it = ctx.const_value_ptrs.erase(it);
      } else {
        ++it;
      }
    }
  }

  void EmitPrototypeDeclarations(std::ostringstream &out) const {
    EmitObjc3IRPrototypeDeclarations(
        Objc3IRPrototypeDeclarationOptions{
            program_, frontend_metadata_, method_definitions_,
            function_signatures_, defined_functions_,
            synthesized_property_accessor_count_,
            ShouldEmitRuntimeBootstrapLowering()},
        out);
  }

  void EmitRuntimeBootstrapLoweringFunctions(std::ostringstream &out) const {
    EmitObjc3IRRuntimeBootstrapLoweringFunctions(
        frontend_metadata_, runtime_metadata_symbols_, out);
  }

  void EmitRuntimeDispatchDeclarations(std::ostringstream &out) const {
    EmitObjc3IRRuntimeDispatchDeclarations(lowering_ir_boundary_,
                                           runtime_dispatch_call_state_, out);
  }

  Objc3IRFunctionDefinitionEmissionCallbacks
  BuildFunctionDefinitionEmissionCallbacks() const {
    return Objc3IRFunctionDefinitionEmissionCallbacks{
        [this](FunctionContext &ctx) { PushScope(ctx); },
        [this](FunctionContext &ctx) { SeedKnownClassReceiverBindings(ctx); },
        [this](const FuncParam &param, std::size_t index,
               const std::string &ptr, FunctionContext &ctx) {
          EmitTypedParamStore(param, index, ptr, ctx);
        },
        [this](const Stmt *stmt, FunctionContext &ctx) {
          EmitStatement(stmt, ctx);
        },
        [this](FunctionContext &ctx, std::size_t depth) {
          EmitAutoreleasepoolUnwindToDepth(ctx, depth);
        },
        [this](const std::string &i32_value, FunctionContext &ctx) {
          EmitTypedReturn(i32_value, ctx);
        },
        [this](const std::string &name) {
          return IsActorImplementation(name);
        },
        [this](const std::string &class_name) {
          return LookupClassReceiverIdentityValue(class_name);
        },
        [this](const Objc3IRMethodDefinition &method_def,
               std::ostringstream &out) { EmitSyntheticMethod(method_def, out); }};
  }

  void EmitFunction(const FunctionDecl &fn, std::ostringstream &out) const {
    EmitObjc3IRFunctionDefinition(
        fn, frontend_metadata_.arc_mode_enabled,
        BuildFunctionDefinitionEmissionCallbacks(), out);
  }

  void EmitMethod(const Objc3IRMethodDefinition &method_def,
                  std::ostringstream &out) const {
    EmitObjc3IRMethodDefinition(
        method_def, frontend_metadata_.arc_mode_enabled,
        BuildFunctionDefinitionEmissionCallbacks(), out);
  }

  void EmitSyntheticMethod(const Objc3IRMethodDefinition &method_def,
                           std::ostringstream &out) const {
    if (method_def.synthetic_method_kind ==
        Objc3IRSyntheticMethodKind::MetaprogrammingDerivedEquality) {
      out << "define i32 @" << method_def.symbol << "(i32 %arg0) {\n";
      out << "entry:\n";
      out << "  ret i32 1\n";
      out << "}\n";
      return;
    }
    if (method_def.synthetic_method_kind ==
            Objc3IRSyntheticMethodKind::MetaprogrammingDerivedHash ||
        method_def.synthetic_method_kind ==
            Objc3IRSyntheticMethodKind::MetaprogrammingDerivedDebugDescription) {
      std::uint32_t seed = 2166136261u;
      for (unsigned char ch : method_def.symbol) {
        seed ^= static_cast<std::uint32_t>(ch);
        seed *= 16777619u;
      }
      out << "define i32 @" << method_def.symbol << "() {\n";
      out << "entry:\n";
      out << "  ret i32 " << static_cast<unsigned long long>(seed & 0x7fffffffu)
          << "\n";
      out << "}\n";
      return;
    }
    Objc3IRSynthesizedPropertyAccessorEmissionStats property_stats;
    if (!EmitObjc3IRSynthesizedPropertyAccessorMethod(method_def, out,
                                                      property_stats)) {
      return;
    }
    synthesized_getter_definition_count_ += property_stats.getter_definition_count;
    synthesized_setter_definition_count_ += property_stats.setter_definition_count;
    current_property_read_helper_call_count_ +=
        property_stats.current_property_read_helper_call_count;
    current_property_write_helper_call_count_ +=
        property_stats.current_property_write_helper_call_count;
    current_property_exchange_helper_call_count_ +=
        property_stats.current_property_exchange_helper_call_count;
    weak_current_property_load_helper_call_count_ +=
        property_stats.weak_current_property_load_helper_call_count;
    weak_current_property_store_helper_call_count_ +=
        property_stats.weak_current_property_store_helper_call_count;
    retain_helper_call_count_ += property_stats.retain_helper_call_count;
    release_helper_call_count_ += property_stats.release_helper_call_count;
    autorelease_helper_call_count_ += property_stats.autorelease_helper_call_count;
  }

  void EmitEntryPoint(std::ostringstream &out) const {
    EmitObjc3IREntryPoint(program_, function_arity_, function_signatures_, out);
  }

  const Objc3Program &program_;
  Objc3IRFrontendMetadata frontend_metadata_;
  Objc3IRRuntimeMetadataSymbols runtime_metadata_symbols_;
  Objc3LoweringIRBoundary lowering_ir_boundary_;
  std::string boundary_error_;
  std::unordered_set<std::string> globals_;
  std::unordered_set<std::string> mutable_global_symbols_;
  std::unordered_map<std::string, int> global_const_values_;
  std::unordered_set<std::string> global_nil_proven_symbols_;
  std::unordered_set<std::string> defined_functions_;
  std::unordered_set<std::string> declared_pure_functions_;
  std::vector<const FunctionDecl *> function_definitions_;
  std::vector<Objc3IRMethodDefinition> method_definitions_;
  std::size_t synthesized_property_accessor_count_ = 0;
  std::vector<Objc3IRMetaprogrammingGlobalArtifact> metaprogramming_global_artifacts_;
  std::size_t metaprogramming_derived_method_count_ = 0;
  std::unordered_map<std::string, FunctionEffectInfo> function_effects_;
  std::unordered_set<std::string> impure_functions_;
  std::unordered_map<std::string, std::size_t> function_arity_;
  std::map<std::string, LoweredFunctionSignature> function_signatures_;
  std::unordered_map<std::string, std::string> direct_dispatch_symbols_by_key_;
  std::map<std::string, std::string> selector_pool_globals_;
  std::map<std::string, std::string> runtime_string_pool_globals_;
  std::map<std::string, TypedKeyPathArtifact> typed_keypath_artifacts_;
  std::unordered_map<std::string, int> class_receiver_constants_;
  std::size_t vector_signature_function_count_ = 0;
  mutable std::vector<std::string> block_function_definitions_;
  mutable std::unordered_set<std::string> emitted_block_invoke_symbols_;
  mutable std::unordered_set<std::string> emitted_block_copy_helper_symbols_;
  mutable std::unordered_set<std::string> emitted_block_dispose_helper_symbols_;
  mutable Objc3IRRuntimeDispatchCallState runtime_dispatch_call_state_;
  mutable std::size_t synthesized_getter_definition_count_ = 0;
  mutable std::size_t synthesized_setter_definition_count_ = 0;
  mutable std::size_t current_property_read_helper_call_count_ = 0;
  mutable std::size_t current_property_write_helper_call_count_ = 0;
  mutable std::size_t current_property_exchange_helper_call_count_ = 0;
  mutable std::size_t weak_current_property_load_helper_call_count_ = 0;
  mutable std::size_t weak_current_property_store_helper_call_count_ = 0;
  mutable std::size_t retain_helper_call_count_ = 0;
  mutable std::size_t release_helper_call_count_ = 0;
  mutable std::size_t autorelease_helper_call_count_ = 0;
  mutable bool unsupported_fail_closed_path_triggered_ = false;
  mutable std::string unsupported_fail_closed_path_reason_;
};

bool EmitObjc3IRText(const Objc3Program &program,
                     const Objc3LoweringContract &lowering_contract,
                     const Objc3IRFrontendMetadata &frontend_metadata,
                     std::string &ir,
                     std::string &error) {
  Objc3IREmitter emitter(program, lowering_contract, frontend_metadata);
  return emitter.Emit(ir, error);
}
