#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <sstream>
#include <string>
#include <vector>

#include "io/objc3_json.h"
#include "sema/model/semantic_type.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;

std::string BuildStringArrayJson(const std::vector<std::string> &values) {
  std::ostringstream out;
  out << "[";
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0u) {
      out << ",";
    }
    out << "\"" << EscapeJsonString(values[i]) << "\"";
  }
  out << "]";
  return out.str();
}

}  // namespace

std::string RenderSerializedRuntimeMetadataReusePayloadJson(
    const Objc3SerializedRuntimeMetadataArtifactReuseSummary &summary,
    const std::string &module_name,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  std::ostringstream out;
  out << "{\n"
      << "    \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "    \"module_name\": \"" << EscapeJsonString(module_name) << "\",\n"
      << "    \"reused_module_names_lexicographic\": "
      << BuildStringArrayJson(summary.reused_module_names_lexicographic)
      << ",\n"
      << "    \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "    \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "    \"runtime_owned_declarations\": "
      << RenderRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"metadata_references\": "
      << RenderRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "    \"ready\": "
      << (IsReadyObjc3SerializedRuntimeMetadataArtifactReuseSummary(summary)
              ? "true"
              : "false")
      << ",\n"
      << "    \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "  }";
  return out.str();
}

std::string RenderRuntimeAwareImportModuleArtifactJson(
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
        &serialized_runtime_metadata_reuse_records) {
  std::ostringstream out;
  out << "{\n"
      << "  \"contract_id\": \"" << EscapeJsonString(summary.contract_id)
      << "\",\n"
      << "  \"source_surface_contract_id\": \""
      << EscapeJsonString(summary.source_surface_contract_id) << "\",\n"
      << "  \"frontend_surface_path\": \""
      << EscapeJsonString(summary.frontend_surface_path) << "\",\n"
      << "  \"artifact\": \"" << EscapeJsonString(summary.artifact_relative_path)
      << "\",\n"
      << "  \"payload_model\": \"" << EscapeJsonString(summary.payload_model)
      << "\",\n"
      << "  \"authority_model\": \"" << EscapeJsonString(summary.authority_model)
      << "\",\n"
      << "  \"payload_ownership_model\": \""
      << EscapeJsonString(summary.payload_ownership_model) << "\",\n"
      << "  \"module_name\": \"" << EscapeJsonString(summary.module_name)
      << "\",\n"
      << "  \"protocol_decl_count\": " << summary.protocol_decl_count << ",\n"
      << "  \"interface_decl_count\": " << summary.interface_decl_count
      << ",\n"
      << "  \"implementation_decl_count\": " << summary.implementation_decl_count
      << ",\n"
      << "  \"interface_category_decl_count\": "
      << summary.interface_category_decl_count << ",\n"
      << "  \"implementation_category_decl_count\": "
      << summary.implementation_category_decl_count << ",\n"
      << "  \"function_decl_count\": " << summary.function_decl_count << ",\n"
      << "  \"module_import_graph_sites\": " << summary.module_import_graph_sites
      << ",\n"
      << "  \"import_edge_candidate_sites\": "
      << summary.import_edge_candidate_sites << ",\n"
      << "  \"namespace_segment_sites\": " << summary.namespace_segment_sites
      << ",\n"
      << "  \"object_pointer_type_sites\": " << summary.object_pointer_type_sites
      << ",\n"
      << "  \"pointer_declarator_sites\": " << summary.pointer_declarator_sites
      << ",\n"
      << "  \"normalized_sites\": " << summary.normalized_sites << ",\n"
      << "  \"contract_violation_sites\": " << summary.contract_violation_sites
      << ",\n"
      << "  \"runtime_owned_declaration_count\": "
      << summary.runtime_owned_declaration_count << ",\n"
      << "  \"metadata_reference_count\": " << summary.metadata_reference_count
      << ",\n"
      << "  \"runtime_aware_import_declarations_landed\": "
      << (summary.runtime_aware_import_declarations_landed ? "true" : "false")
      << ",\n"
      << "  \"module_metadata_import_surface_landed\": "
      << (summary.module_metadata_import_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declaration_import_landed\": "
      << (summary.runtime_owned_declaration_import_landed ? "true" : "false")
      << ",\n"
      << "  \"runtime_metadata_reference_import_landed\": "
      << (summary.runtime_metadata_reference_import_landed ? "true" : "false")
      << ",\n"
      << "  \"public_frontend_api_module_surface_landed\": "
      << (summary.public_frontend_api_module_surface_landed ? "true" : "false")
      << ",\n"
      << "  \"ready_for_import_artifact_emission\": "
      << (summary.ready_for_import_artifact_emission ? "true" : "false")
      << ",\n"
      << "  \"ready_for_frontend_module_consumption\": "
      << (summary.ready_for_frontend_module_consumption ? "true" : "false")
      << ",\n"
      << "  \"runtime_owned_declarations\": "
      << RenderRuntimeOwnedDeclarationsJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"metadata_references\": "
      << RenderRuntimeMetadataReferencesJson(runtime_metadata_source_records)
      << ",\n"
      << "  \"objc_type_system_optional_keypath_lowering_contract\": "
      << type_system_optional_keypath_lowering_contract_json << ",\n"
      << "  \"objc_type_system_optional_keypath_runtime_helper_contract\": "
      << type_system_optional_keypath_runtime_helper_contract_json << ",\n"
      << "  \"objc_type_system_generic_contract_preservation\": "
      << type_system_generic_contract_preservation_json << ",\n"
      << "  \"objc_type_system_nullability_contract_preservation\": "
      << type_system_nullability_contract_preservation_json << ",\n"
      << "  \"objc_type_system_protocol_contract_preservation\": "
      << type_system_protocol_contract_preservation_json << ",\n"
      << "  \"objc_error_handling_result_and_bridging_artifact_replay\": "
      << error_handling_result_and_bridging_artifact_replay_json << ",\n"
      << "  \"objc_concurrency_actor_mailbox_and_isolation_runtime_import_surface\": "
      << concurrency_actor_mailbox_runtime_import_json << ",\n"
      << "  \"objc_interop_foreign_surface_interface_and_module_preservation\": "
      << interop_foreign_surface_interface_preservation_json << ",\n"
      << "  \"objc_interop_header_module_and_bridge_generation\": "
      << interop_header_module_bridge_generation_json << ",\n"
      << "  \"objc_interop_ffi_metadata_and_interface_preservation\": "
      << interop_ffi_metadata_interface_preservation_json << ",\n"
      << "  \"objc_metaprogramming_module_interface_and_replay_preservation\": "
      << metaprogramming_module_interface_replay_preservation_json << ",\n"
      << "  \"objc_metaprogramming_macro_host_process_and_cache_runtime_integration\": "
      << metaprogramming_macro_host_process_cache_runtime_integration_json
      << ",\n"
      << "  \"objc_dispatch_dispatch_metadata_and_interface_preservation\": "
      << dispatch_dispatch_metadata_interface_preservation_json << ",\n"
      << "  \"objc_runtime_block_ownership_artifact_preservation\": "
      << runtime_block_ownership_artifact_preservation_json << ",\n"
      << "  \"objc_runtime_storage_reflection_artifact_preservation\": "
      << runtime_storage_reflection_artifact_preservation_json << ",\n"
      << "  \""
      << kObjc3SerializedRuntimeMetadataArtifactReusePayloadMemberName
      << "\": "
      << RenderSerializedRuntimeMetadataReusePayloadJson(
             serialized_runtime_metadata_artifact_reuse, summary.module_name,
             serialized_runtime_metadata_reuse_records)
      << ",\n"
      << "  \"source_surface_replay_key\": \""
      << EscapeJsonString(summary.source_surface_replay_key) << "\",\n"
      << "  \"replay_key\": \"" << EscapeJsonString(summary.replay_key)
      << "\"\n"
      << "}\n";
  return out.str();
}

}  // namespace objc3::artifacts::frontend
