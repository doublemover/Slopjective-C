Objc3RuntimeAwareImportModuleFrontendClosureSummary
RuntimeAwareImportModuleFrontendClosureArtifactBuilder::BuildSummary(
    const Objc3Program &program,
    const Objc3ParserContractSnapshot &parser_contract_snapshot,
    const Objc3ModuleImportGraphLoweringContract
        &module_import_graph_lowering_contract,
    const Objc3RuntimeMetadataSourceRecordSet &runtime_metadata_source_records) {
  Objc3RuntimeAwareImportModuleFrontendClosureSummary summary;
  summary.fail_closed = true;
  summary.source_surface_contract_ready =
      !program.module_name.empty() &&
      IsValidObjc3ModuleImportGraphLoweringContract(
          module_import_graph_lowering_contract);
  summary.runtime_metadata_source_records_ready =
      IsReadyObjc3RuntimeMetadataSourceRecordSet(runtime_metadata_source_records);
  summary.frontend_surface_published = true;
  summary.import_artifact_template_published = true;
  summary.runtime_aware_import_declarations_landed = true;
  summary.module_metadata_import_surface_landed = true;
  summary.runtime_owned_declaration_import_landed = true;
  summary.runtime_metadata_reference_import_landed = true;
  summary.public_frontend_api_module_surface_landed = true;
  summary.module_name = program.module_name;
  summary.protocol_decl_count = parser_contract_snapshot.protocol_decl_count;
  summary.interface_decl_count = parser_contract_snapshot.interface_decl_count;
  summary.implementation_decl_count =
      parser_contract_snapshot.implementation_decl_count;
  summary.interface_category_decl_count =
      parser_contract_snapshot.interface_category_decl_count;
  summary.implementation_category_decl_count =
      parser_contract_snapshot.implementation_category_decl_count;
  summary.function_decl_count = parser_contract_snapshot.function_decl_count;
  summary.module_import_graph_sites =
      module_import_graph_lowering_contract.module_import_graph_sites;
  summary.import_edge_candidate_sites =
      module_import_graph_lowering_contract.import_edge_candidate_sites;
  summary.namespace_segment_sites =
      module_import_graph_lowering_contract.namespace_segment_sites;
  summary.object_pointer_type_sites =
      module_import_graph_lowering_contract.object_pointer_type_sites;
  summary.pointer_declarator_sites =
      module_import_graph_lowering_contract.pointer_declarator_sites;
  summary.normalized_sites = module_import_graph_lowering_contract.normalized_sites;
  summary.contract_violation_sites =
      module_import_graph_lowering_contract.contract_violation_sites;
  summary.class_record_count =
      runtime_metadata_source_records.classes_lexicographic.size();
  summary.protocol_record_count =
      runtime_metadata_source_records.protocols_lexicographic.size();
  summary.category_record_count =
      runtime_metadata_source_records.categories_lexicographic.size();
  summary.property_record_count =
      runtime_metadata_source_records.properties_lexicographic.size();
  summary.method_record_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.ivar_record_count =
      runtime_metadata_source_records.ivars_lexicographic.size();
  summary.runtime_owned_declaration_count =
      summary.class_record_count + summary.protocol_record_count +
      summary.category_record_count + summary.property_record_count +
      summary.method_record_count + summary.ivar_record_count;
  for (const auto &class_record :
       runtime_metadata_source_records.classes_lexicographic) {
    if (class_record.has_super && !class_record.super_name.empty()) {
      ++summary.superclass_reference_count;
    }
  }
  summary.protocol_reference_count =
      CountProtocolReferences(runtime_metadata_source_records);
  summary.property_accessor_reference_count =
      CountPropertyAccessorReferences(runtime_metadata_source_records);
  summary.property_ivar_binding_reference_count =
      CountPropertyIvarBindingReferences(runtime_metadata_source_records);
  summary.method_selector_reference_count =
      runtime_metadata_source_records.methods_lexicographic.size();
  summary.metadata_reference_count =
      summary.superclass_reference_count + summary.protocol_reference_count +
      summary.property_accessor_reference_count +
      summary.property_ivar_binding_reference_count +
      summary.method_selector_reference_count;
  summary.ready_for_import_artifact_emission =
      summary.source_surface_contract_ready &&
      summary.runtime_metadata_source_records_ready;
  summary.ready_for_frontend_module_consumption =
      summary.ready_for_import_artifact_emission;
  summary.source_surface_replay_key =
      RuntimeAwareImportModuleSurfaceArtifactBuilder::BuildReplayKey(
          program,
          parser_contract_snapshot,
          module_import_graph_lowering_contract);
  if (summary.ready_for_frontend_module_consumption) {
    summary.replay_key = BuildReplayKey(summary);
  }
  if (!IsReadyObjc3RuntimeAwareImportModuleFrontendClosureSummary(summary)) {
    summary.failure_reason =
        "runtime-aware import/module frontend closure summary is incomplete";
  }
  return summary;
}
