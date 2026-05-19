std::string
RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildReplayKey(
    const Objc3RuntimeAwareImportModuleFrontendClosureSummary &summary) {
  std::ostringstream out;
  out << summary.contract_id
      << ";source_surface_contract_id=" << summary.source_surface_contract_id
      << ";frontend_surface_path=" << summary.frontend_surface_path
      << ";payload_model=" << summary.payload_model
      << ";artifact_relative_path=" << summary.artifact_relative_path
      << ";authority_model=" << summary.authority_model
      << ";payload_ownership_model=" << summary.payload_ownership_model
      << ";module_name=" << summary.module_name
      << ";protocol_decl_count=" << summary.protocol_decl_count
      << ";interface_decl_count=" << summary.interface_decl_count
      << ";implementation_decl_count=" << summary.implementation_decl_count
      << ";interface_category_decl_count="
      << summary.interface_category_decl_count
      << ";implementation_category_decl_count="
      << summary.implementation_category_decl_count
      << ";function_decl_count=" << summary.function_decl_count
      << ";module_import_graph_sites=" << summary.module_import_graph_sites
      << ";import_edge_candidate_sites="
      << summary.import_edge_candidate_sites
      << ";namespace_segment_sites=" << summary.namespace_segment_sites
      << ";object_pointer_type_sites=" << summary.object_pointer_type_sites
      << ";pointer_declarator_sites=" << summary.pointer_declarator_sites
      << ";normalized_sites=" << summary.normalized_sites
      << ";contract_violation_sites=" << summary.contract_violation_sites
      << ";class_record_count=" << summary.class_record_count
      << ";protocol_record_count=" << summary.protocol_record_count
      << ";category_record_count=" << summary.category_record_count
      << ";property_record_count=" << summary.property_record_count
      << ";method_record_count=" << summary.method_record_count
      << ";ivar_record_count=" << summary.ivar_record_count
      << ";runtime_owned_declaration_count="
      << summary.runtime_owned_declaration_count
      << ";superclass_reference_count=" << summary.superclass_reference_count
      << ";protocol_reference_count=" << summary.protocol_reference_count
      << ";property_accessor_reference_count="
      << summary.property_accessor_reference_count
      << ";property_ivar_binding_reference_count="
      << summary.property_ivar_binding_reference_count
      << ";method_selector_reference_count="
      << summary.method_selector_reference_count
      << ";metadata_reference_count=" << summary.metadata_reference_count
      << ";source_surface_replay_key=" << summary.source_surface_replay_key;
  return out.str();
}
