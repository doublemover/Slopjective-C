#include "io/objc3_runtime_registration_manifest_document_header.h"

#include <ostream>

#include "io/objc3_process_internal.h"
#include "runtime/metadata/runtime_ownership_contracts.h"

void EmitObjc3RuntimeRegistrationManifestHeaderJson(
    std::ostream &out,
    const Objc3RuntimeTranslationUnitRegistrationManifestArtifactInputs &inputs,
    const Objc3RuntimeRegistrationSymbolOwnerRecord &symbol_owner_record,
    std::size_t runtime_metadata_binary_byte_count) {
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(inputs.contract_id)
      << "\",\n"
      << "  \"owner_split_contract_id\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimeOwnerSplitContractId)
      << "\",\n"
      << "  \"metadata_model_owner\": \""
      << EscapeJsonString(objc3c::runtime::kObjc3RuntimeMetadataModelOwner)
      << "\",\n"
      << "  \"registration_table_owner\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimeRegistrationTableOwner)
      << "\",\n"
      << "  \"manifest_descriptor_artifact_owner\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimeManifestDescriptorArtifactOwner)
      << "\",\n"
      << "  \"bootstrap_replay_owner\": \""
      << EscapeJsonString(objc3c::runtime::kObjc3RuntimeBootstrapReplayOwner)
      << "\",\n"
      << "  \"dispatch_frame_state_owner\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimeDispatchFrameStateOwner)
      << "\",\n"
      << "  \"public_registration_api_owner\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimePublicRegistrationApiOwner)
      << "\",\n"
      << "  \"public_dispatch_diagnostics_owner\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimePublicDispatchDiagnosticsOwner)
      << "\",\n"
      << "  \"fail_closed_ownership_model\": \""
      << EscapeJsonString(
             objc3c::runtime::kObjc3RuntimeFailClosedOwnershipModel)
      << "\",\n"
      << "  \"launch_integration_contract_id\": \""
      << EscapeJsonString(inputs.launch_integration_contract_id) << "\",\n"
      << "  \"translation_unit_registration_contract_id\": \""
      << EscapeJsonString(inputs.translation_unit_registration_contract_id)
      << "\",\n"
      << "  \"runtime_support_library_link_wiring_contract_id\": \""
      << EscapeJsonString(
             inputs.runtime_support_library_link_wiring_contract_id)
      << "\",\n"
      << "  \"manifest_payload_model\": \""
      << EscapeJsonString(inputs.manifest_payload_model) << "\",\n"
      << "  \"manifest_artifact\": \""
      << EscapeJsonString(inputs.manifest_artifact_relative_path) << "\",\n"
      << "  \"object_artifact\": \""
      << EscapeJsonString(inputs.object_artifact_relative_path) << "\",\n"
      << "  \"backend_artifact\": \""
      << EscapeJsonString(inputs.backend_artifact_relative_path) << "\",\n"
      << "  \"runtime_owned_payload_artifacts\": [\n"
      << "    \""
      << EscapeJsonString(inputs.runtime_owned_payload_artifacts[0])
      << "\",\n"
      << "    \""
      << EscapeJsonString(inputs.runtime_owned_payload_artifacts[1])
      << "\",\n"
      << "    \""
      << EscapeJsonString(inputs.runtime_owned_payload_artifacts[2])
      << "\"\n"
      << "  ],\n"
      << "  \"runtime_metadata_binary_byte_count\": "
      << runtime_metadata_binary_byte_count << ",\n"
      << "  \"runtime_support_library_archive_relative_path\": \""
      << EscapeJsonString(inputs.runtime_support_library_archive_relative_path)
      << "\",\n"
      << "  \"registration_entrypoint_symbol\": \""
      << EscapeJsonString(inputs.registration_entrypoint_symbol) << "\",\n"
      << "  \"constructor_root_symbol\": \""
      << EscapeJsonString(symbol_owner_record.constructor_root_symbol) << "\",\n"
      << "  \"constructor_root_ownership_model\": \""
      << EscapeJsonString(inputs.constructor_root_ownership_model) << "\",\n"
      << "  \"manifest_authority_model\": \""
      << EscapeJsonString(inputs.manifest_authority_model) << "\",\n"
      << "  \"constructor_init_stub_symbol\": \""
      << EscapeJsonString(symbol_owner_record.constructor_init_stub_symbol)
      << "\",\n"
      << "  \"constructor_init_stub_ownership_model\": \""
      << EscapeJsonString(inputs.constructor_init_stub_ownership_model)
      << "\",\n"
      << "  \"constructor_priority_policy\": \""
      << EscapeJsonString(inputs.constructor_priority_policy) << "\",\n"
      << "  \"translation_unit_identity_model\": \""
      << EscapeJsonString(symbol_owner_record.translation_unit_identity_model)
      << "\",\n"
      << "  \"runtime_library_resolution_model\": \""
      << EscapeJsonString(inputs.runtime_library_resolution_model) << "\",\n"
      << "  \"cleanup_unwind_runtime_link_model\": \""
      << "linker-response-plus-runtime-support-archive-sidecars-provide-runnable-cleanup-executable-link-inputs\",\n"
      << "  \"driver_linker_flag_consumption_model\": \""
      << EscapeJsonString(inputs.driver_linker_flag_consumption_model)
      << "\",\n"
      << "  \"compile_wrapper_command_surface\": \""
      << EscapeJsonString(inputs.compile_wrapper_command_surface)
      << "\",\n"
      << "  \"compile_proof_command_surface\": \""
      << EscapeJsonString(inputs.compile_proof_command_surface) << "\",\n"
      << "  \"execution_smoke_command_surface\": \""
      << EscapeJsonString(inputs.execution_smoke_command_surface)
      << "\",\n";
}
