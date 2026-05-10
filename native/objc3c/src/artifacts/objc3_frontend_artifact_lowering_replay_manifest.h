#pragma once

#include <initializer_list>
#include <iosfwd>
#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

struct LoweringReplayManifestEntry {
  std::string manifest_key;
  std::string replay_key;
  std::string contract_id;
  bool deterministic = false;
  std::string contract_field_name = "lane_contract";
};

void WriteLoweringReplayManifestEntries(
    std::ostream &manifest,
    std::initializer_list<LoweringReplayManifestEntry> entries);

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
        &runtime_support_library_link_wiring);

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
        &block_determinism_perf_baseline_lowering_contract);

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
        &cross_module_conformance_lowering_contract);

}  // namespace objc3::artifacts::frontend
