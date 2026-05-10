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

}  // namespace objc3::artifacts::frontend
