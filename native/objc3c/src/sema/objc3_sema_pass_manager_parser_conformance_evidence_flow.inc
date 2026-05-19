Objc3ParserSemaConformanceEvidenceRecord
BuildObjc3ParserSemaConformanceEvidenceRecord(
    const Objc3SemaPassManagerInput &input,
    const Objc3SemaParityContractSurface &surface) {
  Objc3ParserSemaConformanceEvidenceRecord record;
  const Objc3ParserSemaConformanceMatrix &matrix =
      surface.parser_sema_conformance_matrix;
  const Objc3ParserSemaConformanceCorpus &corpus =
      surface.parser_sema_conformance_corpus;
  record.stage_input_owner = input.stage_input_owner;
  record.owner_model = input.owner_model;
  record.strict_no_retired_route = input.strict_no_retired_route;
  record.strict_no_compatibility = input.strict_no_compatibility;
  record.conformance_matrix_deterministic =
      surface.deterministic_parser_sema_conformance_matrix &&
      matrix.deterministic;
  record.conformance_corpus_deterministic =
      surface.deterministic_parser_sema_conformance_corpus &&
      corpus.deterministic;
  record.declaration_count_evidence_ready =
      matrix.top_level_declaration_count_matches &&
      matrix.global_decl_count_matches && matrix.protocol_decl_count_matches &&
      matrix.interface_decl_count_matches &&
      matrix.implementation_decl_count_matches &&
      matrix.function_decl_count_matches;
  record.member_count_evidence_ready =
      matrix.protocol_property_decl_count_matches &&
      matrix.protocol_method_decl_count_matches &&
      matrix.protocol_class_method_decl_count_matches &&
      matrix.protocol_instance_method_decl_count_matches &&
      matrix.interface_property_decl_count_matches &&
      matrix.interface_method_decl_count_matches &&
      matrix.interface_class_method_decl_count_matches &&
      matrix.interface_instance_method_decl_count_matches &&
      matrix.implementation_property_decl_count_matches &&
      matrix.implementation_method_decl_count_matches &&
      matrix.implementation_class_method_decl_count_matches &&
      matrix.implementation_instance_method_decl_count_matches;
  record.category_function_evidence_ready =
      matrix.interface_category_decl_count_matches &&
      matrix.implementation_category_decl_count_matches &&
      matrix.function_prototype_count_matches &&
      matrix.function_pure_count_matches;
  record.fingerprint_evidence_ready =
      matrix.ast_shape_fingerprint_matches &&
      matrix.ast_top_level_layout_fingerprint_matches &&
      matrix.parser_contract_snapshot_fingerprint_matches;
  record.parser_budget_replay_evidence_ready =
      matrix.parser_diagnostic_budget_consistent &&
      matrix.parser_token_top_level_budget_consistent &&
      matrix.parser_subset_count_consistent &&
      matrix.parser_contract_snapshot_deterministic &&
      matrix.parser_recovery_replay_ready;
  record.passed_matrix_evidence_count =
      Objc3SemaEvidenceCount(matrix.top_level_declaration_count_matches) +
      Objc3SemaEvidenceCount(matrix.global_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.protocol_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.protocol_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.interface_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_property_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.implementation_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_class_method_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_instance_method_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.interface_category_decl_count_matches) +
      Objc3SemaEvidenceCount(
          matrix.implementation_category_decl_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_prototype_count_matches) +
      Objc3SemaEvidenceCount(matrix.function_pure_count_matches) +
      Objc3SemaEvidenceCount(matrix.ast_shape_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.ast_top_level_layout_fingerprint_matches) +
      Objc3SemaEvidenceCount(
          matrix.parser_contract_snapshot_fingerprint_matches) +
      Objc3SemaEvidenceCount(matrix.parser_diagnostic_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_token_top_level_budget_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_subset_count_consistent) +
      Objc3SemaEvidenceCount(matrix.parser_contract_snapshot_deterministic) +
      Objc3SemaEvidenceCount(matrix.parser_recovery_replay_ready);
  record.required_corpus_case_count = corpus.required_case_count;
  record.passed_corpus_case_count = corpus.passed_case_count;
  record.failed_corpus_case_count = corpus.failed_case_count;
  record.corpus_inventory_ready =
      corpus.required_case_count == 5u &&
      corpus.has_top_level_declaration_count_case &&
      corpus.has_snapshot_fingerprint_case &&
      corpus.has_diagnostic_budget_case && corpus.has_subset_count_case &&
      corpus.has_recovery_replay_case;
  record.corpus_cases_passed =
      corpus.passed_case_count == corpus.required_case_count &&
      corpus.failed_case_count == 0u &&
      corpus.top_level_declaration_count_case_passed &&
      corpus.snapshot_fingerprint_case_passed &&
      corpus.diagnostic_budget_case_passed &&
      corpus.subset_count_case_passed && corpus.recovery_replay_case_passed;
  record.deterministic =
      Objc3SemaOwnerIsExplicit(
          record.parser_sema_conformance_evidence_owner) &&
      Objc3SemaOwnerIsExplicit(record.stage_input_owner) &&
      Objc3SemaOwnerIsExplicit(record.parser_sema_contract_handoff_owner) &&
      record.owner_model == kObjc3SemaNoRetiredRouteOwnerModel &&
      record.strict_no_retired_route && record.strict_no_compatibility &&
      record.required_matrix_evidence_count == 30u &&
      record.passed_matrix_evidence_count ==
          record.required_matrix_evidence_count &&
      record.required_corpus_case_count == 5u &&
      record.passed_corpus_case_count == record.required_corpus_case_count &&
      record.failed_corpus_case_count == 0u &&
      record.conformance_matrix_deterministic &&
      record.conformance_corpus_deterministic &&
      record.declaration_count_evidence_ready &&
      record.member_count_evidence_ready &&
      record.category_function_evidence_ready &&
      record.fingerprint_evidence_ready &&
      record.parser_budget_replay_evidence_ready &&
      record.corpus_inventory_ready && record.corpus_cases_passed;
  return record;
}
