  summary.conformance_matrix_consistent =
      summary.recovery_determinism_consistent &&
      summary.recovery_determinism_ready &&
      !summary.recovery_determinism_key.empty() &&
      summary.canonical_reference_form_count == kObjc3CanonicalReferenceTypeForms.size() &&
      summary.canonical_message_scalar_form_count == kObjc3CanonicalScalarMessageSendTypeForms.size() &&
      summary.canonical_bridge_top_form_count == kObjc3CanonicalBridgeTopReferenceTypeForms.size() &&
      summary.canonical_reference_forms_unique &&
      summary.canonical_message_scalar_forms_unique &&
      summary.canonical_bridge_top_forms_unique &&
      summary.canonical_message_scalars_disjoint_from_reference &&
      summary.canonical_bridge_top_subset_of_reference &&
      summary.canonical_bridge_top_excludes_sel &&
      summary.canonical_reference_includes_sel &&
      summary.canonical_message_scalars_include_i32 &&
      summary.canonical_message_scalars_include_bool &&
      summary.canonical_forms_exclude_unknown &&
      summary.canonical_bridge_top_matches_reference_without_sel;
  summary.conformance_matrix_key = std::string("type-form-conformance-matrix;recovery-consistent=") +
                                   BoolKey(summary.recovery_determinism_consistent) +
                                   ";recovery-ready=" + BoolKey(summary.recovery_determinism_ready) +
                                   ";recovery-key-ready=" + BoolKey(!summary.recovery_determinism_key.empty()) +
                                   ";reference-forms=" + std::to_string(summary.canonical_reference_form_count) +
                                   ";message-scalars=" + std::to_string(summary.canonical_message_scalar_form_count) +
                                   ";bridge-top-forms=" + std::to_string(summary.canonical_bridge_top_form_count);
  summary.conformance_matrix_ready =
      summary.conformance_matrix_consistent &&
      !summary.conformance_matrix_key.empty();
  summary.conformance_corpus_case_count =
      summary.canonical_reference_form_count +
      summary.canonical_message_scalar_form_count +
      summary.canonical_bridge_top_form_count;
  summary.conformance_corpus_passed_case_count =
      summary.conformance_matrix_consistent ? summary.conformance_corpus_case_count : 0u;
  summary.conformance_corpus_failed_case_count =
      summary.conformance_corpus_case_count - summary.conformance_corpus_passed_case_count;
  summary.conformance_corpus_consistent =
      summary.conformance_matrix_consistent &&
      summary.conformance_matrix_ready &&
      !summary.conformance_matrix_key.empty() &&
      summary.conformance_corpus_case_count > 0u &&
      summary.conformance_corpus_passed_case_count == summary.conformance_corpus_case_count &&
      summary.conformance_corpus_failed_case_count == 0u;
  summary.conformance_corpus_key =
      std::string("type-form-conformance-corpus;matrix-consistent=") +
      BoolKey(summary.conformance_matrix_consistent) +
      ";matrix-ready=" + BoolKey(summary.conformance_matrix_ready) +
      ";matrix-key-ready=" + BoolKey(!summary.conformance_matrix_key.empty()) +
      ";case-count=" + std::to_string(summary.conformance_corpus_case_count) +
      ";passed=" + std::to_string(summary.conformance_corpus_passed_case_count) +
      ";failed=" + std::to_string(summary.conformance_corpus_failed_case_count);
  summary.conformance_corpus_ready =
      summary.conformance_corpus_consistent &&
      !summary.conformance_corpus_key.empty();
