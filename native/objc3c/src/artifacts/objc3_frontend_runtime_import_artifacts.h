#pragma once

#include <string>

#include "pipeline/objc3_frontend_types.h"

namespace objc3::artifacts::frontend {

[[nodiscard]] std::string BuildRuntimeAwareImportModuleSurfaceReplayKey(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract);

[[nodiscard]] std::string BuildRuntimeAwareImportModuleSurfaceSummaryJson(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract);

[[nodiscard]] std::string BuildRuntimeAwareImportModuleFrontendClosureReplayKey(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary);

[[nodiscard]] Objc3RuntimeAwareImportModuleFrontendClosureSummary
BuildRuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

[[nodiscard]] std::string BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary);

[[nodiscard]] std::string
BuildCrossModuleRuntimeMetadataSemanticPreservationReplayKey(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary);

[[nodiscard]] Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
BuildCrossModuleRuntimeMetadataSemanticPreservationSummary(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary
        &source_frontend_closure,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

[[nodiscard]] std::string
BuildCrossModuleRuntimeMetadataSemanticPreservationSummaryJson(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary &summary);

[[nodiscard]] std::string BuildImportedRuntimeMetadataSemanticRulesReplayKey(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary &summary);

[[nodiscard]] Objc3ImportedRuntimeMetadataSemanticRulesSummary
BuildImportedRuntimeMetadataSemanticRulesSummary(
    const Objc3CrossModuleRuntimeMetadataSemanticPreservationSummary
        &source_semantic_preservation,
    const std::vector<Objc3ImportedRuntimeModuleSurface> &imported_surfaces,
    std::size_t imported_input_path_count);

[[nodiscard]] std::string BuildSerializedRuntimeMetadataImportLoweringReplayKey(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary);

[[nodiscard]] Objc3SerializedRuntimeMetadataImportLoweringSummary
BuildSerializedRuntimeMetadataImportLoweringSummary(
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules);

[[nodiscard]] std::string BuildSerializedRuntimeMetadataImportLoweringSummaryJson(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary &summary);

[[nodiscard]] std::string BuildSerializedRuntimeMetadataArtifactReuseReplayKey(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary);

[[nodiscard]] Objc3SerializedRuntimeMetadataArtifactReuseSummary
BuildSerializedRuntimeMetadataArtifactReuseSummary(
    const Objc3SerializedRuntimeMetadataImportLoweringSummary
        &serialized_import_lowering,
    const std::string &local_module_name,
    const Objc3RuntimeMetadataSourceRecordSet &reused_runtime_metadata_source_records,
    const std::vector<std::string> &reused_module_names_lexicographic);

[[nodiscard]] std::string BuildSerializedRuntimeMetadataArtifactReuseSummaryJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary);

[[nodiscard]] std::string BuildCrossModuleBuildRuntimeOrchestrationReplayKey(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary);

[[nodiscard]] Objc3CrossModuleBuildRuntimeOrchestrationSummary
BuildCrossModuleBuildRuntimeOrchestrationSummary(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3ImportedRuntimeMetadataSemanticRulesSummary
        &imported_runtime_metadata_semantic_rules,
    const Objc3RuntimeTranslationUnitRegistrationManifestSummary
        &local_runtime_registration_manifest,
    std::size_t direct_import_input_count);

[[nodiscard]] std::string BuildCrossModuleBuildRuntimeOrchestrationSummaryJson(
    const Objc3CrossModuleBuildRuntimeOrchestrationSummary &summary);

[[nodiscard]] std::string RenderRuntimeOwnedDeclarationsJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

[[nodiscard]] std::string RenderRuntimeMetadataReferencesJson(
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

[[nodiscard]] std::string RenderSerializedRuntimeMetadataReusePayloadJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary,
    const std::string &module_name,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records);

[[nodiscard]] std::string RenderRuntimeAwareImportModuleArtifactJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records,
    const std::string &type_system_optional_keypath_lowering_contract_json,
    const std::string &type_system_optional_keypath_runtime_helper_contract_json,
    const std::string &type_system_generic_contract_preservation_json,
    const std::string &type_system_nullability_contract_preservation_json,
    const std::string &type_system_protocol_contract_preservation_json,
    const std::string &error_handling_result_and_bridging_artifact_replay_json,
    const std::string &concurrency_actor_mailbox_runtime_import_json,
    const std::string &interop_foreign_surface_interface_preservation_json,
    const std::string &interop_header_module_bridge_generation_json,
    const std::string &interop_ffi_metadata_interface_preservation_json,
    const std::string &metaprogramming_module_interface_replay_preservation_json,
    const std::string
        &metaprogramming_macro_host_process_cache_runtime_integration_json,
    const std::string &dispatch_dispatch_metadata_interface_preservation_json,
    const std::string &runtime_block_ownership_artifact_preservation_json,
    const std::string &runtime_storage_reflection_artifact_preservation_json,
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary
        &serialized_runtime_metadata_artifact_reuse,
    const Objc3RuntimeMetadataSourceRecordSet
        &serialized_runtime_metadata_reuse_records);

}  // namespace objc3::artifacts::frontend
