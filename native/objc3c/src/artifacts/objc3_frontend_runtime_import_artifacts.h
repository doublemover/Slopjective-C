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
