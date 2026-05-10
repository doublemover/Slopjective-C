std::string RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildReplayKey(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  std::ostringstream out;
  out << kObjc3RuntimeAwareImportModuleSurfaceContractId
      << ";module_name=" << program.module_name
      << ";protocol_decl_count=" << parser_contract_snapshot.protocol_decl_count
      << ";interface_decl_count="
      << parser_contract_snapshot.interface_decl_count
      << ";implementation_decl_count="
      << parser_contract_snapshot.implementation_decl_count
      << ";interface_category_decl_count="
      << parser_contract_snapshot.interface_category_decl_count
      << ";implementation_category_decl_count="
      << parser_contract_snapshot.implementation_category_decl_count
      << ";function_decl_count=" << parser_contract_snapshot.function_decl_count
      << ";module_import_graph_sites="
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ";import_edge_candidate_sites="
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ";object_pointer_type_sites="
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ";normalized_sites="
      << module_import_graph_lowering_contract.normalized_sites
      << ";runtime_aware_import_declarations_landed=false"
      << ";module_metadata_import_surface_landed=false"
      << ";runtime_owned_declaration_import_landed=false"
      << ";runtime_metadata_reference_import_landed=false"
      << ";public_frontend_api_module_surface_landed=false"
      << ";fail_closed=true"
      << ";deterministic="
      << (module_import_graph_lowering_contract.deterministic ? "true" : "false");
  return out.str();
}

std::string RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildSummaryJson(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract) {
  std::ostringstream out;
  out << "{"
      << "\"contract_id\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceContractId
      << "\",\"surface_path\":\""
      << kObjc3RuntimeAwareImportModuleSurfacePath
      << "\",\"source_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceSourceModel
      << "\",\"non_goal_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceNonGoalModel
      << "\",\"failure_model\":\""
      << kObjc3RuntimeAwareImportModuleSurfaceFailureModel
      << "\",\"module_name\":\"" << EscapeJsonString(program.module_name)
      << "\",\"protocol_decl_count\":"
      << parser_contract_snapshot.protocol_decl_count
      << ",\"interface_decl_count\":"
      << parser_contract_snapshot.interface_decl_count
      << ",\"implementation_decl_count\":"
      << parser_contract_snapshot.implementation_decl_count
      << ",\"interface_category_decl_count\":"
      << parser_contract_snapshot.interface_category_decl_count
      << ",\"implementation_category_decl_count\":"
      << parser_contract_snapshot.implementation_category_decl_count
      << ",\"function_decl_count\":"
      << parser_contract_snapshot.function_decl_count
      << ",\"module_import_graph_sites\":"
      << module_import_graph_lowering_contract.module_import_graph_sites
      << ",\"import_edge_candidate_sites\":"
      << module_import_graph_lowering_contract.import_edge_candidate_sites
      << ",\"namespace_segment_sites\":"
      << module_import_graph_lowering_contract.namespace_segment_sites
      << ",\"object_pointer_type_sites\":"
      << module_import_graph_lowering_contract.object_pointer_type_sites
      << ",\"pointer_declarator_sites\":"
      << module_import_graph_lowering_contract.pointer_declarator_sites
      << ",\"normalized_sites\":"
      << module_import_graph_lowering_contract.normalized_sites
      << ",\"contract_violation_sites\":"
      << module_import_graph_lowering_contract.contract_violation_sites
      << ",\"runtime_aware_import_declarations_landed\":false"
      << ",\"module_metadata_import_surface_landed\":false"
      << ",\"runtime_owned_declaration_import_landed\":false"
      << ",\"runtime_metadata_reference_import_landed\":false"
      << ",\"public_frontend_api_module_surface_landed\":false"
      << ",\"fail_closed\":true"
      << ",\"deterministic\":"
      << (module_import_graph_lowering_contract.deterministic ? "true" : "false")
      << ",\"ready_for_core_feature_implementation\":true"
      << ",\"next_issue\":\"objc3c.importmodule.runtimeaware.corefeature.v1\""
      << ",\"replay_key\":\""
      << EscapeJsonString(BuildReplayKey(program,
                                         parser_contract_snapshot,
                                         module_import_graph_lowering_contract))
      << "\"}";
  return out.str();
}
