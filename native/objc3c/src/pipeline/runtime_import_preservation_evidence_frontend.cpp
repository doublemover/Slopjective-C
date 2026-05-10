#include "pipeline/runtime_import_preservation_owners.h"

namespace objc3c::pipeline::runtime_import_preservation {

bool PopulateFrontendClosureSummary(
    const RuntimeImportJsonValue::Object &root,
    Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary,
    std::string &error) {
  summary = Objc3RuntimeAwareImportModuleFrontendClosureSummary{};
  if (!ReadStringMember(root, "contract_id", summary.contract_id, error) ||
      !ReadStringMember(root, "source_surface_contract_id",
                        summary.source_surface_contract_id, error) ||
      !ReadStringMember(root, "frontend_surface_path",
                        summary.frontend_surface_path, error) ||
      !ReadStringMember(root, "payload_model", summary.payload_model, error) ||
      !ReadStringMember(root, "artifact", summary.artifact_relative_path,
                        error) ||
      !ReadStringMember(root, "authority_model", summary.authority_model,
                        error) ||
      !ReadStringMember(root, "payload_ownership_model",
                        summary.payload_ownership_model, error) ||
      !ReadStringMember(root, "module_name", summary.module_name, error) ||
      !ReadSizeMember(root, "protocol_decl_count",
                      summary.protocol_decl_count, error) ||
      !ReadSizeMember(root, "interface_decl_count",
                      summary.interface_decl_count, error) ||
      !ReadSizeMember(root, "implementation_decl_count",
                      summary.implementation_decl_count, error) ||
      !ReadSizeMember(root, "interface_category_decl_count",
                      summary.interface_category_decl_count, error) ||
      !ReadSizeMember(root, "implementation_category_decl_count",
                      summary.implementation_category_decl_count, error) ||
      !ReadSizeMember(root, "function_decl_count",
                      summary.function_decl_count, error) ||
      !ReadSizeMember(root, "module_import_graph_sites",
                      summary.module_import_graph_sites, error) ||
      !ReadSizeMember(root, "import_edge_candidate_sites",
                      summary.import_edge_candidate_sites, error) ||
      !ReadSizeMember(root, "namespace_segment_sites",
                      summary.namespace_segment_sites, error) ||
      !ReadSizeMember(root, "object_pointer_type_sites",
                      summary.object_pointer_type_sites, error) ||
      !ReadSizeMember(root, "pointer_declarator_sites",
                      summary.pointer_declarator_sites, error) ||
      !ReadSizeMember(root, "normalized_sites", summary.normalized_sites,
                      error) ||
      !ReadSizeMember(root, "contract_violation_sites",
                      summary.contract_violation_sites, error) ||
      !ReadSizeMember(root, "runtime_owned_declaration_count",
                      summary.runtime_owned_declaration_count, error) ||
      !ReadSizeMember(root, "metadata_reference_count",
                      summary.metadata_reference_count, error) ||
      !ReadBoolMember(root, "runtime_aware_import_declarations_landed",
                      summary.runtime_aware_import_declarations_landed,
                      error) ||
      !ReadBoolMember(root, "module_metadata_import_surface_landed",
                      summary.module_metadata_import_surface_landed, error) ||
      !ReadBoolMember(root, "runtime_owned_declaration_import_landed",
                      summary.runtime_owned_declaration_import_landed,
                      error) ||
      !ReadBoolMember(root, "runtime_metadata_reference_import_landed",
                      summary.runtime_metadata_reference_import_landed,
                      error) ||
      !ReadBoolMember(root, "public_frontend_api_module_surface_landed",
                      summary.public_frontend_api_module_surface_landed,
                      error) ||
      !ReadBoolMember(root, "ready_for_import_artifact_emission",
                      summary.ready_for_import_artifact_emission, error) ||
      !ReadBoolMember(root, "ready_for_frontend_module_consumption",
                      summary.ready_for_frontend_module_consumption, error) ||
      !ReadStringMember(root, "source_surface_replay_key",
                        summary.source_surface_replay_key, error) ||
      !ReadStringMember(root, "replay_key", summary.replay_key, error)) {
    return false;
  }
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      !summary.source_surface_replay_key.empty();
  summary.runtime_metadata_source_records_ready = false;
  summary.frontend_surface_published = true;
  summary.import_artifact_template_published = true;
  summary.failure_reason.clear();
  return true;
}

}  // namespace objc3c::pipeline::runtime_import_preservation
