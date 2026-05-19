Objc3FrontendTypeSystemSemanticModelRecord
BuildFrontendTypeSystemSemanticModelRecord(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  Objc3FrontendTypeSystemSemanticModelRecord record;
  record.contract_id = summary.contract_id;
  record.optional_binding_sites = summary.optional_binding_sites;
  record.optional_binding_clause_sites = summary.optional_binding_clause_sites;
  record.optional_send_sites = summary.optional_send_sites;
  record.nil_coalescing_sites = summary.nil_coalescing_sites;
  record.typed_keypath_literal_sites = summary.typed_keypath_literal_sites;
  record.typed_keypath_self_root_sites = summary.typed_keypath_self_root_sites;
  record.typed_keypath_class_root_sites =
      summary.typed_keypath_class_root_sites;
  record.canonical_type_entries = summary.canonical_type_entries;
  record.canonical_object_type_entries =
      summary.canonical_object_type_entries;
  record.canonical_nullable_entries = summary.canonical_nullable_entries;
  record.canonical_nonnull_entries = summary.canonical_nonnull_entries;
  record.canonical_implicitly_unwrapped_entries =
      summary.canonical_implicitly_unwrapped_entries;
  record.canonical_null_resettable_entries =
      summary.canonical_null_resettable_entries;
  record.canonical_unspecified_nullability_entries =
      summary.canonical_unspecified_nullability_entries;
  record.canonical_invalid_type_entries =
      summary.canonical_invalid_type_entries;
  record.optional_binding_contract_violation_sites =
      summary.optional_binding_contract_violation_sites;
  record.optional_send_contract_violation_sites =
      summary.optional_send_contract_violation_sites;
  record.optional_flow_contract_violation_sites =
      summary.optional_flow_contract_violation_sites;
  record.deterministic = summary.deterministic;
  record.ready_for_lowering_and_runtime =
      summary.ready_for_lowering_and_runtime;
  record.replay_key = summary.replay_key;
  return record;
}

Objc3FrontendTypeSystemSemanticMetadataRecord
BuildFrontendTypeSystemSemanticMetadataRecord(
    const Objc3SemanticTypeMetadataHandoff &handoff) {
  Objc3FrontendTypeSystemSemanticMetadataRecord record;
  record.deterministic = IsDeterministicSemanticTypeMetadataHandoff(handoff);
  record.functions_lexicographic.reserve(handoff.functions_lexicographic.size());
  for (const Objc3SemanticFunctionTypeMetadata &function :
       handoff.functions_lexicographic) {
    record.functions_lexicographic.push_back(ToFrontendFunctionRecord(function));
  }
  record.interfaces_lexicographic.reserve(
      handoff.interfaces_lexicographic.size());
  for (const Objc3SemanticInterfaceTypeMetadata &interface_metadata :
       handoff.interfaces_lexicographic) {
    record.interfaces_lexicographic.push_back(
        ToFrontendInterfaceRecord(interface_metadata));
  }
  record.implementations_lexicographic.reserve(
      handoff.implementations_lexicographic.size());
  for (const Objc3SemanticImplementationTypeMetadata &implementation :
       handoff.implementations_lexicographic) {
    record.implementations_lexicographic.push_back(
        ToFrontendImplementationRecord(implementation));
  }
  return record;
}

Objc3TypeSystemOptionalKeypathLoweringContract
BuildTypeSystemOptionalKeypathLoweringContract(
    const Objc3TypeSystemTypeSemanticModelSummary &summary) {
  return ToLowerOptionalKeypathLoweringContract(
      BuildFrontendTypeSystemOptionalKeypathLoweringContract(
          BuildFrontendTypeSystemSemanticModelRecord(summary)));
}
