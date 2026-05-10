#include "artifacts/objc3_frontend_runtime_import_artifacts.h"

#include <algorithm>
#include <set>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "io/json/json_writer.h"
#include "io/objc3_json.h"
#include "pipeline/results/evidence_record.h"
#include "sema/model/semantic_type.h"
#include "artifacts/objc3_runtime_import_preservation_artifact_builders.h"
#include "support/objc3_runtime_metadata_record_set.h"

namespace objc3::artifacts::frontend {
namespace {

using objc3::io::EscapeJsonString;
using objc3::io::json::JsonObjectWriter;
using objc3c::support::CountRuntimeMetadataSourceRecordSetDeclarations;
using objc3c::support::CountRuntimeMetadataSourceRecordSetReferences;

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

std::string BuildRuntimeAwareImportModuleSurfaceReplayKey(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildReplayKey(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
}

std::string BuildRuntimeAwareImportModuleSurfaceSummaryJson(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildSummaryJson(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
}

std::string BuildRuntimeAwareImportModuleFrontendClosureReplayKey(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildReplayKey(
          summary);
}

Objc3RuntimeAwareImportModuleFrontendClosureSummary
BuildRuntimeAwareImportModuleFrontendClosureSummary(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  return runtime_import_preservation::
      RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildSummary(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract,
          runtime_metadata_source_records);
}

std::string BuildRuntimeAwareImportModuleFrontendClosureSummaryJson(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  JsonObjectWriter object(out);
  object.StringField("contract_id", summary.contract_id);
  object.StringField("source_surface_contract_id",
                     summary.source_surface_contract_id);
  object.StringField("frontend_surface_path", summary.frontend_surface_path);
  object.StringField("payload_model", summary.payload_model);
  object.StringField("artifact_relative_path", summary.artifact_relative_path);
  object.StringField("authority_model", summary.authority_model);
  object.StringField("payload_ownership_model",
                     summary.payload_ownership_model);
  object.StringField("module_name", summary.module_name);
  object.SizeField("protocol_decl_count", summary.protocol_decl_count);
  object.SizeField("interface_decl_count", summary.interface_decl_count);
  object.SizeField("implementation_decl_count",
                   summary.implementation_decl_count);
  object.SizeField("interface_category_decl_count",
                   summary.interface_category_decl_count);
  object.SizeField("implementation_category_decl_count",
                   summary.implementation_category_decl_count);
  object.SizeField("function_decl_count", summary.function_decl_count);
  object.SizeField("module_import_graph_sites",
                   summary.module_import_graph_sites);
  object.SizeField("import_edge_candidate_sites",
                   summary.import_edge_candidate_sites);
  object.SizeField("namespace_segment_sites", summary.namespace_segment_sites);
  object.SizeField("object_pointer_type_sites",
                   summary.object_pointer_type_sites);
  object.SizeField("pointer_declarator_sites",
                   summary.pointer_declarator_sites);
  object.SizeField("normalized_sites", summary.normalized_sites);
  object.SizeField("contract_violation_sites",
                   summary.contract_violation_sites);
  object.SizeField("class_record_count", summary.class_record_count);
  object.SizeField("protocol_record_count", summary.protocol_record_count);
  object.SizeField("category_record_count", summary.category_record_count);
  object.SizeField("property_record_count", summary.property_record_count);
  object.SizeField("method_record_count", summary.method_record_count);
  object.SizeField("ivar_record_count", summary.ivar_record_count);
  object.SizeField("runtime_owned_declaration_count",
                   summary.runtime_owned_declaration_count);
  object.SizeField("superclass_reference_count",
                   summary.superclass_reference_count);
  object.SizeField("protocol_reference_count",
                   summary.protocol_reference_count);
  object.SizeField("property_accessor_reference_count",
                   summary.property_accessor_reference_count);
  object.SizeField("property_ivar_binding_reference_count",
                   summary.property_ivar_binding_reference_count);
  object.SizeField("method_selector_reference_count",
                   summary.method_selector_reference_count);
  object.SizeField("metadata_reference_count", summary.metadata_reference_count);
  object.BoolField("ready",
                   IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(
                       summary));
  object.BoolField("fail_closed", summary.fail_closed);
  object.BoolField("source_surface_contract_ready",
                   summary.source_surface_contract_ready);
  object.BoolField("runtime_metadata_source_records_ready",
                   summary.runtime_metadata_source_records_ready);
  object.BoolField("frontend_surface_published",
                   summary.frontend_surface_published);
  object.BoolField("import_artifact_template_published",
                   summary.import_artifact_template_published);
  object.BoolField("runtime_aware_import_declarations_landed",
                   summary.runtime_aware_import_declarations_landed);
  object.BoolField("module_metadata_import_surface_landed",
                   summary.module_metadata_import_surface_landed);
  object.BoolField("runtime_owned_declaration_import_landed",
                   summary.runtime_owned_declaration_import_landed);
  object.BoolField("runtime_metadata_reference_import_landed",
                   summary.runtime_metadata_reference_import_landed);
  object.BoolField("public_frontend_api_module_surface_landed",
                   summary.public_frontend_api_module_surface_landed);
  object.BoolField("ready_for_import_artifact_emission",
                   summary.ready_for_import_artifact_emission);
  object.BoolField("ready_for_frontend_module_consumption",
                   summary.ready_for_frontend_module_consumption);
  object.StringField("source_surface_replay_key",
                     summary.source_surface_replay_key);
  object.StringField("replay_key", summary.replay_key);
  object.StringField("failure_reason", summary.failure_reason);
  object.End();
  return out.str();
}

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
