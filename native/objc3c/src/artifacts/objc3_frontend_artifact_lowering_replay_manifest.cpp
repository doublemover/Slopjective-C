#include "artifacts/objc3_frontend_artifact_lowering_replay_manifest.h"

#include <ostream>

namespace objc3::artifacts::frontend {

void WriteLoweringReplayManifestEntries(
    std::ostream &manifest,
    std::initializer_list<LoweringReplayManifestEntry> entries) {
  for (const auto &entry : entries) {
    manifest << "  \"" << entry.manifest_key << "\":{\"replay_key\":\""
             << entry.replay_key << "\",\"" << entry.contract_field_name
             << "\":\"" << entry.contract_id
             << "\",\"deterministic_handoff\":"
             << (entry.deterministic ? "true" : "false") << "},\n";
  }
}

void WriteObjc3FrontendDispatchReplayManifestEntries(
    std::ostream &manifest,
    const std::string &id_class_sel_object_pointer_typecheck_replay_key,
    const Objc3IdClassSelObjectPointerTypecheckContract
        &id_class_sel_object_pointer_typecheck_contract,
    const std::string &dispatch_surface_classification_replay_key,
    const Objc3DispatchSurfaceClassificationContract
        &dispatch_surface_classification_contract,
    const std::string &message_send_selector_lowering_replay_key,
    const Objc3MessageSendSelectorLoweringContract
        &message_send_selector_lowering_contract,
    const std::string &dispatch_abi_marshalling_replay_key,
    const Objc3DispatchAbiMarshallingContract
        &dispatch_abi_marshalling_contract,
    const std::string &nil_receiver_semantics_foldability_replay_key,
    const Objc3NilReceiverSemanticsFoldabilityContract
        &nil_receiver_semantics_foldability_contract,
    const std::string &control_flow_control_flow_safety_lowering_replay_key,
    const Objc3ControlFlowControlFlowSafetyLoweringContract
        &control_flow_control_flow_safety_lowering_contract,
    const std::string &super_dispatch_method_family_replay_key,
    const Objc3SuperDispatchMethodFamilyContract
        &super_dispatch_method_family_contract,
    const std::string &runtime_link_host_link_replay_key,
    const Objc3RuntimeLinkHostLinkContract &runtime_link_host_link_contract,
    const Objc3RuntimeSupportLibraryLinkWiringSummary
        &runtime_support_library_link_wiring) {
  WriteLoweringReplayManifestEntries(
      manifest,
      {{"lowering_id_class_sel_object_pointer_typecheck",
        id_class_sel_object_pointer_typecheck_replay_key,
        kObjc3IdClassSelObjectPointerTypecheckLaneContract,
        id_class_sel_object_pointer_typecheck_contract.deterministic},
       {"lowering_dispatch_surface_classification",
        dispatch_surface_classification_replay_key,
        kObjc3DispatchSurfaceClassificationContractId,
        dispatch_surface_classification_contract.deterministic},
       {"lowering_message_send_selector_lowering",
        message_send_selector_lowering_replay_key,
        kObjc3MessageSendSelectorLoweringLaneContract,
        message_send_selector_lowering_contract.deterministic},
       {"lowering_dispatch_abi_marshalling", dispatch_abi_marshalling_replay_key,
        kObjc3DispatchAbiMarshallingLaneContract,
        dispatch_abi_marshalling_contract.deterministic},
       {"lowering_nil_receiver_semantics_foldability",
        nil_receiver_semantics_foldability_replay_key,
        kObjc3NilReceiverSemanticsFoldabilityLaneContract,
        nil_receiver_semantics_foldability_contract.deterministic},
       {"lowering_control_flow_control_flow_safety",
        control_flow_control_flow_safety_lowering_replay_key,
        kObjc3ControlFlowControlFlowSafetyLoweringLaneContract,
        control_flow_control_flow_safety_lowering_contract.deterministic},
       {"lowering_super_dispatch_method_family",
        super_dispatch_method_family_replay_key,
        kObjc3SuperDispatchMethodFamilyLaneContract,
        super_dispatch_method_family_contract.deterministic},
       {"lowering_runtime_link_host_link", runtime_link_host_link_replay_key,
        kObjc3RuntimeLinkHostLinkLaneContract,
        runtime_link_host_link_contract.deterministic}});
  manifest << "  \"runtime_link_host_link_runtime_dispatch_symbol\":\""
           << runtime_link_host_link_contract.runtime_dispatch_symbol
           << "\",\n";
  manifest << "  \"runtime_support_library_link_wiring_runtime_dispatch_symbol\":\""
           << runtime_support_library_link_wiring.runtime_dispatch_symbol
           << "\",\n";
}

void WriteObjc3FrontendOwnershipAndBlockReplayManifestEntries(
    std::ostream &manifest,
    const std::string &ownership_qualifier_lowering_replay_key,
    const Objc3OwnershipQualifierLoweringContract
        &ownership_qualifier_lowering_contract,
    const std::string &retain_release_operation_lowering_replay_key,
    const Objc3RetainReleaseOperationLoweringContract
        &retain_release_operation_lowering_contract,
    const std::string &autoreleasepool_scope_lowering_replay_key,
    const Objc3AutoreleasePoolScopeLoweringContract
        &autoreleasepool_scope_lowering_contract,
    const std::string &weak_unowned_semantics_lowering_replay_key,
    const Objc3WeakUnownedSemanticsLoweringContract
        &weak_unowned_semantics_lowering_contract,
    const std::string &arc_diagnostics_fixit_lowering_replay_key,
    const Objc3ArcDiagnosticsFixitLoweringContract
        &arc_diagnostics_fixit_lowering_contract,
    const std::string &block_literal_capture_lowering_replay_key,
    const Objc3BlockLiteralCaptureLoweringContract
        &block_literal_capture_lowering_contract,
    const std::string &block_abi_invoke_trampoline_lowering_replay_key,
    const Objc3BlockAbiInvokeTrampolineLoweringContract
        &block_abi_invoke_trampoline_lowering_contract,
    const std::string &block_storage_escape_lowering_replay_key,
    const Objc3BlockStorageEscapeLoweringContract
        &block_storage_escape_lowering_contract,
    const std::string &block_copy_dispose_lowering_replay_key,
    const Objc3BlockCopyDisposeLoweringContract
        &block_copy_dispose_lowering_contract,
    const std::string &block_determinism_perf_baseline_lowering_replay_key,
    const Objc3BlockDeterminismPerfBaselineLoweringContract
        &block_determinism_perf_baseline_lowering_contract) {
  WriteLoweringReplayManifestEntries(
      manifest,
      {{"lowering_ownership_qualifier", ownership_qualifier_lowering_replay_key,
        kObjc3OwnershipQualifierLoweringLaneContract,
        ownership_qualifier_lowering_contract.deterministic},
       {"lowering_retain_release_operation",
        retain_release_operation_lowering_replay_key,
        kObjc3RetainReleaseOperationLoweringLaneContract,
        retain_release_operation_lowering_contract.deterministic},
       {"lowering_autoreleasepool_scope",
        autoreleasepool_scope_lowering_replay_key,
        kObjc3AutoreleasePoolScopeLoweringLaneContract,
        autoreleasepool_scope_lowering_contract.deterministic},
       {"lowering_weak_unowned_semantics",
        weak_unowned_semantics_lowering_replay_key,
        kObjc3WeakUnownedSemanticsLoweringLaneContract,
        weak_unowned_semantics_lowering_contract.deterministic},
       {"lowering_arc_diagnostics_fixit",
        arc_diagnostics_fixit_lowering_replay_key,
        kObjc3ArcDiagnosticsFixitLoweringLaneContract,
        arc_diagnostics_fixit_lowering_contract.deterministic},
       {"lowering_block_literal_capture",
        block_literal_capture_lowering_replay_key,
        kObjc3BlockLiteralCaptureLoweringLaneContract,
        block_literal_capture_lowering_contract.deterministic},
       {"lowering_block_abi_invoke_trampoline",
        block_abi_invoke_trampoline_lowering_replay_key,
        kObjc3BlockAbiInvokeTrampolineLoweringLaneContract,
        block_abi_invoke_trampoline_lowering_contract.deterministic},
       {"lowering_block_storage_escape", block_storage_escape_lowering_replay_key,
        kObjc3BlockStorageEscapeLoweringLaneContract,
        block_storage_escape_lowering_contract.deterministic},
       {"lowering_block_copy_dispose", block_copy_dispose_lowering_replay_key,
        kObjc3BlockCopyDisposeLoweringLaneContract,
        block_copy_dispose_lowering_contract.deterministic},
       {"lowering_block_determinism_perf_baseline",
        block_determinism_perf_baseline_lowering_replay_key,
        kObjc3BlockDeterminismPerfBaselineLoweringLaneContract,
        block_determinism_perf_baseline_lowering_contract.deterministic}});
}

void WriteObjc3FrontendTypeAndModuleReplayManifestEntries(
    std::ostream &manifest,
    const std::string &lightweight_generic_constraint_lowering_replay_key,
    const Objc3LightweightGenericConstraintLoweringContract
        &lightweight_generic_constraint_lowering_contract,
    const std::string &nullability_flow_warning_precision_lowering_replay_key,
    const Objc3NullabilityFlowWarningPrecisionLoweringContract
        &nullability_flow_warning_precision_lowering_contract,
    const std::string &protocol_qualified_object_type_lowering_replay_key,
    const Objc3ProtocolQualifiedObjectTypeLoweringContract
        &protocol_qualified_object_type_lowering_contract,
    const std::string &variance_bridge_cast_lowering_replay_key,
    const Objc3VarianceBridgeCastLoweringContract
        &variance_bridge_cast_lowering_contract,
    const std::string &generic_metadata_abi_lowering_replay_key,
    const Objc3GenericMetadataAbiLoweringContract
        &generic_metadata_abi_lowering_contract,
    const std::string &module_import_graph_lowering_replay_key,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const std::string &namespace_collision_shadowing_lowering_replay_key,
    const Objc3NamespaceCollisionShadowingLoweringContract
        &namespace_collision_shadowing_lowering_contract,
    const std::string &public_private_api_partition_lowering_replay_key,
    const Objc3PublicPrivateApiPartitionLoweringContract
        &public_private_api_partition_lowering_contract,
    const std::string &incremental_module_cache_invalidation_lowering_replay_key,
    const Objc3IncrementalModuleCacheInvalidationLoweringContract
        &incremental_module_cache_invalidation_lowering_contract,
    const std::string &cross_module_conformance_lowering_replay_key,
    const Objc3CrossModuleConformanceLoweringContract
        &cross_module_conformance_lowering_contract) {
  WriteLoweringReplayManifestEntries(
      manifest,
      {{"lowering_lightweight_generic_constraint",
        lightweight_generic_constraint_lowering_replay_key,
        kObjc3LightweightGenericsConstraintLoweringLaneContract,
        lightweight_generic_constraint_lowering_contract.deterministic},
       {"lowering_nullability_flow_warning_precision",
        nullability_flow_warning_precision_lowering_replay_key,
        kObjc3NullabilityFlowWarningPrecisionLoweringLaneContract,
        nullability_flow_warning_precision_lowering_contract.deterministic},
       {"lowering_protocol_qualified_object_type",
        protocol_qualified_object_type_lowering_replay_key,
        kObjc3ProtocolQualifiedObjectTypeLoweringLaneContract,
        protocol_qualified_object_type_lowering_contract.deterministic},
       {"lowering_variance_bridge_cast", variance_bridge_cast_lowering_replay_key,
        kObjc3VarianceBridgeCastLoweringLaneContract,
        variance_bridge_cast_lowering_contract.deterministic},
       {"lowering_generic_metadata_abi", generic_metadata_abi_lowering_replay_key,
        kObjc3GenericMetadataAbiLoweringLaneContract,
        generic_metadata_abi_lowering_contract.deterministic},
       {"lowering_module_import_graph", module_import_graph_lowering_replay_key,
        kObjc3ModuleImportGraphLoweringLaneContract,
        module_import_graph_lowering_contract.deterministic},
       {"lowering_namespace_collision_shadowing",
        namespace_collision_shadowing_lowering_replay_key,
        kObjc3NamespaceCollisionShadowingLoweringLaneContract,
        namespace_collision_shadowing_lowering_contract.deterministic},
       {"lowering_public_private_api_partition",
        public_private_api_partition_lowering_replay_key,
        kObjc3PublicPrivateApiPartitionLoweringLaneContract,
        public_private_api_partition_lowering_contract.deterministic},
       {"lowering_incremental_module_cache_invalidation",
        incremental_module_cache_invalidation_lowering_replay_key,
        kObjc3IncrementalModuleCacheInvalidationLoweringLaneContract,
        incremental_module_cache_invalidation_lowering_contract.deterministic},
       {"lowering_cross_module_conformance",
        cross_module_conformance_lowering_replay_key,
        kObjc3CrossModuleConformanceLoweringLaneContract,
        cross_module_conformance_lowering_contract.deterministic}});
}

void WriteObjc3FrontendErrorReplayManifestEntries(
    std::ostream &manifest,
    const std::string &throws_propagation_lowering_replay_key,
    const Objc3ThrowsPropagationLoweringContract
        &throws_propagation_lowering_contract,
    const std::string &result_like_lowering_replay_key,
    const Objc3ResultLikeLoweringContract &result_like_lowering_contract,
    const std::string &ns_error_bridging_lowering_replay_key,
    const Objc3NSErrorBridgingLoweringContract
        &ns_error_bridging_lowering_contract,
    const std::string &unwind_cleanup_lowering_replay_key,
    const Objc3UnwindCleanupLoweringContract &unwind_cleanup_lowering_contract,
    const std::string &error_handling_throws_abi_propagation_lowering_replay_key,
    bool deterministic_error_handling_throws_abi_propagation_lowering,
    const Objc3ErrorHandlingResultAndBridgingArtifactReplaySummary
        &error_handling_result_and_bridging_artifact_replay_summary) {
  WriteLoweringReplayManifestEntries(
      manifest,
      {{"lowering_throws_propagation", throws_propagation_lowering_replay_key,
        kObjc3ThrowsPropagationLoweringLaneContract,
        throws_propagation_lowering_contract.deterministic},
       {"lowering_result_like", result_like_lowering_replay_key,
        kObjc3ResultLikeLoweringLaneContract,
        result_like_lowering_contract.deterministic},
       {"lowering_ns_error_bridging", ns_error_bridging_lowering_replay_key,
        kObjc3NSErrorBridgingLoweringLaneContract,
        ns_error_bridging_lowering_contract.deterministic},
       {"lowering_unwind_cleanup", unwind_cleanup_lowering_replay_key,
        kObjc3UnwindCleanupLoweringLaneContract,
        unwind_cleanup_lowering_contract.deterministic},
       {"lowering_error_handling_throws_abi_propagation",
        error_handling_throws_abi_propagation_lowering_replay_key,
        kObjc3ErrorHandlingThrowsAbiPropagationLoweringContractId,
        deterministic_error_handling_throws_abi_propagation_lowering,
        "contract_id"},
       {"lowering_error_handling_result_and_bridging_artifact_replay",
        error_handling_result_and_bridging_artifact_replay_summary.replay_key,
        kObjc3ErrorHandlingResultAndBridgingArtifactReplayContractId,
        error_handling_result_and_bridging_artifact_replay_summary.deterministic,
        "contract_id"}});
}

}  // namespace objc3::artifacts::frontend
